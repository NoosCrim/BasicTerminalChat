#ifndef COMMON_H_INCLUDED
#define COMMON_H_INCLUDED
#include <iostream>
#include <boost/asio.hpp>
#include <string>
#include <boost/asio/ip/address.hpp>
#include <boost/bind/bind.hpp>
#include <thread>
#include <memory>
#include <cstdlib>
#include <vector>
#include <queue>
#include <list>
#include "message.h"
namespace net
{

    class net_context
    {
        std::thread thread;
    public:
        net_context()
        {
            _context.poll();
        }
        boost::asio::io_context _context;
        void async_run()
        {
            if(_context.stopped())
            {
                _context.restart();
                thread = std::thread([this](){_context.run();});
            }
        }
        void run()
        {
            if(_context.stopped())
            {
                _context.restart();
                _context.run();
            }
            else
                thread.join();
        }
        void join()
        {
            thread.join();
        }
    };

}
#endif // COMMON_H_INCLUDED
