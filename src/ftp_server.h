#ifndef FTP_SERVER_H_
#define FTP_SERVER_H_

#include <cstring>
#include <thread>

#include "tcp_transport.h"
#include "ftp_observer.h"
#include "tcp_listener.h"


enum class ftp_server_status_t {
	FTP_SERVER_RUNNING,
	FTP_SERVER_STOPPING,
};


class ftp_server {
public:
    const static int    FTP_DEFAULT_CONTROL_PORT;
    const static int    FTP_DEFAULT_DATA_PORT;

private:
    tcp_listener        m_control_channel;

    std::string   m_address;

    int           m_port;

    ftp_server_status_t m_status;

    std::thread         m_server_thread;

    ftp_notifier        m_notifier;

public:
    ftp_server(const std::string & ipv4_address);

    ftp_server(const std::string & ipv4_address, const int port);

public:
	void start(void);

	void stop(void);

	inline ftp_server_status_t status(void) { return m_status; }

	void register_observer(const ftp_observer * const observer, const ftp_observe_event_t event);

	void unregister_observer(const ftp_observer * const observer, const ftp_observe_event_t event);

private:
	void server_thread(void);
};


#endif
