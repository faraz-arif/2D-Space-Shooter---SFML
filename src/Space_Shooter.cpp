#include <SFML/Graphics.hpp>

#include <iostream>

#include <SFML/Window.hpp>

#include<fstream>

#include<cstdlib>

#include<SFML/Audio.hpp>

#include<ctime>

using namespace std;

const int maxBullet = 130;

const int maxEnemy = 40;

const int maxBulletEnemey = 70;

const int maxShieldDrop = 1;

int gameProg = 1;


void openMenu(sf::Font &ft,sf::RenderWindow& window, sf::Sprite& BackGround);
void openInstructions(sf::Font& ft,sf::RenderWindow& window, sf::Sprite& BackGround);
void openPlay(sf::Font& ft, sf::RenderWindow& window, sf::Sprite& BackGround);
void openPause(sf::Font& ft, sf::RenderWindow& window, sf::Sprite& BackGround, int& eCount, int level, int eLevel[], sf::Texture& eT1, sf::Texture& eT2);
void openGameOver(sf::Font& ft, sf::RenderWindow& window, sf::Sprite& BackGround, int& eCount, int& level, int eLevel[], sf::Texture& eT1, sf::Texture& eT2);
void openGameVictory(sf::Font& ft, sf::RenderWindow& window, sf::Sprite& BackGround, int& eCount, int& level, int eLevel[], sf::Texture& eT1, sf::Texture& eT2);
void openLoadGame(sf::Font& ft, sf::RenderWindow& window, sf::Sprite& BackGround, int &eCount);

void sGame(int score, int live, int eCount);

void reset(int &eCount, int &level, int eLevel[], sf::Texture &eT1, sf::Texture &eT2);


struct Bullet {

    sf::CircleShape shape;

    float speed = 20.0f;

    bool on = false;

};

struct eBullet {

    sf::CircleShape shape;

    float speed = 0.f;

    bool on = false;

};
struct Enemy {

    float posX = 0.0f;
    float posY = 0.0f;
    bool on = false;

    int texID = 1;
};

struct shield {
    float posX = 0.0f;
    float posY = 0.0f;
    float speed = 2.5f;
    bool on = false;
    bool shieldON = false;
};


Bullet bullets[maxBullet];
eBullet e_bullets[maxBulletEnemey];
Enemy enemies[maxEnemy];
shield SHIELD[maxShieldDrop];

static bool shieldActive() {
    for (int i = 0; i < maxShieldDrop; i++) {
        if (SHIELD[i].shieldON == true) {
            return true;
        }
    }
    return false;
}

static void spawnEnemies(Enemy enemies[], int& eCount, int count, sf::Texture &eT1, sf::Texture &eT2) {

    eCount = count; 
    float spacing = 198.0f;

    float start = 0.0f;

    for (int i = 0; i < count; i++) {

        int row = i / 10;
        int col = i % 10;

        float xe = start + col * spacing;
        float ye = 70.0f + row * 100.0f;

        enemies[i].posX = xe;
        enemies[i].posY = ye;
        enemies[i].on = true;
        
        if (i % 2 == 0) {
            enemies[i].texID = 1;
        }
        else {
        
            enemies[i].texID = 2;

        }
    }

}


int live = 3;


int score = 0;


static bool enemeyDead(Enemy enemies[], int eCount) {

    for (int i = 0; i < eCount; i++) {

        if (enemies[i].on == true) {
            return false;
        }

    }

    return true;

}


void playerMove(sf::Sprite &player,float plW, float playerBound, float wWidth);

void enemyMove(int level, int eCount, float windowYSize, float windowXSize);
 
void enemyBulletMove(float windowYSize);

void playerBulletMove(int maxBullet);

void playerEnemyCollision(int eCount, int& live, sf::Texture& eT1, sf::Texture& eT2, sf::Sprite &player, sf::RenderWindow &window, sf::Sprite & BackGround,sf::Font &ft, float orPlayerX, float orPlayerY,float orPlayerCX, float orPlayerCY);

void enemyBulletCollision(int eCount, sf::Texture& eT1, sf::Texture& eT2, int& score, int& hScore);

void playerShieldCollision(sf::Sprite &player, shield SHIELD[]);

void bulletofEnemyCollsion(sf::Texture& eT1, sf::Texture& eT2, sf::Sprite& player, int& live, sf::RenderWindow& window, sf::Sprite& BackGround, sf::Font& ft, float orPlayerX, float orPlayerY, float orPlayerCX, float orPlayerCY);


void bulletGenerate(sf::Sprite& player, float plW, int maxBullet);

void enemyShoot(float& eCdown, int eCount, int level, sf::Texture& eT1, sf::Texture& eT2);

void spawnShield(shield  SHIELD[], int eCount);
void shieldMove(shield SHIELD[], sf::Texture &sT, float windowYSize);

int main()
{
    sf::Clock delay;
    float bDelay = 0.2f;

    int hScore = 0;
    srand(int(time(0)));

    ifstream hSFile("Assets/highscore.txt");
    if (hSFile.is_open()) {
        hSFile >> hScore;
        hSFile.close();
    }
    
    sf::Font ft;
    if (!ft.openFromFile("Assets/arial.ttf")) {
        cout << "Error, Couldnt load!" << endl;
    }
    
    sf::Text stxt(ft);
    stxt.setCharacterSize(20);
    stxt.setFillColor(sf::Color::White);

    sf::Text htxt(ft);
    htxt.setCharacterSize(20);
    htxt.setFillColor(sf::Color::White);

    int bCount = 0;

    int eCount = 0;

    bool nextLevel = false;

    int level = 1;
    int eLevel[5] = { 10, 20, 30, 35 ,40 };
    sf::Texture eT1, eT2;

    if (!eT1.loadFromFile("Assets/enemy.png")) {

        cout << "Error, Couldnt load!" << endl;

    }
    if (!eT2.loadFromFile("Assets/enemy2.png")) {

        cout << "Error, Couldnt load!" << endl;

    }

    sf::Texture sT;
    if (!sT.loadFromFile("Assets/sheild.png")) {

        cout << "Error, Couldnt load!" << endl;

    }
    for (int i = 0; i < maxShieldDrop; i++) {
        SHIELD[i].on = false;
        SHIELD[i].shieldON = false;
    }
    
    sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "Space Shooter", sf::State::Fullscreen);
       
    sf::Texture bg;
    if (!bg.loadFromFile("Assets/background.png")) {

        cout << "Error, Couldnt load!" << endl;

    }
    sf::Music shootB;
    if (!shootB.openFromFile ("Assets/fire.mp3")) {
        cout << "Error, Couldnt load!" << endl;
    }
    
    sf::Sprite BackGround(bg);
    sf::Vector2u wSize = window.getSize();
    sf::Vector2u bgSize = bg.getSize();
    BackGround.setScale(sf::Vector2f(float(wSize.x) / bgSize.x , float(wSize.y) / bgSize.y ));
    
    spawnEnemies(enemies, eCount, eLevel[level - 1], eT1, eT2);
    
    sf::Texture plTex;

    if (!plTex.loadFromFile("Assets/player.png")) {

        cout << "Error, Couldnt load!" << endl;

    }
    float plW = 200.0f;
    float plH = 125.0f;
    
    sf::Sprite player(plTex);
    sf::Vector2u plSi = plTex.getSize();
    player.setScale(sf::Vector2f(plW / plSi.x, plH/plSi.y));

    float orPlayerX = player.getScale().x;
    float orPlayerY = player.getScale().y;
    player.setPosition(sf::Vector2f(wSize.x / 2.0f - plW / 2.0f, wSize.y - 125.0f));
    float orPlayerCX = player.getPosition().x;
    float orPlayerCY = player.getPosition().y;

    sf::Texture heart;

    if (!heart.loadFromFile("Assets/heart.png")) {
        
        cout << "Error, Couldnt load!" << endl;
    
    }

    sf::Sprite heartS(heart);
    heartS.setScale(sf::Vector2f(0.15f, 0.15f));
    window.setFramerateLimit(60); 

    float eCdown = 20.0f;


    while (window.isOpen()) {

        while (auto check = window.pollEvent()) {

            if (check->is<sf::Event::Closed>()) {

                window.close();

            }

            if (auto* keyEvent = check->getIf<sf::Event::KeyPressed>()) {

                if (keyEvent->code == sf::Keyboard::Key::Escape) {

                    window.close();

                }

                else if (keyEvent->code == sf::Keyboard::Key::Space) {
                    if (delay.getElapsedTime().asSeconds() >= bDelay) {
                        bulletGenerate(player, plW, maxBullet);
                        delay.restart();
                        shootB.play();
                    }
                }
                else if (keyEvent->code == sf::Keyboard::Key::P) {

                    gameProg = 4;

                }
                else if (keyEvent->code == sf::Keyboard::Key::S) {

                    sGame(score, live, eCount);

                }
            }
        }

        if (gameProg == 1) {
            openMenu(ft, window, BackGround);
        }
        if (gameProg == 2) {
            openInstructions(ft, window, BackGround);
        }
        if(gameProg == 3) {
            openPlay(ft, window, BackGround);

            enemyMove(level, eCount, float(window.getSize().y), float(window.getSize().x));

            eCdown -= 1.0f; 

            enemyShoot(eCdown, eCount, level, eT1, eT2);

            float wWidth = float(window.getSize().x);

            sf::FloatRect playerBound = player.getGlobalBounds();

            playerMove(player, plW, player.getPosition().x, wWidth);

            playerBulletMove(maxBullet);

            enemyBulletMove(float(window.getSize().y));

            enemyBulletCollision(eCount, eT1, eT2, score, hScore);

            shieldMove(SHIELD, sT, float(window.getSize().y));

            if (!nextLevel && enemeyDead(enemies, eCount)) {

                nextLevel = true;

            }

            playerShieldCollision(player, SHIELD);

            bulletofEnemyCollsion(eT1, eT2, player, live, window, BackGround, ft, orPlayerX, orPlayerY, orPlayerCX, orPlayerCY);

            playerEnemyCollision(eCount, live, eT1, eT2, player, window, BackGround, ft, orPlayerX,orPlayerY, orPlayerCX, orPlayerCY);

            if (nextLevel) {

                level++;

                 if (level > 5) {
                     gameProg = 7;
                 }

                 for (int i = 0; i < maxBulletEnemey; i++) {

                    e_bullets[i].on = false;

                 }

                 spawnEnemies(enemies, eCount, eLevel[level - 1], eT1, eT2);

                 nextLevel = false;

                 eCdown = 20.f;

            }

            spawnShield(SHIELD, eCount);

            window.clear();

            window.draw(BackGround);

            for (int i = 0; i < live; i++) {

                heartS.setPosition(sf::Vector2f(window.getSize().x - (i + 1) * (heart.getSize().x * 0.15f + 10), 10.0f));

                window.draw(heartS);
            }

            window.draw(player);

            for (int i = 0; i < maxBullet; i++) {

                if (bullets[i].on) {

                    window.draw(bullets[i].shape);

                }

            }

            for (int i = 0; i < maxShieldDrop; i++) {
                if (SHIELD[i].on) {
                    sf::Sprite sheild(sT);
                    sheild.setScale(sf::Vector2f(0.25f,0.25f));
                    sheild.setPosition(sf::Vector2f(SHIELD[i].posX, SHIELD[i].posY));
                    window.draw(sheild);
                }
            }

             for (int i = 0; i < eCount; i++) {

                if (enemies[i].on) {
                    sf::Sprite es(eT1);
                    if (enemies[i].texID == 1) {
                        es.setTexture(eT1);
                    }
                    else {
                        es.setTexture(eT2);
                    }
                    es.setScale(sf::Vector2f(0.25f, 0.25f));
                    es.setPosition(sf::Vector2f(enemies[i].posX, enemies[i].posY));
                    window.draw(es);

                }

             }

             for (int i = 0; i < maxBulletEnemey; i++) {

                if (e_bullets[i].on) {

                    window.draw(e_bullets[i].shape);

                }

             }

             stxt.setString("Score : " + to_string(score));
             htxt.setString("High Score : " + to_string(hScore));

             stxt.Bold;
             htxt.Bold;
             stxt.setCharacterSize(35);
             htxt.setCharacterSize(35);
             stxt.setFillColor(sf::Color(255,165,0));
             htxt.setFillColor(sf::Color(255,165,0));
             float dist = 20.0f;
             stxt.setPosition(sf::Vector2f(dist, dist));
             htxt.setPosition(sf::Vector2f(dist, 80.0f));

             window.draw(stxt);
             window.draw(htxt);

             window.display();    
        }
        if (gameProg == 4) {
            openPause(ft, window, BackGround, eCount, level, eLevel, eT1, eT2);
        }
        if (gameProg == 6) {
            openGameOver(ft, window, BackGround, eCount, level, eLevel, eT1, eT2);
        }
        if (gameProg == 7) {
            openGameVictory(ft, window, BackGround, eCount, level, eLevel, eT1, eT2);
        }
        if(gameProg == 5) {
            openLoadGame(ft, window, BackGround, eCount);
        }
    }

    ofstream hSwrite("Assets/highscore.txt");
    
    if (hSwrite.is_open()) {
    
        hSwrite << hScore;
        
        hSwrite.close();
    
    }
    
    return 0;

}

void playerMove(sf::Sprite &player, float plW, float playerBound, float wWidth) {

    float speed = 12.5f;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) {

        if (playerBound > speed - 25) {

            player.move(sf::Vector2f(-speed, 0));

        }

    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) {

        if (playerBound + plW < wWidth + 25) {

            player.move(sf::Vector2f(speed, 0));

        }

    }


}

void enemyMove(int level, int eCount, float windowYSize, float windowXSize) {

    float eFallSpeed = 0.4f + level * 1.5f;

    for (int i = 0; i < eCount; i++) {

        if (enemies[i].on) {

            enemies[i].posY += eFallSpeed;
            if (enemies[i].posY > windowYSize) {

                enemies[i].on = false;
            }

        }

    }
  
}

void enemyBulletMove(float windowYSize) {

    for (int i = 0; i < maxBulletEnemey; i++) {

        if (e_bullets[i].on) {

            e_bullets[i].shape.move(sf::Vector2f(0, e_bullets[i].speed));

            if (e_bullets[i].shape.getPosition().y > windowYSize + 10) {

                e_bullets[i].on = false;

            }

        }

    }

}

void playerBulletMove(int maxBullet) {

    for (int i = 0; i < maxBullet; i++) {

        if (bullets[i].on) {

            bullets[i].shape.move(sf::Vector2f(0, -bullets[i].speed));

            if (bullets[i].shape.getPosition().y + bullets[i].shape.getRadius() < 0) {

                bullets[i].on = false;

            }

        }

    }
}

void playerEnemyCollision(int eCount, int &live, sf::Texture& eT1, sf::Texture& eT2, sf::Sprite &player, sf::RenderWindow &window, sf::Sprite &BackGround, sf::Font &ft, float orPlayerX, float orPlayerY, float orPlayerCX, float orPlayerCY) {

    for (int i = 0; i < eCount; i++) {

        if (!enemies[i].on) {

            continue;

        }
        sf::Sprite temp(eT1);

        if (enemies[i].texID == 1) {
            temp.setTexture(eT1);
        }
        else {
            temp.setTexture(eT2);
        }

        temp.setPosition(sf::Vector2f(enemies[i].posX,enemies[i].posY));
        temp.setScale(sf::Vector2f(0.25f, 0.25f));
        sf::FloatRect e = temp.getGlobalBounds();
        sf::FloatRect p1 = player.getGlobalBounds();
        p1.position.x = p1.position.x + 5.0f;
        p1.position.y = p1.position.y + 5.0f;
        p1.size.x = p1.size.x - 10.0f;
        p1.size.y = p1.size.y - 10.0f;
        if (e.findIntersection(p1).has_value()) {

            enemies[i].on = false;
            bool shieldUsed = false;
            for (int j = 0; j < maxShieldDrop; j++) {
                if (SHIELD[j].shieldON) {
                    SHIELD[j].shieldON = false;
                    player.setScale(sf::Vector2f(orPlayerX, orPlayerY));
                    player.setPosition(sf::Vector2f(player.getPosition().x, orPlayerCY));
                    shieldUsed = true;
                    break;
                }
            }
            if (shieldUsed == true) {
                return;
            }
            live--;
            cout << "Lives lost! Remaining Lives = " << live << endl;
            if (live <= 0) { 
                gameProg = 6;
            }
        }
    }

}

void enemyBulletCollision(int eCount, sf::Texture &eT1, sf::Texture &eT2, int& score, int &hScore) {

    for (int i = 0; i < maxBullet; i++) {

        if (bullets[i].on) {

            for (int j = 0; j < eCount; j++) {

                if (enemies[j].on) {
                    sf::FloatRect p = bullets[i].shape.getGlobalBounds();

                    sf::Sprite temp(eT1);

                    if (enemies[j].texID == 1) {
                        temp.setTexture(eT1);
                    }
                    else {
                        temp.setTexture(eT2);
                    }

                    temp.setScale(sf::Vector2f(0.25f, 0.25f));
                    temp.setPosition(sf::Vector2f(enemies[j].posX,enemies[j].posY));

                    sf::FloatRect e = temp.getGlobalBounds();

                    if (p.findIntersection(e).has_value()) {

                        bullets[i].on = false;
                        enemies[j].on = false;
                        score += 10;

                        if (score > hScore) {
                            hScore = score;
                        }

                        break;

                    }
                }
            }

        }

    }

}

void bulletofEnemyCollsion(sf::Texture &eT1, sf::Texture &eT2, sf::Sprite &player, int &live, sf::RenderWindow &window, sf::Sprite &BackGround, sf::Font
    &ft, float orPlayerX,float orPlayerY, float orPlayerCX, float orPlayerCY) {

    for (int i = 0; i < maxBulletEnemey; i++) {

        if (e_bullets[i].on == false) {

            continue;

        }
        sf::FloatRect bu = e_bullets[i].shape.getGlobalBounds();
        sf::FloatRect p1 = player.getGlobalBounds();
        p1.position.x = p1.position.x + 5.0f;
        p1.position.y = p1.position.y + 5.0f;
        p1.size.x = p1.size.x - 10.0f;
        p1.size.y = p1.size.y - 10.0f;
        if (bu.findIntersection(p1).has_value()) {

            e_bullets[i].on = false;
            bool shieldUsed = false;
            for (int j = 0; j < maxShieldDrop; j++) {
                if (SHIELD[j].shieldON) {
                    SHIELD[j].shieldON = false;
                    player.setScale(sf::Vector2f(orPlayerX, orPlayerY));
                    player.setPosition(sf::Vector2f(player.getPosition().x, orPlayerCY));
                    shieldUsed = true;
                    break;
                }
            }
            if (shieldUsed == true) {
                return;
            }
    
            live--;

            if (live <= 0) {

                cout << "Game Over!" << endl;

                gameProg = 6;

            }

        }

    }

}

void bulletGenerate(sf::Sprite &player, float plW,int maxBullet) {
    for (int i = 0; i < maxBullet; i++) {

        if (!bullets[i].on) {

            bullets[i].shape.setRadius(5.0f);
            bullets[i].shape.setFillColor(sf::Color(255, 255, 0));
            bullets[i].shape.setOrigin(sf::Vector2f(bullets[i].shape.getRadius(), bullets[i].shape.getRadius()));
            plW = player.getGlobalBounds().getCenter().x;
            float bx = player.getGlobalBounds().getCenter().x;
            float by = player.getPosition().y - bullets[i].shape.getRadius();

            bullets[i].shape.setPosition(sf::Vector2f(bx, by));
            bullets[i].speed = 20.f;

            bullets[i].on = true;

            break;

        }

    }
}

void enemyShoot(float& eCdown, int eCount, int level, sf::Texture &eT1, sf::Texture &eT2)
{
    if (eCdown <= 0) {
        int shoot = rand() % 2 + 1;

        while (shoot--) {

            int in = rand() % eCount;

            if (!enemies[in].on) {

                continue;
            }

            for (int i = 0; i < maxBulletEnemey; i++) {

                if (!e_bullets[i].on) {


                    e_bullets[i].on = true;


                    float radius = 3.0f + level * 1.5f;
                    float speed = 3.7f + level * 2.5f;

                    sf::Sprite temp(eT1);

                    if (enemies[in].texID == 1) {
                        temp.setTexture(eT1);
                    }
                    else {
                        temp.setTexture(eT2);
                    }

                    temp.setScale(sf::Vector2f(0.25f, 0.25f));
                    temp.setPosition(sf::Vector2f(enemies[in].posX,enemies[in].posY));

                    sf::FloatRect ec = temp.getGlobalBounds();
                    e_bullets[i].shape.setRadius(radius);
                    e_bullets[i].shape.setOrigin(sf::Vector2f(radius, radius));

                    float ex = enemies[in].posX + ec.size.x / 2.0f;
                    float ey = enemies[in].posY + ec.size.y + 5.0f + radius;

                    e_bullets[i].shape.setFillColor(sf::Color(255, 0, 0));
                    e_bullets[i].shape.setPosition(sf::Vector2f(ex, ey));
                    e_bullets[i].speed = speed;

                    break;
                }

            }

        }

        eCdown = 10.0f + float(rand() % 10);

    }
}


void openMenu(sf::Font &ft,sf::RenderWindow& window, sf::Sprite& BackGround) {

    sf::Text title(ft);
    char ti[] = " Space Shooter ";
    title.setString(ti);
    title.setCharacterSize(80);
    sf::Text option1(ft);
    char op1[] = "\nPress 1 to Start Game \n";
    option1.setString(op1);
    option1.setCharacterSize(50);
    sf::Text option2(ft);
    char op2[] = "\nPress 2 to Load Existing Game \n";
    option2.setString(op2);
    option2.setCharacterSize(50);
    sf::Text option3(ft);
    char op3[] = "\nPress 3 to Instructions \n";
    option3.setString(op3);
    option3.setCharacterSize(50);
    sf::Text option4(ft);
    char op4[] = "\nPress 4 to Exit (ESCAPE will always close window) \n";
    option4.setString(op4);
    option4.setCharacterSize(50);

    title.setFillColor(sf::Color(0, 255, 133));
    title.Bold;

    option1.setFillColor(sf::Color::Cyan);
    option2.setFillColor(sf::Color::Cyan);
    option3.setFillColor(sf::Color::Cyan);
    option4.setFillColor(sf::Color::Cyan);

    title.setPosition(sf::Vector2f(300.0f, 30.0f));
    
    option1.setPosition(sf::Vector2f(300.0f, 100.0f));
   
    option2.setPosition(sf::Vector2f(300.0f, 170.0f));

    option3.setPosition(sf::Vector2f(300.0f, 240.0f));
   
    option4.setPosition(sf::Vector2f(300.0f, 310.0f));

    while (gameProg == 1) {

        window.clear();
        window.draw(BackGround);
        window.draw(title);
        window.draw(option1);
        window.draw(option2);
        window.draw(option3);
        window.draw(option4);
        window.display();

        while (auto check = window.pollEvent()) {

            if (check->is<sf::Event::Closed>()) {

                window.close();

            }

            if (auto* keyEvent = check->getIf<sf::Event::KeyPressed>()) {

                if (keyEvent->code == sf::Keyboard::Key::Num1) {

                    gameProg = 3;

                }
                else if (keyEvent->code == sf::Keyboard::Key::Num2) {

                    gameProg = 5;

                }
                else if (keyEvent->code == sf::Keyboard::Key::Num3) {

                    gameProg = 2;

                }
                else if (keyEvent->code == sf::Keyboard::Key::Num4) {

                    window.close();

                }
                else if (keyEvent->code == sf::Keyboard::Key::Escape) {

                    window.close();

                }
            }
        }
    }
}

void openInstructions(sf::Font &ft,sf::RenderWindow& window, sf::Sprite& BackGround) {

    sf::Text instruct(ft);
    char msgIns[] = "  Space Shooter Rules You Must Know !!! \n\n  Press Left / Right Arrows or A / D for movement \n\n  Press SPACE to shoot bullet \n\n  Press DELETE to go back to Menu";
    instruct.setString(msgIns);
    instruct.setFillColor(sf::Color(80,200,120));
    instruct.setCharacterSize(70);
    instruct.Bold;
    instruct.setPosition(sf::Vector2f(window.getSize().x / 8.0f, 150.0f));
    while (gameProg == 2) {

        window.clear();
        window.draw(BackGround);
        window.draw(instruct);
        window.display();

        while (auto check = window.pollEvent()) {

            if (check->is<sf::Event::Closed>()) {

                window.close();

            }

            if (auto* keyEvent = check->getIf<sf::Event::KeyPressed>()) {

                if (keyEvent->code == sf::Keyboard::Key::Delete) {

                    gameProg = 1;

                }

                else if (keyEvent->code == sf::Keyboard::Key::Escape) {

                    window.close();

                }
            }
        }
    }
}

void openPlay(sf::Font& ft, sf::RenderWindow& window, sf::Sprite& BackGround) {

    gameProg = 3;

}

void openPause(sf::Font& ft, sf::RenderWindow& window, sf::Sprite& BackGround, int &eCount, int level, int eLevel[], sf::Texture& eT1, sf::Texture& eT2) {

    sf::Text pause(ft);
    char oPause[] = " Game Paused !!! \n\n Press P to Play \n\n Press R to Restart \n\n Press ESC to close game ";
    pause.setString(oPause);
    pause.setFillColor(sf::Color::Yellow);
    pause.setCharacterSize(100);
    pause.Bold;
    pause.setPosition(sf::Vector2f(window.getSize().x /7.0f, 20.5f));
    
    while (gameProg == 4) {

        window.clear();
        window.draw(BackGround);
        window.draw(pause);
        window.display();
        sf::sleep(sf::seconds(0.5));
        while (auto check = window.pollEvent()) {

            if (check->is<sf::Event::Closed>()) {

                window.close();

            }

            if (auto* keyEvent = check->getIf<sf::Event::KeyPressed>()) {

                if (keyEvent->code == sf::Keyboard::Key::P) {

                    gameProg = 3;

                }
                else if (keyEvent->code == sf::Keyboard::Key::R) {

                    reset(eCount, level, eLevel, eT1, eT2);

                    gameProg = 3;

                }
                else if (keyEvent->code == sf::Keyboard::Key::Escape) {

                    window.close();

                }
                else if (keyEvent->code == sf::Keyboard::Key::S) {
                    sGame(score, live, eCount);
                }
            }
        }

    }
}

void openGameOver(sf::Font& ft, sf::RenderWindow& window, sf::Sprite& BackGround, int& eCount, int& level, int eLevel[], sf::Texture& eT1, sf::Texture& eT2) {

    char gameFinish[] = " You Lost :( \n Press R to Play Again \n Press Esc to Exit ";
    sf::Text displayGameFinish(ft);
    displayGameFinish.setString(gameFinish);
    displayGameFinish.setFillColor(sf::Color::Red);
    displayGameFinish.setCharacterSize(80);
    displayGameFinish.Bold;
    displayGameFinish.setPosition(sf::Vector2f(window.getSize().x / 4.0f, 150.0f));

    while (gameProg == 6) {

        window.clear();
        window.draw(BackGround);
        window.draw(displayGameFinish);
        window.display();
        while (auto check = window.pollEvent()) {

            if (check->is<sf::Event::Closed>()) {

                window.close();

            }

            if (auto* keyEvent = check->getIf<sf::Event::KeyPressed>()) {

                if (keyEvent->code == sf::Keyboard::Key::R) {

                    reset(eCount, level, eLevel, eT1, eT2);

                    gameProg = 3;

                }

                else if (keyEvent->code == sf::Keyboard::Key::Escape) {

                    window.close();

                }
            }
        }
        sf::sleep(sf::milliseconds(50));

    }
}

void openGameVictory(sf::Font& ft, sf::RenderWindow& window, sf::Sprite& BackGround, int& eCount, int& level, int eLevel[], sf::Texture &eT1, sf::Texture &eT2) {
    // victory.play();
    char gameFinish[] = " You Won !! \n Press R to Play Again \n Press Esc to Exit ";
    sf::Text displayGameFinish(ft);
    displayGameFinish.setString(gameFinish);
    displayGameFinish.setFillColor(sf::Color::Blue);
    displayGameFinish.setCharacterSize(100);
    displayGameFinish.Bold;
    displayGameFinish.setPosition(sf::Vector2f(window.getSize().x / 4.0f, 200.0f));
    while (gameProg == 7) {

        window.clear();
        window.draw(BackGround);
        window.draw(displayGameFinish);
        window.display();
        while (auto check = window.pollEvent()) {

            if (check->is<sf::Event::Closed>()) {

                window.close();

            }

            if (auto* keyEvent = check->getIf<sf::Event::KeyPressed>()) {

                if (keyEvent->code == sf::Keyboard::Key::R) {

                    reset(eCount, level, eLevel, eT1, eT2);

                    gameProg = 3;

                }

                else if (keyEvent->code == sf::Keyboard::Key::Escape) {

                    window.close();

                }
                else if (keyEvent->code == sf::Keyboard::Key::S) {
                    sGame(score, live, eCount);
                }
            }
        }
        sf::sleep(sf::milliseconds(500));

    }
}

void sGame(int score, int live, int eCount) {
    fstream saveFile("Assets/savegame.txt");
    if (saveFile.is_open()) {
        saveFile << score << " " << live << " " << eCount << "\n";
        for (int i = 0; i < eCount; i++) {
            saveFile << enemies[i].posX << " " << enemies[i].posY << " " << enemies[i].on << " " << enemies[i].texID << "\n";
        }
        saveFile.close();
        gameProg = 3;
        cout << " Game Saved !" << endl;
    }

}

void openLoadGame(sf::Font& ft, sf::RenderWindow& window, sf::Sprite& BackGround,int&eCount) {
    char gameFound[] = " Game Found :) \n Game will begin in 3 seconds. \n Enjoy your game!!";
    sf::Text displayGameFound(ft);
    displayGameFound.setString(gameFound);
    displayGameFound.setFillColor(sf::Color::Yellow);
    displayGameFound.setCharacterSize(60);
    displayGameFound.Bold;
    displayGameFound.setPosition(sf::Vector2f(window.getSize().x / 4.0f, 500.0f));

    sf::Text displayNotFound(ft);
    char gameNotFound[] = " Game Not Found :( \n\n You will b redirected to menu in 3 seconds \n\n Start a new game from there ";
    displayNotFound.setString(gameNotFound);
    displayNotFound.setFillColor(sf::Color::Red);
    displayNotFound.setCharacterSize(45);
    displayNotFound.Bold;
    displayNotFound.setPosition(sf::Vector2f(window.getSize().x / 4.0f, 10.0f));
    fstream loadFile("Assets/savegame.txt");
    if (loadFile.is_open()) {

        loadFile >> score >> live;
        loadFile >> eCount;
        for (int i = 0; i < eCount; i++) {
            loadFile >> enemies[i].posX >> enemies[i].posY >> enemies[i].on >> enemies[i].texID;
        }
        loadFile.close();
        window.clear();
        window.draw(BackGround);
        window.draw(displayGameFound);
        window.display();
        sf::sleep(sf::seconds(3));
        gameProg = 3;
    }
    else {
        window.clear();
        window.draw(BackGround);
        window.draw(displayNotFound);
        window.display();
        sf::sleep(sf::seconds(3));
        gameProg = 1;
    }

}
void reset(int& eCount, int& level, int eLevel[], sf::Texture& eT1, sf::Texture& eT2) {
    eCount = 0;
    level = 1;
    live = 3;
    score = 0;
    spawnEnemies(enemies, eCount, eLevel[level-1], eT1,eT2);
    for (int i = 0; i < maxBullet; i++) {
        bullets[i].on = false;
    }
    for (int i = 0; i < maxBulletEnemey; i++) {
        e_bullets[i].on = false;
    }
    for (int i = 0; i < maxShieldDrop; i++) {
        SHIELD[i].shieldON = false;
        SHIELD[i].on = false;
    }
    gameProg = 3;
}

void playerShieldCollision(sf::Sprite& player, shield SHIELD[]) {

    sf::FloatRect playerBOund = player.getGlobalBounds();
    bool shieldOn = shieldActive();
    for (int i = 0; i < maxShieldDrop; i++) {
        if (SHIELD[i].on) {
            sf::FloatRect shieldBound({ SHIELD[i].posX,SHIELD[i].posY}, {30.0f,30.0f});

            if (playerBOund.findIntersection(shieldBound).has_value()) {
                if (shieldOn) {
                    SHIELD[i].on = false;
                    continue;
                }
                SHIELD[i].shieldON = true;
                SHIELD[i].on = false;
                player.setScale(sf::Vector2f(player.getScale().x * 1.3f, player.getScale().y * 1.3f));
                player.setPosition(sf::Vector2f(player.getPosition().x, player.getPosition().y - 50)); 
                break;
            }
        }
    }
}
void spawnShield(shield  SHIELD[], int eCount) {
    if (rand() % 200 < 10) {
        for (int i = 0; i < maxShieldDrop; i++) {
            if (SHIELD[i].on == false && SHIELD[i].shieldON == false) {
                SHIELD[i].on = true;
                SHIELD[i].shieldON = false;
                if (eCount >= 1) {
                    int random = rand() % eCount;
                    SHIELD[i].posX = enemies[random].posX;
                    SHIELD[i].posY = enemies[random].posY;
                }
                else {
                    continue;
                }
            }
        }
    }
}

void shieldMove(shield SHIELD[], sf::Texture& sT, float windowYSize) {
    for (int i = 0; i < maxShieldDrop; i++) {
        if (SHIELD[i].on) {
            SHIELD[i].posY += SHIELD[i].speed;
            if (SHIELD[i].posY > windowYSize) {
                SHIELD[i].on = false;
            }
        }
    }
}