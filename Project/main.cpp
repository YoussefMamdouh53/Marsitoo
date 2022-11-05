#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Network.hpp>
#include <SFML/System.hpp>
#include <bits/stdc++.h>
#include <chrono>

#include "Collision.hpp"

#define ROCKET_SCALE 0.1f
#define PI 3.14f

#define START 0
#define MAIN 1
#define GAME_OVER 2

using namespace sf;

typedef struct
{
    float vx;
    float vy;
    float x;
    float y;
    float angle;
    float force;
} Rocket;

std::chrono::time_point<std::chrono::system_clock> start, end;

RenderWindow app(VideoMode(1900,900), "Project");
Font font;
Texture rocketT, bgT, flameT, rockT, groundT, gameoverT;
Sprite rocket_obj, bg, flame, rock, ground, gameoverImg;
Rocket rocket;
Text vx_speed_txt, vy_speed_txt, altitude_txt, fuel_txt, time_txt, angle_txt, readytxt;

Music intro;
SoundBuffer boom_buf, rocket_buf;
Sound boom, rocketEngine;

bool boomOn = false;
bool startOn = false;
bool engineOn = false;

IpAddress ip = IpAddress::getLocalAddress();
UdpSocket socket;

float flameScale = 0.1;
float g = 0.2;
float fuel = 100;
long time_frame = 0;

int gameView = START;

void rotate_rocket_r()
{
    if (rocket.angle < 45)
    {
        rocket.angle += 1;
    }
}
void rotate_rocket_l()
{
    if (rocket.angle > -45)
    {
        rocket.angle -= 1;
    }
}

void change_speed(int s)
{
    switch (s)
    {
    case 0:
        rocket.force = 0;
        flameScale = 0;
        rocketEngine.setVolume(0);
        break;
    case 1:
        rocket.force = 0.05;
        flameScale=0.06;
        rocketEngine.setVolume(25);

        break;
    case 2:
        rocket.force = 0.10;
        flameScale=0.08;
        rocketEngine.setVolume(50);

        break;
    case 3:
        rocket.force = 0.19;
        flameScale=0.1;
        rocketEngine.setVolume(75);

        break;
    case 4:
        rocket.force = 0.30;
        flameScale=0.12;
        rocketEngine.setVolume(100);

        break;
    }
}

void update()
{
    if (fuel <= 0) change_speed(0);

    float fx = rocket.force * std::sin(rocket.angle * PI / 180);
    float fy = rocket.force * std::cos(rocket.angle * PI / 180);
    rocket.vy += -fy;
    rocket.vx += fx;
    rocket.vy += g;

    rocket.y += rocket.vy;
    rocket.x += rocket.vx;

    fuel -= rocket.force * 0.05;

    rocket_obj.setRotation(rocket.angle);
    rocket_obj.setPosition({rocket.x,rocket.y});

    flame.setPosition({rocket.x,rocket.y});
    flame.setRotation(rocket.angle);
    flame.setScale({flameScale,-flameScale});


    if (rocket.x > app.getSize().x + rocket_obj.getGlobalBounds().width / 2 || rocket.x < -rocket_obj.getGlobalBounds().width/2 || rocket.y < -rocket_obj.getGlobalBounds().height/2)
    {
        std::cout << "gameover" << std::endl;
        gameView = GAME_OVER;

    }

    if (rocket_obj.getGlobalBounds().intersects(ground.getGlobalBounds()))
    {
        if (rocket.vy > 2.5 || rocket.angle > 5 || rocket.angle < -5)
        {
            std::cout << "Gameover" << std::endl;
            gameView = GAME_OVER;

        }
        else
        {
            change_speed(0);
            rocket.vy = 0;
            rocket.vx = 0;
            rocket.y = ground.getPosition().y - rocketT.getSize().y * ROCKET_SCALE / 2;
            rocket.angle = 0;
        }
    }

    end = std::chrono::system_clock::now();
     std::chrono::duration<double> elapsed_seconds = end - start;
    std::time_t end_time = std::chrono::system_clock::to_time_t(end);

    //Update displayed information..
    angle_txt.setString("Angle: " + std::to_string((int)rocket.angle));
    vx_speed_txt.setString("Horizontal Speed : " + std::to_string(rocket.vx));
    vy_speed_txt.setString("Vertical Speed: " + std::to_string(rocket.vy));
    altitude_txt.setString("Altitude: " + std::to_string(app.getSize().y - rocket.y - rocket_obj.getGlobalBounds().height / 2 - ground.getGlobalBounds().height));
    time_txt.setString("Time: " + std::to_string((int)elapsed_seconds.count()));
    fuel_txt.setString("Fuel: " + std::to_string((int)fuel) + "%");

    // Collision Detection..
    if (rocket_obj.getGlobalBounds().intersects(rock.getGlobalBounds()))
    {
        // Pixel Perfect Collision..
        if (Collision::PixelPerfectTest(rock, rocket_obj))
        {
            std::cout << "Collide" << std::endl;
            gameView = GAME_OVER;
        }
    }

}

void init()
{
    start = std::chrono::system_clock::now();

    bool loadState = true;
    if (!intro.openFromFile("sounds/start.wav")) app.close(), loadState = false;
    if (!boom_buf.loadFromFile("sounds/boom.wav")) app.close(), loadState = false;
    if (!rocket_buf.loadFromFile("sounds/rocket.wav")) app.close(), loadState = false;
    if (!rocketT.loadFromFile("images/rocket.png")) app.close(), loadState = false;
    if (!bgT.loadFromFile("images/bg.png")) app.close(), loadState = false;
    if (!flameT.loadFromFile("images/flame.png")) app.close(), loadState = false;
    if (!rockT.loadFromFile("images/rock.png")) app.close(), loadState = false;
    if (!groundT.loadFromFile("images/groundT2.jpg")) app.close(), loadState = false;
    if (!gameoverT.loadFromFile("images/gameover.jpg")) app.close(), loadState = false;
    if (!font.loadFromFile("fonts/neuropol.ttf")) app.close(), loadState = false;

    readytxt.setFont(font);
    readytxt.setString("Press Enter to Start!");
    readytxt.setCharacterSize(52);
    readytxt.setPosition(app.getSize().x/2 - readytxt.getGlobalBounds().width/2, app.getSize().y/2 - readytxt.getGlobalBounds().height/2);

    intro.setLoop(true);
    boom.setBuffer(boom_buf);
    rocketEngine.setBuffer(rocket_buf);
    rocketEngine.setVolume(75);

    if (!loadState)
    {
        std::cout << "Error loading files!" << std::endl;
    }

    rock.setTexture(rockT);
    rock.setPosition(app.getSize().x / 2, 700);
    rock.scale({0.5,0.5});

    rocket_obj.setTexture(rocketT);
    rocket_obj.scale({ROCKET_SCALE, ROCKET_SCALE});
    rocket_obj.setOrigin({rocketT.getSize().x / 2, rocketT.getSize().y / 2});

    rocket.x = app.getSize().x / 2;
    rocket.y = app.getSize().y / 2;
    rocket.angle = 0;

    change_speed(3);

    bgT.setRepeated(true);
    bg.setTextureRect({0,0,app.getSize().x,app.getSize().y});
    bg.setTexture(bgT);

    flame.setTexture(flameT);
    flame.setScale({flameScale,-flameScale});
    flame.setOrigin({flameT.getSize().x / 2, rocketT.getSize().y / 2 + rocketT.getSize().y / 2});

    groundT.setRepeated(true);
    ground.setTexture(groundT);
    ground.setTextureRect({0,0,app.getSize().x, 50});
    ground.setPosition({0, app.getSize().y - 50});

    vx_speed_txt.setFont(font);
    vy_speed_txt.setFont(font);
    altitude_txt.setFont(font);
    angle_txt.setFont(font);
    time_txt.setFont(font);
    fuel_txt.setFont(font);

    vx_speed_txt.setCharacterSize(16);
    vy_speed_txt.setCharacterSize(16);
    altitude_txt.setCharacterSize(16);
    angle_txt.setCharacterSize(16);
    time_txt.setCharacterSize(16);
    fuel_txt.setCharacterSize(16);

    vx_speed_txt.setString("Horizontal Speed : 55");
    vy_speed_txt.setString("Vertical: 55");
    altitude_txt.setString("Altitude: 100");
    angle_txt.setString("Angle: 45");
    time_txt.setString("Time: 10");
    fuel_txt.setString("Fuel: 55%");

    vx_speed_txt.setPosition({100,50});
    vy_speed_txt.setPosition({100,100});
    altitude_txt.setPosition({100,150});
    angle_txt.setPosition({600,50});
    time_txt.setPosition({600,100});
    fuel_txt.setPosition({600,150});

    gameoverImg.setTexture(gameoverT);
    gameoverImg.setOrigin({gameoverT.getSize().x/2,gameoverT.getSize().y/2});
    gameoverImg.setPosition({app.getSize().x/2,app.getSize().y/2});

}

void network()
{
    end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;
    std::time_t end_time = std::chrono::system_clock::to_time_t(end);
    if ((int)elapsed_seconds.count() == time_frame){
        return;
    }
    time_frame = (int)elapsed_seconds.count();

    /*if (socket.connect("10.171.224.130", 53000) != Socket::Done){
        std::cout << "error" <<std::endl;
        return;
    }*/

    std::string angle, vx, vy, f, alt;
    angle = "Angle: " + std::to_string((int)rocket.angle);
    vx = "Horizontal Speed : " + std::to_string(rocket.vx);
    vy = "Vertical Speed: " + std::to_string(rocket.vy);
    alt = "Altitude: " + std::to_string(app.getSize().y - rocket.y - rocket_obj.getGlobalBounds().height / 2 - ground.getGlobalBounds().height);
    f = "Fuel: " + std::to_string((int)fuel) + "%";
    std::string send = angle + "," + vx + "," + vy + ","   + alt  + ","  + f;
    socket.send(send.c_str(), send.size() * sizeof(char), "10.171.224.130", 53000);
/*
    socket.send(angle.c_str(), sizeof(angle));
    socket.send(vx.c_str(), sizeof(vx));
    socket.send(vy.c_str(), sizeof(vy));
    socket.send(alt.c_str(), sizeof(alt));
    socket.send(f.c_str(), sizeof(f));*/
}

int main()
{
    app.setFramerateLimit(60);
    init();

    while (app.isOpen())
    {
        Event e;
        while (app.pollEvent(e))
        {
            if (e.type == Event::Closed)
                app.close();
        }

        if (gameView == GAME_OVER)
        {
            if (!boomOn)
            {
                boom.play();
                intro.stop();
                rocketEngine.stop();
                boomOn = true;
            }
            app.clear();
            app.draw(gameoverImg);
            app.display();
            continue;
        }

        if (gameView == START)
        {
            if (!startOn)
            {
                intro.play();
                rocketEngine.stop();
                boom.stop();
                startOn = true;
            }
            if (Keyboard::isKeyPressed(Keyboard::Enter)) gameView = MAIN;
            app.clear();
            app.draw(readytxt);
            app.display();
            continue;
        }

        if (!engineOn)
        {
            rocketEngine.play();
            intro.stop();
            boom.stop();
            engineOn = true;
        }

        if (Keyboard::isKeyPressed(Keyboard::Right))
        {
            rotate_rocket_r();
        }
        if (Keyboard::isKeyPressed(Keyboard::Left))
        {
            rotate_rocket_l();
        }
        if (Keyboard::isKeyPressed(Keyboard::Num0))
        {
            change_speed(0);
        }
        if (Keyboard::isKeyPressed(Keyboard::Num1))
        {
            change_speed(1);
        }
        if (Keyboard::isKeyPressed(Keyboard::Num2))
        {
            change_speed(2);
        }
        if (Keyboard::isKeyPressed(Keyboard::Num3))
        {
            change_speed(3);
        }
        if (Keyboard::isKeyPressed(Keyboard::Num4))
        {
            change_speed(4);
        }

        update();
        Thread th(&network);
        th.launch();

        app.clear();

        app.draw(bg);
        app.draw(flame);
        app.draw(rocket_obj);
        app.draw(ground);
        app.draw(rock);

        app.draw(fuel_txt);
        app.draw(time_txt);
        app.draw(altitude_txt);
        app.draw(vx_speed_txt);
        app.draw(vy_speed_txt);
        app.draw(angle_txt);

        app.display();

    }
}
