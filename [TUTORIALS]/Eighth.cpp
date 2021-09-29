#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <SFML/Network.hpp>
#include <SFML/Audio.hpp>

using namespace sf;

int main_8()
{
	RenderWindow window8(VideoMode(1980, 1080), "VIEW Example!", Style::Default);
	window8.setFramerateLimit(60);

	View mainView;
	// try out different dimentions to see the dfference
	mainView.setSize(600, 600);

	RectangleShape Mover;
	Mover.setSize(Vector2f(50.f,50.f));
	Mover.setPosition(Vector2f(500.f, 500.f));

	RectangleShape Bar;
	Bar.setSize(Vector2f(1000.f, 5.f));
	Bar.setPosition(Vector2f(50.f, 50.f));

	CircleShape Object;
	Object.setRadius(10.f);
	Object.setPosition(Vector2f(720.f, 720.f));

	while (window8.isOpen())
	{
		// EVENT POLLING
		Event event;
		while (window8.pollEvent(event))
		{
			if (event.type == sf::Event::Closed || Keyboard::isKeyPressed(Keyboard::Escape))
				window8.close();
		}


		// Update
		if (Keyboard::isKeyPressed(Keyboard::W) && Mover.getPosition().y > 0)
			Mover.move(0.f, -5.f);

		if (Keyboard::isKeyPressed(Keyboard::A) && Mover.getPosition().x > 0)
			Mover.move(-5.f, 0.f);

		if (Keyboard::isKeyPressed(Keyboard::D) && Mover.getPosition().x + Mover.getSize().x < window8.getSize().x)
			Mover.move(+5.f, 0.f);

		if (Keyboard::isKeyPressed(Keyboard::S) && Mover.getPosition().y + Mover.getSize().y < window8.getSize().y)
			Mover.move(0.f, +5.f);

		// this makes the view to make player position as view center
		// that is, it seems like the enviroment is moving in the reverse direction of player direction
		mainView.setCenter(Mover.getPosition());

		// Draw
		window8.clear();

		// Main View
		window8.setView(mainView);
		window8.draw(Mover);
		window8.draw(Object);

		// Defualt full window view
		// drawing in this view will not change the dementions of the object
		window8.setView(window8.getDefaultView());
		window8.draw(Bar);

		window8.display();
	}
	return 0;
}