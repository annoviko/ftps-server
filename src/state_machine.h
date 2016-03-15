#ifndef STATE_MACHINE_H_
#define STATE_MACHINE_H_

#include <map>


template <typename state, typename event, typename action>
class state_machine {
private:
    std::map<std::pair<state, event>, action>   m_table;

public:
    void add_transition(state cur_state, event impact, state next_success_state, state next_fail_state, action function) {

    }
};


#endif
