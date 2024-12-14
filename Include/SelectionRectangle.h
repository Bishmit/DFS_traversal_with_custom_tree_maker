#pragma once
#include<SFML/Graphics.hpp>
#include<iostream>

class SelectionRectangle
{
public: 
	SelectionRectangle(); 

	void update(sf::Vector2i& mousepos);
	void render(sf::RenderWindow &window);
	void mouseControl(sf::Vector2i &mousepos);

	bool isSelectionToolClicked();

	void dynamicWidthHeightAdjustment(sf::Vector2i& mousepos);
	bool ctrlEnabled = false; 

	sf::RectangleShape selectionTool;
private: 
	bool isClicked = false, isChecked = false; 
	
};

