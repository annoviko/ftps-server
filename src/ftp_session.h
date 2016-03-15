#ifndef FTP_SESSION_H_
#define FTP_SESSION_H_

#include "ftp_user.h"
#include "tcp_transport.h"

#include "state_machine.h"
#include "ftp_command.h"
#include "ftp_observer.h"

#include <vector>
#include <functional>


enum class ftp_session_state_t {
    FTP_SESSION_AUTH_WAIT_LOGIN,
    FTP_SESSION_AUTH_WAIT_PASSWORD,
    FTP_SESSION_AUTH_SUCCESS,
    FTP_SESSION_DATA_ACTION,
    FTP_SESSION_TERMINATED,
};


enum class ftp_transfer_t {
    TYPE_TRANSFER_BINARY,
    TYPE_TRANSFER_ASCII,
};


enum class ftp_protection_t {
    FTP_PROTECTION_CLEAR,
    FTP_PROTECTION_SAFE,
    FTP_PROTECTION_CONFIDENTIAL,
    FTP_PROTECTION_PRIVATE,
};


typedef state_machine<ftp_session_state_t, ftp_command_t, std::function<void (void)> >      ftp_state_machine;


class ftp_session {
private:
    const static int    MAX_AUTH_ATTEMPTS;
    static int          DATA_PORT_ALLOCATION;

    ftp_state_machine m_ftp_stm;

private:
    ftp_user        m_user;
    std::string     m_folder;

    ftp_transfer_t          m_transfer_type;
    ftp_session_state_t     m_state;
    std::string             m_argument; /* TODO: state machine is require */

    ftp_notifier *          m_notifier;

    tcp_client          m_control_channel;
    tcp_client          m_data_channel;
    int                 m_buffer_file_size;

    int                 m_auth_attempt;
    int                 m_protection_buffer_size;
    ftp_protection_t    m_protection_level;

public:
    ftp_session(tcp_client & client_control_channel, ftp_notifier * notifier);

    ftp_session(ftp_session && another_session);

    ~ftp_session(void);

public:
    void start(void);

    void stop(void);

public:
    void client_session_thread(void);

    void process_command(const ftp_command & input_command);

    void command_user(const ftp_command & input_command);

    void command_pass(const ftp_command & input_command);

    void command_syst(const ftp_command & input_command);

    void command_pwd(const ftp_command & input_command);

    void command_port(const ftp_command & input_command);

    void command_list(const ftp_command & input_command);

    void command_cwd(const ftp_command & input_command);

    void command_stor(const ftp_command & input_command);

    void command_type(const ftp_command & input_command);

    void command_feat(const ftp_command & input_command);

    void command_auth(const ftp_command & input_command);

    void command_pbsz(const ftp_command & input_command);

    void command_prot(const ftp_command & input_command);

    void command_pasv(const ftp_command & input_command);

    void command_allo(const ftp_command & input_command);

    void command_dele(const ftp_command & input_command);

    void command_rnfr(const ftp_command & input_command);

    void command_rnto(const ftp_command & input_command);

    void command_quit(const ftp_command & input_command);
};

#endif
