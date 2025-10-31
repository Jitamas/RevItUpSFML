// Developers: Simon and Simon
// Record: 10/26/25 2:50 am - Failing to load font




#include <SFML/Window.hpp>
#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <optional>
#include <filesystem>
#include <regex>
#include <algorithm>
// HTTP + JSON for search
#include <curl/curl.h>
#include <nlohmann/json.hpp>
using namespace sf;

enum class programState
{
    MainMenu,
    Settings,
    Inventory,
    Scheduling,
    Parts,
};

class Box
{
public:
    RectangleShape shape;
    // Constructor: set everything once
    Box(float xCoord, float yCoord, float width, float height, Color color = Color::White)
    {
        shape.setSize({width, height});
        shape.setPosition({xCoord, yCoord});
        color.a = 200;
        shape.setFillColor(color);
        shape.setOutlineColor(Color::Black);
        shape.setOutlineThickness(2.f);
    }
};

// Assets (textures + font) are loaded at runtime inside main() so failures can be
// handled gracefully instead of throwing during static initialization.
// Load font from project assets folder
Font font(std::filesystem::path("../assets/AppleGaramond.ttf"));

using json = nlohmann::json;

struct SearchResult {
    std::string title;
    std::string link;
    std::string snippet;
    std::string price;  // New field for price information
};

// Last search error text (used for UI feedback)
static std::string lastSearchError;

// Function to extract price from text
std::string extractPrice(const std::string& text) {
    std::regex price_patterns[] = {
        std::regex(R"(\$\s*[\d,]+\.?\d*)"),           // $ 123.45 or $1,234
        std::regex(R"(USD\s*[\d,]+\.?\d*)"),          // USD 123.45
        std::regex(R"([\d,]+\.?\d*\s*USD)"),          // 123.45 USD
        std::regex(R"(Price:\s*\$?[\d,]+\.?\d*)"),    // Price: $123.45
        std::regex(R"(Cost:\s*\$?[\d,]+\.?\d*)"),     // Cost: $123.45
        std::regex(R"(\$[\d,]+\.?\d*\s*each)"),       // $123.45 each
        std::regex(R"(\$[\d,]+\.?\d*\s*ea)"),         // $123.45 ea
        std::regex(R"([\d,]+\.?\d*\s*dollars?)"),     // 123.45 dollars
        std::regex(R"(\b[\d,]+\.?\d*\$)"),            // 123.45$
        std::regex(R"(\$[\d,]+)"),                    // $123 (no cents)
    };
    
    std::smatch match;
    for (const auto& pattern : price_patterns) {
        if (std::regex_search(text, match, pattern)) {
            std::string price_str = match.str();
            // Clean up the price string
            if (price_str.find("$") == std::string::npos && price_str.find("USD") == std::string::npos) {
                price_str = "$" + price_str;
            }
            return price_str;
        }
    }
    return "";
}

// Function to check if text is related to car parts
bool isCarPartsRelated(const std::string& text) {
    std::vector<std::string> car_keywords = {
        "automotive", "auto", "car", "vehicle", "engine", "brake", "tire", "wheel",
        "transmission", "battery", "oil", "filter", "spark plug", "alternator",
        "radiator", "suspension", "exhaust", "clutch", "belt", "hose", "gasket",
        "headlight", "taillight", "bumper", "fender", "mirror", "windshield",
        "parts", "replacement", "oem", "aftermarket"
    };
    
    std::string lower_text = text;
    std::transform(lower_text.begin(), lower_text.end(), lower_text.begin(), ::tolower);
    
    for (const auto& keyword : car_keywords) {
        if (lower_text.find(keyword) != std::string::npos) {
            return true;
        }
    }
    return false;
}

// Function to check if this looks like a product listing rather than a general website
bool isProductListing(const std::string& title, const std::string& snippet) {
    std::string combined = title + " " + snippet;
    std::transform(combined.begin(), combined.end(), combined.begin(), ::tolower);
    
    // Look for product indicators
    std::vector<std::string> product_indicators = {
        "part number", "part #", "sku", "model", "brand", "oem", "aftermarket",
        "fits", "compatible", "replacement", "genuine", "premium", "quality",
        "warranty", "manufactured", "specification", "dimensions"
    };
    
    // Look for store/retail indicators
    std::vector<std::string> store_indicators = {
        "autozone", "advance auto", "o'reilly", "rockauto", "partsgeek",
        "buy", "shop", "store", "retailer", "dealer"
    };
    
    bool has_product_indicator = false;
    bool has_store_indicator = false;
    
    for (const auto& indicator : product_indicators) {
        if (combined.find(indicator) != std::string::npos) {
            has_product_indicator = true;
            break;
        }
    }
    
    for (const auto& indicator : store_indicators) {
        if (combined.find(indicator) != std::string::npos) {
            has_store_indicator = true;
            break;
        }
    }
    
    return has_product_indicator || has_store_indicator;
}

static size_t WriteCallbackMain(void* contents, size_t size, size_t nmemb, void* userp)
{
    size_t total = size * nmemb;
    std::string* s = static_cast<std::string*>(userp);
    s->append(static_cast<char*>(contents), total);
    return total;
}

std::vector<SearchResult> perform_search(const std::string& query)
{
    std::vector<SearchResult> results;
    const char* api_key = std::getenv("GOOGLE_API_KEY");
    const char* cx = std::getenv("GOOGLE_CX");
    if (!api_key || !cx) {
        lastSearchError = "Missing GOOGLE_API_KEY or GOOGLE_CX environment variables. Please set them up first.";
        return results;
    }

    CURL* curl = curl_easy_init();
    if (!curl) return results;

    std::string response;
    
    // Enhanced search query targeting product listings from major retailers
    std::string enhanced_query = query + " site:autozone.com OR site:rockauto.com OR site:advanceautoparts.com OR site:oreillyauto.com OR site:partsgeek.com price";
    
    char* esc = curl_easy_escape(curl, enhanced_query.c_str(), static_cast<int>(enhanced_query.size()));
    std::string url = "https://www.googleapis.com/customsearch/v1?key=" + std::string(api_key) + "&cx=" + std::string(cx) + "&q=" + (esc ? esc : "");
    if (esc) curl_free(esc);

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallbackMain);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "revitup-sfml/1.0");

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        lastSearchError = std::string("HTTP request failed: ") + curl_easy_strerror(res);
        curl_easy_cleanup(curl);
        return results;
    }
    curl_easy_cleanup(curl);

    try {
        auto j = json::parse(response);
        if (j.contains("items") && j["items"].is_array()) {
            for (const auto& item : j["items"]) {
                std::string title = item.value("title", "(no title)");
                std::string snippet = item.value("snippet", "");
                std::string link = item.value("link", "");
                
                // Filter: only include results that are car parts related AND look like product listings
                if (isCarPartsRelated(title + " " + snippet) && isProductListing(title, snippet)) {
                    SearchResult r;
                    r.title = title;
                    r.link = link;
                    r.snippet = snippet;
                    
                    // Extract price information from title and snippet
                    std::string price = extractPrice(title + " " + snippet);
                    if (price.empty()) {
                        // Try to extract from meta description or other fields if available
                        if (item.contains("htmlSnippet")) {
                            price = extractPrice(item["htmlSnippet"].get<std::string>());
                        }
                    }
                    r.price = price.empty() ? "Contact for price" : price;
                    
                    results.push_back(r);
                }
            }
            if (results.empty())
                lastSearchError = "No product listings found. Try searching for specific part numbers or brands.";
            else
                lastSearchError.clear();
        }
        else if (j.contains("error")) {
            lastSearchError = j["error"].value("message", "Search API error");
        }
        else {
            lastSearchError = "No product listings found.";
        }
    } catch (const std::exception& ex) {
        lastSearchError = std::string("Failed to parse search response: ") + ex.what();
    }
    return results;
}




//Function to CHECK if mouse is over buttons
bool isMouseOver(RectangleShape& button, RenderWindow& window)
{
    Vector2i mousePosition = Mouse::getPosition(window);
    return button.getGlobalBounds().contains(static_cast<Vector2f>(mousePosition));
}

//Function to SHOW if mouse is hovering a button
void mouseHoverColor(RectangleShape& button, RenderWindow& window)
{
    if (isMouseOver(button, window))
    {
        button.setFillColor(Color(128,128,128));
    }
    else
    {
        button.setFillColor(Color::White);
    }
}








int main()
{
    RenderWindow window(VideoMode({1920, 1080}), "My window");
    window.setVerticalSyncEnabled(true); // call it once after creating the window

    programState state = programState::MainMenu;

    // Load assets (textures + font) here so we can handle failures gracefully
    Texture mainMenuBackground;
    Sprite mainMenuBackgroundSprite(mainMenuBackground);
    Texture settingsbackground;
    Sprite settingsbackgroundSprite(settingsbackground);
    Texture schedulingbackground;
    Sprite schedulingbackgroundSprite(schedulingbackground);
    Font font;
    bool assetsOk = true;
    if (!mainMenuBackground.loadFromFile("../assets/RIUtestpic1.jpg")) {
        std::cerr << "Failed to load ../assets/RIUtestpic1.jpg\n";
        assetsOk = false;
    } else {
        mainMenuBackground.setSmooth(true);
        mainMenuBackgroundSprite.setTexture(mainMenuBackground);
    }
    if (!settingsbackground.loadFromFile("../assets/settingsbackground.jpg")) {
        std::cerr << "Failed to load ../assets/settingsbackground.jpg\n";
        assetsOk = false;
    } else {
        settingsbackgroundSprite.setTexture(settingsbackground);
    }
    if (!schedulingbackground.loadFromFile("../assets/schedulingbackground.jpg")) {
        std::cerr << "Failed to load ../assets/schedulingbackground.jpg\n";
        assetsOk = false;
    } else {
        schedulingbackgroundSprite.setTexture(schedulingbackground);
    }
    if (!font.openFromFile("../assets/AppleGaramond.ttf")) {
        std::cerr << "Failed to load ../assets/AppleGaramond.ttf\n";
        assetsOk = false;
    }

    // Search UI state for Parts page
    std::string partsSearchQuery;
    bool partsSearchFocused = false;
    std::vector<SearchResult> partsSearchResults;
    int partsResultOffset = 0; // for scrolling results
    bool partsSearchPerformed = false;

    // run the program as long as the window is open
    while (window.isOpen())
    {
        // Layout constants for Parts page (keep in sync with drawing)
        constexpr float partsBgX = 200.f;
        constexpr float partsBgY = 90.f;
        constexpr float partsBgW = 1600.f;
        constexpr float partsBgH = 900.f;
        constexpr float searchBoxX = 240.f;
        constexpr float searchBoxY = 120.f;
        constexpr float searchBoxW = 800.f;
        constexpr float searchBoxH = 40.f;
        constexpr float backButtonW = 80.f;
        constexpr float backButtonH = 30.f;
        constexpr float backButtonMargin = 20.f; // margin from right edge of partsBg

        // check all the window's events that were triggered since the last iteration of the loop
        while (const std::optional event = window.pollEvent())
        {
            // "close requested" event: we close the window
            if (event->is<Event::Closed>())
                window.close();

            // Text input for search when focused
            if (event->is<Event::TextEntered>())
            {
                if (state == programState::Parts && partsSearchFocused)
                {
                    const auto* te = event->getIf<Event::TextEntered>();
                    if (te)
                    {
                        uint32_t code = te->unicode;
                        if (code >= 32 && code < 127)
                        {
                            partsSearchQuery.push_back(static_cast<char>(code));
                        }
                    }
                }
            }

            // Key presses (Enter, Backspace, arrows)
            if (event->is<Event::KeyPressed>())
            {
                const auto* kp = event->getIf<Event::KeyPressed>();
                if (kp && state == programState::Parts)
                {
                    using Key = Keyboard::Key;
                    if (kp->code == Key::Enter)
                    {
                        // perform search
                        partsSearchResults = perform_search(partsSearchQuery);
                        partsSearchPerformed = true;
                        partsResultOffset = 0;
                    }
                    else if (kp->code == Key::Backspace)
                    {
                        if (!partsSearchQuery.empty()) partsSearchQuery.pop_back();
                    }
                    else if (kp->code == Key::Escape)
                    {
                        // go back to main menu
                        state = programState::MainMenu;
                    }
                    else if (kp->code == Key::Up)
                    {
                        if (partsResultOffset > 0) partsResultOffset -= 1;
                    }
                    else if (kp->code == Key::Down)
                    {
                        partsResultOffset += 1;
                    }
                }
            }

            // Mouse click to focus search box
            if (event->is<Event::MouseButtonPressed>())
            {
                const auto* mb = event->getIf<Event::MouseButtonPressed>();
                if (mb && mb->button == Mouse::Button::Left && state == programState::Parts)
                {
                    // Prefer the event-provided position (mb->position)
                    Vector2i mp = mb->position;
                    // Back button area: top-right of parts panel
                    float backX1 = partsBgX + partsBgW - backButtonMargin - backButtonW; // left
                    float backX2 = backX1 + backButtonW; // right
                    float backY1 = partsBgY + 10.f; // roughly where we draw it (100)
                    float backY2 = backY1 + backButtonH;
                    if (mp.x >= static_cast<int>(backX1) && mp.x <= static_cast<int>(backX2) && mp.y >= static_cast<int>(backY1) && mp.y <= static_cast<int>(backY2))
                    {
                        state = programState::MainMenu;
                        continue;
                    }

                    // search box area
                    float sx1 = searchBoxX;
                    float sx2 = searchBoxX + searchBoxW;
                    float sy1 = searchBoxY;
                    float sy2 = searchBoxY + searchBoxH;
                    if (mp.x >= static_cast<int>(sx1) && mp.x <= static_cast<int>(sx2) && mp.y >= static_cast<int>(sy1) && mp.y <= static_cast<int>(sy2))
                    {
                        partsSearchFocused = true;
                    }
                    else
                    {
                        partsSearchFocused = false;
                    }
                }
            }
        }


        //  Button 1
        RectangleShape button1(Vector2f(200,50));
        button1.setPosition(Vector2f(250.f,362.f));
        button1.setFillColor(Color::White);
        button1.setOutlineColor(Color::Black);
        button1.setOutlineThickness(2);
        //Creating Text for Button 1
        Text button1Text(font, "Inventory",30);
        button1Text.setFillColor(Color::Black);
        button1Text.setPosition({272.f, 362.f });

        // Button 2
        RectangleShape button2(Vector2f(200,50));
        button2.setPosition(Vector2f(250.f,430.f));
        button2.setFillColor(Color::White);
        button2.setOutlineColor(Color::Black);
        button2.setOutlineThickness(2);
        //Creating Text for Button 2
        Text button2Text(font, "Scheduling",30);
        button2Text.setFillColor(Color::Black);
        button2Text.setPosition({272.f, 430.f });

        // Button 3
        RectangleShape button3(Vector2f(200,50));
        button3.setPosition(Vector2f(250.f,500.f));
        button3.setFillColor(Color::White);
        button3.setOutlineColor(Color::Black);
        button3.setOutlineThickness(2);
        //Creating Text for Button 3
        Text button3Text(font, "Parts",30);
        button3Text.setFillColor(Color::Black);
        button3Text.setPosition({272.f, 500.f });

        // Button 4
        RectangleShape button4(Vector2f(200,50));
        button4.setPosition(Vector2f(250.f,570.f));
        button4.setFillColor(Color::White);
        button4.setOutlineColor(Color::Black);
        button4.setOutlineThickness(2);
        //Creating Text for Button 4
        Text button4Text(font, "Settings",30);
        button4Text.setFillColor(Color::Black);
        button4Text.setPosition({272.f, 570.f });

        // Button 5
        RectangleShape button5(Vector2f(200,50));
        button5.setPosition(Vector2f(250.f,570.f));
        button5.setFillColor(Color::White);
        button5.setOutlineColor(Color::Black);
        button5.setOutlineThickness(2);
        //Creating Text for Button 5
        Text button5Text(font, "Exit",30);
        button5Text.setFillColor(Color::Black);
        button5Text.setPosition({272.f, 570.f });


        //If the left mouse button is pressed
        if (isButtonPressed(Mouse::Button::Left))
        {
            if (isMouseOver(button1, window))
            {
                state = programState::Inventory;
            }
        }
        if (isButtonPressed(Mouse::Button::Left))
        {
            if (isMouseOver(button2, window))
            {
                state = programState::Scheduling;
            }
        }
        if (isButtonPressed(Mouse::Button::Left))
        {
            if (isMouseOver(button3, window))
            {
                state = programState::Parts;
            }
        }
        if (isButtonPressed(Mouse::Button::Left))
        {
            if (isMouseOver(button4, window))
            {
                state = programState::Settings;
            }
        }
        if (isButtonPressed(Mouse::Button::Left))
        {
            if (isMouseOver(button5, window))
            {
                window.close();
            }
        }

        switch (state)
        {
        case programState::MainMenu:
            mainMenuBackgroundSprite.setScale({3.0f,3.0f});
            window.draw(mainMenuBackgroundSprite);
            window.draw(button1);
            window.draw(button1Text);
            window.draw(button2);
            window.draw(button2Text);
            window.draw(button3);
            window.draw(button3Text);
            window.draw(button4);
            window.draw(button4Text);
            window.draw(button5);
            window.draw(button5Text);
            break;
        case programState::Settings:
            window.draw(settingsbackgroundSprite);
                break;
        case programState::Scheduling:
            window.draw(schedulingbackgroundSprite);
                break;
        case programState::Inventory:
            //draw inventory stuff here
                break;
        case programState::Parts:
            // Draw parts UI: search box + results
            {
                // background (solid)
                RectangleShape partsBg(Vector2f(1600.f, 900.f));
                partsBg.setPosition(Vector2f{200.f, 90.f});
                partsBg.setFillColor(Color(220, 220, 220));
                window.draw(partsBg);

                // Back button (top-right of parts panel)
                RectangleShape backButton(Vector2f(backButtonW, backButtonH));
                // compute position from partsBg constants
                float backX = partsBgX + partsBgW - backButtonMargin - backButtonW;
                float backY = partsBgY + 10.f;
                backButton.setPosition(Vector2f{backX, backY});
                backButton.setFillColor(Color::White);
                backButton.setOutlineColor(Color::Black);
                backButton.setOutlineThickness(2.f);
                window.draw(backButton);
                Text backText(font, "Back", 16);
                backText.setFillColor(Color::Black);
                backText.setPosition(Vector2f{backX + 12.f, backY + 4.f});
                window.draw(backText);

                // Search box
                RectangleShape searchBox(Vector2f(searchBoxW, searchBoxH));
                searchBox.setPosition(Vector2f{searchBoxX, searchBoxY});
                searchBox.setFillColor(Color::White);
                searchBox.setOutlineColor(partsSearchFocused ? Color::Blue : Color::Black);
                searchBox.setOutlineThickness(partsSearchFocused ? 3.f : 2.f);
                window.draw(searchBox);

                Text searchText(font, partsSearchQuery.empty() ? "Search: brake pads, oil filter, AC Delco..." : partsSearchQuery, 20);
                searchText.setFillColor(partsSearchQuery.empty() ? Color(128,128,128) : Color::Black);
                searchText.setPosition(Vector2f{250.f, 125.f});
                window.draw(searchText);

                // Results list
                float ry = 180.f;
                int idx = 0;
                if (partsSearchPerformed && partsSearchResults.empty())
                {
                    std::string msg = lastSearchError.empty() ? "No results." : lastSearchError;
                    Text info(font, msg, 16);
                    info.setFillColor(lastSearchError.empty() ? Color(100,100,100) : Color::Red);
                    info.setPosition(Vector2f{250.f, ry});
                    window.draw(info);
                }
                else
                {
                    for (const auto& r : partsSearchResults)
                    {
                        if (idx++ < partsResultOffset) continue;
                        Text title(font, r.title, 18);
                        title.setFillColor(Color::Black);
                        title.setPosition(Vector2f{250.f, ry});
                        window.draw(title);

                        Text link(font, r.link, 14);
                        link.setFillColor(Color(0,0,180));
                        link.setPosition(Vector2f{250.f, ry + 22.f});
                        window.draw(link);

                        // Display price information
                        Text price(font, "Price: " + r.price, 16);
                        price.setFillColor(Color(0,128,0)); // Green color for price
                        price.setPosition(Vector2f{250.f, ry + 40.f});
                        window.draw(price);

                        ry += 80.f; // Increased spacing to accommodate price
                        if (ry > 900.f) break;
                    }
                }
            }
            break;
        }

        window.display();
    }
}