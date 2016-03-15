#include "tcp_transport.h"

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <stdexcept>
#include <iostream>


tcp_transport::tcp_transport(void) {
    m_socket = socket(AF_INET, SOCK_STREAM, 0);
    m_secure = false;
    m_address = "";
    m_port = 0;
}


tcp_transport::tcp_transport(const int tcp_socket_descr) :
        m_socket(tcp_socket_descr),
        m_secure(false),
        m_address(""),
        m_port(0)
{ }


tcp_transport::tcp_transport(tcp_transport && other_transport) {
    m_socket    = std::move(other_transport.m_socket);
    m_session   = std::move(other_transport.m_session);
    m_secure    = std::move(other_transport.m_secure);
    m_address   = std::move(other_transport.m_address);
    m_port      = std::move(other_transport.m_port);

    other_transport.m_socket    = NULL;
}


tcp_transport::~tcp_transport(void) { }


void tcp_transport::set_buffer_size(const int buffer_size) {
    (void) buffer_size;
}


size_t tcp_transport::push(const char * byte_sequence, const size_t sequence_length) const {
    if (m_secure) {
        return m_session.push(byte_sequence, sequence_length);
    }
    else {
        return send(m_socket, byte_sequence, sequence_length, 0);
    }
}


size_t tcp_transport::pull(const size_t receive_buffer_length, void * receive_buffer) const {
    if (m_secure) {
        return m_session.pull(receive_buffer_length, receive_buffer);
    }
    else {
        return recv(m_socket, receive_buffer, receive_buffer_length, 0);
    }
}


void tcp_transport::close(void) {
    if (m_secure) {
        m_session.close();
    }

    ::close(m_socket);
    m_socket = 0;
}


void tcp_transport::create_tls_session(void) {
    m_session = tls_session(m_socket);
}


tcp_transport & tcp_transport::operator=(tcp_transport && other_transport) {
    if (this != &other_transport) {
        m_socket    = std::move(other_transport.m_socket);
        m_session   = std::move(other_transport.m_session);
        m_secure    = std::move(other_transport.m_secure);
        m_address   = std::move(other_transport.m_address);
        m_port      = std::move(other_transport.m_port);

        other_transport.m_socket    = 0;
        other_transport.m_secure    = false;
        other_transport.m_port      = 0;
    }

    return *this;
}



tcp_client::tcp_client(void) { }


tcp_client::tcp_client(const int tcp_socket_descr) : tcp_transport(tcp_socket_descr) { }


tcp_client::tcp_client(tcp_client && other_client) : tcp_transport(std::move(other_client)) { }


tcp_client::~tcp_client(void) { }


void tcp_client::connect_to(const std::string & address, const int port) {
    struct sockaddr_in client_address;
    client_address.sin_family = AF_INET;
    client_address.sin_port = htons(port);
    client_address.sin_addr.s_addr = inet_addr(address.c_str());

    if (connect(m_socket, (struct sockaddr *) &client_address, sizeof(client_address)) < 0) {
        throw std::runtime_error("impossible to connect to " + address + ":" + std::to_string(port));
    }

    m_address = address;
    m_port = port;
}


tcp_client & tcp_client::operator =(tcp_client && other_transport) {
    static_cast<tcp_transport &>(*this) = std::move(other_transport);
    return *this;
}



tcp_listener::tcp_listener(void) : tcp_transport() { }


tcp_listener::tcp_listener(const std::string & address, const int port, const int queue_size) {
    m_address = address;
    m_port = port;

    initialize_listener(address, port, queue_size);
}


tcp_listener::tcp_listener(tcp_listener && other_listener) : tcp_transport(std::move(other_listener)) { }


tcp_listener::~tcp_listener(void) { }


void tcp_listener::initialize_listener(const std::string & address, const int port, const int queue_size) {
    struct sockaddr_in ip4_server_address;
    ip4_server_address.sin_family = AF_INET;
    ip4_server_address.sin_port = htons(port);

    inet_pton(AF_INET, address.c_str(), &ip4_server_address.sin_addr);

    if (bind(m_socket, (struct sockaddr *) &ip4_server_address, sizeof(ip4_server_address)) == -1) {
        throw std::runtime_error("impossible to run server on " + address + ":" + std::to_string(port));
    }

    listen(m_socket, queue_size);
}


void tcp_listener::accept_transport_client(tcp_client & client) const {
    struct sockaddr_in ipv4_addr_client;
    int ipv4_addr_client_len = sizeof(struct sockaddr_in);

    int client_socket = accept(m_socket, (struct sockaddr *) &ipv4_addr_client, (socklen_t *) &ipv4_addr_client_len);
    if (client_socket < 0) {
        throw std::runtime_error("impossible to accept incoming connection.");
    }

    client = tcp_client(client_socket);
}


tcp_listener & tcp_listener::operator =(tcp_listener && other_transport) {
    static_cast<tcp_transport &>(*this) = std::move(other_transport);
    return *this;
}
