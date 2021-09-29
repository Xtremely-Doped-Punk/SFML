#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>

using namespace sf;

// Len of Vector -> |V| = sqrt(Vx^2 + Vy^2)
// Norm of Vector -> u = V/|V|
Vector2f normalize(Vector2f vec)
{
    double x = vec.x; 
    double y = vec.y;

    float length = (float)(x * x + y * y);
    if (length == 0.f)
        return vec;
    length = sqrt(1.f / length);
    x = (double)(x * length);
    y = (double)(y * length);

    vec.x = x;
    vec.y = y;
    return vec;
}

Vector2f rotateBy(Vector2f vec, float degrees, const Vector2f& center = Vector2f())
{
    double x = vec.x;
    double y = vec.y;

    degrees *= 3.14159f / 180.f;
    const float cs = cos(degrees);
    const float sn = sin(degrees);

    x -= center.x;
    y -= center.y;

    double nx = (double)(x * cs - y * sn);
    double ny = (double)(x * sn + y * cs);

    x = nx;
    y = ny;

    x += center.x;
    y += center.y;

    vec.x = x;
    vec.y = y;
    return vec;
}

// returns trig angle
float VecAngle(Vector2f vec)
{
    double x = vec.x;
    double y = vec.y;

    if (y == 0)
        return x < 0 ? 180 : 0;
    else if (x == 0)
        return y < 0 ? 270 : 90;

    if (y > 0)
        if (x > 0)
            return atan(y / x) * 180.f / 3.14159f;
        else
            return 180.0 - atan(y / -x) * 180.f / 3.14159f;
    else
        if (x > 0)
            return 360.0 - atan(-y / x) * 180.f / 3.14159f;
        else
            return 180.0 + atan(-y / -x) * 180.f / 3.14159f;
}

float VecNorm(Vector2f vec)
{
    double x = vec.x;
    double y = vec.y;

    return sqrt(x * x + y * y);
}

float SDT;
float dt;


class Bullet
{
    Texture *BulletTex;
    Vector2f vel;
    float xfactor;
    float yfactor;

public:
    Sprite shape;

    Bullet() {}
    ~Bullet() {}

    void setup(Texture* Tex, Vector2f PlayerSize)
    {
        this->BulletTex = Tex;
        this->shape.setTexture(*BulletTex);
        // bullet size should be 5 times less than Player size
        float scale = std::fmaxf(PlayerSize.x / (5 * getSize().x), PlayerSize.y / (5 * getSize().y));
        /*std::cout << "\nScale-Cal: " << scale << "\n";
        std::cout << "Sc-part-1: " << PlayerSize.x / (5 * getSize().x) << "\t" << "Sc-part-2: " << PlayerSize.y / (5 * getSize().y) << "\n";
        std::cout << "Sc-part-1.1: " << PlayerSize.x << "\t" << "Sc-part-1.2: " << getSize().x << "\n";
        std::cout << "Sc-part-2.1: " << PlayerSize.y << "\t" << "Sc-part-2.2: " << getSize().y << "\n";*/
        this->shape.setScale(scale, scale);
        this->shape.setOrigin(getSize().x / 2, getSize().y / 2);
        
    }

    void setPosition(Vector2f PlayerCenter, Vector2f initVel = Vector2f(0.f, 0.f), float xfactor = 1.00125, float yfactor = 1)
    {
        this->shape.setPosition(PlayerCenter - shape.getOrigin());
        vel = initVel;
        this->xfactor = xfactor;
        this->yfactor = yfactor;
    }

    void curv_move()
    {
        vel = Vector2f(vel.x / xfactor, vel.y / yfactor);
        shape.move(vel * (dt / SDT));
        xfactor += 0.005 * SDT * (dt / SDT);
        yfactor -= 0.0001 * SDT * (dt / SDT);

        //std::cout << xfactor << "\t" << yfactor << "\n";
    }

    void linear_move()
    {
        shape.move(vel * (dt / SDT));
    }

    Vector2f getSize()
    {
        return Vector2f(shape.getGlobalBounds().width, shape.getGlobalBounds().height);
    }
};


class Enemy
{
    static std::vector<Texture> ShipTex;
    static std::vector<Texture> SpaceTex;
    static std::vector<Texture> UFOTex;

    static Font font;
    Bullet TempBullet;

    int random; // tells the choice of which enemy

    float speed;
    float acc;
    int dir;
    float ufoY;
    
    // Private Functions
    void init_ship()
    {
        this->shape.setTexture(ShipTex[0]);
        this->HPmax = 5;
        //std::cout << "Ship Loaded\n";
        this->speed = 0;
        this->acc = 0;
    }

    void init_space()
    {
        this->shape.setTexture(SpaceTex[0]);
        this->HPmax = 1;
        //std::cout << "Space Loaded\n";
        this->speed = 1.5 * dir;
        this->acc = 0;
    }

    void init_ufo()
    {
        this->shape.setTexture(UFOTex[0]);
        this->HPmax = 2;
        //std::cout << "UFO Loaded\n";
        this->speed = 1.5 * dir;
        this->acc = 0;
        this->ufoY = 1.5f;
        shootOffset *= 1.5;
    }

    void ship_update(RenderWindow* window6, Vector2f PlayerPosition)
    {
        // follows the player, trying to hit them, in circular way
        Vector2f vector = this->getCenterPos() - PlayerPosition;
        float radius = VecNorm(vector);

        this->acc += 0.00001f * SDT * SDT * (dt/SDT);
        float angvel = (sqrt(this->acc * radius) / radius) * 180 / 3.14159f; // angular velocity
        this->speed += angvel * dir; // direction of acc
        
        /*std::cout << "\nCRT-Position: " << getCenterPos().x << "," << getCenterPos().y << "";
        std::cout << "\nSpeed: " << (speed) * dt / SDT;
        std::cout << "\nVec : (" << vector.x << "," << vector.y << ")\t Radius: " << radius;*/
        
        Vector2f rotation = rotateBy(vector, (speed) * dt / SDT);
        //std::cout << "\nAcc: " << acc << "\t Ang-vel: " << angvel * 180 / 3.14159f;

        //Const velocity towards player be 0.375f
        Vector2f position = PlayerPosition + rotation - this->shape.getOrigin() - Vector2f((0.35 + angvel) * normalize(vector).x, (0.35 + angvel) * normalize(vector).y);;

        // Leave the screen too - much (Range window size +- 70)
        if ((position.x > window6->getSize().x + 70) || (position.x < -70) ||
            (position.y > window6->getSize().y + 70) || (position.y < -70))
        {
            //std::cout << "\nPosition: " << position.x << "," << position.y << "";
            /*if (position.x > window6->getSize().x + 70)
                position.x -= 70/2;
            else if (position.x < -70)
                position.x += 70/2;
            
            if (position.y > window6->getSize().y + 70)
                position.y -= 70/2;
            else if (position.y < -70)
                position.y += 70/2;*/
            
            //std::cout << "\nBang!";

            this->dir = -this->dir; // inverse the speed
            this->speed = -this->speed / 3; // bouncing back ruduces velocity 3 times
            this->acc = this->acc / 1.5f; // acc reduced by 1.5 times
        }
        //std::cout << "\nNXT-Position: " << position.x << "," << position.y << "\n\n";

        // rotate ship facing player
        this->shape.setRotation(VecAngle(-vector));
        this->hp.setRotation(VecAngle(-vector));
               
        //Let time interval be 0.1 sec
        this->shape.setPosition(position);
        this->hp.setPosition(this->shape.getPosition() + Vector2f((getSize().x - this->hp.getGlobalBounds().width - 25) / 2, getSize().y - this->hp.getGlobalBounds().width - 25));

       // change the the texture to make the ship look damaged
       if (this->HP < 3)
           this->shape.setTexture(ShipTex[1]);

      // sleep(milliseconds(200));
    }

    void space_update(RenderWindow* window6)
    {
        // 1> sliding movement in top x axis
        
        this->acc += 0.01f * SDT  * (dt / SDT);
        this->speed += dir * acc * SDT * (dt / SDT);
        Vector2f position = this->getCenterPos();

        position += Vector2f(speed * dt / SDT, 0);

        //std::cout << "\nCRT-Position: " << shape.getPosition().x << "," << shape.getPosition().y << "";
        //std::cout << "\nCRT-Position: " << getCenterPos().x << "," << getCenterPos().y << "";
        //std::cout << "\nSpeed: " << (speed) * dt / SDT << "\tdir: " << dir << "\tacc: " << acc;

         //Leave the screen too - much (Range window size +- 25)
        if ((position.x > window6->getSize().x + 25) || (position.x < -25))
        {
            if (position.x > window6->getSize().x + 25)
                position.x = window6->getSize().x + 25;
            else if (position.x < -25)
                position.x = -25;

            this->dir = -this->dir; // inverse the speed
            this->speed = -this->speed / 3; // bouncing back ruduces velocity 3 times
            this->acc = this->acc / 1.2f; // acc reduced by 1.2 times
        }
        //std::cout << "\nNXT- Position: (" << position.x << "," << position.y << ")";

        position -= this->shape.getOrigin();
        //std::cout << "\nNXT- Position: (" << position.x << "," << position.y << ")\n\n";
        this->shape.setPosition(position);
        this->hp.setPosition(this->shape.getPosition() + Vector2f((getSize().x - this->hp.getGlobalBounds().width - 10) / 2, getSize().y - this->hp.getGlobalBounds().width - 10));

        //sleep(milliseconds(100));


        // 2> shooting at regular interval (1 bullet per 60 sec)
        if (shootTimer >= shootOffset)
        {
            TempBullet.setPosition(getCenterPos());
            bullets.push_back(TempBullet);
            /*std::cout << "\ncount: " << bullets.size() << "\nScale: " << TempBullet.shape.getScale().x << ", " << TempBullet.shape.getScale().y;
            std::cout << "\nPos: " << TempBullet.shape.getPosition().x + TempBullet.shape.getOrigin().x << ", "
                << TempBullet.shape.getPosition().y + TempBullet.shape.getOrigin().y;            */
            //std::cout << "\nSize: " << TempBullet.getSize().x << ", " << TempBullet.getSize().y << "\n";
            //std::cout << "\nPlayer Size: " << getSize().x << ", " << getSize().y << "\n";
            shootTimer = 0;
        }
        else    shootTimer += (dt / SDT);

        // 3> Bullet Movements
        for (size_t i = 0; i < bullets.size(); i++)
        {
            bullets[i].shape.move(0, 5.f * (dt / SDT));
            /*std::cout << "\n" << i << "th-Pos: " << bullets[i].shape.getPosition().x + bullets[i].shape.getOrigin().x << ", "
                << bullets[i].shape.getPosition().y + bullets[i].shape.getOrigin().y;*/
            if (bullets[i].shape.getPosition().y > window6->getSize().y)
                bullets.erase(bullets.begin() + i);
        }
    }

    void ufo_update(RenderWindow* window6)
    {
        // 1> X axis movement --> same like spaceship

        this->acc += 0.01f * SDT * (dt / SDT);
        this->speed += dir * acc * SDT * (dt / SDT);
        Vector2f position = this->getCenterPos();

        position += Vector2f(speed * dt / SDT, 0);

        //std::cout << "\nCRT-Position: " << shape.getPosition().x << "," << shape.getPosition().y << "";
        //std::cout << "\nCRT-Position: " << getCenterPos().x << "," << getCenterPos().y << "";
        //std::cout << "\nSpeed: " << (speed) * dt / SDT << "\tdir: " << dir << "\tacc: " << acc;

         //Leave the screen too - much (Range window size +- 25)
        if ((position.x > window6->getSize().x + 25) || (position.x < -25))
        {
            if (position.x > window6->getSize().x + 25)
                position.x = window6->getSize().x + 25;
            else if (position.x < -25)
                position.x = -25;

            this->dir = -this->dir; // inverse the speed
            this->speed = -this->speed / 3; // bouncing back ruduces velocity 3 times
            this->acc = this->acc / 1.2f; // acc reduced by 1.2 times
        }
        //std::cout << "\nNXT- Position: (" << position.x << "," << position.y << ")";

        // Y axis movement --> upper y axis limit = 150, lower y axis limit = 250
        position += Vector2f(0, ufoY * dt / SDT);
        //Range
        if ((position.y > 250) || (position.y < 150))
        {
            if (position.y > 250)
                position.y = 250;
            else if (position.y < 150)
                position.y = 150;

            this->ufoY = -this->ufoY;
        }

        position -= this->shape.getOrigin();
        this->shape.setPosition(position);
        this->hp.move(position - hp.getPosition());

        //sleep(milliseconds(100));


        // 2> shooting at regular interval (1 bullet per 60 sec)
        if (shootTimer >= shootOffset)
        {
            TempBullet.setPosition(getCenterPos(),Vector2f(speed*3, 2.75f));
            bullets.push_back(TempBullet);
            /*std::cout << "\ncount: " << bullets.size() << "\nScale: " << TempBullet.shape.getScale().x << ", " << TempBullet.shape.getScale().y;
            std::cout << "\nPos: " << TempBullet.shape.getPosition().x + TempBullet.shape.getOrigin().x << ", "
                << TempBullet.shape.getPosition().y + TempBullet.shape.getOrigin().y;            */
                //std::cout << "\nSize: " << TempBullet.getSize().x << ", " << TempBullet.getSize().y << "\n";
                //std::cout << "\nPlayer Size: " << getSize().x << ", " << getSize().y << "\n";
            shootTimer = 0;
        }
        else    shootTimer += (dt / SDT);

        // 3> Bullet Movements
        for (size_t i = 0; i < bullets.size(); i++)
        {
            bullets[i].curv_move();
            /*std::cout << "\n" << i << "th-Pos: " << bullets[i].shape.getPosition().x + bullets[i].shape.getOrigin().x << ", "
                << bullets[i].shape.getPosition().y + bullets[i].shape.getOrigin().y;*/
            if (bullets[i].shape.getPosition().y > window6->getSize().y)
                bullets.erase(bullets.begin() + i);
        }
    }

public:
    Sprite shape;
    Text hp;
    int HP;
    int HPmax;
    std::vector<Bullet> bullets;
    int shootOffset = 120;
    float shootTimer = shootOffset/2;
    static float SpawnTimer;
    static const int SpawnOffset = 60;
    static const int maxSpawn = 5;

    Enemy() : speed(0.f), acc(dt)
    {   
        dir = (rand() % 2) == 0 ? -1 : 1;
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
        TempBullet = Bullet();
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

        if (random == 1)
        {
            this->TempBullet.setup(&SpaceTex[1], getSize());
            /*std::cout << "\ncount: " << bullets.size() << "\nScale: " << TempBullet.shape.getScale().x << ", " << TempBullet.shape.getScale().y;
            std::cout << "\nPos: " << TempBullet.shape.getPosition().x + TempBullet.shape.getOrigin().x << ", "
                << TempBullet.shape.getPosition().y + TempBullet.shape.getOrigin().y << "\n\n";*/
        }
        else if (random == 2)
            this->TempBullet.setup(&UFOTex[1], getSize());

        
    }

    void update(RenderWindow* window6, Vector2f PlayerPosition)
    {
        if (random == 0)
            ship_update(window6, PlayerPosition);
        else if (random == 1)
            space_update(window6);
        else if (random == 2)
            ufo_update(window6);
    }

    void setPosition(Vector2u WindowSize)
    {
        float randx = (float(rand()) / RAND_MAX) * int(WindowSize.x - getSize().x);
        
        if ((random == 0) || (random == 1))
            this->shape.setPosition(randx, 0);
        else if (random == 2)
            this->shape.setPosition(randx, 100.f - this->shape.getOrigin().y);
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

    Vector2f getCenterPos()
    {
        return Vector2f(shape.getPosition() + shape.getOrigin());
    }
};  // Initailizing Static Variables
float Enemy::SpawnTimer = 0.f;
std::vector<Texture> Enemy::ShipTex = {};
std::vector<Texture> Enemy::SpaceTex = {};
std::vector<Texture> Enemy::UFOTex = {};
Font Enemy::font = Font();


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
    float HP;
    int lives;
    Vector2f InitSize;
        
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
        TempBullet = Bullet();

        float scale = 1.f;

        this->HP = HPmax;

        if (lives == 3)
            this->shape.setTexture(PlayerTex[0]);
        else if (lives == 2)
            this->shape.setTexture(PlayerTex[1]);
        else if (lives == 1)
            this->shape.setTexture(PlayerTex[2]);
        // Player size should be 10 times less than Window size
        InitSize = Vector2f(shape.getGlobalBounds().width, shape.getGlobalBounds().height);
        if (!((WindowSize->x == InitSize.x) && (WindowSize->y == InitSize.y)))
            scale = std::fminf(WindowSize->x / (10 * this->InitSize.x), WindowSize->y / (10 * InitSize.y));

        this->shape.setScale(scale, scale);
        this->shape.setPosition(Position);
        InitSize = Vector2f(shape.getGlobalBounds().width, shape.getGlobalBounds().height);
        this->shape.setOrigin(InitSize.x / 2, InitSize.y / 2);
        this->TempBullet.setup(&PlayerTex[3], getSize());
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
        Loader.loadFromFile("Textures/bullet.png");
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
        
        // Rotations
        Vector2f MousePtr = window6->mapPixelToCoords(Mouse::getPosition(*window6));

        /*Vector2f curPos;
        curPos.x = shape.getGlobalBounds().left;
        curPos.y = shape.getGlobalBounds().top;
        
        const float PI = 3.14159265;

        float dx = curPos.x - MousePtr.x;
        float dy = curPos.y - MousePtr.y;
        float rotation = (atan2(dy, dx)) * 180 / PI;

        this->shape.setOrigin(shape.getLocalBounds().width / 2, shape.getLocalBounds().height / 2);
        this->shape.setRotation(rotation + 180);
        this->shape.setOrigin(InitSize.x / 2, InitSize.y / 2);
        std::cout << rotation << "\n";
        std::cout << "Mouse: (" << MousePtr.x << ", " << MousePtr.y << ")\n\n";
        std::cout << dx << ", " << dy << "\n\n";*/
        //sleep(milliseconds(100));

        // 2> Shooting (New bullets)
        if (Mouse::isButtonPressed(Mouse::Left) && shootTimer >= shootOffset)
        {
            Vector2f vector = normalize(Vector2f(MousePtr) - getCenterPos());
            TempBullet.setPosition(getCenterPos(), Vector2f(vector.x * 10, vector.y * 10));
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
            //bullets[i].shape.move(0, -10.f * (dt / SDT));
            bullets[i].linear_move();
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
        return InitSize;
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
    window6.setFramerateLimit(60);

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
    Font Sfont;
    if (!Sfont.loadFromFile("Fonts/Revolvingdor.otf"))
        throw("Font could not be loaded!!!");

    Text scoreboard("Score: " + std::to_string(score).substr(0, std::to_string(score).find(".") + 2), Sfont);
    scoreboard.setCharacterSize(int((window6.getSize().x + window6.getSize().y) / 128));
    scoreboard.setFillColor(Color::Green);
    scoreboard.setPosition(window6.getSize().x - scoreboard.getGlobalBounds().width -10, window6.getSize().y - scoreboard.getGlobalBounds().height -10);
    

    // Life
    Font Lfont;
    if (!Lfont.loadFromFile("Fonts/Revolvingdor.otf"))
        throw("Font could not be loaded!!!");

    Text lifeboard("Lives: " + std::to_string(player.lives) + ", HP: " + std::to_string(player.HP).substr(0, std::to_string(player.HP).find(".") + 2), Lfont);
    lifeboard.setCharacterSize(int((window6.getSize().x + window6.getSize().y) / 128));
    lifeboard.setFillColor(Color::Magenta);
    lifeboard.setPosition(0, window6.getSize().y - scoreboard.getGlobalBounds().height - 10);


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
            lifeboard.setString("Lives: " + std::to_string(player.lives) + ", HP: " + std::to_string(player.HP).substr(0, std::to_string(player.HP).find(".") + 2));

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
                //Updating Enemy Movements
                Enemies[i].update(&window6, player.getCenterPos());

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

                // Enemy bullets damage player
                for (size_t j = 0; j < Enemies[i].bullets.size(); j++) 
                {
                    if (player.shape.getGlobalBounds().intersects(Enemies[i].bullets[j].shape.getGlobalBounds()))
                    {
                        player.HP -= 0.5;
                        lifeboard.setString("Lives: " + std::to_string(player.lives) + ", HP: " + std::to_string(player.HP).substr(0, std::to_string(player.HP).find(".") + 2));
                        Enemies[i].bullets.erase(Enemies[i].bullets.begin() + j);
                        break;
                    }
                }


                if (Enemies[i].shape.getGlobalBounds().intersects(player.shape.getGlobalBounds()))
                {
                    Enemies.erase(Enemies.begin() + i);
                    player.HP -= 3;
                    lifeboard.setString("Lives: " + std::to_string(player.lives) + ", HP: " + std::to_string(player.HP).substr(0, std::to_string(player.HP).find(".") + 2));
                    //std::cout << "\nHP: " << player.HP << "\tLives: " << player.lives;
                }
                else if (Enemies[i].HP <= 0)
                {
                    score += float(Enemies[i].HPmax) / 2;
                    scoreboard.setString("Score: " + std::to_string(score).substr(0, std::to_string(score).find(".") + 2));
                    Enemies.erase(Enemies.begin() + i);
                }
                else;
            }
        }

        // DRAWINGS
        window6.clear();

        // Draw Stuff here ---
        window6.draw(player.shape);
        //window6.display();
        //sleep(milliseconds(100));
        window6.draw(TempEnemy.shape);
        for (size_t i = 0; i < player.bullets.size(); i++)
            window6.draw(player.bullets[i].shape);
        for (size_t i = 0; i < Enemies.size(); i++)
        {
            window6.draw(Enemies[i].shape);
            window6.draw(Enemies[i].hp);
            for (size_t j = 0; j < Enemies[i].bullets.size(); j++)
                window6.draw(Enemies[i].bullets[j].shape);
        }

        window6.draw(scoreboard);
        window6.draw(lifeboard);

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