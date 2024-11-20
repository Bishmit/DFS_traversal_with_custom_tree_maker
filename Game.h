#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "makeCircle.h"
#include <iostream>
#include <queue>
#include <memory>
#include "global.h"

//float zoomFactor = 1.1f;

class Game {
public:
    Game();
    void run();
    void updateMousePosition();
private:

    // Core SFML window and properties
    sf::RenderWindow window;
    bool drawLine;
    bool snappingMode;
    sf::Font font; 
    sf::RectangleShape buttonDfs, buttonBfs, buttonClearTraversal, buttonClearAll; 
    sf::Text textDfs, textBfs, textClearTraversal, textClearAll; 
    sf::View view = window.getDefaultView();

    // Mouse and selection states
    sf::Vector2i newMousePosition;
    makeCircle* selectedNode = nullptr;
    int selectedCircleIndex = -1;

    // Circles (nodes) in the game
    std::vector<std::unique_ptr<makeCircle>> circles;
    std::vector<makeCircle*> visitedNodes;

    // Core methods for game loop
    void processEvents();
    void update();
    void render();

    // Event handling
    void handleKeyPress(sf::Event& event);

    // Circle creation and connections
    void addNewCircle();
    void connectNodes(makeCircle* node1, makeCircle* node2);

    // Snapping mode logic
    void processSnappingMode();
    void selectStartingCircle();
    void selectNearestCircleAndConnect();

    // Connection rendering
    void drawConnections(sf::RenderWindow& window);
    bool stop = false; 
    bool isScrolled = false; 
    bool AdjustViewCoordinates = false; 
  
    bool selectDFS = false; 
    bool selectBFS = false; 
    sf::Clock clock;
    bool shouldRenderSquare = false;
    // Helper function
    bool isNear(const sf::Vector2f& pos1, const sf::Vector2f& pos2);
    void doDFS(makeCircle* node, makeCircle* parentNode, std::vector<makeCircle*>& visitedNode);
    void doBFS(makeCircle* startNode, makeCircle* parentNode, std::vector<makeCircle*>& visitedNodes);
    void createButton(int x, int y, sf::RectangleShape& button, sf::Text& text, const std::string& title);
    void clearTraversal();
    void clearGraph();
};