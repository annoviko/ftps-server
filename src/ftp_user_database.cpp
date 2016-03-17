#include "ftp_user_database.h"

#include <algorithm>
#include <iostream>


const std::vector<ftp_user>	ftp_user_database::FTP_USER_DATABASE = {
		ftp_user("admin", "admin", "/"),
		ftp_user("annoviko", "12345", "/home/")
};


int ftp_user_database::get_user(const std::string & login, const std::string & password, ftp_user & user) {
    std::cout << "FTP server: search for " << login << ", password: " << password << std::endl;

    for (const ftp_user & candidate : FTP_USER_DATABASE) {
        if ( (candidate.login() == login) && (candidate.password() == password) ) {
            user = candidate;
            return OPERATION_SUCCESS;
        }
    }

    return OPERATION_FAILURE;
}


