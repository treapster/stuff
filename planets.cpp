#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <cmath>
#include <iostream>
#include <vector>

const float G = 2;
const float launch_speed_multiplier = 0.01;
const float view_multiplier = 1.1;
float sumulation_speed_multiplier = 1.0;
int width = 1920;
int height = 1080;

class planet {
public:
    float mass;
    sf::Vector2<float> speed;
    sf::CircleShape shape;
    sf::Vector2<float> center(void)
    {
        sf::Vector2<float> out;
        out.x = shape.getPosition().x + shape.getRadius();
        out.y = shape.getPosition().y + shape.getRadius();
        return out;
    }
};
float distance(sf::Vector2<float> x) { return sqrt(x.x * x.x + x.y * x.y); }
sf::Vector2<float> tofloat(sf::Vector2<int> x) {
    sf::Vector2<float> out;
    out.x = x.x;
    out.y = x.y;
    return out;
}
int todegrees(double x) { return int(x * 57.296); }

void update(std::vector<planet>& planet, sf::RenderWindow& window) {
    float dist;
    float force;
    sf::Vector2<float> unary;
    int n = planet.size();
    for (int i = 0; i < n - 1; i++) {
        for (int j = i + 1; j < n; j++) {
            dist = distance(planet[j].center() - planet[i].center());
            force = sumulation_speed_multiplier * (planet[j].mass * planet[i].mass) / (dist * dist);
            unary = (planet[j].center() - planet[i].center()) / dist;
            planet[i].speed += (unary * force * G) / planet[i].mass;
            planet[j].speed += ((-unary) * force * G) / planet[j].mass;
        }
    }
    for (int i = 0; i < n; i++) {
        planet[i].shape.move(sumulation_speed_multiplier * planet[i].speed);
        window.draw(planet[i].shape);
    }
}
void getPlanet(std::vector<planet>& planet, int i, sf::RenderWindow& window, sf::View& view) {
    bool flag = true;
    sf::Vector2<float> temp;
    sf::Event event;
    sf::RectangleShape line;
    sf::Vector2<float> lastPosition = window.mapPixelToCoords(sf::Mouse::getPosition(window));
    while (flag) {
        window.pollEvent(event);
        if (event.type == sf::Event::Closed) {
            window.close();
            break;
        }

        if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Right)) {
            temp = window.mapPixelToCoords(sf::Mouse::getPosition(window));
            sf::Vector2<float> move = lastPosition - temp;
            view.move(move);
            window.setView(view);
        }
        lastPosition = window.mapPixelToCoords(sf::Mouse::getPosition(window));
        if (event.type == sf::Event::MouseWheelMoved) {
            view.zoom(1 - event.mouseWheel.delta * 1.0 / 10);
            window.setView(view);
        }

        for (int j = 0; j < i; j++) {
            window.draw(planet[j].shape);
        }
        window.display();

        window.clear();

        while (sf::Mouse::isButtonPressed(sf::Mouse::Left) && flag) {
            temp = window.mapPixelToCoords(sf::Mouse::getPosition(window));
            planet[i].shape.setPosition(temp.x - planet[i].shape.getRadius(),
                temp.y - planet[i].shape.getRadius());

            for (int j = 0; j <= i; j++) {
                window.draw(planet[j].shape);
            }
            window.display();
            window.clear();
            while (!sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
                window.pollEvent(event);
                if (event.type == sf::Event::Closed) {
                    window.close();
                    break;
                }
                if (event.type == sf::Event::MouseWheelMoved) {
                    view.zoom(1 - event.mouseWheel.delta * 1.0 / 10);
                    window.setView(view);
                }
                if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Right)) {
                    temp = window.mapPixelToCoords(sf::Mouse::getPosition(window));
                    sf::Vector2<float> move = lastPosition - temp;
                    view.move(move);
                    window.setView(view);
                }
                lastPosition = window.mapPixelToCoords(sf::Mouse::getPosition(window));

                line.setSize(sf::Vector2<float>(
                    distance(window.mapPixelToCoords(sf::Mouse::getPosition(window)) - planet[i].center()),
                    3.0));
                line.setPosition(
                    window.mapPixelToCoords(sf::Mouse::getPosition(window)));
                temp = window.mapPixelToCoords(sf::Mouse::getPosition(window));
                if (temp.x < planet[i].center().x) {
                    line.setRotation(
                        todegrees(asin((planet[i].center().y - temp.y) / (distance(temp - planet[i].center())))));
                }
                else {
                    line.setRotation(
                        180 - todegrees(asin((planet[i].center().y - temp.y) / (distance(temp - planet[i].center())))));
                }
                planet[i].speed = (temp - planet[i].center()) * launch_speed_multiplier;
                for (int j = 0; j <= i; j++) {
                    window.draw(planet[j].shape);
                }
                window.draw(line);
                window.display();
                window.clear();
                flag = false;
            }
        }
    }
}

int main() {
    sf::Event event;
    int n;
    std::cout << "Enter planets number\n";
    std::cin >> n;
    std::cout << "Enter masses\n";
    std::vector<planet> planet(n);
    float tmp;

    for (int i = 0; i < n; i++) {
        std::cin >> tmp;
        planet[i].mass = tmp;
        planet[i].shape.setRadius(pow(tmp, 0.33333) * 2.0);
    }

    for (int i = 0; i < n; i++) {
        if (planet[i].mass > 1999)
            planet[i].shape.setFillColor(sf::Color(230, 30, 30));
        else
            planet[i].shape.setFillColor(sf::Color(50, 200, 50));
    }
    sf::RenderWindow window(sf::VideoMode(width, height), "Planets");
    window.setFramerateLimit(60);
    window.clear();
    window.display();
    sf::View view = window.getView();
    int i = 0;
    while (i < n) {
        if (!sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
            getPlanet(planet, i, window, view);
            i++;
        }
    }
    sf::RectangleShape pixel;
    pixel.setSize(sf::Vector2<float>(1.0f, 1.0f));
    sf::Vector2<float> lastPosition = window.mapPixelToCoords(sf::Mouse::getPosition(window));
    sf::Vector2<float> curPosition = window.mapPixelToCoords(sf::Mouse::getPosition(window));
    while (window.isOpen()) {
        window.pollEvent(event);
        if (event.type == sf::Event::Closed) {
            window.close();
            return 0;
        }

        if (window.hasFocus()) {
            if (event.type == sf::Event::MouseWheelMoved) {
                view.zoom(1 - event.mouseWheel.delta * 1.0 / 10);
                window.setView(view);
            }
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::D) {
                    sumulation_speed_multiplier += 0.15;
                    std::cout << 0.15 << "\n";
                }
                if (event.key.code == sf::Keyboard::A) {
                    sumulation_speed_multiplier -= 0.15;
                    std::cout << -0.15 << "\n";
                }
            }
            if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
                curPosition = window.mapPixelToCoords(sf::Mouse::getPosition(window));
                sf::Vector2<float> move = lastPosition - curPosition;
                view.move(move);
                window.setView(view);
            }
            window.clear();
            lastPosition = window.mapPixelToCoords(sf::Mouse::getPosition(window));
            update(planet, window);
            window.display();
        }
    }
}
