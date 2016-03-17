#ifndef FTP_USER_DATABASE_H_
#define FTP_USER_DATABASE_H_

#include <vector>

#include "ftp_user.h"


#define OPERATION_SUCCESS       0
#define OPERATION_FAILURE       -1


class ftp_user_database {
private:
	static const std::vector<ftp_user>	FTP_USER_DATABASE;

public:
	static int get_user(const std::string & login, const std::string & password, ftp_user & user);
};


#endif
