// Simple Google Custom Search example using libcurl and nlohmann::json
// Usage: set environment variables GOOGLE_API_KEY and GOOGLE_CX (custom search engine id)
// Then run: search_tool "honda brake pads"

#include <curl/curl.h>
#include <nlohmann/json.hpp>

#include <cstdlib>
#include <iostream>
#include <string>
#include <sstream>

using json = nlohmann::json;

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp)
{
    size_t total = size * nmemb;
    std::string* s = static_cast<std::string*>(userp);
    s->append(static_cast<char*>(contents), total);
    return total;
}

int main(int argc, char** argv)
{
    const char* api_key = std::getenv("GOOGLE_API_KEY");
    const char* cx = std::getenv("GOOGLE_CX");

    if (!api_key || !cx)
    {
        std::cerr << "Environment variables GOOGLE_API_KEY and GOOGLE_CX must be set.\n";
        std::cerr << "Example (PowerShell): $env:GOOGLE_API_KEY='YOUR_KEY'; $env:GOOGLE_CX='YOUR_CX'\n";
        return 1;
    }

    std::string query = "honda brake pads";
    if (argc > 1)
    {
        std::ostringstream oss;
        for (int i = 1; i < argc; ++i)
        {
            if (i > 1) oss << ' ';
            oss << argv[i];
        }
        query = oss.str();
    }

    CURL* curl = curl_easy_init();
    if (!curl)
    {
        std::cerr << "Failed to initialize curl\n";
        return 1;
    }

    std::string response;

    // build URL-escaped query
    char* esc = curl_easy_escape(curl, query.c_str(), static_cast<int>(query.size()));
    std::string url = "https://www.googleapis.com/customsearch/v1?key=" + std::string(api_key) + "&cx=" + std::string(cx) + "&q=" + (esc ? esc : "");
    if (esc) curl_free(esc);

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "revitup-sfml/1.0");

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK)
    {
        std::cerr << "curl error: " << curl_easy_strerror(res) << "\n";
        curl_easy_cleanup(curl);
        return 1;
    }

    curl_easy_cleanup(curl);

    try
    {
        auto j = json::parse(response);
        if (j.contains("items") && j["items"].is_array())
        {
            std::cout << "Search results for: '" << query << "'\n\n";
            int idx = 1;
            for (const auto& item : j["items"])
            {
                std::string title = item.value("title", "(no title)");
                std::string link = item.value("link", "");
                std::string snippet = item.value("snippet", "");
                std::cout << idx++ << ") " << title << "\n";
                std::cout << "   " << link << "\n";
                if (!snippet.empty()) std::cout << "   " << snippet << "\n";
                std::cout << "\n";
            }
        }
        else
        {
            std::cerr << "No results found or API error. Raw response:\n" << response << "\n";
            return 1;
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "JSON parse error: " << e.what() << "\nResponse was:\n" << response << "\n";
        return 1;
    }

    return 0;
}
