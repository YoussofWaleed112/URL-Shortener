#include <iostream>
#include <string>
#include <limits>
#include "header_files/UrlShortener.h"
#include "header_files/validator.h"


void printMenu() {
    std::cout << "  1. Shorten a URL\n";
    std::cout << "  2. Resolve a short URL\n";
    std::cout << "  3. Exit\n\n";
    std::cout << "  Choice: ";
}

int main() {
    UrlShortener shortener("data/mappings.txt");
    std::cout << " URL Shortener Service \n";

    while (true) {
        printMenu();

        int choice;
        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cout << "\n  Error: please enter 1, 2, or 3.\n\n";
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if (choice == 3) {
            std::cout << "\n  Goodbye.\n\n";
            break;
        }

        if (choice == 1) {
            std::cout << "\n  Enter long URL: ";
            std::string url;
            std::getline(std::cin, url);
            

            if (!Validator::isValidUrl(url)) {
                std::cout << "\n  Error: invalid URL.\n";
                std::cout << "  URL must start with http:// or https://, contain a dot in the\n";
                std::cout << "  domain, and have no spaces.\n\n";
                continue;
            }

            try {
                std::string shortUrl = shortener.shorten(url);
                std::cout << "\n  Short URL: " << shortUrl << "\n\n";
            } catch (const std::exception& e) {
                std::cout << "\n  Error: " << e.what() << "\n\n";
            }

        } else if (choice == 2) {
            std::cout << "\n  Enter short URL: ";
            std::string shortUrl;
            std::getline(std::cin, shortUrl);

            try {
                std::string longUrl = shortener.resolve(shortUrl);
                std::cout << "\n  Original URL: " << longUrl << "\n\n";
            } catch (const std::exception& e) {
                std::cout << "\n  Error: " << e.what() << "\n\n";
            }

        } else {
            std::cout << "\n  Error: please enter 1, 2, or 3.\n\n";
        }
    }

    return 0;
}
