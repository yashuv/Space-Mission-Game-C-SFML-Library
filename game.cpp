#include<iostream>
#include"SFML\Graphics.hpp"
#include"SFML\Window.hpp"
#include"SFML\System.hpp"
#include"SFML\Audio.hpp"
#include<cstdlib>
#include<vector>
#include<windows.h>

using namespace sf;

void main_menu();
void instructions();

int main_called = 0;

int main()
{
    srand(time(NULL));

    if(main_called == 0)
        main_menu();

    RenderWindow window(VideoMode(800, 600), "Space Mission Game");
    window.setFramerateLimit(60);

//Background
	Texture background;
	background.loadFromFile("Textures/background.png");
	Sprite bg;
	bg.setTexture(background);

//Player
    Sprite player;
    Texture playerTex;
	playerTex.loadFromFile("Textures/player.png");
    player.setTexture(playerTex);
    player.setPosition(400.f, 500.f);
    int life = 3;
    int level = 1;
    bool pause=false;
    int Enemy_kill = 5;
    int score = -1;


//Enemy
    float Ymove = 3.f;
    Texture enemyTex;
	enemyTex.loadFromFile("Textures/enemy.png");

    Sprite enemy;
    enemy.setTexture(enemyTex);
    int enemySpawnTimer = 0;

    std::vector<Sprite> enemies;
    enemies.push_back(Sprite(enemy));

//Bullet
    Texture bulletTex;
	bulletTex.loadFromFile("Textures/bullet.png");
	Sprite bullet;
	bullet.setTexture(bulletTex);
	std::vector<Sprite> bullets;
	bullets.push_back(Sprite(bullet));
	int shootTimer = 20;

//Texts
    Font font;
	font.loadFromFile("Fonts/Arial.ttf");

    Text scoreText;
    scoreText.setFont(font);
	scoreText.setCharacterSize(20);
	scoreText.setFillColor(Color::White);
	scoreText.setPosition(10.f, 10.f);

	Text lifeText;
	lifeText.setFont(font);
	lifeText.setCharacterSize(20);
	lifeText.setFillColor(Color::White);
	lifeText.setPosition(715.f, 10.f);

	Text gameOverText;
	gameOverText.setFont(font);
	gameOverText.setCharacterSize(30);
	gameOverText.setStyle(Text::Bold);
	gameOverText.setFillColor(Color::Red);
	gameOverText.setPosition(340 , 250);
	gameOverText.setString("GAME OVER!");

	Text levelText;
	levelText.setFont(font);
	levelText.setCharacterSize(20);
	levelText.setFillColor(Color::White);
	levelText.setPosition(10.f, 32.f);

	Text levelDisplay;
	levelDisplay.setFont(font);
	levelDisplay.setCharacterSize(30);
	levelDisplay.setFillColor(Color::White);
	levelDisplay.setPosition(360 , window.getSize().y / 2);

//SOUNDS and Music
    SoundBuffer buffer_background;
    Sound background_sound;
    SoundBuffer buffer_shoot;
    Sound shoot_sound;
    sf::SoundBuffer buffer_explosion;
    sf::Sound explosion_sound;

    buffer_background.loadFromFile("Audios/background.wav");
	background_sound.setBuffer(buffer_background);
	background_sound.setLoop(true);
	background_sound.play();

	buffer_shoot.loadFromFile("Audios/laser.wav");
    shoot_sound.setBuffer(buffer_shoot);
    buffer_explosion.loadFromFile("Audios/explosion.wav");
    explosion_sound.setBuffer(buffer_explosion);



//GAME LOOP
game:
	while (window.isOpen() && (!pause))
	{
		Event event;
		while (window.pollEvent(event) && (!pause))
		{
			if (event.type == Event::Closed)
            {
                background_sound.pause();
                window.close();
            }

            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
            {
                background_sound.pause();
                window.close();
            }

            if ((event.type == Event::KeyPressed) && Keyboard::isKeyPressed(Keyboard::P))
                pause = true;
		}

//UPDATE
		//PLAYER
		if (Keyboard::isKeyPressed(Keyboard::Left))
            player.move(-10.f, 0.f);
        if (Keyboard::isKeyPressed(Keyboard::Right))
            player.move(10.f, 0.f);

        if(player.getPosition().x <= 0)
            player.setPosition(1.f,player.getPosition().y);
        if(player.getPosition().x >=735 )
            player.setPosition(735.f,player.getPosition().y);

        //BULLETS
        if (shootTimer < 15)
            shootTimer++;

        if (Keyboard::isKeyPressed(Keyboard::Space) && shootTimer >= 15) //Shooting
        {
            shoot_sound.play();

            bullet.setPosition(player.getPosition().x, player.getPosition().y);
            bullets.push_back(bullet);
            //bullets.setPosition(player.getPosition().x, player.getPosition().y);
            shootTimer = 0; //reset timer
        }

        for (size_t i = 0; i < bullets.size(); i++)
        {
            //Move
            bullets[i].move(0.f, -10.f);

            //Out of window bounds
            if (bullets[i].getPosition().y > window.getSize().y)
            {
                bullets.erase(bullets.begin() + i);
                break;
            }

            //Enemy collision
            for (size_t k = 0; k < enemies.size(); k++)
            {
                if (bullets[i].getGlobalBounds().intersects(enemies[k].getGlobalBounds()))
                {
                    score += 1;
                    explosion_sound.play();
                    enemies.erase(enemies.begin() + k);

                    bullets.erase(bullets.begin() + i);
                    break;
                }
            }
        }

        //ENEMIES
		for (size_t i = 0; i < enemies.size(); i++)
		{
			enemies[i].move(0.f, Ymove);

			if (enemies[i].getPosition().y >  window.getSize().y)
				enemies.erase(enemies.begin() + i);
            if (enemies[i].getGlobalBounds().intersects(player.getGlobalBounds()))
            {
                explosion_sound.play();
                enemies.erase(enemies.begin() + i);
                life -= 1;
            }
		}

		if (enemySpawnTimer < 40)
			enemySpawnTimer++;

		if (enemySpawnTimer >= 40)
		{
			enemy.setPosition(rand()%int(window.getSize().x - enemy.getGlobalBounds().width), 0.f);
			enemies.push_back(Sprite(enemy));
			enemySpawnTimer = 0;
		}

		if(score == Enemy_kill )
        {
            level++;
            Ymove += 5.f;

            levelDisplay.setString("Level : " + std::to_string(level));
            //Enemy_kill += Enemy_kill;

        }

        //UI Update
        scoreText.setString("Score : " + std::to_string(score));
        lifeText.setString("Life : " + std::to_string(life));
        levelText.setString("Level  : " +  std::to_string(level));
//DRAW ========================================================================
		window.clear();
		window.draw(bg);
        window.draw(player);
        window.draw(scoreText);
        window.draw(lifeText);
        window.draw(levelText);

        //Enemies
        for(size_t i=0; i<enemies.size(); i++)
            window.draw(enemies[i]);

        //Bullets
		for (size_t i = 0; i < bullets.size(); i++)
			window.draw(bullets[i]);

        if(score == Enemy_kill)
        {
            window.draw(levelDisplay);
            window.display();
            Enemy_kill += Enemy_kill;
            Sleep(2000);
        }


        if(life <= 0)
        {
            window.draw(gameOverText);
        }

        window.display();

        if(life <= 0)
            break;


	}
	while ((window.isOpen()) && (pause))
    {
        sf::Event event;
        while ((window.pollEvent(event)) && (pause))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
                return 1;
            }

            if ((event.type == Event::KeyPressed) && Keyboard::isKeyPressed(Keyboard::P))
            {
                pause = false;
                goto game;
            }
        }
        window.display();
    }


    Sleep(3000);

	return 0;
}

//FUNCTIONS
void main_menu()
{
    main_called = 1;
    RenderWindow window(VideoMode(800, 600), "Space Mission Game");

    Texture background;
	background.loadFromFile("Textures/menu.png");
	Sprite bg;
	bg.setTexture(background);

    while (window.isOpen())
	{
		Event event;
		while (window.pollEvent(event))
		{
			if (event.type == Event::Closed)
				window.close();
            if (event.type == sf::Event::KeyPressed)
            {
                if (event.key.code==sf::Keyboard::Enter)
                {
                    main();
                }

                  if (event.key.code==sf::Keyboard::Escape)  //close
                  {
                     window.close();
                     exit(0);
                  }

                  if ( Keyboard::isKeyPressed(Keyboard::LControl))
                  {
                      instructions();
                  }

                if ( Keyboard::isKeyPressed(Keyboard::RControl))
                  {
                      instructions();
                  }
            }
            window.clear();
            window.draw(bg);
            window.display();
        }

    }

}

void instructions()
{
    RenderWindow window(VideoMode(800, 600), "Space Mission Game");

    Texture background;
	background.loadFromFile("Textures/instructions.png");
	Sprite bg;
	bg.setTexture(background);

    while (window.isOpen())
	{
		Event event;
		while (window.pollEvent(event))
		{
			if (event.type == Event::Closed)
				window.close();
            if (event.type == sf::Event::KeyPressed)
            {
                  if (event.key.code==sf::Keyboard::Escape)  //close
                  {
                     main_menu();
                  }
            }
            window.clear();
            window.draw(bg);
            window.display();
        }
    }
}
