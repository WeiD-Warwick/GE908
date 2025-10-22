//
// Created by W.D. on 18/10/25
//

#pragma once
#include <string>
#include "iostream"

class GELog {

public:
    static GELog& shared() {
        static GELog shared;
        return shared;
    }

    void info(const std::string& content) {
		std::cout << "\033[0m"
			<< "[INFO] "
			<< content
			<< std::endl;
	}

    void error(const std::string& content) {
		std::cout << "\033[31m"
			<< "[ERROR] "
			<< content
			<< std::endl;
	}

    void warning(const std::string& content) {
		std::cout << "\033[33m"
			<< "[WARNING] "
			<< content
			<< std::endl;
	}

private:
    GELog() = default;
    GELog(const GELog&) = delete;
    GELog& operator=(const GELog&) = delete;
};