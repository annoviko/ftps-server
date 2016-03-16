#include "file_system.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

#include <fstream>
#include <algorithm>


int path::get_canonical_full_path(const std::string & path, std::string & canonical_path) {
    int result = OPERATION_SUCCESS;

	char buffer_path_name[PATH_MAX + 1];
	char * pointer_buffer_path_name = realpath(path.c_str(), buffer_path_name);

	if (pointer_buffer_path_name) {
		canonical_path = std::string(buffer_path_name);

		/* if it is directory let's make sure that it is ended by separator */
		if (directory::exists(pointer_buffer_path_name)) {
			if (canonical_path.substr(canonical_path.length() - 1, canonical_path.length()) != "/") {
				canonical_path.append("/");
			}
		}
	}
	else {
	    canonical_path.clear();
		result = OPERATION_FAILURE;
	}

	return result;
}


int path::get_file_name(const std::string & path, std::string & file_name) {
    int result = OPERATION_SUCCESS;

    std::string canonical_path;
    path::get_canonical_full_path(path, canonical_path);

    std::string::size_type position = canonical_path.find_first_of("/");
    if (position != std::string::npos) {
        file_name = canonical_path.substr(position + 1);
    }
    else {
        file_name.clear();
        result = OPERATION_FAILURE;
    }

    return result;
}


bool path::is_path_rooted(const std::string & path) {
    if (path.substr(0, 1) == "/") {
        return true;
    }

    return false;
}


int path::move(const std::string & old_path, const std::string & new_path) {
    int result = OPERATION_SUCCESS;

    std::string canonical_path_file1;
    path::get_canonical_full_path(old_path, canonical_path_file1);

    if (rename(canonical_path_file1.c_str(), new_path.c_str()) != 0) {
        result = OPERATION_FAILURE;
    }

    return result;
}



bool file::exists(const std::string & path) {
    std::string canonical_path;

    if (path::get_canonical_full_path(path, canonical_path)) {
        struct stat file_info;

        if (lstat(canonical_path.c_str(), &file_info) >= 0) {
            if (S_ISREG(file_info.st_mode)) {
                return true;
            }
        }
    }

    return false;
}


int file::erase(const std::string & path) {
    int result = OPERATION_SUCCESS;

    std::string canonical_path;
    path::get_canonical_full_path(path, canonical_path);

    if (remove(canonical_path.c_str()) != 0) {
        result = OPERATION_FAILURE;
    }

    return result;
}


int file::copy(const std::string & path_file1, const std::string & path_file2) {
    int result = OPERATION_SUCCESS;

    std::string canonical_path_file1;
    std::string canonical_path_file2;

    path::get_canonical_full_path(path_file1, canonical_path_file1);
    path::get_canonical_full_path(path_file2, canonical_path_file2);

    if (file::exists(canonical_path_file1) && !file::exists(canonical_path_file2)) {
        result = OPERATION_FAILURE;
    }
    else if (canonical_path_file1 != canonical_path_file2) {
        std::ifstream source(canonical_path_file1, std::fstream::binary);
        std::ofstream destination(canonical_path_file2, std::fstream::binary);

        destination << source.rdbuf();

        source.close();
        destination.close();
    }

    return result;
}


int file::move(const std::string & path_file1, const std::string & path_file2) {
    int result = OPERATION_SUCCESS;

    if (file::exists(path_file1) && !file::exists(path_file2)) {
        result = OPERATION_FAILURE;
    }
    else {
        path::move(path_file1, path_file2);
    }

    return result;
}



int directory::get_files(const std::string & path, std::vector<std::string> & files) {
	get_specify_files(path, "", file_t::FILE_ANY, files);
}


int directory::get_files(const std::string & path, const std::string & pattern, std::vector<std::string> & files) {
	return get_specify_files(path, pattern, file_t::FILE_ANY, files);
}


int directory::get_directories(const std::string & path, std::vector<std::string> & directories) {
	return get_specify_files(path, "", file_t::FILE_DIRECTORY, directories);
}


int directory::get_directories(const std::string & path, const std::string & pattern, std::vector<std::string> & directories) {
	return get_specify_files(path, pattern, file_t::FILE_DIRECTORY, directories);
}


int directory::get_specify_files(const std::string & path, const std::string & pattern, const file_t file_type, std::vector<std::string> & files) {
    int operation_result = OPERATION_SUCCESS;

	DIR * directory_descr = NULL;
	struct dirent * entry = NULL;

	std::string canonical_file_path;
	path::get_canonical_full_path(path, canonical_file_path);

	directory_descr = opendir(canonical_file_path.c_str());
	if (directory_descr) {
        files.clear();

        while ( (entry = readdir(directory_descr)) != NULL ) {
            std::string file_name = std::string(entry->d_name);
            std::string full_file_path = canonical_file_path + file_name;

            bool type_permission = is_file_type(full_file_path, file_type);
            bool name_permission = pattern.empty() ? true : is_file_pattern(file_name, pattern);

            if (type_permission && name_permission) {
                files.push_back(file_name);
            }
        }

        std::sort(files.begin(), files.end());
        closedir(directory_descr);
	}
	else {
	    operation_result = OPERATION_FAILURE;
	}

	return operation_result;
}


bool directory::exists(const std::string & path) {
	char buffer_path_name[PATH_MAX + 1];
	char * canonical_file_path = realpath(path.c_str(), buffer_path_name);

	/* canonical file path exists, now check that it is directory */
	if (canonical_file_path) {
		return is_file_type(std::string(buffer_path_name), file_t::FILE_DIRECTORY);
	}

	return false;
}


bool directory::is_file_type(const std::string & file_name, const file_t file_type) {
    bool file_found_result = false;

	struct stat file_info;
	if (lstat(file_name.c_str(), &file_info) != 0) {
		return false;
	}

	switch(file_type) {
		case file_t::FILE_REGULAR: {
			if (S_ISREG(file_info.st_mode)) {
			    file_found_result = true;
			}

			break;
		}

		case file_t::FILE_DIRECTORY: {
			if (S_ISDIR(file_info.st_mode)) {
			    file_found_result = true;
			}

			break;
		}

		case file_t::FILE_ANY: {
		    file_found_result = true;
		    break;
		}

		default: {
		    file_found_result = false;
			break;
		}
	}

	return file_found_result;
}


bool directory::is_file_pattern(const std::string & file, const std::string & pattern) {
	if (pattern.length() > file.length()) {
		return false;
	}

	return (file.substr(0, pattern.length()) == pattern);
}


int directory::move(const std::string & path_directory1, const std::string & path_directory2) {
    int result = OPERATION_SUCCESS;

    if (file::exists(path_directory1) && !file::exists(path_directory2)) {
        result = OPERATION_FAILURE;
    }

    path::move(path_directory1, path_directory2);

    return result;
}

