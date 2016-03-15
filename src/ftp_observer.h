#ifndef FTP_OBSERVER_H_
#define FTP_OBSERVER_H_

#include <map>

#include "ftp_command.h"



enum class ftp_observe_event_t {
    FTP_EVENT_FILE_PUT,
    FTP_EVENT_FILE_GET,
    FTP_EVENT_ANY,
};


class ftp_observer {
public:
    ftp_observer(void) { };

    virtual ~ftp_observer(void) { };

public:
    virtual void notify_signal(const std::string & info) = 0;
};


class ftp_notifier {
private:
    typedef std::map<ftp_observer *, ftp_observe_event_t>             observe_table_t;
    typedef std::pair<ftp_observer *, ftp_observe_event_t>            observe_table_pair_t;
    typedef std::map<ftp_observer *, ftp_observe_event_t>::iterator   observe_table_iter_t;

private:
    observe_table_t m_observers;

public:
    ftp_notifier(void);

    ~ftp_notifier(void);

public:
    void register_observer(const ftp_observer * observer, const ftp_observe_event_t & event);

    void unregister_observer(const ftp_observer * observer, const ftp_observe_event_t & event);

    void notify_observers(const ftp_observe_event_t & event, const std::string & message) const;
};


#endif
