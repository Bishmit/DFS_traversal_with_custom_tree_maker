#include "../Include/SelectionRectangle.h"

SelectionRectangle::SelectionRectangle()
{
	selectionTool.setFillColor(sf::Color::White); 
	selectionTool.setPosition(sf::Vector2f(0.f, 0.f)); 
	selectionTool.setSize(sf::Vector2f(20.f, 20.f)); 
	selectionTool.setFillColor(sf::Color(255, 255, 255, 40));
}

void SelectionRectangle::update(sf::Vector2i& mousepos)
{
	if (isSelectionToolClicked()) { 
		if (!isChecked) {
			selectionTool.setPosition(sf::Vector2f(mousepos));
			isChecked = true; 
		}
		dynamicWidthHeightAdjustment(mousepos); 
	}
	else {
		selectionTool.setSize(sf::Vector2f(0.f, 0.f));
		isChecked = false; 
	}
	 
}

void SelectionRectangle::render(sf::RenderWindow& window)
{

	window.draw(selectionTool); 
}

void SelectionRectangle::mouseControl(sf::Vector2i& mousepos) 
{
	selectionTool.setPosition(mousepos.x, mousepos.y); 
}

bool SelectionRectangle::isSelectionToolClicked() 
{
	// checks if the rectangle is clicked and not left yet i.e is dragged
	return (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl));
}

void SelectionRectangle::dynamicWidthHeightAdjustment(sf::Vector2i& mousepos) 
{
	// change the width and height of the selectiontool in real time
	auto bounds = selectionTool.getGlobalBounds(); 
	selectionTool.setSize(sf::Vector2f(mousepos.x - selectionTool.getPosition().x, mousepos.y - selectionTool.getPosition().y));
}

