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

	void info(const std::string& content);

	void error(const std::string& content);

	void warning(const std::string& content);

private:
    GELog() = default;
    GELog(const GELog&) = delete;
    GELog& operator=(const GELog&) = delete;
};