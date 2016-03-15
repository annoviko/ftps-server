#ifndef FTP_USER_DATABASE_H_
#define FTP_USER_DATABASE_H_

#include <vector>

#include "ftp_user.h"


class ftp_user_database {
private:
	static const std::vector<ftp_user>	FTP_USER_DATABASE;

public:
	static const ftp_user & get_user(const std::string & login, const std::string & password);
};


#endif
