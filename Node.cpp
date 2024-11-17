// Node.cpp
#include "Node.h"

Node::Node(float radius, float x, float y) : radius(radius) {
    circle.setRadius(radius);
    circle.setPosition(x, y);
    circle.setFillColor(sf::Color::White);
}

void Node::update(const sf::Vector2i& mousePos, bool mousePressed, bool mouseReleased, Node*& source) {
    if (!source) {
        if (mousePressed && isMouseInside(mousePos)) {
            source = this;
        }
    }
    else {
        if (mouseReleased && isMouseInside(mousePos)) {
            source = nullptr;
        }
    }
}

void Node::render(sf::RenderWindow& window) {
    window.draw(circle);
}

bool Node::isMouseInside(const sf::Vector2i& mousePos) {
    sf::FloatRect bounds = circle.getGlobalBounds();
    return bounds.contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));
}

sf::Vector2f Node::getPosition() const {
    return circle.getPosition() + sf::Vector2f(radius, radius);
}
