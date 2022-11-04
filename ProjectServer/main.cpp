#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include<iostream>
using namespace sf;

Text t1 , t2 , t3 , t4 ,t5 ;
//Text *txt[5] = {&t1,&t2,&t3,&t4,&t5};
int it = 0;

int main()
{
	RenderWindow window(sf::VideoMode(1900, 900), "SFML works!");
    //window.setFillColor(sf::Color::Green);
    window.setFramerateLimit(30);

    Font font;
    font.loadFromFile("dash/Dash-Horizon-Demo.otf");




    t1.setFont(font);
    t1.setString("Horizontal speed : ");
    t1.setCharacterSize(50);
    t1.setFillColor(Color::Red);
    t1.setPosition(50,100);
/*
    t2.setFont(font);
    t2.setString("vertical speed : ");
    t2.setCharacterSize(50);
    t2.setFillColor(Color::Red);
    t2.setPosition(50,200);

    t3.setFont(font);
    t3.setString("attitude  : ");
    t3.setCharacterSize(50);
    t3.setFillColor(Color::Red);
    t3.setPosition(900,100);

    t4.setFont(font);
    t4.setString("angle : ");
    t4.setCharacterSize(50);
    t4.setFillColor(Color::Red);
    t4.setPosition(900,200);

    t5.setFont(font);
    t5.setString("fuel : ");
    t5.setCharacterSize(50);
    t5.setFillColor(Color::Red);
    t5.setPosition(900,300);

*/
	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == Event::Closed)
				window.close();
		}



TcpListener listner;
    if(listner.listen(53000) != Socket::Done) {
        std::cout << "Error";
    }else{
        std::cout<<"Listen success"<<std::endl;
    }

    TcpSocket client;
    if(listner.accept(client) != Socket::Done) {
        std::cout << "Error";
    } else {
        // Receive a message from the client
            char buffer[1024];
            size_t received = 0;
            client.receive(buffer, sizeof(buffer), received);
            t1.setString(buffer);
            std::cout << buffer << std::endl;
    }


		window.clear();
		//window.draw();
		window.draw(t1);
		/*window.draw(t2);
		window.draw(t3);
		window.draw(t4);
		window.draw(t5);*/
		window.display();

	}
return 0;
}
