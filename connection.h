#ifndef CONNECTION_H_INCLUDED
#define CONNECTION_H_INCLUDED
#include "common.h"
namespace net
{
    template<typename ID_T>
    class connection
    {
    protected:
        boost::system::error_code& ec;
        net_context& context;
        boost::asio::ip::tcp::socket socket;
        bool closed;
        bool asyncReads, asyncAccepts;
        virtual void on_read(message<ID_T>& tempMsg)//called when async_read reads
        {
            std::string str;
            str.resize(tempMsg.header.body_size);
            copy(tempMsg.body.begin(), tempMsg.body.end(), str.begin());
            if(str == "exit")
            {
                std::clog << "Closing connection..." << std::endl;
                socket.cancel(ec);
                socket.close(ec);
                if(!ec)
                    std::clog << "Connection closed!" << std::endl;
                else
                {
                    std::cerr << ec.message() << std::endl;
                }
                return;
            }
            else std::cout << "\nReceived: " <<  str << std::endl;
            if(socket.is_open())
                async_read_header();
        }
    private:
        void _on_read(message<ID_T>* tempMsg)
        {
            on_read(*tempMsg);
            delete tempMsg;
        }
        void async_read_header()
        {
            message<ID_T>* tempMsg = new message<ID_T>;
            boost::asio::async_read(socket, boost::asio::buffer(&(tempMsg->header), sizeof(tempMsg->header)),[this, tempMsg](boost::system::error_code ec, size_t length)
            {
                if(!ec)
                {
                    if(tempMsg->header.body_size)
                    {
                        tempMsg->body.resize(tempMsg->header.body_size);
                        async_read_body(tempMsg);//tempMsg gets deleted down the way, when no longer needed
                    }
                    else if(socket.is_open())
                    {
                        _on_read(tempMsg);
                    }
                }
                else
                {
                    std::cerr << ec.message() << std::endl;
                    if(ec == boost::asio::error::eof)
                    {
                        return;
                    }
                }

            });
        }
        void async_read_body(message<ID_T>* tempMsg)
        {
            boost::asio::async_read(socket, boost::asio::buffer(tempMsg->body.data(), tempMsg->header.body_size),[this, tempMsg](boost::system::error_code ec, size_t length)
            {
                if(!ec)
                    _on_read(tempMsg);//deletes tempMsg at the end
                else
                {
                    if(ec == boost::asio::error::eof)
                    {
                        return;
                    }
                    std::cerr << ec.message() << std::endl;
                }
            });
        }
    public:
        connection(boost::asio::ip::tcp::socket& _socket, net_context& _context, boost::system::error_code& _ec) : ec(_ec), context(_context), socket(std::move(_socket))
        {

        }
        virtual ~connection(){}
        void close()
        {
            socket.cancel(ec);
            if(ec)
                std::cerr << ec.message() << std::endl;
            socket.close(ec);
            if(ec)
                std::cerr << ec.message() << std::endl;
        }
        bool send(const message<ID_T>& msg)
        {
            socket.write_some(boost::asio::buffer(&(msg.header), sizeof(message<ID_T>::header)), ec);
            if(ec)
            {
                std::cerr << ec.message() << std::endl;
                return false;
            }
            socket.write_some(boost::asio::buffer(msg.body.data(), msg.header.body_size), ec);
            if(ec)
            {
                std::cerr << ec.message() << std::endl;
                return false;
            }
            return true;
        }
        void read(message<ID_T>& outMsg)
        {
            boost::asio::read(socket, boost::asio::buffer(&outMsg.header, sizeof(message<ID_T>::header)), ec);
            if(ec) std::cerr << ec.message() << std::endl;
            outMsg.body.resize(outMsg.header.body_size);
            boost::asio::read(socket, boost::asio::buffer(outMsg.body.data(), outMsg.header.body_size), ec);
            if(ec) std::cerr << ec.message() << std::endl;
            return;
        }
        void start_read() //async
        {
            async_read_header();
            context.async_run();
        }
        void stop_read()
        {
            socket.cancel(ec);
            if(ec)
                std::cerr << ec.message() << std::endl;
        }
    };
}


#endif // CONNECTION_H_INCLUDED
