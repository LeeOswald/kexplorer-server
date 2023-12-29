#include "client.hxx"

#include <kesrv/util/netutil.hxx>
#include <kesrv/util/request.hxx>

#include <iostream>

#include <boost/asio.hpp>
#include <boost/program_options.hpp>


int main(int argc, char* argv[])
{
    try
    {
        namespace po = boost::program_options;
        po::options_description options("Command line options");
        options.add_options()
            ("help,h", "display this message")
            ("verbose,v", "display debug output")
            ("address,a", po::value<std::string>(), "server address:port")
            ("command,c", po::value<std::string>(), "execute command")
        ;

        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, options), vm);
        po::notify(vm);

        if (vm.count("help") || !vm.count("command"))
        {
            std::cerr << options << "\n";
            return EXIT_SUCCESS;
        }

        bool verbose = (vm.count("verbose") > 0);

        std::string addr("127.0.0.1:6665");
        if (vm.count("address"))
        {
            addr = vm["address"].as<std::string>();
        }

        Kes::initialize();

        boost::asio::io_context io;

        auto endpoint = Kes::Util::splitAddress(addr);

        Kesctl::Client client(io, endpoint.first.c_str(), endpoint.second, verbose, std::cout, std::cerr);

        boost::asio::signal_set signals(io);
        signals.add(SIGINT);
        signals.add(SIGTERM);
#if !KES_WINDOWS
        signals.add(SIGPIPE);
        signals.add(SIGHUP);
#endif
        signals.async_wait(
            [&io, &client]([[maybe_unused]] boost::system::error_code ec, [[maybe_unused]] int signo)
            {
                client.stop();
                io.stop();
            }
        );


        auto cmd = vm["command"].as<std::string>();
        client.command(Kes::Util::Request::simple(cmd.c_str()));

        io.run();

        Kes::finalize();
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << "\n";
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
