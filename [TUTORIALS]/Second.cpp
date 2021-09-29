#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>

using namespace sf;

int dir; // direction of hoop
bool isShot = false; // boolean to check if the hoop is shot with button over the hoop drawing
int keytime = 1; int counter = 0; // set keytime to for max smoothness, ie, no time delay in input
    // what's a key-time? 
    //-> to limit the need to check for any updation after very small consider-able amount of frame to increase performance
float score = 0;

// FUNCTIONS
void Update(RenderWindow* window, CircleShape* hoop, CircleShape* ball, RectangleShape* rectange)
{
    /*/ rectangle movement using keyboard and mouse buttons
    if (counter < keytime)   counter++;
    else 
    {
        counter = 0; // resetting counter

        // changing postion of recatange
        // the additional && conditions are done so that the box does not leave the canvas region
        if (Keyboard::isKeyPressed(Keyboard::W) && rectangle.getPosition().y > 0)
            rectangle.move(0.f, -5.f);

        if (Keyboard::isKeyPressed(Keyboard::A) && rectangle.getPosition().x > 0)
            rectangle.move(-15.f, 0.f);
        
        if (Keyboard::isKeyPressed(Keyboard::D) && rectangle.getPosition().x + rectangle.getSize().x < window.getSize().x)
            rectangle.move(+15.f, 0.f);
        
        if (Keyboard::isKeyPressed(Keyboard::S) && rectangle.getPosition().y + rectangle.getSize().y < window.getSize().y)
            rectangle.move(0.f, +15.f);

        // changes color when button is pressed
        if (Mouse::isButtonPressed(Mouse::Left))
            rectangle.setFillColor(Color::Cyan);
        else
            rectangle.setFillColor(Color::Green);
    }*/


    // hoop (circle) loop movement
    if (hoop->getPosition().x <= 0)
        dir = 1;
    else if (hoop->getPosition().x + hoop->getRadius()*2 >= window->getSize().x)
        dir = 0;

    if (dir == 0)
        hoop->move(-5.f, 0.f);
    else
        hoop->move(+5.f, 0.f);

    
    // ball (circle) mouse tracking
    if (Mouse::isButtonPressed(Mouse::Left))
        isShot = true;

    if (!isShot)
        ball->setPosition(Mouse::getPosition(*window).x, ball->getPosition().y); // moving only in X direction
    //note: get position returns with respect to destop, here we specify to relative to be inside the window-screen
    //to know the exact difference, when we dont mention the window, the window dimentions are taken to be destop's
    //thus, the cursor needs to be at the left-top corner of destop to be at the same rest position in the window-screen
    else
        ball->move(0.f, -20.f); // movement of shooting: which needs to fast
        

    // updating scores
    if (ball->getGlobalBounds().intersects(hoop->getGlobalBounds()))  // checks if ball intersected with the hoop
    {
        score += 2;
        std::cout << "Nice Shot! :) \t";
    }
    else if (ball->getPosition().y < 0)   // checks if ball after shooting reached top of screen, ie, if the shoot didn't touch the hoop 
    {
        score -= 0.5;
        std::cout << "You Missed! :( \t";
    }

    // reseting and checking if the ball hit the hoop after shooting
    if ((ball->getPosition().y < 0) || (ball->getGlobalBounds().intersects(hoop->getGlobalBounds())))
    {
        isShot = false; // reseting the fired state of ball
        ball->setPosition(ball->getPosition().x, window->getSize().y - ball->getRadius() * 2 - 5); // setting back to the default Y position
        std::cout << "Score:" << score << "\n"; // priting score every time shot is being fired
    }
    
}

void Draw(RenderWindow* window, CircleShape* hoop, CircleShape* ball, RectangleShape* rectange)
{
    window->clear(Color::Transparent);

    // Draw Stuff here ---
    //window.draw(*rectangle);
    window->draw(*hoop);
    window->draw(*ball);

    window->display();
}

// NOTE: while running this fie, change main_x to main and in other files (of same dir) with main to main_x
int main_2()
{
    // GLOBAL VARS
    RenderWindow window(VideoMode(854, 480), "Small Shooter Game!", Style::Default);
    RectangleShape rectangle(Vector2f(50.f, 100.f)); // x and y sizes
    CircleShape hoop; 
    CircleShape ball; 

    // window
    window.setFramerateLimit(60);

    // rectangle
    rectangle.setPosition(Vector2f(window.getSize().x /2, window.getSize().y / 2)); // setting at centre of window
    rectangle.setFillColor(sf::Color::Green);

    // hoop
    hoop.setRadius(40.f);
    hoop.setFillColor(Color::White);
    hoop.setOutlineThickness(7);
    hoop.setOutlineColor(Color::Blue);
    hoop.setPosition(Vector2f(0.f, hoop.getOutlineThickness()));


    // ball
    ball.setRadius(15.f);
    ball.setFillColor(Color::Red);
    ball.setPosition(Vector2f(0.f, window.getSize().y - ball.getRadius()*2 - 5)); // adding small number to not make the ball at the edges


    
    // main loop
    while (window.isOpen())
    {
        // EVENT POLLING
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            if (Keyboard::isKeyPressed(Keyboard::Escape))
                window.close();
        }

        // UPDATION
        Update(&window, &hoop, &ball, &rectangle);

        // DRAWINGS
        Draw(&window, &hoop, &ball, &rectangle);
    }

    return 0;
}