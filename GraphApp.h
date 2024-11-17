// GraphApp.h
#ifndef GRAPHAPP_H
#define GRAPHAPP_H

#include <SFML/Graphics.hpp>
#include <vector>
#include "Node.h"
#include "Edge.h"

class GraphApp {
public:
    GraphApp();
    void run();

private:
    sf::RenderWindow window;
    std::vector<Node> nodes;
    std::vector<Edge> edges;
    Node* source;

    void processEvents();
    void update();
    void render();
    void addEdge(Node* start, Node* end);

    bool previousMousePressed;
};

#endif // GRAPHAPP_H
