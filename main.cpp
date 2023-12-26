#include <iostream>
#include "networking.h"
using namespace std;
int main(int argc, char* argv[])
{
    if(argc == 1)
    {
        std::cerr << "Usage: \n\t" << argv[0] << " host [port]\n\t" << argv[0] << " client [ipv4] [port]" << std::endl;
        return -1;
    }
    string arg = argv[1];
    net::net_context context;
    boost::system::error_code ec;
    if(arg == "host")
    {
        if(argc != 2)
        {
            std::cerr << "Usage: \n\t" << argv[0] << " host [port]\n\t" << argv[0] << " client [ipv4] [port]" << std::endl;
            return -1;
        }
        unsigned int PORT = stoi(argv[2]);
        net::connector<char> server(context, ec);
        net::connection<char> *connection = server.accept(PORT);
        if(connection)
            connection->start_read();
        else
            return 1;
        string s;
        while(s!="exit")
        {
            getline(cin, s);
            net::message<char> msg;
            msg << s;
            if(!connection->send(msg))
            {
                cerr << "Connection lost!" << endl;
                connection->close();
                context.join();
                delete connection;
                return 1;
            }
        }
        connection->close();
        context.join();
        delete connection;
        clog << "Server closed!" << endl;
    }
    else if(arg == "client")
    {
        if(argc != 3)
        {
            std::cerr << "Usage: \n\t" << argv[0] << " host [port]\n\t" << argv[0] << " client [ipv4] [port]" << std::endl;
            return -1;
        }
        unsigned int PORT = stoi(argv[3]);
        net::connector<char> client(context, ec);
        net::connection<char>* connection = client.connect(argv[2], PORT);
        if(connection)
            connection->start_read();
        else
            return 1;
        string s = "";
        while(s!="exit")
        {
            getline(cin, s);
            net::message<char> msg;
            msg << s;
            if(!connection->send(msg))
            {
                cerr << "Connection lost!" << endl;
                connection->close();
                context.join();
                delete connection;
                return 1;
            }
        }
        connection->close();
        context.join();
        delete connection;
        clog << "Connection closed!" << endl;
    }
    return 0;
}
