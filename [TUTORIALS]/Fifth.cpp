#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>

using namespace sf;

// Len of Vector -> |V| = sqrt(Vx^2 + Vy^2)
// Norm of Vector -> u = V/|V|
// class bullet is for trajectories
class BulletShape
{
    Vector2f curv_vel;
    float max_speed;

public:
    CircleShape bullet;

    BulletShape(float radius = 3.f)
        : curv_vel(0.f, 0.f), max_speed(20.f)
    {
        this->bullet.setRadius(radius);
        this->bullet.setFillColor(Color::Red);
    }

    void setup(Vector2f PlayerCenter, Vector2f MousePos)
    {        
        Vector2f AimDir;
        Vector2f AimDirNorm;
        AimDir = MousePos - PlayerCenter;
        AimDirNorm = AimDir / float(sqrt(pow(AimDir.x, 2) + pow(AimDir.y, 2)));

        bullet.setPosition(PlayerCenter);
        curv_vel = AimDirNorm * max_speed;
    }

    void move()
    {
        bullet.move(curv_vel);
    }
};


// NOTE: while running this fie, change main_x to main and in other files (of same dir) with main to main_x
int main_5()
{
    // Random Seed
    srand(time(NULL)); // time(0) gives current time of system in secs

    // Window
    RenderWindow window5(VideoMode(854, 480), "360 shooter!", Style::Default); // window
    window5.setFramerateLimit(60);

    // Enemy
    CircleShape TempEnemy(15.f);
    TempEnemy.setPointCount(6);
    TempEnemy.setOutlineThickness(5.f);
    std::vector<CircleShape> Enemies;
    int SpawnTimer = 0;
    int SpawnOffset = 60;
    int maxSpawn = 5;

    // Player
    CircleShape Player(20.f);
    Player.setFillColor(Color(255,255,255,100)); // slightly transparent
    Player.setOutlineThickness(10.f);
    Player.setOutlineColor(Color::White);
    int score = 0;

    Font font;
    if (!font.loadFromFile("Fonts/notacumulus.ttf"))
        throw("Font could not be loaded!!!");

    Text scoreboard("Score: " + std::to_string(score), font);
    scoreboard.setCharacterSize(int(window5.getSize().y / 32));
    scoreboard.setFillColor(Color::Green);
    scoreboard.setOutlineThickness(1);
    scoreboard.setPosition(window5.getSize().x - scoreboard.getGlobalBounds().width, 0);


    Vector2f PlayerCenter;
    Vector2f MousePos;
        
    // Bullets
    BulletShape TempBullet;
    std::vector<BulletShape> bullets;
    int shootTimer = 0; 
    int shootOffset = 5;


    // main loop
    while (window5.isOpen())
    {
        // EVENT POLLING
        sf::Event event;
        while (window5.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window5.close();
            if (Keyboard::isKeyPressed(Keyboard::Escape))
                window5.close();
        }



        // UPDATION
        //1> Spawn Settings for Enemy
        if (SpawnTimer >= SpawnOffset && Enemies.size() < maxSpawn || Enemies.size() == 0) // there will always be one enemy no matter other conditons
        {
            do { // enemy should not be spawned touching Player
                float randomX = (float(rand()) / RAND_MAX) * (window5.getSize().x - TempEnemy.getRadius() * 2 - TempEnemy.getOutlineThickness() - 5);
                float randomY = (float(rand()) / RAND_MAX) * (window5.getSize().y - TempEnemy.getRadius() * 2 - TempEnemy.getOutlineThickness() - 5);

                TempEnemy.setPosition(randomX, randomY);

            } while (TempEnemy.getGlobalBounds().intersects(Player.getGlobalBounds()));

            //std::cout << TempEnemy.getPosition().x << "\t" << TempEnemy.getPosition().y << "\n";
            
            // random positions and colors
            TempEnemy.setFillColor(Color((float(rand()) / RAND_MAX)* 255, (float(rand()) / RAND_MAX) * 255, (float(rand()) / RAND_MAX) * 255));
            TempEnemy.setOutlineColor(Color((float(rand()) / RAND_MAX) * 255, (float(rand()) / RAND_MAX) * 255, (float(rand()) / RAND_MAX) * 255));
            // setting origin for rotation
            TempEnemy.setOrigin(Vector2f(TempEnemy.getRadius(), TempEnemy.getRadius()));

            Enemies.push_back(TempEnemy);
            SpawnTimer = 0;
        }
        else if (SpawnTimer < SpawnOffset)    SpawnTimer++;

        // 2> Enemy movement
        for (size_t i = 0; i < Enemies.size(); i++)
        {
            Enemies[i].rotate((float(rand()) / RAND_MAX) * 10); // rotate

            // Collisions between Player and Enemy
            if (Enemies[i].getGlobalBounds().intersects(Player.getGlobalBounds()))
            {
                Enemies.erase(Enemies.begin() + i);
                score -= 5;
                //std::cout << "Player Hurt! :( \t" << "Score:" << score << "\n";
                scoreboard.setString("Player Hurt! :( \tScore: " + std::to_string(score));
                scoreboard.setOutlineColor(Color::Red);
                scoreboard.setPosition(window5.getSize().x - scoreboard.getGlobalBounds().width, 0);
            }
        }

        // 3> Player movement
        // the additional && conditions are done so that the box does not leave the canvas region
        if (Keyboard::isKeyPressed(Keyboard::W) && Player.getPosition().y > 0)
            Player.move(0.f, -10.f);
        if (Keyboard::isKeyPressed(Keyboard::A) && Player.getPosition().x > 0)
            Player.move(-10.f, 0.f);
        if (Keyboard::isKeyPressed(Keyboard::D) && Player.getPosition().x + Player.getRadius() * 2 < window5.getSize().x)
            Player.move(+10.f, 0.f);
        if (Keyboard::isKeyPressed(Keyboard::S) && Player.getPosition().y + Player.getRadius() * 2 < window5.getSize().y)
            Player.move(0.f, +10.f);


        // 4> Bullets
        PlayerCenter = Vector2f(Player.getPosition().x + Player.getRadius() + Player.getOutlineThickness(), Player.getPosition().y + Player.getRadius() + Player.getOutlineThickness());
        MousePos = Vector2f(Mouse::getPosition(window5));
        if (Mouse::isButtonPressed(Mouse::Left) && shootTimer >= shootOffset)
        {
            TempBullet.setup(PlayerCenter, MousePos);
            bullets.push_back(TempBullet);
        }
        else    shootTimer++;

        for (size_t i = 0; i < bullets.size(); i++)
        {
            bullets[i].move();
            
            // Out of Bounds Check
            if (bullets[i].bullet.getPosition().y <= 0 || bullets[i].bullet.getPosition().x <= 0 || 
                bullets[i].bullet.getPosition().y >= window5.getSize().y || bullets[i].bullet.getPosition().x >= window5.getSize().x)
            {
                bullets.erase(bullets.begin() + i);
            }
            else {

                // bullet collision with enemies
                for (size_t j = 0; j < Enemies.size(); j++)
                {
                    if (bullets[i].bullet.getGlobalBounds().intersects(Enemies[j].getGlobalBounds())) // checks if any bullets intersects with enemy
                    {
                        // removing both enemy and bullet
                        Enemies.erase(Enemies.begin() + j);
                        bullets.erase(bullets.begin() + i);
                        score += 1;
                        //std::cout << "Enemy Shot! :) \t" << "Score:" << score << "\n";
                        scoreboard.setString("Enemy Shot! :) \tScore: " + std::to_string(score));
                        scoreboard.setOutlineColor(Color::Blue);
                        scoreboard.setPosition(window5.getSize().x - scoreboard.getGlobalBounds().width, 0);
                        break;
                    }
                }
            }
        }




        // DRAWINGS
        window5.clear();

        // Draw Stuff here ---
        window5.draw(Player);
        for (size_t i = 0; i < bullets.size(); i++)
            window5.draw(bullets[i].bullet);
        for (size_t i = 0; i < Enemies.size(); i++)
            window5.draw(Enemies[i]);
        window5.draw(scoreboard);
            
        window5.display();
    }

    return 0;
}