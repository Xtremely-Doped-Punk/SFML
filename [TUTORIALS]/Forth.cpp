#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>

using namespace sf;


// NOTE: while running this fie, change main_x to main and in other files (of same dir) with main to main_x
int main_4()
{
    // Random Seed
    srand(time(NULL)); // time(0) gives current time of system in secs

    // Window
    RenderWindow window4(VideoMode(854, 480), "Dodge Enimies!", Style::Default); // window
    window4.setFramerateLimit(60);

    // Enemy
    Texture EnemyTex;
    Sprite TempEnemy;
    int SpawnTimer = 0;
    int SpawnOffset = 60;

    if (!EnemyTex.loadFromFile("Textures/dog.png"))
        throw "Could'nt load Enemy Texture!";

    TempEnemy.setTexture(EnemyTex);
    TempEnemy.setScale(0.1f, 0.1f);
    std::vector <Sprite> Enemies;

    // Player
    Texture PlayerTex;
    Sprite Player; int maxHP = 10;
    int hp = maxHP; std::cout << "Player HP stats:\t" << hp << "\n";
    RectangleShape hpBar;
    hpBar.setFillColor(Color::Red);
    hpBar.setSize(Vector2f(window4.getSize().x, 5.f));
    hpBar.setPosition(0.f, window4.getSize().y - 5);

    if (!PlayerTex.loadFromFile("Textures/derp.png"))
        throw "Could'nt load Player Texture!";

    Player.setTexture(PlayerTex);
    Player.setScale(0.15f, 0.15f);



    // main loop
    while (window4.isOpen())
    {
        // EVENT POLLING
        sf::Event event;
        while (window4.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window4.close();
            if (Keyboard::isKeyPressed(Keyboard::Escape))
                window4.close();
        }



        // UPDATION
        // 1> Spawn Settings for Enemy
        if (SpawnTimer >= SpawnOffset)
        {
            float random = (float(rand()) / RAND_MAX) * (window4.getSize().y - TempEnemy.getGlobalBounds().height - 5);
            //std::cout << random << "\n";
            TempEnemy.setPosition(window4.getSize().x - TempEnemy.getGlobalBounds().width, random);
            Enemies.push_back(Sprite(TempEnemy)); // creates a copy of the object and sends into vector
            SpawnTimer = 0;
        }
        else if (SpawnTimer < SpawnOffset)    SpawnTimer++;

        // 2> Enemy movement
        for (size_t i = 0; i < Enemies.size(); i++)
        {
            Enemies[i].move(-5.f, 0.f); // moving towards player in left end

            if(Enemies[i].getPosition().x < (0 - TempEnemy.getGlobalBounds().width)) //if we keep just 0, enemy will be vanished as soon as its left top reaches 0
                Enemies.erase(Enemies.begin() + i);

            // Collisions between Player and Enemy
            if (Enemies[i].getGlobalBounds().intersects(Player.getGlobalBounds()))
            {
                Enemies.erase(Enemies.begin() + i);
                hp--;
                hpBar.setSize(Vector2f(hp * window4.getSize().x / maxHP, hpBar.getSize().y));
                std::cout << "Player Hurt! :( \t" << "HP: " << hp << "\n";
            }
        }

        // 3> Player movement
        Player.setPosition(Player.getPosition().x, Mouse::getPosition(window4).y); // moving only in X direction
        if (Player.getPosition().y < 0)     Player.setPosition(Player.getPosition().x, 0);
        else if (Player.getPosition().y > (window4.getSize().y - Player.getGlobalBounds().height - 5))
            Player.setPosition(Player.getPosition().x, window4.getSize().y - Player.getGlobalBounds().height - 5);
        


         // DRAWINGS
        window4.clear();
       
        // Draw Stuff here ---
        window4.draw(Player);
        for (size_t i = 0; i < Enemies.size(); i++)
            window4.draw(Enemies[i]);
        window4.draw(hpBar);

        window4.display();


        // End of Game Condition
        if (hp == 0)
        {
            std::cout << "Player out of lives!!! \nGAME OVER!!!";
            break;
        }
    }

    return 0;
}