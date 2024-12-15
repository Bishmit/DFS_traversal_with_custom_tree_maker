#pragma once
#include <SFML/Graphics.hpp>
#include "SelectionRectangle.h"

class makeCircle
{
public:
    sf::CircleShape circle;
    std::vector<makeCircle*> connections;
    bool highlighted = false; 
    bool isUnderSelection = false; 
    sf::Time highlightStartTime = sf::Time::Zero;
    sf::Clock clock;
    makeCircle(float radius, int x, int y, sf::Font& font, int number);
    void update(sf::Vector2i &mousepos, bool mousepressed, int &selectedIndex, int currentIndex, bool isDragging);
    void changeDisplacement(sf::Vector2f displacement);
    void render(sf::RenderWindow& window);
    bool mouselockwithcircle = false; 
    bool outside = true; 
    sf::FloatRect getBounds();
    const sf::Vector2f& getPos();
    void setPosition(sf::Vector2f& position);
    void addConnection(makeCircle* otherNode);
    void setcolor(sf::Color color);
    SelectionRectangle sr; 
   
private:  
    bool isDragged; 
    sf::Font font; 
    sf::Text text; 
};
