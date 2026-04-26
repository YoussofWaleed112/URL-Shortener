#pragma once
#include <string>
#include <map>

class UrlShortener {
private:
    std::map<std::string, std::string> longToShort;
    std::map<std::string, std::string> shortToLong;
    std::string storageFile;

    // Short code generation internals
    std::string generateShortCode(const std::string& longUrl);
    std::string abbreviateDomain(const std::string& host);
    std::string extractPathSlug(const std::string& path);
    std::string removeVowels(const std::string& s);
    std::string resolveCollision(const std::string& candidate, const std::string& longUrl);

public:
    explicit UrlShortener(const std::string& storageFile = "data/mappings.txt");

    // Main operations
    std::string shorten(const std::string& longUrl);
    std::string resolve(const std::string& shortUrl);

    // Save to file
    void loadFromFile();
    void saveToFile() const;
};
