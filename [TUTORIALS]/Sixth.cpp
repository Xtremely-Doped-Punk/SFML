#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>

using namespace sf;

// Len of Vector -> |V| = sqrt(Vx^2 + Vy^2)
// Norm of Vector -> u = V/|V|

float SDT;
float dt;


class Enemy
{
    static std::vector<Texture> ShipTex;
    static std::vector<Texture> SpaceTex;
    static std::vector<Texture> UFOTex;

    static Font font;

    int random; // tells the choice of which enemy

    float speedX; float speedY;
    
    // Private Functions
    void init_ship()
    {
        this->shape.setTexture(ShipTex[0]);
        this->HPmax = 5;
        //std::cout << "Ship Loaded\n";
    }

    void init_space()
    {
        this->shape.setTexture(SpaceTex[0]);
        this->HPmax = 1;
        //std::cout << "Space Loaded\n";
    }

    void init_ufo()
    {
        this->shape.setTexture(UFOTex[0]);
        this->HPmax = 2;
        //std::cout << "UFO Loaded\n";
    }

    void ship_update(RenderWindow* window6)
    {
        // follows the player, trying to hit them, in circular way
    }

    void space_update(RenderWindow* window6)
    {
        // sliding movement in top x axis
    }

    void ufo_update(RenderWindow* window6)
    {
        // upper y axis limit = 50, lower y axis limit = 250
    }

public:
    Sprite shape;
    Text hp;
    int HP;
    int HPmax;
    static float SpawnTimer;
    static const int SpawnOffset = 60;
    static const int maxSpawn = 5;

    Enemy() : speedX(0.f), speedY(0.f) 
    {        
        hp.setFont(font);
        hp.setFillColor(Color::White);
    }
    ~Enemy() {}

    static void staticInitTex()
    {
        Texture Loader;

        // Ship Files
        Loader.loadFromFile("Textures/enemy-ship.png");
        ShipTex.push_back(Loader);
        Loader.loadFromFile("Textures/enemy-ship_O.png");
        ShipTex.push_back(Loader);

        // Space Files
        Loader.loadFromFile("Textures/enemy-space-ship.png");
        SpaceTex.push_back(Loader);
        Loader.loadFromFile("Textures/E-S_bullet.png");
        SpaceTex.push_back(Loader);

        // UFO Files
        Loader.loadFromFile("Textures/ufo.png");
        UFOTex.push_back(Loader);
        Loader.loadFromFile("Textures/enemy-missile.png");
        UFOTex.push_back(Loader);

        // Font Files
        Enemy::font.loadFromFile("Fonts/Terasu Brush.otf");
    }

    void setup(Vector2f PlayerSize)
    {
        // reseting variables
        shape = Sprite();
        float scale = 1.f;

        // Chosing Random Enemy
        random = rand() % 3;
        if (random == 0)    init_ship();
        else if (random == 1)   init_space();
        else if (random == 2)   init_ufo();

        this->HP = this->HPmax;
        // enemy size should be same of Player size
        if (!((PlayerSize.x == getSize().x) && (PlayerSize.y == getSize().y)))
            scale = std::fmaxf(PlayerSize.x / (getSize().x), PlayerSize.y / (getSize().y));
                
        this->shape.setScale(scale, scale);
        //std::cout << PlayerSize.x << "/" << (getSize().x) << "\t" << PlayerSize.y << "/" << (getSize().y) << "\n";

        this->shape.setOrigin(getSize().x / 2, getSize().y / 2);

        hp.setCharacterSize(int(getSize().y / 6));
        setText();
    }

    void update(RenderWindow* window6)
    {
        if (random == 0)
            ship_update(window6);
        else if (random == 1)
            space_update(window6);
        else if (random == 2)
            ufo_update(window6);
    }

    void setPosition(Vector2u WindowSize)
    {
        this->shape.setPosition((float(rand()) / RAND_MAX) * int(WindowSize.x - getSize().x), 0);
        
        // bug needs to be fixed ----
        //float width = this->hp.getGlobalBounds().width;
        //std::cout << width << "\n";
        // ----
        this->hp.setPosition(this->shape.getPosition() + Vector2f((getSize().x /*- this->hp.getGlobalBounds().width*/ - 10) / 2, getSize().y - 10));
    }

    void setText()
    {
        hp.setString(std::to_string(HP) + "/" + std::to_string(HPmax));
    }

    Vector2f getSize()
    {
        return Vector2f(shape.getGlobalBounds().width, shape.getGlobalBounds().height);
    }

};  // Initailizing Static Variables
float Enemy::SpawnTimer = 0.f;
std::vector<Texture> Enemy::ShipTex = {};
std::vector<Texture> Enemy::SpaceTex = {};
std::vector<Texture> Enemy::UFOTex = {};
Font Enemy::font = Font();


class Bullet
{
    Texture BulletTex;
    Vector2f curv_vel;
    float max_speed;
    static constexpr const char* TexFiles[] = { "Textures/bullet.png" };
 
public:
    Sprite shape;
    
    Bullet() : curv_vel(0.f, 0.f), max_speed(20.f) {}
    ~Bullet() {}

    void setup (Vector2f PlayerSize)
    {
        this->BulletTex.loadFromFile(Bullet::TexFiles[0]);
        this->shape.setTexture(BulletTex);
        // bullet size should be 5 times less than Player size
        float scale = std::fmaxf(PlayerSize.x / (5 * getSize().x), PlayerSize.y / (5 * getSize().y));
        //std::cout << "Scale-Cal: " << scale << "\n";
        this->shape.setScale(scale, scale);
        this->shape.setOrigin(getSize().x / 2, getSize().y / 2);
    }

    void setPosition(Vector2f PlayerCenter)
    {   
        this->shape.setPosition(PlayerCenter - shape.getOrigin());
    }

    void move()
    {
        shape.move(curv_vel * (dt / SDT));
    }

    Vector2f getSize()
    {
        return Vector2f(shape.getGlobalBounds().width, shape.getGlobalBounds().height);
    }
};


class Player
{    
    int HPmax;
    const int livesMAX = 3;
    // Loading 3 Textures -> 3 Lives
    static std::vector<Texture> PlayerTex;
    // each player should have his own set of bullets
    Bullet TempBullet; //(player.getSize(), player.getCenterPos());
    float shootTimer = 0;
    int shootOffset = 5;
    Vector2f* WindowSize;

public:
    Sprite shape;
    std::vector<Bullet> bullets;
    int HP;
    int lives;
        
    Player(Vector2f* WindowSize) : HPmax(10) 
    { 
        this->lives = livesMAX;
        this->WindowSize = WindowSize;
        setup(); 
        this->shape.setPosition((this->WindowSize->x - this->getSize().x) / 2, this->WindowSize->y - this->getSize().y);
    }
    ~Player() {}
    
    void setup()
    {
        Vector2f Position = this->shape.getPosition();
        // reseting variables
        shape = Sprite();
        float scale = 1.f;

        this->HP = HPmax;

        if (lives == 3)
            this->shape.setTexture(PlayerTex[0]);
        else if (lives == 2)
            this->shape.setTexture(PlayerTex[1]);
        else if (lives == 1)
            this->shape.setTexture(PlayerTex[2]);
        // Player size should be 10 times less than Window size
        if (!((WindowSize->x == getSize().x) && (WindowSize->y == getSize().y)))
            scale = std::fminf(WindowSize->x / (10 * this->getSize().x), WindowSize->y / (10 * this->getSize().y));

        this->shape.setScale(scale, scale);
        this->shape.setOrigin(this->getSize().x / 2, this->getSize().y / 2);
        this->shape.setPosition(Position);
        this->TempBullet.setup(getSize());
    }

    static void staticInitTex()
    {
        Texture Loader;

        // Texture Files
        Loader.loadFromFile("Textures/battleship1.png");
        PlayerTex.push_back(Loader);
        Loader.loadFromFile("Textures/battleship2.png");
        PlayerTex.push_back(Loader);
        Loader.loadFromFile("Textures/battleship3.png");
        PlayerTex.push_back(Loader);
    }

    void update(RenderWindow* window6)
    {
        // 1> Player movement
        // the additional && conditions are done so that the box does not leave the canvas region
        if (Keyboard::isKeyPressed(Keyboard::W) && shape.getPosition().y > 10)
            shape.move(0.f, -10.f * (dt / SDT));
        if (Keyboard::isKeyPressed(Keyboard::A) && shape.getPosition().x > 10)
            shape.move(-10.f * (dt / SDT), 0.f);
        if (Keyboard::isKeyPressed(Keyboard::D) && shape.getPosition().x + getSize().x < window6->getSize().x)
            shape.move(+10.f * (dt / SDT), 0.f);
        if (Keyboard::isKeyPressed(Keyboard::S) && shape.getPosition().y + getSize().y < window6->getSize().y)
            shape.move(0.f, +10.f * (dt / SDT));
        
        /*shape.setPosition(Mouse::getPosition(*window6).x, shape.getPosition().y); // moving only in X direction

        if (shape.getPosition().x < 0)     shape.setPosition(0, shape.getPosition().y);
        else if (shape.getPosition().x > (window6->getSize().x - getSize().x))
            shape.setPosition(window6->getSize().x - getSize().x, shape.getPosition().y);*/

        // 2> Shooting (New bullets)
        if (Mouse::isButtonPressed(Mouse::Left) && shootTimer >= shootOffset)
        {
            TempBullet.setPosition(getCenterPos());
            bullets.push_back(TempBullet);
            /*std::cout << "\ncount: " << bullets.size(); << "\nSize: " << TempBullet.shape.getScale().x << ", " << TempBullet.shape.getScale().y;
            std::cout << "\nPos: " << TempBullet.shape.getPosition().x + TempBullet.shape.getOrigin().x << ", " 
                << TempBullet.shape.getPosition().y + TempBullet.shape.getOrigin().y;
            std::cout << "\nPlayPos: " << getCenterPos().x << ", " << getCenterPos().y << "\n";*/
            shootTimer = 0;
        }
        else    shootTimer += (dt / SDT);

        // 3> Bullet Movements
        for (size_t i = 0; i < bullets.size(); i++)
        {
            bullets[i].shape.move(0, -10.f * (dt / SDT));

            if (bullets[i].shape.getPosition().y < 0)
                bullets.erase(bullets.begin() + i);
        }

        // 4> Player's Life
        if (HP <= 0)
        {
            lives--;
            if (lives > 0)
                setup();
        }
    }


    Vector2f getSize()
    {
        return Vector2f(shape.getGlobalBounds().width, shape.getGlobalBounds().height);
    }

    Vector2f getCenterPos()
    {
        return Vector2f(shape.getPosition() + shape.getOrigin());
    }
};
std::vector<Texture> Player::PlayerTex = {};



// NOTE: while running this fie, change main_x to main and in other files (of same dir) with main to main_x
int main()
{
    // Random Seed
    srand(time(NULL)); // time(0) gives current time of system in secs

    // Window
    RenderWindow window6(VideoMode(720, 854), "Battleship!", Style::Default); // window    
    //window6.setFramerateLimit(60);

    // Frame rate independent
    //int FPS = 60; //standard fps for u designed ur game
    SDT = 0.01666f;// average dt of the standard fps
    Clock clock;
    dt = 0.f;

    // Player
    Player::staticInitTex();
    Vector2f WindowSize = Vector2f(window6.getSize());
    Player player(&WindowSize);
    float score = 0;

    // Enemy
    Enemy::staticInitTex();
    Enemy TempEnemy;
    std::vector<Enemy> Enemies;
    

    // Score
    Font font;
    if (!font.loadFromFile("Fonts/Revolvingdor.otf"))
        throw("Font could not be loaded!!!");

    Text scoreboard("Score: " + std::to_string(score).substr(0, std::to_string(score).find(".") + 2), font);
    scoreboard.setCharacterSize(int((window6.getSize().x + window6.getSize().y) / 128));
    scoreboard.setFillColor(Color::Green);
    scoreboard.setPosition(window6.getSize().x - scoreboard.getGlobalBounds().width -10, window6.getSize().y - scoreboard.getGlobalBounds().height -10);
    

    //Vector2f PlayerCenter;
    //Vector2f MousePos;

    // Game Over
    Font endfont;
    if (!endfont.loadFromFile("Fonts/Revolvingdor.otf"))
        throw("Font could not be loaded!!!");

    Text gameover("Player out of lives!!!\n GAME OVER!!!", endfont);
    gameover.setCharacterSize(int((window6.getSize().x + window6.getSize().y) / 16));
    gameover.setFillColor(Color::Red);
    while (gameover.getLocalBounds().width > WindowSize.x)
    {
        gameover.setCharacterSize(gameover.getCharacterSize() / 2);
    }
    gameover.setPosition((window6.getSize().x - gameover.getGlobalBounds().width) / 2, (window6.getSize().y - gameover.getGlobalBounds().height) / 2);


    // main loop
    while (window6.isOpen())
    {
        // EVENT POLLING
        sf::Event event;
        while (window6.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window6.close();
            if (Keyboard::isKeyPressed(Keyboard::Escape))
                window6.close();
        }

        //Frame rate Independent Clock updation
        dt = clock.restart().asSeconds();

        // UPDATION
        // End of Game Condition
        if (player.lives > 0)
        {
            // Players and their Bulltets
            player.update(&window6);

            //Enemies Spawn
            if (Enemy::SpawnTimer >= Enemy::SpawnOffset && Enemies.size() < Enemy::maxSpawn || Enemies.size() == 0)
            {
                bool frag = true;
                TempEnemy.setup(player.getSize());
                // conditions so that new enemy doesnt spawn over the previous enemies
                do {
                    TempEnemy.setPosition(window6.getSize());
                    frag = false;
                    for (size_t i = 0; i < Enemies.size(); i++)
                    {
                        if (Enemies[i].shape.getGlobalBounds().intersects(TempEnemy.shape.getGlobalBounds()))
                        {
                            frag = true;    break;
                        }
                    }
                } while (frag);

                Enemies.push_back(TempEnemy);
                Enemy::SpawnTimer = 0;
            }
            else if (Enemy::SpawnTimer < Enemy::SpawnOffset)    Enemy::SpawnTimer += (dt / SDT);

            // Enemy updatings & bullets hitting enemies
            for (size_t i = 0; i < Enemies.size(); i++)
            {
                for (size_t j = 0; j < player.bullets.size(); j++)
                {
                    if (Enemies[i].shape.getGlobalBounds().intersects(player.bullets[j].shape.getGlobalBounds()))
                    {
                        Enemies[i].HP--;
                        Enemies[i].setText();
                        player.bullets.erase(player.bullets.begin() + j);
                        break;
                    }
                }

                if (Enemies[i].shape.getGlobalBounds().intersects(player.shape.getGlobalBounds()))
                {
                    Enemies.erase(Enemies.begin() + i);
                    player.HP -= 3;
                    //std::cout << "\nHP: " << player.HP << "\tLives: " << player.lives;
                }
                else if (Enemies[i].HP <= 0)
                {
                    score += float(Enemies[i].HPmax) / 2;
                    scoreboard.setString("Score: " + std::to_string(score).substr(0, std::to_string(score).find(".") + 2));
                    Enemies.erase(Enemies.begin() + i);
                }
                else
                {

                }
            }
        }

        // DRAWINGS
        window6.clear();

        // Draw Stuff here ---
        window6.draw(player.shape);
        //window6.draw(TempEnemy.shape);
        for (size_t i = 0; i < player.bullets.size(); i++)
            window6.draw(player.bullets[i].shape);
        for (size_t i = 0; i < Enemies.size(); i++)
        {
            window6.draw(Enemies[i].shape);
            window6.draw(Enemies[i].hp);
        }

        window6.draw(scoreboard);

        // GameOver
        if (player.lives <= 0)
        {
            //std::cout << "Player out of lives!!! \nGAME OVER!!!";
            window6.draw(gameover);
        }
        
        window6.display();
    }
    return 0;
}