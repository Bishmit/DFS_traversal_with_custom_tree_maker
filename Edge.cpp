// Edge.cpp
#include "Edge.h"

Edge::Edge(Node* start, Node* end) : start(start), end(end) {}

void Edge::render(sf::RenderWindow& window) {
    if (start && end) {
        sf::Vertex line[] = {
            sf::Vertex(start->getPosition(), sf::Color::White),
            sf::Vertex(end->getPosition(), sf::Color::White)
        };
        window.draw(line, 2, sf::Lines);
    }
}
