#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
//#include <cstdlib>

using namespace sf;


// NOTE: while running this fie, change main_x to main and in other files (of same dir) with main to main_x
int main_3()
{
    // Random Seed
    srand(time(NULL)); // time(0) gives current time of system in secs

    // Variables
    RenderWindow window3(VideoMode(854, 480), "Ball Shooter!", Style::Default); // window
    //Balls
    std::vector<RectangleShape> enemies;
    CircleShape player;
    std::vector<CircleShape> projectiles;
    Vector2f playerCenter;
    int shootTimer = 0; // counter for shooting
    int shootOffset = 5; // minimum time(or)frames for the next bullet to be fired
    int SpawnTimer = 0; // counter for enemy spawn
    int SpawnOffset = 60; // minimum time(or)frames for the next enemy to be spawned
    int maxSpawn = 3; // max number of enemy can be spawned, for another to be spawned, previous enemy needs to killed
    int score = 0; int counter = 0;

    // window
    window3.setFramerateLimit(60);
    
    // projectiles
    CircleShape TempProj;
    TempProj.setFillColor(Color::Red);
    TempProj.setRadius(5.f);
    //projectiles.push_back(TempProj); // for testing 1st input

    // enemies
    RectangleShape TempEnemy;
    TempEnemy.setFillColor(Color::Green);
    TempEnemy.setSize(Vector2f(50.f, 50.f));
    //enemies.push_back(TempEnemy); // for testing 1st input
    
    // player
    player.setFillColor(Color::White);
    player.setRadius(30.f);
    player.setOutlineThickness(10.f);
    player.setOutlineColor(Color::Cyan);
    player.setPosition(window3.getSize().x /2 - player.getRadius(), window3.getSize().y - player.getRadius()*2 - player.getOutlineThickness()*2);
    


    // main loop
    while (window3.isOpen())
    {
        // EVENT POLLING
        sf::Event event;
        while (window3.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window3.close();
            if (Keyboard::isKeyPressed(Keyboard::Escape))
                window3.close();
        }

        
        
        // UPDATION
        // 1) Updating Player Position
        player.setPosition(Mouse::getPosition(window3).x, player.getPosition().y); // moving only in X direction

        // 2) Updating Projections of Bullets 
        playerCenter = Vector2f(player.getPosition().x + player.getRadius(), player.getPosition().y + player.getRadius());
        // shootTimer is the counter to check with minimum offset for the next bullet to be fired
        if (Mouse::isButtonPressed(Mouse::Left) && shootTimer >= shootOffset)
        {
            TempProj.setPosition(playerCenter); // projection of bullets need come from player circle's centre
            projectiles.push_back(TempProj);
            shootTimer = 0;
        }
        else    shootTimer++;

        //Out of Bounds bullets to be errased
        for (size_t i = 0; i < projectiles.size(); i++)
        {
            projectiles[i].move(0.f, -10.f); // moving projections in upward y axis
            if (projectiles[i].getPosition().y <= 0)
            {
                // if a projective reaches the top end, it is erased
                projectiles.erase(projectiles.begin() + i);  
                // to erase the specific index:
                // we need to add the i number to the vector.begin()
                // vec.begin() is begining index iterator
            }
        }

        // 3) Updating Enemies
        
        // incresing and decreasing no.of spawn with keboard itself
        if (Keyboard::isKeyPressed(Keyboard::Up) && counter > SpawnOffset)
        {
            maxSpawn++; counter = 0;
            std::cout << "No.of Enemies Spawn:\t" << maxSpawn << "\n";
        }
        else if (Keyboard::isKeyPressed(Keyboard::Down) && counter > SpawnOffset)
        {
            maxSpawn--; counter = 0;
            std::cout << "No.of Enemies Spawn:\t" << maxSpawn << "\n";
        }
        else    counter++;

        if (SpawnTimer >= SpawnOffset && enemies.size() < maxSpawn) // to check spawnTimer and maintain MaxSpawn
        {
            // 1st way of random generating (resultant is int)
            //float randomX = rand() % int(window3.getSize().x - TempEnemy.getSize().x);
            // random positioning of enemy: (along top, in randomized x axis)
            // rand() gives random integer ranging from 1 to RandMax value, i.e., 32767;
            // %is mod division, i.e., gives the remainder(Int) as the result,  so it requires integral divisor

            // 2nd way of random generating (resultant is float) (better as its more randomized)
            float randomX = (float(rand())/RAND_MAX) * int(window3.getSize().x - TempEnemy.getSize().x);
            // converting rand value to float and dividing by RandMax gives (float)rand[0,1]
            // its multiplied to our required size length

            //std::cout << "\n" << randomX;
            TempEnemy.setPosition(randomX, 0);
            enemies.push_back(TempEnemy);
            SpawnTimer = 0;
        }
        else if(SpawnTimer < SpawnOffset)    SpawnTimer++;

        for (size_t i = 0; i < enemies.size(); i++)
        {
            enemies[i].move(0.f, 2.f); // enemies move forward towards player (bottom)

            if (enemies[i].getGlobalBounds().intersects(player.getGlobalBounds())) // if enemy hits player
            {
                enemies.erase(enemies.begin() + i);
                score -= 5;
                std::cout << "Player Hurt! :( \t" << "Score:" << score << "\n";
                break;
            }

            if (enemies[i].getPosition().y > window3.getSize().y)
            {
                //enemies.erase(enemies.begin() + i);
                enemies[i].setPosition(enemies[i].getPosition().x, 0.f); // when it reaches bottom it re-continues from top
                break;
            }

            // checking if bullet projectiles lands on enemy boxes
            for (size_t j = 0; j < projectiles.size(); j++)
            {
                if (projectiles[j].getGlobalBounds().intersects(enemies[i].getGlobalBounds())) // checks if any bullets intersects with enemy
                {
                    // removing both enemy and bullet
                    enemies.erase(enemies.begin() + i);
                    projectiles.erase(projectiles.begin() + j);
                    score += 1;
                    std::cout << "Enemy Shot! :) \t" << "Score:" << score << "\n";
                    break;
                }
            }                                  
        }



        // DRAWINGS
        window3.clear();

        // Draw Stuff here ---
        window3.draw(player);
        for (size_t i = 0; i < enemies.size(); i++)
        {
            window3.draw(enemies[i]);
        }
        for (size_t i = 0; i < projectiles.size(); i++)
        {
            window3.draw(projectiles[i]);
        }

        window3.display();
    }

    return 0;
}
