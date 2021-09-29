// SFML.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>

using namespace sf; // where ever we use sf::_something_ -> can be directly written as _something_

// NOTE: while running this fie, change main_x to main and in other files (of same dir) with main to main_x 
int main_1()
{
    RenderWindow window(VideoMode(854, 480), "SFML Example!", Style::Default);
    // blank window where we draw everything
    // sf-stylr id default is windowed mode -> can also be changed to full screen

    window.setFramerateLimit(60); //limit frame rate

    CircleShape circle(50.f); // radius
    circle.setFillColor(sf::Color::Green); // default white
    circle.setPosition(Vector2f(50.f,50.f)); // set initial position

    RectangleShape rectangle(Vector2f(50.f,100.f)); // x and y sizes
    rectangle.setPosition(Vector2f(800.f, 350.f)); // set initial position
    rectangle.setFillColor(sf::Color(85, 100, 225, 100)); // input parameters (R, G, B, Alpha -> 0(invisible) to 255(opaque) )

    // Triangle
    CircleShape triangle;
    triangle.setRadius(75.f);
    triangle.setPointCount(3); // taking one 3 points out of a circle makes it a triangle, for 4 points it becomes a square
    triangle.setFillColor(sf::Color::Red);
    triangle.setOutlineThickness(5.f);
    triangle.setOutlineColor(Color::Cyan);


    // Lines
    Vertex line[] = {Vertex(Vector2f(100.f, 400.f)), Vertex(Vector2f(50.f, 100.f)) };

    ConvexShape convex;
    convex.setPosition(Vector2f(400.f,50.f));

    convex.setPointCount(5);

    convex.setPoint(0, Vector2f(0, 0)); // index and coordinate as parameters
    convex.setPoint(1, Vector2f(150, 10));
    convex.setPoint(2, Vector2f(120, 90));
    convex.setPoint(3, Vector2f(30, 100));
    convex.setPoint(4, Vector2f(0, 50));

    while (window.isOpen()) // return boolean to check if the window is still open
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            
            if (event.type == Event::KeyPressed && event.key.code == Keyboard::Escape)
            {// checks if the event type is any key in keyboard is pressed
                window.close(); // if esc is pressed window is closed
            }
            
            if (event.type == Event::MouseButtonPressed && event.key.code == Mouse::Middle)
            {// checks if the event type is any key in keyboard is pressed
                window.close(); // if esc is pressed window is closed
            }
                   
            /*
            // alternate way to check key pressed
            if (Keyboard::isKeyPressed(Keyboard::Escape))
                window.close();

            // Mouse input
                if (Mouse::isButtonPressed(Mouse::Middle))
                    window.close();
            */
        }

        //Update
        /*
        circle.move(0.5f, 0.15f); // moving centre in (x-step, y-step)
        circle.rotate(4.f); // how faster u u want 
        // Note: the origin is not set so default for every shape the orgin in on the top-left-most end
        rectangle.move(-0.5f, -0.15f); 
        rectangle.rotate(7.f); 
        */

        //Draw (the updated ones)
        // when clear is not used the previous drawn object remains in th window with overlaping new drawings
        window.clear(); // clear and draw every time
        //window.clear(Color::Magenta); // clears the window with the color mentioned

        /*
        window.draw(circle);
        window.draw(rectangle);
        */
        window.draw(triangle);
        window.draw(line, 2, Lines); // vertices-buffer, no of vertices, primitive types which r inbuit
        window.draw(convex);
        window.display(); // to show the window
    }

    return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
