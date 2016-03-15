#ifndef TCP_TRANSPORT_H_
#define TCP_TRANSPORT_H_


#include <string>

#include "tls_session.h"


class tcp_transport {
private:
    int                 m_socket;
    tls_session         m_session;
    bool                m_secure;

    std::string         m_address;
    int                 m_port;

private:
    /* forbid creating instances and allow inheritance for concrete classes */
    friend class tcp_client;

    friend class tcp_listener;

private:
    tcp_transport(void);

    tcp_transport(const int tcp_socket_descr);

    tcp_transport(const tcp_transport & other_transport) = delete;

    tcp_transport(tcp_transport && other_transport);

    virtual ~tcp_transport(void);

public:
    virtual size_t push(const char * byte_sequence, const size_t sequence_length) const;

    virtual size_t pull(const size_t receive_buffer_length, void * receive_buffer) const;

    virtual void close(void);

    virtual void create_tls_session(void);

    virtual void set_buffer_size(const int size);

    inline bool secure(void) { return m_secure; }

    inline std::string & address(void) { return m_address; }

    inline int port(void) { return m_port; }

public:
    tcp_transport & operator=(const tcp_transport & other_transport) = delete;

    tcp_transport & operator=(tcp_transport && other_transport);
};



class tcp_client : public tcp_transport {
public:
    tcp_client(void);

    tcp_client(const int tcp_socket_descr);

    tcp_client(const tcp_client & other_client) = delete;

    tcp_client(tcp_client && other_client);

    virtual ~tcp_client(void);

public:
    virtual void connect_to(const std::string & address, const int port);

public:
    tcp_client & operator=(const tcp_client & other_client) = delete;

    tcp_client & operator=(tcp_client && other_transport);
};



class tcp_listener : public tcp_transport {
public:
    tcp_listener(void);

    tcp_listener(const std::string & address, const int port, const int queue_size);

    tcp_listener(const tcp_listener & other_listener) = delete;

    tcp_listener(tcp_listener && other_listener);

    virtual ~tcp_listener(void);

public:
    virtual void accept_transport_client(tcp_client & client) const;

private:
    void initialize_listener(const std::string & address, const int port, const int queue_size);

public:
    tcp_listener & operator=(const tcp_listener & other_transport) = delete;

    tcp_listener & operator=(tcp_listener && other_transport);
};


#endif
