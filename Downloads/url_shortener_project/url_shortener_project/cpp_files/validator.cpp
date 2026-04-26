#include "validator.h"
#include <algorithm>

namespace Validator {

    bool isValidUrl(const std::string& url) {
        // Must start with http:// or https://
        bool hasScheme = (url.substr(0, 7) == "http://" || url.substr(0, 8) == "https://");
        if (!hasScheme) return false;

        // Must not contain whitespace
        for (char c : url) {
            if (c == ' ' || c == '\t' || c == '\n') return false;
        }

        // Extract host and verify it has a dot
        std::string host = extractHost(url);
        if (host.empty()) return false;

        size_t dot = host.find('.');
        if (dot == std::string::npos) return false;
        if (dot == 0 || dot == host.size() - 1) return false;  // dot at start or end

        return true;
    }



    std::string extractHost(const std::string& url) {
        size_t schemeEnd = url.find("://");
        if (schemeEnd == std::string::npos) return "";

        size_t hostStart = schemeEnd + 3;
        size_t hostEnd = url.find('/', hostStart);

        if (hostEnd == std::string::npos)
            return url.substr(hostStart);

        return url.substr(hostStart, hostEnd - hostStart);
    }

    std::string extractPath(const std::string& url) {
        size_t schemeEnd = url.find("://");
        if (schemeEnd == std::string::npos) return "";

        size_t hostStart = schemeEnd + 3;
        size_t pathStart = url.find('/', hostStart);

        if (pathStart == std::string::npos) return "";
        return url.substr(pathStart);
    }
}
