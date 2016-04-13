#ifndef TCP_TRANSPORT_H_
#define TCP_TRANSPORT_H_


#include <string>

#include "tls_session.h"


#define OPERATION_SUCCESS       0
#define OPERATION_FAILURE       -1


class tcp_transport {
protected:
    int                 m_socket;
    tls_session         m_session;
    bool                m_secure;

    std::string         m_address;
    int                 m_port;

public:
    tcp_transport(void);

    tcp_transport(const int tcp_socket_descr);

    tcp_transport(const tcp_transport & other_transport) = delete;

    tcp_transport(tcp_transport && other_transport);

    virtual ~tcp_transport(void);

public:
    virtual ssize_t push(const char * byte_sequence, const size_t sequence_length) const;

    virtual ssize_t pull(const size_t receive_buffer_length, void * receive_buffer) const;

    virtual void close(void);

    virtual int set_tls(void);

public:
    inline bool secure(void) { return m_secure; }

    inline std::string & address(void) { return m_address; }

    inline int port(void) { return m_port; }

public:
    tcp_transport & operator=(const tcp_transport & other_transport) = delete;

    tcp_transport & operator=(tcp_transport && other_transport);
};


#endif
