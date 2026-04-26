#pragma once
#include <string>

namespace Validator {
    bool isValidUrl(const std::string& url);
    std::string extractHost(const std::string& url);    // "github.com"
    std::string extractPath(const std::string& url);    // "/user/project/issues/154"
}
