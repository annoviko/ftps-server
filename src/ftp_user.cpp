#include "ftp_user.h"


ftp_user::ftp_user(void) :
    m_login(""),
    m_password(""),
    m_folder("")
{ }


ftp_user::ftp_user(const std::string & login, const std::string & password, const std::string & folder) :
    m_login(login),
    m_password(password),
    m_folder(folder)
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
