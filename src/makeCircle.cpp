#include "../Include/makeCircle.h"
#include <iostream>

makeCircle::makeCircle(float radius, int x, int y) {
    circle.setRadius(radius);
    circle.setPosition(static_cast<float>(x), static_cast<float>(y));
    circle.setFillColor(sf::Color::White);
    isDragged = false;
}

void makeCircle::update(sf::Vector2i& mousePos, bool mousePressed, int& selectedIndex, int currentIndex, bool isDragging) {
    // Check if the mouse was initially pressed inside the circle to start dragging
        if (mousePressed) {
            if (circle.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                selectedIndex = currentIndex; // Lock onto this circle
            }
        }

        // Release the circle when the mouse button is released
        if (!mousePressed && selectedIndex == currentIndex) {
            selectedIndex = -1;
        }

        // Only move the circle if it’s the one currently selected

        if (selectedIndex == currentIndex) {
            circle.setPosition(static_cast<float>(mousePos.x) - circle.getRadius(), static_cast<float>(mousePos.y) - circle.getRadius());
        }
    }
  

void makeCircle::changeDisplacement(sf::Vector2f displacement) { 
     circle.setPosition(circle.getPosition() + displacement);  
}


void makeCircle::render(sf::RenderWindow& window) {
    window.draw(circle);
}

sf::FloatRect makeCircle::getBounds() {
    return circle.getGlobalBounds();
}

const sf::Vector2f& makeCircle::getPos() {
    return circle.getPosition();
}

void makeCircle::setPosition(sf::Vector2f& position) {
    circle.setPosition(position);
}

void makeCircle::addConnection(makeCircle* otherNode) {
    if (otherNode && otherNode != this && std::find(connections.begin(), connections.end(), otherNode) == connections.end()) {
        connections.push_back(otherNode);
    }
}

void makeCircle:: setcolor(sf::Color color) {
    circle.setFillColor(color); 
}

