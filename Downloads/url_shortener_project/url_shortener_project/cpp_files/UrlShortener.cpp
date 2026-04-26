#include "UrlShortener.h"
#include "validator.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <vector>
#include <stdexcept>

// Domain abbreviation table
static const std::map<std::string, std::string> DOMAIN_TABLE = {
    {"google.com",       "gogle"},
    {"meet.google.com",  "mt-gogle"},
    {"docs.google.com",  "docs-g"},
    {"drive.google.com", "drv-g"},
    {"mail.google.com",  "gmail"},
    {"calendar.google.com", "gcal"},
    {"github.com",       "gh"},
    {"youtube.com",      "yt"},
    {"twitter.com",      "twtr"},
    {"x.com",            "x"},
    {"linkedin.com",     "lnkd"},
    {"stackoverflow.com","stkov"},
    {"reddit.com",       "rddt"},
    {"wikipedia.org",    "wiki"},
    {"amazon.com",       "amzn"},
    {"facebook.com",     "fb"},
    {"instagram.com",    "ig"},
    {"medium.com",       "mdm"},
    {"notion.so",        "ntn"},
    {"figma.com",        "fgma"},
    {"slack.com",        "slck"},
    {"discord.com",      "dscrd"},
    {"twitch.tv",        "twtch"},
    {"spotify.com",      "sptfy"},
    {"netflix.com",      "ntflx"},
    {"airbnb.com",       "airb"},
    {"uber.com",         "uber"},
    {"lyft.com",         "lyft"},
    {"paypal.com",       "ppal"},
    {"dropbox.com",      "dbx"},
    {"salesforce.com",   "slsfc"},
    {"adobe.com",        "adbe"},
    {"canva.com",        "cnva"},
    {"zoom.us",          "zoom"},
    {"asana.com",        "asana"},
    {"trello.com",       "trello"},
    {"gitlab.com",       "gitlab"},
    {"bitbucket.org",    "bbkt"},
    {"heroku.com",       "hrku"},
    {"netlify.com",      "ntlfy"},
    {"vercel.com",       "vrcel"},
    {"codepen.io",       "cpn"},
    {"jsfiddle.net",     "jsfd"},
    {"hackernews.com",   "hn"},
    {"producthunt.com",  "phnt"},
    {"behance.net",      "bhnc"},
    {"dribbble.com",     "drbl"},
    {"medium.com",       "mdm"},
    {"quora.com",        "quora"},
    {"yahoo.com",        "yhoo"},
    {"bing.com",         "bing"},
    {"chatgpt.com",       "chatgpt"},
    {"openai.com",       "opnai"},
    {"deepmind.com",     "deepmnd"},
    {"anthropic.com",    "anthrp"},
    {"stability.ai",     "stblty"},
    {"midjourney.com",   "mdjrny"},
};


UrlShortener::UrlShortener(const std::string& file) : storageFile(file) {
    loadFromFile();
}

// Public: shorten 
std::string UrlShortener::shorten(const std::string& longUrl) {
    // Duplicate check — return existing short URL
    auto it = longToShort.find(longUrl);
    if (it != longToShort.end()) {
        return it->second;
    }

    std::string shortUrl = generateShortCode(longUrl);
    longToShort[longUrl]  = shortUrl;
    shortToLong[shortUrl] = longUrl;
    saveToFile();
    return shortUrl;
}

// ─── Public: resolve ─────────────────────────────────────────────────────────
std::string UrlShortener::resolve(const std::string& shortUrl) {
    auto it = shortToLong.find(shortUrl);
    if (it == shortToLong.end()) {
        throw std::runtime_error("Short URL not found: " + shortUrl);
    }
    return it->second;
}


std::string UrlShortener::generateShortCode(const std::string& longUrl) {
    std::string host = Validator::extractHost(longUrl);
    std::string path = Validator::extractPath(longUrl);

    std::string domainPart = abbreviateDomain(host);
    std::string slugPart   = extractPathSlug(path);

    std::string possibleShortUrl = "https://" + domainPart;
    if (!slugPart.empty()) {
        possibleShortUrl += "/" + slugPart;
    }

    return resolveCollision(possibleShortUrl, longUrl);
}


std::string UrlShortener::abbreviateDomain(const std::string& host) {
    // Remove www.
    std::string h = host;
    if (h.substr(0, 4) == "www.") {
        h = h.substr(4);
    }

    // Check known table (full host first, then base domain)
    auto it = DOMAIN_TABLE.find(h);
    if (it != DOMAIN_TABLE.end()) return it->second;

    // Try subdomain.domain
    size_t firstDot = h.find('.');
    if (firstDot != std::string::npos) {
        std::string baseDomain = h.substr(firstDot + 1);
        auto it2 = DOMAIN_TABLE.find(baseDomain);
        if (it2 != DOMAIN_TABLE.end()) {
            
            std::string sub = h.substr(0, firstDot).substr(0, 2);
            return sub + "-" + it2->second;
        }
    }

    // Unknown domain: remove vowels, truncate to 4 chars
    size_t lastDot = h.rfind('.');
    std::string name = (lastDot != std::string::npos) ? h.substr(0, lastDot) : h;

    // Remove hyphens and dots, lowercase
    std::string cleaned;
    for (char c : name) {
        if (c != '-' && c != '.') cleaned += std::tolower(c);
    }

    std::string noVowels = removeVowels(cleaned);
    if (noVowels.size() > 4) noVowels = noVowels.substr(0, 4);
    if (noVowels.empty()) noVowels = cleaned.substr(0, 4);  // fallback if all vowels

    return noVowels;
}


std::string UrlShortener::extractPathSlug(const std::string& path) {
    if (path.empty() || path == "/") return "";

    // Split path on '/'
    std::vector<std::string> segments;
    std::stringstream ss(path);
    std::string seg;
    while (std::getline(ss, seg, '/')) {
        // Split on '?' to remove query params from segments (e.g. /watch?v=abcxyz123 → "watch")
        size_t q = seg.find('?');
        if (q != std::string::npos) seg = seg.substr(0, q);
        if (!seg.empty()) segments.push_back(seg);
    }

    // Also check query string values (e.g. ?v=abcxyz123 → "abcxyz")
    size_t qPos = path.find('?');
    if (qPos != std::string::npos) {
        std::string query = path.substr(qPos + 1);
        std::stringstream qs(query);
        std::string param;
        std::string bestQValue;
        while (std::getline(qs, param, '&')) {
            size_t eq = param.find('=');
            if (eq != std::string::npos) {
                std::string val = param.substr(eq + 1);
                if (val.size() > bestQValue.size()) bestQValue = val;
            }
        }
        if (!bestQValue.empty()) {
            std::string slug;
            for (char c : bestQValue) {
                if (std::isalnum(c)) slug += std::tolower(c);
            }
            if (!slug.empty()) return slug.substr(0, 6);
        }
    }

    if (segments.empty()) return "";

    // Walk right to left: prefer pure numeric segments (IDs)
    for (int i = (int)segments.size() - 1; i >= 0; i--) {
        const std::string& s = segments[i];
        bool isNumeric = !s.empty() && std::all_of(s.begin(), s.end(), ::isdigit);
        if (isNumeric) {
            return s.substr(0, 6);  // cap at 6 digits
        }
    }

    // No numeric segment — take rightmost alphanumeric slug
    for (int i = (int)segments.size() - 1; i >= 0; i--) {
        const std::string& s = segments[i];
        std::string alnum;
        for (char c : s) {
            if (std::isalnum(c)) alnum += std::tolower(c);
        }
        if (alnum.size() >= 2) {
            return alnum.substr(0, 4);
        }
    }

    return "";
}

std::string UrlShortener::removeVowels(const std::string& s) {
    std::string result;
    const std::string vowels = "aeiouAEIOU";
    for (char c : s) {
        if (vowels.find(c) == std::string::npos) {
            result += c;
        }
    }
    return result;
}

std::string UrlShortener::resolveCollision(const std::string& possibleShortUrl, const std::string& longUrl) {
    // If possibleShortUrl is free, use it
    if (shortToLong.find(possibleShortUrl) == shortToLong.end()) {
        return possibleShortUrl;
    }
    // If it already maps to the same long URL, no collision
    if (shortToLong.at(possibleShortUrl) == longUrl) {
        return possibleShortUrl;
    }
    // Append incrementing suffix: /abc → /abc2, /abc3, etc.
    int suffix = 2;
    while (true) {
        std::string attempt = possibleShortUrl + std::to_string(suffix);
        if (shortToLong.find(attempt) == shortToLong.end()) {
            return attempt;
        }
        suffix++;
    }
}

void UrlShortener::loadFromFile() {
    std::ifstream file(storageFile);
    if (!file.is_open()) return;  // No file yet 

    std::string line;
    while (std::getline(file, line)) {
        size_t tab = line.find('\t');
        if (tab == std::string::npos) continue;

        std::string longUrl  = line.substr(0, tab);
        std::string shortUrl = line.substr(tab + 1);

        if (!longUrl.empty() && !shortUrl.empty()) {
            longToShort[longUrl]  = shortUrl;
            shortToLong[shortUrl] = longUrl;
        }
    }
}

void UrlShortener::saveToFile() const {
    
    std::ofstream file(storageFile);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot write to file: " + storageFile);
    }

    for (const auto& pair : longToShort) {
        file << pair.first << '\t' << pair.second << '\n';
    }
}
