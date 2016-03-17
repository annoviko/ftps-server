#include "ftp_observer.h"


ftp_notifier::ftp_notifier(void) : m_observers() { }


ftp_notifier::~ftp_notifier(void) { }


int ftp_notifier::register_observer(const ftp_observer * const observer, const ftp_observe_event_t & event) {
    m_observers.insert(observe_table_pair_t((ftp_observer *) observer, event));
    return OPERATION_SUCCESS;
}


int ftp_notifier::unregister_observer(const ftp_observer * observer, const ftp_observe_event_t & event) {
    for (observe_table_iter_t iter = m_observers.begin(); iter != m_observers.end(); iter++) {
        if ((iter->first == observer) && (iter->second == event)) {
            m_observers.erase(iter);
            return OPERATION_SUCCESS;
        }
    }

    return OPERATION_FAILURE;
}


void ftp_notifier::notify_observers(const ftp_observe_event_t & event, const std::string & message) const {
    for (auto & observer : m_observers) {
        if (observer.second == event) {
            observer.first->notify_signal(message);
        }
    }
}
