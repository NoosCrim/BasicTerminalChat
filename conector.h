#ifndef CONECTOR_H_INCLUDED
#define CONECTOR_H_INCLUDED
#include "common.h"
#include "connection.h"
namespace net
{
    template<typename ID_T, typename C_T = connection<ID_T>>
    class connector
    {
    protected:
        net_context& context;
        boost::system::error_code& ec;
        boost::asio::ip::tcp::acceptor acceptor;
        boost::asio::ip::tcp::socket socket;
        void async_accept()
        {
            acceptor.async_accept(context._context,
                [this](boost::system::error_code l_ec, boost::asio::ip::tcp::socket l_socket)
                {
                    if(!ec)
                    {
                        std::clog << "Accepted connection!" << std::endl;
                        on_accept(new C_T(l_socket, context, ec));
                    }
                    else
                    {
                        std::cerr << ec.message() << std::endl;
                    }
                });
        }
        virtual void on_accept(C_T *new_connection)
        {
            new_connection->start_read();
            async_accept();
        };
        virtual void on_connect(C_T *new_connection){};
    public:
        connector(net_context& _context, boost::system::error_code& _ec): context(_context), ec(_ec), acceptor(_context._context), socket(_context._context)
        {
            static_assert(std::is_convertible<C_T*, connection<ID_T>*>::value, "C_T must derive publicly from connection<ID_T>!");
        }
        C_T* connect(const char* address, unsigned int port)
        {
            std::clog << "Connecting to a host..." << std::endl;
            socket.connect(boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string(address), port), ec);
            if(!ec)
            {
                std::clog << "Connected!" << std::endl;
                return new C_T(socket, context, ec);
            }
            std::cerr << ec.message() << std::endl;
            return nullptr;
        }
        void async_connect(const char* address, unsigned int port) //async
        {
            std::clog << "Connecting to a host..." << std::endl;
            socket.async_connect(boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string(address), port),
                [this](boost::system::error_code ec)
                {
                    if(!ec)
                    {
                        std::clog << "Connected!" << std::endl;
                        on_connect(new C_T(socket, ec));
                    }
                    else
                        std::cerr << ec.message() << std::endl;
                });
            context.async_run();
        }
        void start_accept(unsigned int _port) //async
        {
            acceptor = boost::asio::ip::tcp::acceptor(context, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), _port));
            if(!ec)
            {
                std::clog << "Listening for connections..." << std::endl;
                async_accept();
            }
            else
                std::cerr << ec.message() << std::endl;
            context.async_run();
        }
        C_T* accept(unsigned int _port)
        {
            acceptor = boost::asio::ip::tcp::acceptor(context._context, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), _port));
            boost::asio::ip::tcp::socket socket(context._context);
            std::clog << "Listeining for connections..." << std::endl;
            acceptor.accept(socket, ec);
            if(!ec)
            {
                std::clog << "Connected!" << std::endl;
                return new C_T(socket, context, ec);
            }
            std::cerr << ec.message() << std::endl;
            return nullptr;

        }
    };
}


#endif // CONECTOR_H_INCLUDED
