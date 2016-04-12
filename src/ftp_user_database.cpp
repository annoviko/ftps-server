#include "ftp_user_database.h"

#include <algorithm>
#include <iostream>


std::map<std::string, ftp_user> ftp_user_database::FTP_USER_DATABASE = {
    { "admin", ftp_user("admin", "admin", "/home/") },
    { "annoviko", ftp_user("annoviko", "12345", "/home/annoviko/") }
};


int ftp_user_database::get(const std::string & login, const std::string & password, ftp_user & user) {
    std::cout << "FTP server: search for " << login << ", password: " << password << std::endl;

    auto user_iterator = FTP_USER_DATABASE.find(user.login());
    if (user_iterator != FTP_USER_DATABASE.end()) {
        ftp_user & candidate = user_iterator->second;
        if (candidate.password() == password) {
            std::cout << "FTP server: user is found with login " << login << ", password: " << password << std::endl;

            user = candidate;
            return OPERATION_SUCCESS;
        }
    }

    return OPERATION_FAILURE;
}


int ftp_user_database::add(const ftp_user & user) {
    int operation_result = true;

    if (FTP_USER_DATABASE.find(user.login()) == FTP_USER_DATABASE.end()) {
        FTP_USER_DATABASE[user.login()] = user;

        std::cout << "FTP server: add user with login '" << user.login() << "' to FTP database" << std::endl;
    }
    else {
        std::cout << "FTP server: impossible to add user with login '" << user.login() << "' to FTP database" << std::endl;

        operation_result = false;
    }

    return operation_result;
}


int ftp_user_database::erase(const ftp_user & user) {
    int operation_result = true;

    auto user_iterator = FTP_USER_DATABASE.find(user.login());
    if (user_iterator != FTP_USER_DATABASE.end()) {
        FTP_USER_DATABASE.erase(user_iterator);

        std::cout << "FTP server: remove user with login '" << user.login() << "' from FTP database" << std::endl;
    }
    else {
        std::cout << "FTP server: impossible to remove user with login '" << user.login() << "' from FTP database (does not exist)" << std::endl;
    }

    return operation_result;
}


