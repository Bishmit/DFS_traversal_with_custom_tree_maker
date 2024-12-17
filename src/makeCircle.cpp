#include "../Include/makeCircle.h"
#include <iostream>

makeCircle::makeCircle(float radius, int x, int y, sf::Font& font, int number)
{
    circle.setRadius(radius);
    circle.setPosition(static_cast<float>(x), static_cast<float>(y));
    circle.setFillColor(sf::Color::White);
    isDragged = false;

    // Set up the number text
    text.setFont(font);
    text.setString(std::to_string(number));
    text.setCharacterSize(static_cast<unsigned int>(radius * 1.5f));
    text.setFillColor(sf::Color::Black);

    // Center the text in the circle
    sf::FloatRect textBounds = text.getLocalBounds();
    text.setOrigin(textBounds.width / 2, textBounds.height / 2);
    text.setPosition(
        circle.getPosition().x + radius/1.25,  
        circle.getPosition().y + radius/1.25   
    );
}

void makeCircle::update(sf::Vector2i& mousePos, bool mousePressed, int& selectedIndex, int currentIndex, bool isDragging) 
{
    static bool locked = false;
   
    // Check if the mouse was initially pressed inside the circle to start dragging
    if (mousePressed && !locked && circle.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
        selectedIndex = currentIndex; // Lock onto this circle
        locked = true;
        isDragging = true;
    }
    
        // Release the circle when the mouse button is released
        if (!mousePressed && locked) {
            locked = false; 
            isDragging = false; 
            selectedIndex = -1;
            mousePressed = false; 
        }

        // Only move the circle if it’s the one currently selected
        if (selectedIndex == currentIndex && locked) { 
            circle.setPosition(static_cast<float>(mousePos.x) - circle.getRadius(), static_cast<float>(mousePos.y) - circle.getRadius());
            text.setPosition(
                circle.getPosition().x + circle.getRadius() / 1.25,  
                circle.getPosition().y + circle.getRadius() / 1.25   
            );
        }
    }
  

void makeCircle::changeDisplacement(sf::Vector2f displacement) 
{ 
     circle.setPosition(circle.getPosition() + displacement);  
     text.setPosition(
         circle.getPosition().x + circle.getRadius() / 1.25,  
         circle.getPosition().y + circle.getRadius() / 1.25   
     );
}


void makeCircle::render(sf::RenderWindow& window) 
{
    window.draw(circle);
    window.draw(text); 
}

sf::FloatRect makeCircle::getBounds() 
{
    return circle.getGlobalBounds();
}

const sf::Vector2f& makeCircle::getPos() 
{
    return circle.getPosition();
}

void makeCircle::setPosition(sf::Vector2f& position) 
{
    circle.setPosition(position);
}

void makeCircle::addConnection(makeCircle* otherNode) 
{
    if (otherNode && otherNode != this && std::find(connections.begin(), connections.end(), otherNode) == connections.end()) {
        connections.push_back(otherNode);
    }
}

void makeCircle:: setcolor(sf::Color color) 
{
    circle.setFillColor(color); 
}

