#ifndef _FTP_TCP_CLIENT_H_
#define _FTP_TCP_CLIENT_H_


#include "tcp_transport.h"


class tcp_client : public tcp_transport {
public:
    tcp_client(void);

    tcp_client(const int tcp_socket_descr);

    tcp_client(const tcp_client & other_client) = delete;

    tcp_client(tcp_client && other_client);

    virtual ~tcp_client(void);

public:
    virtual int connect(const std::string & address, const int port);

public:
    tcp_client & operator=(const tcp_client & other_client) = delete;

    tcp_client & operator=(tcp_client && other_transport);
};

#endif
