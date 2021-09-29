#include <iostream>
#include <sstream>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <SFML/Network.hpp>
#include <SFML/Audio.hpp>

using namespace sf;

int main9()
{
	// Window
	const unsigned WIDTH = 1280;
	const unsigned HEIGHT = 720;
	RenderWindow window9(VideoMode(WIDTH, HEIGHT), "Collisions & Views!", Style::Default);
	window9.setFramerateLimit(60);

	// View
	View view;
	view.setSize(WIDTH, HEIGHT);
	view.setCenter(WIDTH / 2, HEIGHT / 2);
	float viewspeed = 20.f;

	// Clock
	Clock clock;
	float dt = 0.f;
	const float FPS = 60.f;

	// GridSize
	const float gridsize = 50.f; // like making grids of window of single grid length
	const unsigned gridsizeU = static_cast<unsigned>(gridsize);

	// Mouse Conifg
	Vector2i mousePosScreen = Mouse::getPosition();
	Vector2i mousePosWindow;
	Vector2f mousePosView;
	Vector2u mousePosGrid;

	// UI
	Font font;
	font.loadFromFile("Fonts/notacumulus.ttf");
	Text text;
	text.setCharacterSize(15);
	text.setFillColor(Color::White);
	text.setFont(font);
	text.setPosition(WIDTH - text.getGlobalBounds().width, HEIGHT - text.getGlobalBounds().height);
	text.setString("TEST");

	// Player
	const float speed = 100.f;
	Vector2f velocity;
	RectangleShape Mover;
	FloatRect nextPos;
	Mover.setFillColor(Color::Green);
	Mover.setSize(Vector2f(gridsize, gridsize));
	Mover.setPosition(0.f, 0.f);
	//Mover.setPosition(Vector2f(window9.getSize().x/2, window9.getSize().y / 2));

	// Tile Map
	const int mapsize = 20;
	std::vector<std::vector<RectangleShape>> tileMap;
	tileMap.resize(mapsize, std::vector<RectangleShape>());
	
	for (int i = 0; i < mapsize; i++)
	{
		tileMap[i].resize(mapsize, RectangleShape());
		for (int j = 0; j < mapsize; j++)
		{
			tileMap[i][j].setSize(Vector2f(gridsize, gridsize));
			tileMap[i][j].setFillColor(Color::Yellow);
			tileMap[i][j].setOutlineThickness(1.f);
			tileMap[i][j].setOutlineColor(Color::Blue);
			tileMap[i][j].setPosition(i * gridsize, j * gridsize);
		}
	}
	// For a vary large tile map, render the whole could make the app slow
	// in that situations we make a window from the tile map for we need to render
	int fromX = 0;
	int toX = 0;
	int fromY = 0;
	int toY = 0;

	// Tile Elements
	RectangleShape tileSelector(Vector2f(gridsize, gridsize));
	tileSelector.setFillColor(Color::Transparent);
	tileSelector.setOutlineThickness(3.f);
	tileSelector.setOutlineColor(Color::Magenta);

	// Walls
	std::vector<RectangleShape> walls;

	RectangleShape wall;
	wall.setFillColor(Color::Red);
	wall.setSize(Vector2f(gridsize, gridsize));

	// example walls
	
	wall.setPosition(Vector2f(gridsize * 2, gridsize * 3));
	walls.push_back(wall);
	wall.setPosition(Vector2f(gridsize * 4, gridsize * 5));
	walls.push_back(wall);
	wall.setPosition(Vector2f(gridsize * 6, gridsize * 7));
	walls.push_back(wall);
	


	while (window9.isOpen())
	{
		// Cloct Timer for each frame
		dt = clock.restart().asSeconds();

		// Mouse Updation
		mousePosScreen = Mouse::getPosition(); // global coordinates according to whole device screen 
		mousePosWindow = Mouse::getPosition(window9); // local coordinates according to whole envirment window 
		// NOTE: In order to update a View related setting, the window needs to set to that view
		window9.setView(view);
		mousePosView = window9.mapPixelToCoords(mousePosWindow); // global coordinates according to set view
		if (mousePosView.x >= 0.f)
			mousePosGrid.x = mousePosView.x / gridsizeU;
		if (mousePosView.y >= 0.f)
			mousePosGrid.y = mousePosView.y / gridsizeU;
		// resting view
		window9.setView(window9.getDefaultView());

		// Update Tile
		tileSelector.setPosition(mousePosGrid.x * gridsize, mousePosGrid.y * gridsize);

		// UI update
		std::stringstream ss;
		ss << "Screen: (" << mousePosScreen.x << ", " << mousePosScreen.y << ")\n"
			<< "Window: (" << mousePosWindow.x << ", " << mousePosWindow.y << ")\n"
			<< "View: (" << mousePosView.x << ", " << mousePosView.y << ")\n"
			<< "Grid: (" << mousePosGrid.x << ", " << mousePosGrid.y << ")\n";

		text.setString(ss.str());
		text.setPosition(WIDTH - text.getGlobalBounds().width, HEIGHT - text.getGlobalBounds().height);


		// EVENT POLLING
		Event event;
		while (window9.pollEvent(event))
		{
			if (event.type == sf::Event::Closed || Keyboard::isKeyPressed(Keyboard::Escape))
				window9.close();
		}


		//Add walls
		if (Mouse::isButtonPressed(Mouse::Left))
		{
			bool exists = false;
			for (size_t i = 0; i < walls.size() && !exists; i++)
			{
				if (walls[i].getPosition().x / (int)gridsize == mousePosGrid.x
					&& walls[i].getPosition().y / (int)gridsize == mousePosGrid.y)
				{
					exists = true;
				}
			}

			if (!exists)
			{
				wall.setPosition(mousePosGrid.x * gridsize, mousePosGrid.y * gridsize);
				walls.push_back(wall);
			}
		}

		//Remove walls
		if (Mouse::isButtonPressed(Mouse::Right))
		{
			int index = -1;
			for (size_t i = 0; i < walls.size(); i++)
			{
				if (walls[i].getPosition().x / (int)gridsize == mousePosGrid.x
					&& walls[i].getPosition().y / (int)gridsize == mousePosGrid.y)
				{
					index = i;
					break;
				}
			}

			if (index != -1)
			{
				walls.erase(walls.begin() + index);
			}
		}

		// View Movement
		if (Keyboard::isKeyPressed(Keyboard::W))
		{
			view.move(0.f, -viewspeed * FPS * dt);
		}
		if (Keyboard::isKeyPressed(Keyboard::A))
		{
			view.move(-viewspeed * FPS * dt, 0.f);
		}
		if (Keyboard::isKeyPressed(Keyboard::D))
		{
			view.move(viewspeed * FPS * dt, 0.f);
		}
		if (Keyboard::isKeyPressed(Keyboard::S))
		{
			view.move(0.f, viewspeed * FPS * dt);
		}

		// Player Mover
		velocity = Vector2f(0.f, 0.f);
		if (Keyboard::isKeyPressed(Keyboard::Up))
		{
			velocity.y -= speed * FPS * dt;
		}
		if (Keyboard::isKeyPressed(Keyboard::Left))
		{
			velocity.x -= speed * FPS * dt;
		}
		if (Keyboard::isKeyPressed(Keyboard::Right))
		{
			velocity.x += speed * FPS * dt;
		}
		if (Keyboard::isKeyPressed(Keyboard::Down))
		{
			velocity.y += speed * FPS * dt;
		}
		//easy fix 
		//Mover.move(velocity);

		//Wall and collisions
		for (auto& wall : walls)
		{
			//easy fix
			/*if (wall.getGlobalBounds().intersects(Mover.getGlobalBounds()))
			{
				Mover.move(-velocity);
			}*/

			FloatRect MoverBounds = Mover.getGlobalBounds();
			FloatRect wallBounds = wall.getGlobalBounds();

			nextPos = MoverBounds;
			nextPos.left += velocity.x;
			nextPos.top += velocity.y;

			if (wallBounds.intersects(MoverBounds))
			{
				//  *------------*  (Top Left and right Corners)
				//  |            |
				//  |            |
				//  |            |
				//  *------------* (Bottom left and right corners)

				// Left Wall Collision
				if (MoverBounds.left < wallBounds.left
					&& MoverBounds.left + MoverBounds.width < wallBounds.left + wallBounds.width
					&& MoverBounds.top + MoverBounds.height > wallBounds.top
					&& MoverBounds.top < wallBounds.top + wallBounds.height)
				{
					velocity.x = 0;
					Mover.setPosition(wallBounds.left - MoverBounds.width, MoverBounds.top);
				}

				// Right Wall Collision
				if (MoverBounds.left > wallBounds.left
					&& MoverBounds.left + MoverBounds.width > wallBounds.left + wallBounds.width
					&& MoverBounds.top + MoverBounds.height > wallBounds.top
					&& MoverBounds.top < wallBounds.top + wallBounds.height)
				{
					velocity.x = 0;
					Mover.setPosition(wallBounds.left + wallBounds.width, MoverBounds.top);
				}

				// Top Wall Collision
				if (MoverBounds.top < wallBounds.top
					&& MoverBounds.top + MoverBounds.height < wallBounds.top + wallBounds.height / 4 //to be more precise, put: MoverBounds.top + MoverBounds.height < wallBounds.top
					&& MoverBounds.left + MoverBounds.width > wallBounds.left
					&& MoverBounds.left < wallBounds.left + wallBounds.width)
				{
					velocity.y = 0;
					Mover.setPosition(MoverBounds.left, wallBounds.top - MoverBounds.height);
				}

				// Bottom Wall Collision
				if (MoverBounds.top > wallBounds.top
					&& MoverBounds.top > wallBounds.top + 3 * wallBounds.height / 4 //to be more precise, put: MoverBounds.top > wallBounds.top + wallBounds.height
					&& MoverBounds.left + MoverBounds.width > wallBounds.left
					&& MoverBounds.left < wallBounds.left + wallBounds.width)
				{
					velocity.y = 0;
					Mover.setPosition(MoverBounds.left, wallBounds.top + wallBounds.height);
				}

			}
		}
		Mover.move(velocity);


		//Collision screen 
		//Left collision 
		if (Mover.getPosition().x < 0.f)
			Mover.setPosition(0.f, Mover.getPosition().y);
		//Top collision 
		if (Mover.getPosition().y < 0.f)
			Mover.setPosition(0.f, Mover.getPosition().y);
		//Right collision
		if (Mover.getPosition().x + Mover.getGlobalBounds().width > WIDTH)
			Mover.setPosition(WIDTH - Mover.getGlobalBounds().width, Mover.getPosition().y);
		//Bottom collision 
		if (Mover.getPosition().y + Mover.getGlobalBounds().height > HEIGHT)
			Mover.setPosition(Mover.getPosition().x, HEIGHT - Mover.getGlobalBounds().height);



		// Render
		window9.clear();

		// View
		window9.setView(view);
		// Draw Game related stuff
		
		// tile maping
		// Mouse pointer Tile Rendeing
		//fromX = mousePosGrid.x - 2;		toX = mousePosGrid.x + 3;
		//fromY = mousePosGrid.y - 2;		toY = mousePosGrid.y + 3;

		// View Center Tile Rendering
		fromX = view.getCenter().x / gridsize - 2;		toX = view.getCenter().x / gridsize + 3;
		fromY = view.getCenter().y / gridsize - 2;		toY = view.getCenter().y / gridsize + 3;


		if (fromX < 0) fromX = 0;
		else if (fromX > mapsize) fromX = mapsize - 1;
		if (fromY < 0) fromY = 0;
		else if (fromY > mapsize) fromY = mapsize - 1;

		if (toX < 0) toX = 0;
		else if (toX > mapsize) toX = mapsize - 1;
		if (toY < 0) toY = 0; 
		else if (toY > mapsize) toY = mapsize - 1;

		for (int i = fromX; i < toX; i++)
		{
			for (int j = fromY; j < toY; j++)
			{
				window9.draw(tileMap[i][j]);
			}
		}
		window9.draw(Mover);
		window9.draw(tileSelector);
		for (auto& wall : walls)
		{
			window9.draw(wall);
		}

		// Default window view
		window9.setView(window9.getDefaultView());		
		// Draw UI related stuff
		window9.draw(text);
		window9.display();
	}

	return 0;
}