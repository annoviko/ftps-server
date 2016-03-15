#include "ftp_observer.h"


ftp_notifier::ftp_notifier(void) : m_observers() { }


ftp_notifier::~ftp_notifier(void) { }


void ftp_notifier::register_observer(const ftp_observer * const observer, const ftp_observe_event_t & event) {
    m_observers.insert(observe_table_pair_t((ftp_observer *) observer, event));
}


void ftp_notifier::unregister_observer(const ftp_observer * observer, const ftp_observe_event_t & event) {
    observe_table_iter_t iter = m_observers.find((ftp_observer *) observer);
    if (iter != m_observers.end()) {
        m_observers.erase(iter);
    }
}


void ftp_notifier::notify_observers(const ftp_observe_event_t & event, const std::string & message) const {
    for (auto & observer : m_observers) {
        if (observer.second == event) {
            observer.first->notify_signal(message);
        }
    }
}
