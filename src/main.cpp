// Developers: Simon and Simon
// Record: 10/26/25 2:50 am - Failing to load font




#include <SFML/Window.hpp>
#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <optional>
#include <filesystem>
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

Texture settingsbackground(std::filesystem::path("../../assets/settingsbackground.jpg"),true);
Sprite settingsbackgroundSprite(settingsbackground);

Texture schedulingbackground(std::filesystem::path("../../assets/schedulingbackground.jpg"),true);
Sprite schedulingbackgroundSprite(schedulingbackground);




// Font test
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
            //draw parts stuff here
                break;
        }

        window.display();
    }
}