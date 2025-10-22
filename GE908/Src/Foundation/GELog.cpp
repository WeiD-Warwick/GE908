#include "GELog.h"

void GELog::info(const std::string& content) {
	std::cout << "\033[0m"
			  << "[INFO] "
			  << content
			  << std::endl;
}

void GELog::error(const std::string& content) {
	std::cout << "\033[31m"
			  << "[ERROR] "
			  << content
			  << std::endl;
}

void GELog::warning(const std::string& content) {
	std::cout << "\033[33m"
			  << "[WARNING] "
			  << content
			  << std::endl;
}