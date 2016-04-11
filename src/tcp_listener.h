#ifndef _FTP_TCP_LISTENER_H_
#define _FTP_TCP_LISTENER_H_


#include "tcp_transport.h"


class tcp_listener : public tcp_transport {
private:
    int     m_queue_size;

public:
    tcp_listener(void);

    tcp_listener(const std::string & address, const int port, const int queue_size);

    tcp_listener(const tcp_listener & other_listener) = delete;

    tcp_listener(tcp_listener && other_listener);

    virtual ~tcp_listener(void);

public:
    virtual int bind(void);

    virtual int accept(tcp_transport & transport) const;

public:
    tcp_listener & operator=(const tcp_listener & other_transport) = delete;

    tcp_listener & operator=(tcp_listener && other_transport);
};


#endif
