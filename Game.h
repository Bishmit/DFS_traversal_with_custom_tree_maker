#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "makeCircle.h"
#include <iostream>
#include <queue>
#include <memory>

class Game {
public:
    Game();
    void run();
    void updateMousePosition();
private:

    //SFML window and properties
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
    float colorUpdateInterval = 0.5f;
    int newnodeColoring;
    int nodecoloring = 0; 
    bool isbuttonChecked = false; 

    // Circles (nodes) in the game
    std::vector<std::unique_ptr<makeCircle>> circles;
    std::vector<makeCircle*> visitedNodesDFS;
    std::vector<makeCircle*> visitedNodesBFS; 

    //methods for game loop
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

    // Connection making
    void makeConnections(sf::RenderWindow& window, sf::Clock& animationClock);

    // some other variable

    bool stop = false; 
    bool isScrolled = false; 
    bool AdjustViewCoordinates = false; 
  
    bool selectDFS = false; 
    bool selectBFS = false; 
    std::vector<sf::RectangleShape> rectangles; 
    sf::Clock clock;
    bool isDoneWithCircleColoring = false;
    bool something = false; 

    // Helper function
    bool isNear(const sf::Vector2f& pos1, const sf::Vector2f& pos2);
    void doDFS(makeCircle* node, makeCircle* parentNode, std::vector<makeCircle*>& visitedNode);
    void doBFS(makeCircle* startNode, makeCircle* parentNode, std::vector<makeCircle*>& visitedNodes);
    void createButton(int x, int y, sf::RectangleShape& button, sf::Text& text, const std::string& title);

    //reset clear function 
    void clearTraversal();
    void clearGraph();
};