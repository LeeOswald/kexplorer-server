#include "client.hxx"
#include "commands.hxx"

#include <iostream>

#include <boost/asio.hpp>
#include <boost/program_options.hpp>

namespace
{

std::pair<std::string, uint16_t> endpointFromString(const std::string& s)
{
    auto p0 = s.find_last_of('.');
    auto p1 = s.find_last_of(':');
    if ((p0 == s.npos) || (p1 == s.npos))
        throw std::invalid_argument("Invalid server address:port pair");

    std::string addressPart, portPart;
    if (p1 > p0)
    {
        // IPv4 127.0.0.1:8080
        addressPart = s.substr(0, p1);
        portPart = s.substr(p1 + 1);
    }
    else if (p0 > p1)
    {
        // IPv6 ::1.8080
        addressPart = s.substr(0, p0);
        portPart = s.substr(p0 + 1);
    }

    uint16_t port = uint16_t(std::strtoul(portPart.c_str(), nullptr, 10));
    if (!port)
        throw std::invalid_argument("Invalid server port");

    return std::make_pair(addressPart, port);
}


} // namespace {}


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
            ("command,c", po::value<std::string>(), "command to execute")
        ;

        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, options), vm);
        po::notify(vm);

        if (vm.count("help"))
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

        boost::asio::io_context io;

        boost::asio::signal_set signals(io);
        signals.add(SIGINT);
        signals.add(SIGTERM);
        signals.add(SIGPIPE);
        signals.add(SIGHUP);

        signals.async_wait(
            [&](boost::system::error_code /*ec*/, int /*signo*/)
            {
                io.stop();
            }
        );

        auto endpoint = endpointFromString(addr);

        Kesctl::Client client(io, endpoint.first.c_str(), endpoint.second, verbose, std::cout, std::cerr);

        if (vm.count("command") > 0)
        {
            auto command = vm["command"].as<std::string>();
            auto response = client.command(Kesctl::simpleCommand(command));

            std::cout << response << "\n";
        }
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << "\n";
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
