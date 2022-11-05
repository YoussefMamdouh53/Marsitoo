#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include<iostream>
using namespace sf;

Text t1 , t2 , t3 , t4 ,t5 ;
Text *txt[5] = {&t1,&t2,&t3,&t4,&t5};
int it = 0;

int main()
{
	RenderWindow window(sf::VideoMode(1900, 900), "Rocket Dashboard!");
    window.setFramerateLimit(1);

    Font font;
    font.loadFromFile("dash/Dash-Horizon-Demo.otf");




    t1.setFont(font);
    t1.setCharacterSize(50);
    t1.setFillColor(Color::Red);
    t1.setPosition(50,100);

    t2.setFont(font);
    t2.setCharacterSize(50);
    t2.setFillColor(Color::Red);
    t2.setPosition(50,200);

    t3.setFont(font);
    t3.setCharacterSize(50);
    t3.setFillColor(Color::Red);
    t3.setPosition(900,100);

    t4.setFont(font);
    t4.setCharacterSize(50);
    t4.setFillColor(Color::Red);
    t4.setPosition(900,200);

    t5.setFont(font);
    t5.setCharacterSize(50);
    t5.setFillColor(Color::Red);
    t5.setPosition(900,300);

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == Event::Closed)
				window.close();
		}


        UdpSocket client;
        client.bind(53000);
        IpAddress sender;
        unsigned short int port;
        // Receive a message from the client
        char buffer[1024];
        size_t received = 0;
        client.receive(buffer, sizeof(buffer), received, sender, port);

        std::string str = buffer;
        int start = 0;
        int end = str.find(",");
        while (end != -1) {
            txt[it]->setString(str.substr(start, end - start));
            start = end + 1;
            end = str.find(",", start);
            it = (it + 1 ) % 5;
        }
        txt[it]->setString(str.substr(start, end - start));
        it = (it + 1 ) % 5;

		window.clear();
		window.draw(t1);
		window.draw(t2);
		window.draw(t3);
		window.draw(t4);
		window.draw(t5);
		window.display();

	}
return 0;
}
