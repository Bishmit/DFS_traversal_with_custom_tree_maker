#pragma once
#include<SFML/Graphics.hpp>
class SelectionRectangle
{
public: 
	SelectionRectangle(); 

	void update(sf::Vector2i& mousepos);
	void render(sf::RenderWindow &window);
	void mouseControl(sf::Vector2i &mousepos);

	bool isSelectionToolClicked();

private: 
	sf::RectangleShape selectionTool; 

};

