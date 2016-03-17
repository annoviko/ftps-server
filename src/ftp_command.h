#ifndef FTP_COMMAND_H_
#define FTP_COMMAND_H_


#include <unordered_map>
#include <vector>


enum class ftp_command_t {
	FTP_COMMAND_USER,
	FTP_COMMAND_PASS,
	FTP_COMMAND_SYST,
	FTP_COMMAND_PWD,
	FTP_COMMAND_CWD,
	FTP_COMMAND_PORT,
	FTP_COMMAND_LIST,
	FTP_COMMAND_STOR,
	FTP_COMMAND_TYPE,
	FTP_COMMAND_QUIT,
	FTP_COMMAND_FEAT,
	FTP_COMMAND_AUTH,
	FTP_COMMAND_PBSZ,
	FTP_COMMAND_PROT,
	FTP_COMMAND_PASV,
	FTP_COMMAND_ALLO,
	FTP_COMMAND_DELE,
	FTP_COMMAND_RNFR,
	FTP_COMMAND_RNTO,
	FTP_COMMAND_SITE_UTIME,
	FTP_COMMAND_UNKNOWN,
	FTP_COMMAND_INCOMPLETE,
};


class ftp_command {
private:
    typedef std::unordered_map<std::string, ftp_command_t>                  ftp_cmd_map_t;
    typedef std::unordered_map<std::string, ftp_command_t>::iterator        ftp_cmd_iter_t;
    typedef std::unordered_map<std::string, ftp_command_t>::const_iterator  ftp_cmd_const_iter_t;

private:
	const static ftp_cmd_map_t  FTP_MAP_COMMAND;


private:
	ftp_command_t	            m_command;

	std::vector<std::string>	m_arguments;


public:
	ftp_command(const std::string & command_line);


public:
	inline const ftp_command_t & get_command(void) const { return m_command; }

	inline const std::string & get_argument(void) const { return m_arguments.at(0); }

	inline const std::string & get_argument(const size_t index) const { return m_arguments.at(index); }
};

#endif
