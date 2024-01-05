#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <sstream> // for ostringstream
#include <iomanip> // for setPricision
#include <string>
#include <vector>
#include <windows.h>

// 어서와 c++ 은 처음이지 -> 7장 SFML LunarLander 게임을 수정한 코드임.
// 각각의 이미지 파일, 폰트, 소리 파일은 다운로드 해야한다.

using namespace sf;
using namespace std;

class LunarLander {
public:
    static bool success_status;
    static bool game_over_status;
    static int stage;

private:
    double x;
    double y;
    double height;
    double fuel;
    double velocity;

    string status;
    string stage_string;

    Sprite spaceship;
    Sprite burst;
    Texture t1;
    Texture t2;
    
    Text text;
    Text stage_text;

    Font font;

    void Cal_height();

public:
    LunarLander(double h, double v, double f);
    void checklanded();
    void update(bool up_keypressed = false);
    void draw(RenderWindow& window);
    LunarLander& operator=(const LunarLander&);
};

bool LunarLander::success_status = false;
bool LunarLander::game_over_status = false;
int LunarLander::stage = 1;

void LunarLander::Cal_height() {
    height = 576 - y; //640 + -y - 64;
}

LunarLander::LunarLander(double h, double v, double f) : x(300), y(h), velocity(v), fuel(f) {
    Cal_height();

    if (!t1.loadFromFile("images/ufo.png")) {
        cerr << "Error loading ufo.png" << endl;
    }
    spaceship.setTexture(t1);
    spaceship.setPosition(x, y);

    if (!t2.loadFromFile("images/burn.png")) {
        cerr << "Error loading burn.png" << endl;
    }
    burst.setTexture(t2);
    burst.setPosition(x + 17, y + 58);

    if (!font.loadFromFile("Font/Consolas.ttf")) {
        cerr << "Error loading Consolas.ttf" << endl;
    }
    text.setFont(font);
    text.setCharacterSize(20);

    stage_text.setFont(font);
    stage_text.setCharacterSize(50);

}

void LunarLander::checklanded() {
    if (-10.0 <= velocity && velocity <= 10.0 &&  height <= 5.0) {
        success_status = true;
    }
    else if (height >= 640 || height <= 0) {
        game_over_status = true;
    }
    else if(fuel <= 0){
        game_over_status = true;
    }
}

void LunarLander::update(bool up_keypressed) {
    if (fuel <= 0) return;

    velocity += 0.3;

    if (up_keypressed && fuel > 0) {
        fuel -= 3;
        velocity -= 0.5;
    }

    y += velocity;
    Cal_height();

    spaceship.setPosition(x, y);
    burst.setPosition(x + 17, y + 58);

    if (up_keypressed) {
        burst.setScale(1.0f, 1.4f);
    }
    else {
        burst.setScale(1.0f, 1.0f);
    }
    
    std::ostringstream out;
    out << fixed << std::setprecision(1) << height;
    string height_str = out.str();

    out.str("");
    out.clear();

    out << velocity;
    string velocity_str = out.str();

    out.str("");
    out.clear();

    out << fixed << std::setprecision(0) << fuel;
    string fuel_str = out.str();

    status = "Press up key for mission\nHeight: " + height_str + "\nVelocity: " + velocity_str + "\nFuel: " + fuel_str;
    // 소수점 한자리가 나오도록 설정하고 싶음.
}

void LunarLander::draw(RenderWindow& window) {
    
    window.draw(spaceship);
    window.draw(burst);
    
    text.setString(status);
    text.setPosition(10, 70);

    stage_string = "Stage : " + to_string(stage);
    stage_text.setString(stage_string);

    sf::FloatRect textRect = text.getLocalBounds();
    stage_text.setOrigin(textRect.left + textRect.width / 2.0f,
        textRect.top);
    stage_text.setPosition(sf::Vector2f(window.getSize().x / 2.0f, 0));

    window.draw(text);
    window.draw(stage_text);
    
    if (success_status || game_over_status) {
        Text end_text;
        Font end_font;

        end_font.loadFromFile("Font/Consolas.ttf");
        end_text.setFont(end_font);

        end_text.setCharacterSize(30);
        end_text.setString(success_status ? "Mission Complete!" : "Game Over!");
        end_text.setFillColor(success_status ? Color::Green : Color::Red);

        sf::FloatRect end_textRect = end_text.getLocalBounds();

        end_text.setOrigin(end_textRect.left + end_textRect.width / 2.0f,
            end_textRect.top + end_textRect.height / 2.0f);
        
        end_text.setPosition(sf::Vector2f(window.getSize().x / 2.0f, window.getSize().y / 2.0f + 100.0f));

        window.draw(end_text);
    }
}

LunarLander& LunarLander::operator=(const LunarLander& orig) {
    this->y = orig.y;
    this->velocity = orig.velocity;
    this->fuel = orig.fuel - 100 * (this->stage - 1);

    Cal_height();

    if (!t1.loadFromFile("images/ufo.png")) {
        cerr << "Error loading ufo.png" << endl;
    }
    spaceship.setTexture(t1);
    spaceship.setPosition(x, y);

    if (!t2.loadFromFile("images/burn.png")) {
        cerr << "Error loading burn.png" << endl;
    }
    burst.setTexture(t2);
    burst.setPosition(x + 17, y + 58);

    if (!font.loadFromFile("Font/Consolas.ttf")) {
        cerr << "Error loading Consolas.ttf" << endl;
    }
    text.setFont(font);
    text.setCharacterSize(20);

    stage_text.setFont(font);
    stage_text.setCharacterSize(50);

    return *this;
}

int main() {
    RenderWindow window(VideoMode(640, 640), "LunarLander");
    window.setFramerateLimit(60);

    Texture t;
    Sprite background;
    if (!t.loadFromFile("images/space.png")) {
        cerr << "Error loading space.png" << endl;
    }
    background.setTexture(t);

    SoundBuffer booster_buffer;
    SoundBuffer stage_end_buffer;

    Sound booster;
    Sound stage_end;

    booster_buffer.loadFromFile("booster.mp3");
    booster.setBuffer(booster_buffer);

    LunarLander lander(300.0, 1.0, 2100.0);

    while (window.isOpen()) {
        Event e;
        while (window.pollEvent(e)) {
            if (e.type == Event::Closed) {
                window.close();
            }
        }

        lander.checklanded();

        if (lander.success_status || lander.game_over_status) {
            Event temp;

            if (lander.success_status) {
                stage_end_buffer.loadFromFile("winning.mp3");
            }
            else {
                stage_end_buffer.loadFromFile("losing.mp3");
            }

            stage_end.setBuffer(stage_end_buffer);

            window.clear();
            window.draw(background);
            lander.draw(window);
            stage_end.play();
            window.display();

            while (window.waitEvent(temp)) {
                if (temp.type == Event::MouseButtonPressed) {
                    if (temp.mouseButton.button == Mouse::Left) {
                        // 마우스 왼쪽 버튼을 누르면 게임을 재시작한다.
                        lander = LunarLander(300, 1.0, 2100); // 적절한 초기화 파라미터를 설정하세요.
                        if(lander.success_status) 
                            ++lander.stage; // 스테이지를 증가시킨다.
                        LunarLander::success_status = false;
                        LunarLander::game_over_status = false;
                        break; // 이벤트 처리를 마치고 루프를 빠져나간다.
                    }
                    else if (temp.mouseButton.button == Mouse::Right) {
                        // 마우스 오른쪽 버튼을 누르면 윈도우를 닫는다.
                        window.close();
                        break; // 이벤트 처리를 마치고 루프를 빠져나간다.
                    }
                }
                else if(temp.type == Event::Closed) {
                    window.close();
                    break;
                }
            }
        }
        else {
            if (Keyboard::isKeyPressed(Keyboard::Up)) {
                booster.play();
                lander.update(true);
            }
            else {
                lander.update();
            }
        }
        window.clear();
        window.draw(background);
        lander.draw(window);
        window.display();
    }
    return 0;
}