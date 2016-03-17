#include "ftp_server.h"
#include "ftp_observer.h"

#include <iostream>

class main_observer : public ftp_observer {
public:
    main_observer(void) { }

    virtual ~main_observer(void) { }

public:
    virtual void notify_signal(const std::string & info) {
        std::cout << "OBSERVER: file uploaded: " << info << std::endl;
    }
};

int main(int argc, char *argv[]) {
    (void) argc;
    (void) argv;

	ftp_server server("127.0.0.1");
	server.start();

	main_observer file_getput_obsever;
	server.register_observer(&file_getput_obsever, ftp_observe_event_t::FTP_EVENT_FILE_PUT);

	while(1) {	}

	server.stop();

	return 0;
}
