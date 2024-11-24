#include "SelectionRectangle.h"

SelectionRectangle::SelectionRectangle()
{
	selectionTool.setFillColor(sf::Color::White); 
	selectionTool.setPosition(sf::Vector2f(200.f, 200.f)); 
	selectionTool.setSize(sf::Vector2f(20.f, 20.f)); 
}

void SelectionRectangle::update(sf::Vector2i& mousepos)
{
	if (isSelectionToolClicked()) {
		mouseControl(mousepos);
	}
}

void SelectionRectangle::render(sf::RenderWindow& window)
{
	window.draw(selectionTool); 
}

void SelectionRectangle::mouseControl(sf::Vector2i& mousepos) {
	selectionTool.setPosition(mousepos.x, mousepos.y); 
}

bool SelectionRectangle::isSelectionToolClicked() {
	// checks if the rectangle is clicked and not left yet i.e is dragged
	return (sf::Mouse::isButtonPressed(sf::Mouse::Left)); 
		
}