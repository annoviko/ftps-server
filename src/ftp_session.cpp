#include "ftp_session.h"
#include "ftp_command.h"
#include "ftp_user_database.h"
#include "file_system.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <limits.h>

#include <iostream>
#include <fstream>
#include <algorithm>
#include <thread>


const int ftp_session::MAX_AUTH_ATTEMPTS = 3;
int ftp_session::DATA_PORT_ALLOCATION = 40150;

//state_machine<ftp_session_state_t, ftp_command_t, std::function<void (void)> > ftp_session::m_ftp_stm;

ftp_session::ftp_session(tcp_transport & client_control_channel, ftp_notifier * notifier) :
        m_transfer_type(ftp_transfer_t::TYPE_TRANSFER_BINARY),
        m_state(ftp_session_state_t::FTP_SESSION_AUTH_WAIT_LOGIN),
        m_notifier(notifier),
        m_control_channel(std::move(client_control_channel)),
        m_auth_attempt(0)
{
//    m_ftp_stm.add_transition(ftp_session_state_t::FTP_SESSION_AUTH_WAIT_LOGIN, ftp_command_t::FTP_COMMAND_PWD,
//                             ftp_session_state_t::FTP_SESSION_AUTH_WAIT_PASSWORD, ftp_session_state_t::FTP_SESSION_AUTH_WAIT_LOGIN,
//                             std::bind(&ftp_session::command_feat, this));

    //m_ftp_stm.add_transition(ftp_session_state_t::FTP_SESSION_AUTH_WAIT_PASSWORD, ftp_command_t::)
}


ftp_session::ftp_session(ftp_session && another_session) {
    /* TODO: only pointers can be used by server, otherwise we have got troubles */
    m_user = std::move(another_session.m_user);
    m_folder = std::move(another_session.m_folder);
}


ftp_session::~ftp_session(void) { }


void ftp_session::start(void) {
    std::thread client_handler(&ftp_session::client_session_thread, this);
    client_handler.detach();
}


void ftp_session::stop(void) { }


void ftp_session::client_session_thread(void) {
    ssize_t message_length = 0;
    char client_message[2000];

    std::string server_message = "220 FTP server is ready\r\n";
    m_control_channel.push(server_message.c_str(), server_message.length());

    std::thread::id tid = std::this_thread::get_id();

    while( (m_state != ftp_session_state_t::FTP_SESSION_TERMINATED) &&
           ((message_length = m_control_channel.pull(2000, client_message)) > 0) ) {

        std::cout << "FTP session '" << tid << "': receive command " << std::string(client_message, message_length) << "" << std::endl;

        /* parse client message */
        std::string client_message_string(client_message);
        ftp_command command = ftp_command(client_message_string);

        process_command(command);
    }

    if (message_length == 0) {
        std::cout << "FTP session: client disconnected - server client thread is terminated." << std::endl;
    }
    else if (message_length == (ssize_t) -1) {
        std::cout << "FTP session: receive failure - server client thread is terminated." << std::endl;
    }

    m_control_channel.close();

    m_state = ftp_session_state_t::FTP_SESSION_TERMINATED;

    std::cout << "FTP session: client session thread '" << tid << "' is terminated." << std::endl;
}


void ftp_session::process_command(const ftp_command & input_command) {
    std::string server_message = "";

    switch(input_command.get_command()) {
        case ftp_command_t::FTP_COMMAND_USER: {
            command_user(input_command);
            break;
        }
        case ftp_command_t::FTP_COMMAND_PASS: {
            command_pass(input_command);
            break;
        }
        case ftp_command_t::FTP_COMMAND_SYST: {
            command_syst(input_command);
            break;
        }
        case ftp_command_t::FTP_COMMAND_PWD: {
            command_pwd(input_command);
            break;
        }
        case ftp_command_t::FTP_COMMAND_CWD: {
            command_cwd(input_command);
            break;
        }
        case ftp_command_t::FTP_COMMAND_PORT: {
            command_port(input_command);
            break;
        }
        case ftp_command_t::FTP_COMMAND_LIST: {
            command_list(input_command);
            break;
        }
        case ftp_command_t::FTP_COMMAND_STOR: {
            command_stor(input_command);
            break;
        }
        case ftp_command_t::FTP_COMMAND_TYPE: {
            command_type(input_command);
            break;
        }
        case ftp_command_t::FTP_COMMAND_FEAT: {
            command_feat(input_command);
            break;
        }
        case ftp_command_t::FTP_COMMAND_AUTH: {
            command_auth(input_command);
            break;
        }
        case ftp_command_t::FTP_COMMAND_PBSZ: {
            command_pbsz(input_command);
            break;
        }
        case ftp_command_t::FTP_COMMAND_PROT: {
            command_prot(input_command);
            break;
        }
        case ftp_command_t::FTP_COMMAND_PASV: {
            command_pasv(input_command);
            break;
        }
        case ftp_command_t::FTP_COMMAND_ALLO: {
            command_allo(input_command);
            break;
        }
        case ftp_command_t::FTP_COMMAND_DELE: {
            command_dele(input_command);
            break;
        }
        case ftp_command_t::FTP_COMMAND_RNFR: {
            command_rnfr(input_command);
            break;
        }
        case ftp_command_t::FTP_COMMAND_RNTO: {
            command_rnto(input_command);
            break;
        }
        case ftp_command_t::FTP_COMMAND_QUIT: {
            command_quit(input_command);
            break;
        }
        case ftp_command_t::FTP_COMMAND_INCOMPLETE: {
            server_message = "550 command is incomplete\r\n";
            break;
        }
        case ftp_command_t::FTP_COMMAND_SITE_UTIME: {
            server_message = "202 command is not supported\r\n";
            break;
        }
        default: {
            server_message = "500 command is unknown\r\n";
            break;
        }
    }

    if (!server_message.empty()) {
        m_control_channel.push(server_message.c_str(), server_message.length());
    }
}


void ftp_session::command_user(const ftp_command & input_command) {
    std::string login = input_command.get_argument(0);

    if (m_state != ftp_session_state_t::FTP_SESSION_AUTH_WAIT_LOGIN) {
        std::string server_message = "500 USER incorrect command order\r\n";
        m_control_channel.push(server_message.c_str(), server_message.length());
    }

    m_user.set_login(login);
    m_state = ftp_session_state_t::FTP_SESSION_AUTH_WAIT_PASSWORD;

    std::string server_message = "331 password is needed\r\n";
    m_control_channel.push(server_message.c_str(), server_message.length());
}


void ftp_session::command_pass(const ftp_command & input_command) {
    std::string password = input_command.get_argument(0);

    if (m_state != ftp_session_state_t::FTP_SESSION_AUTH_WAIT_PASSWORD) {
        std::string server_message = "530 PASS incorrect command order\r\n";
        m_control_channel.push(server_message.c_str(), server_message.length());
    }

    m_user.set_password(password);

    if (ftp_user_database::get(m_user.login(), m_user.password(), m_user) != OPERATION_SUCCESS) {
        std::string server_message = "430 authentication failure\r\n";
        m_control_channel.push(server_message.c_str(), server_message.length());

        m_auth_attempt++;
        m_state = ftp_session_state_t::FTP_SESSION_AUTH_WAIT_LOGIN;

        if (m_auth_attempt > ftp_session::MAX_AUTH_ATTEMPTS) {
            m_state = ftp_session_state_t::FTP_SESSION_TERMINATED;
        }
    }
    else {
        m_folder = m_user.folder();

        std::string server_message = "230 access is performed\r\n";
        m_control_channel.push(server_message.c_str(), server_message.length());
    }
}


void ftp_session::command_syst(const ftp_command & input_command) {
    (void) input_command;
    /* TODO: check state */
    std::string server_message = "215 UNIX Type: L8\r\n";
    m_control_channel.push(server_message.c_str(), server_message.length());
}


void ftp_session::command_pwd(const ftp_command & input_command) {
    (void) input_command;
    /* TODO: check state */
    std::string server_message = "257 " + m_folder + "\r\n";
    m_control_channel.push(server_message.c_str(), server_message.length());
}


void ftp_session::command_port(const ftp_command & input_command) {
    /* TODO: check state */
    std::string server_message = "220 PORT command is accepted\r\n";

    std::string address = input_command.get_argument(0);
    int port = std::stoi(input_command.get_argument(1));

    m_data_channel = std::move(tcp_client());
    if (m_data_channel.connect(address, port) != OPERATION_SUCCESS) {
        server_message = "520 PORT cannot create data channel\r\n";
    }
    else {
        std::cout << "FTP server: data channel is connected to " << address << ":" << port << std::endl;

        m_control_channel.push(server_message.c_str(), server_message.length());
    }

    m_control_channel.push(server_message.c_str(), server_message.length());
}


void ftp_session::command_list(const ftp_command & input_command) {
    (void) input_command;

    /* TODO: check state */
    std::vector<std::string> list_files;
    directory::get_files(m_folder, list_files);

    std::string server_data_message = "";
    for (std::string file: list_files) {
    	server_data_message.append(file + "\n");
    }

    std::cout << "FTP server: requested list of file of directory " << m_folder << ":" << std::endl;
    std::cout << server_data_message << std::endl;

    std::string server_control_message = "150 ASCII data connection\r\n";
    m_control_channel.push(server_control_message.c_str(), server_control_message.length());

    m_data_channel.push(server_data_message.c_str(), server_data_message.length());

    server_control_message = "226 ASCII Transfer complete\r\n";
    m_control_channel.push(server_control_message.c_str(), server_control_message.length());

    m_data_channel.close();
}


void ftp_session::command_cwd(const ftp_command & input_command) {
    std::string server_message = "250 CWD command is successful\r\n";

    std::string folder = input_command.get_argument(0);

    std::vector<std::string> list_files;
    std::string path_folder;

    if (path::is_path_rooted(folder)) {
        /* full path is specified */
        path_folder = folder;
    }
    else if (path::is_path_homed(folder)) {
        /* full path is specified but with home sign */
        path_folder = m_user.folder() + folder.substr(2);
    }
    else {
        /* relative path is specified */
        path_folder = m_folder + folder;
    }

    std::string canonical_full_path;
    path::get_canonical_full_path(path_folder, canonical_full_path);

    if (!directory::exists(canonical_full_path)) {
        std::cout << "FTP server: requested directory '" << path_folder << "' is not found." << std::endl;
        server_message = "553 CWD folder is not found\r\n";
    }
    else {
        m_folder = canonical_full_path;

        std::cout << "FTP server: current directory for user: " << m_folder << std::endl;
    }

    m_control_channel.push(server_message.c_str(), server_message.length());
}


void ftp_session::command_stor(const ftp_command & input_command) {
    /* TODO: check state */
    std::string file_name = input_command.get_argument(0);

    std::ofstream::openmode transfer_mode = std::ofstream::out | std::ofstream::app;
    if (m_transfer_type == ftp_transfer_t::TYPE_TRANSFER_BINARY) {
        transfer_mode |= std::ofstream::binary;
    }

    std::string file_path = m_folder + file_name;
    std::cout << "FTP server: store file at the following path: " << file_path << std::endl;
    std::ofstream file_transfer(file_path, transfer_mode);

    char receive_buffer[2000];
    ssize_t message_length = 0;

    std::string server_control_message = "150 STOR file status is OK\r\n";
    m_control_channel.push(server_control_message.c_str(), server_control_message.length());

    if (m_buffer_file_size > 0) {
        /* if we have been asked to allocate place for file than we need to stop receiving by ourself when file is received */
        ssize_t file_total_size = m_buffer_file_size;
        while( ( (message_length = m_data_channel.pull(2000, receive_buffer)) > 0 ) && (file_total_size != 0) ) {
            file_transfer.write(receive_buffer, message_length);

            file_total_size -= message_length;
        }
    }
    else {
        /* if we haven't been asked to allocate place for file than we need just to wait when socket will be closed by client */
        while( ( message_length = m_data_channel.pull(2000, receive_buffer)) > 0 ) {
            file_transfer.write(receive_buffer, message_length);
        }
    }

    file_transfer.close();
    m_data_channel.close();

    server_control_message = "226 ASCII Transfer complete\r\n";
    m_control_channel.push(server_control_message.c_str(), server_control_message.length());

    m_notifier->notify_observers(ftp_observe_event_t::FTP_EVENT_FILE_PUT, file_path);
}


void ftp_session::command_type(const ftp_command & input_command) {
    std::string type = input_command.get_argument(0);

    if (type == "I") {
        m_transfer_type = ftp_transfer_t::TYPE_TRANSFER_BINARY;
    }
    else if (type == "A") {
        m_transfer_type = ftp_transfer_t::TYPE_TRANSFER_ASCII;
    }
    else {
        std::string server_message = "500 TYPE specified type is not supported\r\n";
        m_control_channel.push(server_message.c_str(), server_message.length());

        return;
    }

    std::string server_message = "250 TYPE command is successful\r\n";
    m_control_channel.push(server_message.c_str(), server_message.length());
}


void ftp_session::command_feat(const ftp_command & input_command) {
    (void) input_command;

    std::string server_message = "211 FEAT Extensions supported:\n AUTH TLS\n PBSZ\n PROT\n211 END\r\n";
    m_control_channel.push(server_message.c_str(), server_message.length());
}


void ftp_session::command_auth(const ftp_command & input_command) {
    std::string service = input_command.get_argument(0);

    if (service != "TLS") {
        std::string server_message = "500 AUTH specified service is not supported\r\n";
        m_control_channel.push(server_message.c_str(), server_message.length());

        return;
    }

    std::cout << "AUTH TLS is handled" << std::endl;

    std::string server_message = "234 AUTH TLS successful\r\n";
    m_control_channel.push(server_message.c_str(), server_message.length());

    m_control_channel.set_tls();
}


void ftp_session::command_pbsz(const ftp_command & input_command) {
    m_protection_buffer_size = std::stoi(input_command.get_argument(0));

    std::string server_message = "200 PBSZ command is successful\r\n";
    m_control_channel.push(server_message.c_str(), server_message.length());
}


void ftp_session::command_prot(const ftp_command & input_command) {
    std::string level_protection = input_command.get_argument(0);

    if (level_protection == "C") {
        m_protection_level = ftp_protection_t::FTP_PROTECTION_CLEAR;
    }
    else if (level_protection == "S") {
        m_protection_level = ftp_protection_t::FTP_PROTECTION_SAFE;
    }
    else if (level_protection == "E") {
        m_protection_level = ftp_protection_t::FTP_PROTECTION_CONFIDENTIAL;
    }
    else if (level_protection == "P") {
        m_protection_level = ftp_protection_t::FTP_PROTECTION_PRIVATE;
    }

    std::string server_message = "200 PROT command is successful\r\n";
    m_control_channel.push(server_message.c_str(), server_message.length());
}


void ftp_session::command_pasv(const ftp_command & input_command) {
    (void) input_command;

    std::string address = m_control_channel.address();
    int port = DATA_PORT_ALLOCATION++;

    tcp_listener listener;
    bool allocation_flag = false;
    for (int attempt = 0; attempt < 20; attempt++) {
        listener = tcp_listener(address, port, 1);
        if (listener.bind() != OPERATION_SUCCESS) {
            std::cout << "FTP server: attempt to get port " << port << " is failed, try to take next" << std::endl;
            port = DATA_PORT_ALLOCATION++;
        }
        else {
            allocation_flag = true;
            break;
        }
    }

    if (!allocation_flag) {
        std::string server_message = "500 PASV command if failure";
        m_control_channel.push(server_message.c_str(), server_message.length());
    }
    else {
        int high_port_byte = (port & 0xFF00) >> 8;
        int low_port_byte = (port & 0x00FF);

        std::string formater_address = address;
        std::replace(formater_address.begin(), formater_address.end(), '.', ',');

        std::string server_message = "227 Entering Passive Mode (" + formater_address + "," + std::to_string(high_port_byte) + "," + std::to_string(low_port_byte) + ")\r\n";
        std::cout << server_message << std::endl;

        m_control_channel.push(server_message.c_str(), server_message.length());

        std::cout << "FTP server: passive mode data channel is allocated." << std::endl;

        listener.accept(m_data_channel);

        std::cout << "FTP server: passive mode incoming data connection." << std::endl;

        if ( (m_control_channel.secure()) && (m_protection_level == ftp_protection_t::FTP_PROTECTION_PRIVATE) ) {
            /* create TLS session for data channel */
            m_data_channel.set_tls();
        }

        listener.close();
    }
}


void ftp_session::command_allo(const ftp_command & input_command) {
    m_buffer_file_size = std::stoi(input_command.get_argument(0));

    std::string server_message = "200 ALLO command is successful\r\n";
    m_control_channel.push(server_message.c_str(), server_message.length());
}


void ftp_session::command_dele(const ftp_command & input_command) {
    std::string path = input_command.get_argument(0);
    std::string server_message = "250 DELE command is successful\r\n";

    std::string file_path;
    if (!path::is_path_rooted(path)) {
        file_path = m_folder + path;
    }
    else {
        path::get_canonical_full_path(path, file_path);
    }

    if (file::erase(file_path) != OPERATION_SUCCESS) {
        server_message = "250 DELE command is failure\r\n";
    }

    m_control_channel.push(server_message.c_str(), server_message.length());
}


void ftp_session::command_rnfr(const ftp_command & input_command) {
    std::string input_old_path = input_command.get_argument(0);
    if (!path::is_path_rooted(input_old_path)) {
        input_old_path = m_folder + input_old_path;
    }

    std::string canonical_old_path;
    path::get_canonical_full_path(input_old_path, canonical_old_path);

    std::string server_message;
    if (!file::exists(canonical_old_path) && !directory::exists(canonical_old_path)) {
        server_message = "500 RNRF command is failure (specified file or directory does not exists)\r\n";
    }
    else {
        server_message = "350 Requested file action pending further information\r\n";
    }

    m_control_channel.push(server_message.c_str(), server_message.length());
    m_argument = canonical_old_path;
}


void ftp_session::command_rnto(const ftp_command & input_command) {
    std::string input_new_path = input_command.get_argument(0);
    if (!path::is_path_rooted(input_new_path)) {
        input_new_path = m_folder + input_new_path;
    }

    std::string server_message = "250 RNTO command is successful\r\n";
    if (file::exists(input_new_path) || directory::exists(input_new_path)) {
        server_message = "500 RNTO command is failure (specified file or directory already exists)\r\n";
    }
    else {
        if (path::move(m_argument, input_new_path) != OPERATION_SUCCESS) {
            server_message = "500 RNTO command is failure (system error occurs)\r\n";
        }
    }

    m_control_channel.push(server_message.c_str(), server_message.length());
}


void ftp_session::command_quit(const ftp_command & input_command) {
    (void) input_command;
    m_state = ftp_session_state_t::FTP_SESSION_TERMINATED;
}
