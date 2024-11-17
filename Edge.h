// Edge.h
#ifndef EDGE_H
#define EDGE_H

#include <SFML/Graphics.hpp>
#include "Node.h"

class Edge {
public:
    Edge(Node* start, Node* end);

    void render(sf::RenderWindow& window);

private:
    Node* start;
    Node* end;
};

#endif // EDGE_H
