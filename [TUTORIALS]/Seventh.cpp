#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>

using namespace sf;

int main_7()
{
    // Window
    RenderWindow window7(VideoMode(854, 480), "FrameRate Independent!", Style::Default); // window
    int FPS = 60; // change different values to see the change
    //window7.setFramerateLimit(FPS); // or comment of this statement to make the window frame rate limitless
    
    // FPS stuff
    float movement = 10.f; // standard for 60 fps
    int FPS_std = 60;

    Clock clock;
    float dt = 0.f;

    float avgDT = 0.f;
    Clock TotalTime;

    // Objects
    int keytime = 1; int counter = 0;
    RectangleShape rectangle(Vector2f(50.f, 50.f)); 
    rectangle.setPosition(Vector2f(window7.getSize().x / 2, window7.getSize().y / 2)); // setting at centre of window
    rectangle.setFillColor(sf::Color::Green);

    // Accelerated Motion
    Vector2f dir;
    Vector2f CurVel;
    float MaxVel = 25.f; // for any case even with acceleration, the object should not exceed the Max Velocity
    float acc = 2.f;
    float drag = 0.5f;

    while (window7.isOpen())
    {
        // EVENT POLLING
        sf::Event event;
        while (window7.pollEvent(event))
        {
            if (event.type == sf::Event::Closed || Keyboard::isKeyPressed(Keyboard::Escape))
                window7.close();
        }

        // Clock debug
        dt = clock.restart().asSeconds(); // resets the clock and returns time ellapsed
        // thus dt -> time taken for each frame
        // to make frame rate independent:
        // step-1: set a basic framerate to work on ur entire project, here  60 fps
        // step-2: make a dt for calculating time taken for each frame
        // step-3: wherever u make updations; 
        // lets say for a 60 fps game with a const dt = "dt_std" value(std for standard which ur designing on first), u made a update of move 10.f or something else
        // using ratio and proportion-> 60 : dt_std :: x : dt_unk
        // where x is changed fps and dt_unk (unk -> unknown) is the dt for x fps
        // we can say that 60 * dt_std = 1 sec = x * dt_unk
        // dt_std : 10.f :: dt_unk : move_x.f
        // move_x = 10.f * dt_unk / dt_std  => 10.f * dt_unk * 60
        
        
        // Update
        // Accelerated Motion:
        // means that if we hold on to the movement-key, the speed gets accelerated
        // once we leave the pressed key, the drag will slow down the movement of object
        if (counter < keytime)   counter++;
        else
        {
            counter = 0; // resetting counter
            dir = Vector2f(0.f, 0.f); // reseting direction

            // changing postion of recatange
            // the additional && conditions are done so that the box does not leave the canvas region
            if (Keyboard::isKeyPressed(Keyboard::W) && rectangle.getPosition().y > 0)
            {
                //rectangle.move(0.f, -movement * dt * FPS_std); // ordinary movement
                // accelerated movement:
                dir.y = -1.f;
                if (abs(CurVel.y) < MaxVel)
                    CurVel.y += acc * dir.y * dt * FPS_std;
            }

            if (Keyboard::isKeyPressed(Keyboard::A) && rectangle.getPosition().x > 0)
            {
                //rectangle.move(-movement * dt * FPS_std, 0.f); // ordinary movement
                // accelerated movement:
                dir.x = -1.f;
                if (abs(CurVel.x) < MaxVel)
                    CurVel.x += acc * dir.x * dt * FPS_std;
            }

            if (Keyboard::isKeyPressed(Keyboard::D) && rectangle.getPosition().x + rectangle.getSize().x < window7.getSize().x)
            {
                //rectangle.move(+movement * dt * FPS_std, 0.f); // ordinary movement
                // accelerated movement:
                dir.x = +1.f;
                if (abs(CurVel.x) < MaxVel)
                    CurVel.x += acc * dir.x * dt * FPS_std;
            }

            if (Keyboard::isKeyPressed(Keyboard::S) && rectangle.getPosition().y + rectangle.getSize().y < window7.getSize().y)
            {
                //rectangle.move(0.f, +movement * dt * FPS_std); // ordinary movement
                // accelerated movement:
                dir.y = +1.f;
                if (abs(CurVel.y) < MaxVel)
                    CurVel.y += acc * dir.y * dt * FPS_std;
            }

            // Drag Movement:
            if (CurVel != Vector2f(0.f, 0.f))
            {                
                float absVelx = fmaxf(abs(CurVel.x) - drag * dt * FPS_std, 0);
                float absVely = fmaxf(abs(CurVel.y) - drag * dt * FPS_std, 0);

                if (signbit(CurVel.x))  CurVel.x = -absVelx;
                else  CurVel.x = absVelx;
                if (signbit(CurVel.y))  CurVel.y = -absVely;
                else  CurVel.y = absVely;

                // Confirmation Movement:
                rectangle.move(CurVel.x * dt * FPS_std, CurVel.y * dt * FPS_std);
                // Note: Important
                // It does'nt matter what ever changes we made before in the each step
                // even while moving it needs to updated along with frame rate independent like above
            }
            // changes color when button is pressed
            if (Mouse::isButtonPressed(Mouse::Left))
                rectangle.setFillColor(Color::Cyan);
            else
                rectangle.setFillColor(Color::Green);
        }

        // DRAWINGS
        window7.clear();

        // Draw Stuff here ---
        window7.draw(rectangle);

        window7.display();

        avgDT += dt;
    }
    std::cout << "dt for " << FPS << "fps : " << dt << "\n";
    std::cout << "MOVE for " << FPS << "fps : " << movement * dt * FPS_std << "\n";
    avgDT = avgDT / (FPS * TotalTime.restart().asSeconds());
    std::cout << "average dt for " << FPS << "fps : " << avgDT << "\n";

    return 0;
}