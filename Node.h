// Node.h
#ifndef NODE_H
#define NODE_H

#include <SFML/Graphics.hpp>

class Node {
public:
    Node(float radius, float x, float y);

    void update(const sf::Vector2i& mousePos, bool mousePressed, bool mouseReleased, Node*& source);
    void render(sf::RenderWindow& window);
    bool isMouseInside(const sf::Vector2i& mousePos);
    sf::Vector2f getPosition() const;

private:
    sf::CircleShape circle;
    float radius;
};

#endif // NODE_H
