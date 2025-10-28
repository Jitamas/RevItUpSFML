// Developers: Simon Mekhail,
// Record: 10/26 7:44PM - Need to add mouse hover color




#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <optional>
#include <filesystem>
using namespace sf;

enum class programState
{
    MainMenu,
    Inventory,
    Scheduling,
    AI_Assistant,
    Parts,
    Settings,
};


class Box
{
public:
    // Constructor: set everything once
    RectangleShape shape;
    //Class for a box shape(square/rectangle)
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

//  For -> MainMenu Background Image
//  (Texture) loads to GPU memory ||
Texture mainMenuBackground(std::filesystem::path("../../assets/RIUtestpic1.jpg"),true);
//  (Sprite) Creates a drawable object that references the texture
Sprite mainMenuBackgroundSprite(mainMenuBackground);

// For Inventory Background Image
Texture inventorybackground(std::filesystem::path("../../assets/inventorybackground.png"),true);
Sprite inventorybackgroundSprite(inventorybackground);

// For Scheduling Background Image
Texture schedulingbackground(std::filesystem::path("../../assets/schedulingbackground.jpg"),true);
Sprite schedulingbackgroundSprite(schedulingbackground);

// For Parts Background Image
Texture partsbackground(std::filesystem::path("../../assets/partsbackground.jpg"),true);
Sprite partsbackgroundSprite(partsbackground);

// For AI Background Image
Texture aibackground(std::filesystem::path("../../assets/aibackground.jpg"),true);
Sprite aibackgroundSprite(aibackground);

// For Settings Background Image
Texture settingsbackground(std::filesystem::path("../../assets/settingsbackground.jpg"),true);
Sprite settingsbackgroundSprite(settingsbackground);


// Font
Font font(std::filesystem::path("../../assets/AppleGaramond.ttf"));


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

    // run the program as long as the window is open
    while (window.isOpen())
    {
        // check all the window's events that were triggered since the last iteration of the loop
        while (const std::optional event = window.pollEvent())
        {
            // "close requested" event: we close the window
            if (event->is<Event::Closed>())
                window.close();
        }


        // Inventory Button (Main Menu)
        RectangleShape inventoryButton(Vector2f(200,50));
        inventoryButton.setPosition(Vector2f(250.f,362.f));
        inventoryButton.setFillColor(Color::White);
        inventoryButton.setOutlineColor(Color::Black);
        inventoryButton.setOutlineThickness(2);
        //Creating Text for Inventory Button
        Text inventoryButtonText(font, "Inventory",30);
        inventoryButtonText.setFillColor(Color::Black);
        inventoryButtonText.setPosition({272.f, 362.f });

        // Inventory BACK Button (Inventory Menu)
        RectangleShape inventoryBack(Vector2f(100,50));
        inventoryBack.setPosition(Vector2f(150.f,900.f));
        inventoryBack.setFillColor(Color::White);
        inventoryBack.setOutlineColor(Color::Black);
        inventoryBack.setOutlineThickness(2);
        //Creating Text for Inventory BACK Button (Inventory Menu)
        Text inventoryBackText(font, "Back",30);
        inventoryBackText.setFillColor(Color::Black);
        inventoryBackText.setPosition({172.f, 900.f });

        // Scheduling Button (Main Menu)
        RectangleShape schedulingButton(Vector2f(200,50));
        schedulingButton.setPosition(Vector2f(250.f,430.f));
        schedulingButton.setFillColor(Color::White);
        schedulingButton.setOutlineColor(Color::Black);
        schedulingButton.setOutlineThickness(2);
        //Creating Text for Scheduling Button
        Text schedulingButtonText(font, "Scheduling",30);
        schedulingButtonText.setFillColor(Color::Black);
        schedulingButtonText.setPosition({272.f, 430.f });

        // Scheduling BACK Button (Scheduling Menu)
        RectangleShape schedulingBack(Vector2f(100,50));
        schedulingBack.setPosition(Vector2f(150.f,900.f));
        schedulingBack.setFillColor(Color::White);
        schedulingBack.setOutlineColor(Color::Black);
        schedulingBack.setOutlineThickness(2);
        //Creating Text for Scheduling Button
        Text schedulingBackText(font, "Back",30);
        schedulingBackText.setFillColor(Color::Black);
        schedulingBackText.setPosition({172.f, 900.f });

        // Parts Button (Main Menu)
        RectangleShape partsButton(Vector2f(200,50));
        partsButton.setPosition(Vector2f(250.f,500.f));
        partsButton.setFillColor(Color::White);
        partsButton.setOutlineColor(Color::Black);
        partsButton.setOutlineThickness(2);
        //Creating Text for Parts Button
        Text partsButtonText(font, "Parts",30);
        partsButtonText.setFillColor(Color::Black);
        partsButtonText.setPosition({272.f, 500.f });

        // Parts BACK Button (Parts Menu)
        RectangleShape partsBack(Vector2f(100,50));
        partsBack.setPosition(Vector2f(150.f,900.f));
        partsBack.setFillColor(Color::White);
        partsBack.setOutlineColor(Color::Black);
        partsBack.setOutlineThickness(2);
        //Creating Text for Parts Button (parts menu)
        Text partsBackText(font, "Back",30);
        partsBackText.setFillColor(Color::Black);
        partsBackText.setPosition({172.f, 900.f });

        // AI Assistant Button
        RectangleShape aiButton(Vector2f(200,50));
        aiButton.setPosition(Vector2f(250.f,570.f));
        aiButton.setFillColor(Color::White);
        aiButton.setOutlineColor(Color::Black);
        aiButton.setOutlineThickness(2);
        // Creating Text for AI Assistant Button
        Text aiButtonText(font, "AI Assistant",30);
        aiButtonText.setFillColor(Color::Black);
        aiButtonText.setPosition({272.f, 570.f });

        // AI BACK Button
        RectangleShape aiBack(Vector2f(100,50));
        aiBack.setPosition(Vector2f(150.f,900.f));
        aiBack.setFillColor(Color::White);
        aiBack.setOutlineColor(Color::Black);
        aiBack.setOutlineThickness(2);
        //Creating Text for AI BACK Button (AI Menu)
        Text aiBackText(font, "Back",30);
        aiBackText.setFillColor(Color::Black);
        aiBackText.setPosition({172.f, 900.f });

        // Settings Button
        RectangleShape settingsButton(Vector2f(200,50));
        settingsButton.setPosition(Vector2f(250.f,640.f));
        settingsButton.setFillColor(Color::White);
        settingsButton.setOutlineColor(Color::Black);
        settingsButton.setOutlineThickness(2);
        //Creating Text for Settings Button
        Text settingsButtonText(font, "Settings",30);
        settingsButtonText.setFillColor(Color::Black);
        settingsButtonText.setPosition({272.f, 640.f });

        // Settings BACK Button (Settings Menu)
        RectangleShape settingsBack(Vector2f(100,50));
        settingsBack.setPosition(Vector2f(150.f,900.f));
        settingsBack.setFillColor(Color::White);
        settingsBack.setOutlineColor(Color::Black);
        settingsBack.setOutlineThickness(2);
        //Creating Text for Settings BACK Button (Settings Menu)
        Text settingsBackText(font, "Back",30);
        settingsBackText.setFillColor(Color::Black);
        settingsBackText.setPosition({172.f, 900.f });

        // Exit Button
        RectangleShape exitButton(Vector2f(200,50));
        exitButton.setPosition(Vector2f(250.f,710.f));
        exitButton.setFillColor(Color::White);
        exitButton.setOutlineColor(Color::Black);
        exitButton.setOutlineThickness(2);
        //Creating Text for Exit Button
        Text exitButtonText(font, "Exit",30);
        exitButtonText.setFillColor(Color::Black);
        exitButtonText.setPosition({272.f, 710.f });


        //If the left mouse button is pressed
        if (isButtonPressed(Mouse::Button::Left))
        {
            if (isMouseOver(inventoryButton, window))
                state = programState::Inventory;
            else if (isMouseOver(schedulingButton, window))
                state = programState::Scheduling;
            else if (isMouseOver(partsButton, window))
                state = programState::Parts;
            else if (isMouseOver(aiButton, window))
                state = programState::AI_Assistant;
            else if (isMouseOver(settingsButton, window))
                state = programState::Settings;
            else if (isMouseOver(exitButton, window))
                window.close();
            else if (isMouseOver(inventoryBack, window))
                state = programState::MainMenu;
            else if (isMouseOver(schedulingBack, window))
                state = programState::MainMenu;
            else if (isMouseOver(partsBack, window))
                state = programState::MainMenu;
            else if (isMouseOver(settingsBack,window))
                state = programState::MainMenu;
        }

        window.clear();

        switch (state)
        {
        case programState::MainMenu:
            mainMenuBackgroundSprite.setScale({3.0f,3.0f});
            window.draw(mainMenuBackgroundSprite);
            window.draw(inventoryButton);
            window.draw(inventoryButtonText);
            window.draw(schedulingButton);
            window.draw(schedulingButtonText);
            window.draw(partsButton);
            window.draw(partsButtonText);
            window.draw(aiButton);
            window.draw(aiButtonText);
            window.draw(settingsButton);
            window.draw(settingsButtonText);
            window.draw(exitButton);
            window.draw(exitButtonText);
            break;
        case programState::Inventory:
            window.draw(inventorybackgroundSprite);
            window.draw(inventoryBack);
            window.draw(inventoryBackText);
            break;
        case programState::Scheduling:
            window.draw(schedulingbackgroundSprite);
            window.draw(schedulingBack);
            window.draw(schedulingBackText);
            break;
        case programState::Parts:
            window.draw(partsbackgroundSprite);
            window.draw(partsBack);
            window.draw(partsBackText);
            break;
        case programState::AI_Assistant:
            window.draw(aibackgroundSprite);
            window.draw(aiBack);
            window.draw(aiBackText);
            break;
        case programState::Settings:
            window.draw(settingsbackgroundSprite);
            window.draw(settingsBack);
            window.draw(settingsBackText);
            break;
        }
        window.display();
    }
}
