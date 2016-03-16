#ifndef FILE_SYSTEM_H_
#define FILE_SYSTEM_H_

#include <string>
#include <vector>


#define OPERATION_SUCCESS           0
#define OPERATION_FAILURE           -1


enum class file_t {
	FILE_REGULAR,
	FILE_DIRECTORY,
	FILE_ANY,
};


class path {
public:
	static int get_canonical_full_path(const std::string & path, std::string & canonical_path);

	static int get_file_name(const std::string & path, std::string & file_name);

	static bool is_path_rooted(const std::string & path);

	static int move(const std::string & old_path, const std::string & new_path);
};


class file {
public:
    static bool exists(const std::string & path);

    static int erase(const std::string & path);

    static int copy(const std::string & path_file1, const std::string & path_file2);

    static int move(const std::string & path_file1, const std::string & path_file2);
};


class directory {
public:
	static int get_files(const std::string & path, std::vector<std::string> & files);

	static int get_files(const std::string & path, const std::string & pattern, std::vector<std::string> & files);

	static int get_directories(const std::string & path, std::vector<std::string> & directories);

	static int get_directories(const std::string & path, const std::string & pattern, std::vector<std::string> & files);

	static bool exists(const std::string & path);

	static int move(const std::string & path_directory1, const std::string & path_directory2);

private:
	static int get_specify_files(const std::string & path, const std::string & pattern, const file_t file_type, std::vector<std::string> & files);

	static bool is_file_type(const std::string & file_name, const file_t file_type);

	static bool is_file_pattern(const std::string & file_name, const std::string & pattern);
};


#endif
