#include "ftp_server.h"
#include "ftp_session.h"
#include "ftp_command.h"


#include <exception>
#include <string>
#include <cstring>
#include <iostream>
#include <sstream>
#include <memory>
#include <list>

#include <unistd.h>

const int ftp_server::FTP_DEFAULT_CONTROL_PORT = 21;
const int ftp_server::FTP_DEFAULT_DATA_PORT = 20;


ftp_server::ftp_server(const std::string & ipv4_address) :
        m_address(ipv4_address),
        m_port(FTP_DEFAULT_CONTROL_PORT),
        m_status(ftp_server_status_t::FTP_SERVER_STOPPING)
{ }


ftp_server::ftp_server(const std::string & ipv4_address, const int port) :
        m_address(ipv4_address),
        m_port(port),
        m_status(ftp_server_status_t::FTP_SERVER_STOPPING)
{ }


void ftp_server::start(void) {
    m_server_thread = std::thread(&ftp_server::server_thread, this);

    std::cout << "FTP server: server is started." << std::endl;
}


void ftp_server::stop(void) {
	m_status = ftp_server_status_t::FTP_SERVER_STOPPING;
	m_server_thread.join();

	std::cout << "FTP server: server is stopped." << std::endl;
}


void ftp_server::server_thread(void) {
    std::list<std::shared_ptr<ftp_session>> client_sessions;

    m_control_channel = tcp_listener(m_address, m_port, 10);

    //tcp_listener listener(m_address, m_port, 10);
    //m_control_channel = std::move(listener);

    std::cout << "move control channel" << std::endl;

    m_status = ftp_server_status_t::FTP_SERVER_RUNNING;

    while(status() == ftp_server_status_t::FTP_SERVER_RUNNING) {
        tcp_client client_connection;
        m_control_channel.accept_transport_client(client_connection);

        std::cout << "FTP server: incoming connection - create new server client thread." << std::endl;

        std::shared_ptr<ftp_session> session(new ftp_session(client_connection, &m_notifier));
        session->start();

        client_sessions.push_back(session);
    }

    std::cout << "FTP server: server main thread is terminated." << std::endl;
}


void ftp_server::register_observer(const ftp_observer * const observer, const ftp_observe_event_t event) {
    m_notifier.register_observer(observer, event);
}


void ftp_server::unregister_observer(const ftp_observer * const observer, const ftp_observe_event_t event) {
    m_notifier.unregister_observer(observer, event);
}
