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

	ftp_user(const std::string & user_login, const std::string & user_password, const std::string & user_folder);

public:
	inline const std::string & login(void) const { return m_login; }

	inline const std::string & password(void) const { return m_password; }

	inline const std::string & folder(void) const { return m_folder; }

	inline void set_login(const std::string & user_login) { m_login = user_login; }

	inline void set_password(const std::string & user_password) { m_password = user_password; }

	inline void set_folder(const std::string & user_folder) { m_folder = user_folder; }

public:
	ftp_user & operator=(const ftp_user & other);

	bool operator==(const ftp_user & other) const;
};

#endif
