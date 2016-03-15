#include "ftp_command.h"

#include <sstream>
#include <iostream>


const std::unordered_map<std::string, ftp_command_t> ftp_command::FTP_MAP_COMMAND = {
		{ "USER",       ftp_command_t::FTP_COMMAND_USER         },
		{ "PASS",       ftp_command_t::FTP_COMMAND_PASS         },
		{ "SYST",       ftp_command_t::FTP_COMMAND_SYST         },
		{ "PWD",        ftp_command_t::FTP_COMMAND_PWD          },
		{ "CWD",        ftp_command_t::FTP_COMMAND_CWD          },
		{ "PORT",       ftp_command_t::FTP_COMMAND_PORT         },
		{ "STOR",       ftp_command_t::FTP_COMMAND_STOR         },
		{ "TYPE",       ftp_command_t::FTP_COMMAND_TYPE         },
		{ "QUIT",       ftp_command_t::FTP_COMMAND_QUIT         },
		{ "LIST",       ftp_command_t::FTP_COMMAND_LIST         },
		{ "FEAT",       ftp_command_t::FTP_COMMAND_FEAT         },
		{ "AUTH",       ftp_command_t::FTP_COMMAND_AUTH         },
		{ "PBSZ",       ftp_command_t::FTP_COMMAND_PBSZ         },
		{ "PROT",       ftp_command_t::FTP_COMMAND_PROT         },
		{ "PASV",       ftp_command_t::FTP_COMMAND_PASV         },
		{ "ALLO",       ftp_command_t::FTP_COMMAND_ALLO         },
		{ "DELE",       ftp_command_t::FTP_COMMAND_DELE         },
		{ "RNFR",       ftp_command_t::FTP_COMMAND_RNFR         },
		{ "RNTO",       ftp_command_t::FTP_COMMAND_RNTO         },
		{ "SITE UTIME", ftp_command_t::FTP_COMMAND_SITE_UTIME   },
};


ftp_command::ftp_command(const std::string & command_line) {
	std::istringstream stream_parser(command_line);

	std::string ascii_command;
	stream_parser >> ascii_command;

	try {
		m_command = FTP_MAP_COMMAND.at(ascii_command);
	}
	catch(std::out_of_range & exception) {
		std::cout << "ftp_command: command " << ascii_command << " is unknown." << std::endl;
		m_command = ftp_command_t::FTP_COMMAND_UNKNOWN;
	}

	switch(m_command) {
        case ftp_command_t::FTP_COMMAND_USER:
        case ftp_command_t::FTP_COMMAND_PASS:
        case ftp_command_t::FTP_COMMAND_TYPE:
        case ftp_command_t::FTP_COMMAND_AUTH:
        case ftp_command_t::FTP_COMMAND_PBSZ:
        case ftp_command_t::FTP_COMMAND_PROT:
        case ftp_command_t::FTP_COMMAND_ALLO:
        case ftp_command_t::FTP_COMMAND_DELE:
        case ftp_command_t::FTP_COMMAND_RNFR:
        case ftp_command_t::FTP_COMMAND_RNTO: {
            std::string argument;
            stream_parser >> argument;

            m_arguments.push_back(argument);

            break;
        }

        case ftp_command_t::FTP_COMMAND_SITE_UTIME: {
            std::string argument;
            stream_parser >> argument;  /* time - ignore it */

            stream_parser >> argument;  /* name of file that should be transfered */
            m_arguments.push_back(argument);

            break;
        }

        case ftp_command_t::FTP_COMMAND_STOR: {
        	/* TODO: several files */
        	std::string argument;
        	stream_parser >> argument;

        	m_arguments.push_back(argument);

        	break;
        }

        case ftp_command_t::FTP_COMMAND_CWD: {
        	/* TODO: name of folder can be with spaces */
        	std::string argument;
        	stream_parser >> argument;

        	m_arguments.push_back(argument);

        	break;
        }

        case ftp_command_t::FTP_COMMAND_PORT: {
            std::string argument, address, port, token;

            stream_parser >> argument;
            stream_parser.str(argument);

            /* extract IP address */
            for (int i = 0; i < 4; i++) {
                if (std::getline(stream_parser, token, ',')) {
                    if (address.empty()) {
                        address.assign(token);
                    }
                    else {
                        address.append("." + token);
                    }
                }
                else {
                    m_command = ftp_command_t::FTP_COMMAND_INCOMPLETE;
                    return;
                }
            }

            std::cout << "FTP server: PORT command parsing: address: '" << address << "'" << std::endl;

            /* extract port number */
            if (std::getline(stream_parser, token, ',')) {
                int port_number = std::stoi(token);

                stream_parser >> token;
                port_number = (port_number << 8) | std::stoi(token);

                port = std::to_string(port_number);
            }
            else {
                m_command = ftp_command_t::FTP_COMMAND_INCOMPLETE;
                return;
            }

            std::cout << "FTP server: PORT command parsing: port: " << port << std::endl;

            m_arguments.push_back(address);
            m_arguments.push_back(port);

            break;
        }

        default:
            break;
	}
}
