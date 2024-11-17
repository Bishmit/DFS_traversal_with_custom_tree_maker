// GraphApp.cpp
#include "GraphApp.h"
#include <iostream>

GraphApp::GraphApp() : window(sf::VideoMode(800, 600), "Graph Connections"), source(nullptr), previousMousePressed(false) {
    window.setFramerateLimit(60);

    // Example: Adding a few nodes
    nodes.emplace_back(30, 100, 100);
    nodes.emplace_back(30, 300, 300);
    nodes.emplace_back(30, 500, 200);
}

void GraphApp::run() {
    while (window.isOpen()) {
        processEvents();
        update();
        render();
    }
}

void GraphApp::processEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            window.close();
    }
}

void GraphApp::update() {
    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    bool mousePressed = sf::Mouse::isButtonPressed(sf::Mouse::Left);
    bool mouseReleased = !mousePressed && previousMousePressed;

    for (auto& node : nodes) {
        node.update(mousePos, mousePressed, mouseReleased, source);

        if (source && node.isMouseInside(mousePos) && mouseReleased && &node != source) {
            addEdge(source, &node);
            source = nullptr;
        }
    }

    previousMousePressed = mousePressed;
}

void GraphApp::render() {
    window.clear();

    for (auto& edge : edges) {
        edge.render(window);
    }

    for (auto& node : nodes) {
        node.render(window);
    }

    window.display();
}

void GraphApp::addEdge(Node* start, Node* end) {
    edges.emplace_back(start, end);
}

