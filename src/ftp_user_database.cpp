#include "ftp_user_database.h"

#include <algorithm>
#include <iostream>


const std::vector<ftp_user>	ftp_user_database::FTP_USER_DATABASE = {
		ftp_user("admin", "admin", "/"),
		ftp_user("annoviko", "12345", "/home/")
};


const ftp_user & ftp_user_database::get_user(const std::string & login, const std::string & password) {
    std::cout << "FTP server: search for " << login << ", password: " << password << std::endl;

    for (const ftp_user & user : FTP_USER_DATABASE) {
        if ( (user.login() == login) && (user.password() == password) ) {
            return user;
        }
    }

    throw std::bad_alloc();
}


