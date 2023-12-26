#ifndef MESSAGE_H_INCLUDED
#define MESSAGE_H_INCLUDED
namespace net
{
    template<typename ID_T>
    struct message //stack-like container storing raw data. Used for network comunication
    {
        struct header
        {
            ID_T ID;
            uint32_t body_size = 0;
        } header;
        std::vector<uint8_t> body;
        template<typename T>
        friend message& operator<<(message& m, const T& in)
        {
            m.body.resize(m.body.size() + sizeof(T));
            m.header.body_size = m.body.size();
            *((T*)&(m.body[m.body.size() - sizeof(T)])) = in;
            return m;
        }
        template<typename T, uint32_t arr_size>
        friend message& operator<<(message& m, const T (&in)[arr_size])
        {
            uint32_t pre_size = m.body.size();
            m.body.resize(m.body.size() + sizeof(T)*arr_size);
            m.header.body_size = m.body.size();
            copy(in, in + arr_size, m.body.data()+pre_size);
            return m;
        }
        friend message& operator<<(message& m, const std::string& in)
        {
            uint32_t pre_size = m.body.size();
            m.body.resize(m.body.size() + in.size());
            m.header.body_size = m.body.size();
            copy(in.begin(), in.end(), m.body.data()+pre_size);
            return m;
        }
        template<typename T>
        friend message& operator>>(message& m, T& in)
        {
            if(m.body.size() < sizeof(T))
                throw("message: error: tried reading more data than avaliable from message!");
            in = *((T*)&(m.body[m.body.size() - sizeof(T)]));
            m.body.resize(m.body.size()-sizeof(T));
            m.header.body_size = m.body.size();
            return m;
        }
        template<typename T, size_t arr_size>
        friend message& operator>>(message& m, T (&in)[arr_size])
        {
            uint32_t pre_size = m.body.size();
            if(m.body.size() < sizeof(T) * arr_size)
                throw("message: error: tried reading more data than avaliable from message!");
            m.body.resize(m.body.size() + sizeof(T)*arr_size);
            m.header.body_size = m.body.size();
            copy(in, in+arr_size, m.body.data()+pre_size);
            return m;
        }
    };
}


#endif // MESSAGE_H_INCLUDED
