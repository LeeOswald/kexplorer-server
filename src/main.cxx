#include "logger.hxx"

#include <iostream>

#include <boost/program_options.hpp>

#include <fcntl.h>
#include <sys/stat.h>


namespace
{


bool daemonize() noexcept
{
    auto pid = ::fork();

    if (pid < 0)
        return false;

    if (pid > 0)
        return true;

    if (::setsid() < 0)
        return false;

    auto null = ::open("/dev/null", O_RDWR);
    ::dup2(null, STDIN_FILENO);
    ::dup2(null, STDOUT_FILENO);
    ::dup2(null, STDERR_FILENO);
    ::close(null);

    ::chdir("/");

    ::signal(SIGCHLD, SIG_IGN);
    ::signal(SIGHUP, SIG_IGN);

    pid = ::fork();

    if (pid < 0)
        return false;

    if (pid > 0)
        return true;

    ::umask(0);

    return true;
}

} // namespace {}


int main(int argc, char* argv[])
{
    if (::geteuid() != 0)
    {
        std::cerr << "Root privileges required\n";
        return EXIT_FAILURE;
    }

    try
    {
        namespace po = boost::program_options;
        po::options_description options("Command line options");
        options.add_options()
            ("help,h", "display this message")
            ("verbose,v", "display debug output")
            ("daemon,d", "run as a daemon")
        ;

        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, options), vm);
        po::notify(vm);

        if (vm.count("help"))
        {
            std::cerr << options << "\n";
            return EXIT_SUCCESS;
        }

        Kes::Log::Level logLevel = vm.count("verbose") ? Kes::Log::Level::Debug : Kes::Log::Level::Info;

        Kes::Private::Logger logger(logLevel, "/var/log/kexplorer-server.log");

        if (vm.count("daemon"))
            if (!daemonize())
                return EXIT_FAILURE;
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << "\n";
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
