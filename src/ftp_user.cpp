#include "ftp_user.h"


ftp_user::ftp_user(void) :
    m_login(""),
    m_password(""),
    m_folder("")
{ }


ftp_user::ftp_user(const std::string & user_login, const std::string & user_password, const std::string & user_folder) :
    m_login(user_login),
    m_password(user_password),
    m_folder(user_folder)
{ }


ftp_user::ftp_user(const ftp_user & another_user) :
    m_login(another_user.m_login),
    m_password(another_user.m_password),
    m_folder(another_user.m_folder)
{ }


ftp_user::ftp_user(ftp_user && another_user) {
    m_login = std::move(another_user.m_login);
    m_password = std::move(another_user.m_password);
    m_folder = std::move(another_user.m_folder);
}


ftp_user & ftp_user::operator=(const ftp_user & other) {
    if (this == &other) {
        return *this;
    }

    m_login = other.m_login;
    m_password = other.m_password;
    m_folder = other.m_folder;

    return *this;
}


bool ftp_user::operator==(const ftp_user & other) const {
	return ( (m_login == other.m_login) &&
			 (m_password == other.m_password) );
}
