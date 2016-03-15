#ifndef FTP_USER_H_
#define FTP_USER_H_


#include <string>


class ftp_user {
private:
	std::string	m_login;
	std::string	m_password;
	std::string	m_folder;

public:
	ftp_user(void);

	ftp_user(const ftp_user & another_user);

	ftp_user(ftp_user && another_user);

	ftp_user(const std::string & login, const std::string & password, const std::string & folder);

public:
	const std::string & login(void) const { return m_login; }

	const std::string & password(void) const { return m_password; }

	const std::string & folder(void) const { return m_folder; }

	const void set_login(const std::string & login) { m_login = login; }

	const void set_password(const std::string & password) { m_password = password; }

	const void set_folder(const std::string & folder) { m_folder = folder; }

public:
	ftp_user & operator=(const ftp_user & other);

	bool operator==(const ftp_user & other) const;
};

#endif
