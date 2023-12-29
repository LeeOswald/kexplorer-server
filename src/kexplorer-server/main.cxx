#include <kesrv/condition.hxx>
#include <kesrv/knownprops.hxx>
#include <kesrv/processmanager/processmanager.hxx>
#include <kesrv/util/exceptionutil.hxx>

#include "globalcmdhandler.hxx"
#include "iorunner.hxx"
#include "logger.hxx"
#include "requestprocessor.hxx"
#include "sessionhandler.hxx"
#include "tcpserver.hxx"

#include <iostream>
#include <sstream>

#include <boost/program_options.hpp>

#include <fcntl.h>
#include <sys/stat.h>


namespace
{

Kes::Log::ILog* g_log = nullptr;


void daemonize() noexcept
{
    // Fork the process and have the parent exit. If the process was started
    // from a shell, this returns control to the user. Forking a new process is
    // also a prerequisite for the subsequent call to setsid().
    auto pid = ::fork();

    if (pid < 0)
        ::exit(EXIT_FAILURE);

    if (pid > 0)
        ::exit(EXIT_SUCCESS);

    // Make the process a new session leader. This detaches it from the terminal.
    if (::setsid() < 0)
        ::exit(EXIT_FAILURE);

    // Close the standard streams. This decouples the daemon from the terminal that started it.
    auto null = ::open("/dev/null", O_RDWR);
    ::dup2(null, STDIN_FILENO);
    ::dup2(null, STDOUT_FILENO);
    ::dup2(null, STDERR_FILENO);
    ::close(null);

    // A process inherits its working directory from its parent. This could be
    // on a mounted filesystem, which means that the running daemon would
    // prevent this filesystem from being unmounted. Changing to the root
    // directory avoids this problem.
    ::chdir("/");

    ::signal(SIGCHLD, SIG_IGN);
    ::signal(SIGHUP, SIG_IGN);

    // A second fork ensures the process cannot acquire a controlling terminal.
    pid = ::fork();

    if (pid < 0)
        ::exit(EXIT_FAILURE);

    if (pid > 0)
        ::exit(EXIT_SUCCESS);

    // The file mode creation mask is also inherited from the parent process.
    // We don't want to restrict the permissions on files created by the
    // daemon, so the mask is cleared.
    ::umask(0);
}

void terminateHandler()
{
    std::ostringstream ss;
    ss << boost::stacktrace::stacktrace();

    LogFatal(g_log, "std::terminate() called from\n%s", ss.str().c_str());

    std::abort();
}

} // namespace {}


int main(int argc, char* argv[])
{
#if !KES_DEBUG
    if (::geteuid() != 0)
    {
        std::cerr << "Root privileges required\n";
        return EXIT_FAILURE;
    }
#endif


    namespace po = boost::program_options;
    po::options_description options("Command line options");
    options.add_options()
        ("help,h", "display this message")
        ("verbose,v", "display debug output")
        ("daemon,d", "run as a daemon")
        ("address,a", po::value<std::string>(), "server bind address:port")
    ;

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, options), vm);
    po::notify(vm);

    if (vm.count("help"))
    {
        std::cerr << options << "\n";
        return EXIT_SUCCESS;
    }

    if (vm.count("daemon"))
        daemonize();

    Kes::initialize();

    Kes::Log::Level logLevel = vm.count("verbose") ? Kes::Log::Level::Debug : Kes::Log::Level::Info;

#if !KES_DEBUG
    Kes::Private::Logger logger(logLevel, "/var/log/kexplorer-server.log");
#else
    Kes::Private::Logger logger(logLevel, "kexplorer-server.log");
#endif

    g_log = &logger;

    std::set_terminate(terminateHandler);

    try
    {
        std::string bindAddr("127.0.0.1:6665");
        if (vm.count("address"))
        {
            bindAddr = vm["address"].as<std::string>();
        }

        logger.write(Kes::Log::Level::Info, "Binding to address %s", bindAddr.c_str());

        Kes::Condition exitCondition(false);

        const size_t threadCount = 1;
        std::unique_ptr<Kes::Private::IoRunner> runner(new  Kes::Private::IoRunner(threadCount, &logger));

        auto& io = runner->io_context();
        boost::asio::signal_set signals(io);
        signals.add(SIGINT);
        signals.add(SIGTERM);
        signals.add(SIGPIPE);
        signals.add(SIGHUP);

        std::optional<int> signalReceived;

        signals.async_wait(
            [&exitCondition, &signalReceived, &logger](boost::system::error_code ec, int signo)
            {
                if (!ec)
                {
                    signalReceived = signo;
                    exitCondition.set();
                }
                else
                {
                    logger.write(Kes::Log::Level::Error, "Failed to wait for signals: %s", ec.message().c_str());
                }
            }
        );

        Kes::Private::RequestProcessor requestProcessor(&logger);
        Kes::Private::GlobalCmdHandler globalHandler(&requestProcessor, exitCondition, &logger);
        Kes::Private::ProcessManager processManaher(&requestProcessor, &logger);

        const size_t bufferSize = 65536;
        const size_t bufferLimit = 65536;
        Kes::Private::SessionHandlerOptions sho(bufferSize, bufferLimit, &requestProcessor, &logger);
        Kes::Private::TcpServer<Kes::Private::SessionHandler, Kes::Private::SessionHandlerOptions> server(runner->io_context(), sho, bindAddr.c_str(), bufferSize, &logger);

        exitCondition.wait();
        if (signalReceived)
        {
            logger.write(Kes::Log::Level::Warning, "Exiting due to signal %d", *signalReceived);
        }

        io.stop();

        Kes::finalize();

        g_log = nullptr;
    }
    catch (Kes::Exception& e)
    {
        Kes::Util::logException(g_log, Kes::Log::Level::Fatal, e);
        return EXIT_FAILURE;
    }
    catch (std::exception& e)
    {
        Kes::Util::logException(g_log, Kes::Log::Level::Fatal, e);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
