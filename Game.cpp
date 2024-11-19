#include "Game.h"

Game::Game()
    : window(sf::VideoMode(1200, 780), "Graph Traversal",
      sf::Style::Titlebar | sf::Style::Close), drawLine(false), snappingMode(false), selectedNode(nullptr) {
    window.setFramerateLimit(60);
    if (!font.loadFromFile("TimesNewRoman.ttf")) {
        std::cout << "Font not loaded\n";
    }
    createButton(1200 - 150, 10, buttonDfs, textDfs, "DFS");
    createButton(1200 - 150, 60, buttonBfs, textBfs, "BFS");
    createButton(1200 - 150, 110, buttonClearTraversal, textClearTraversal, "Reset Traversal");
    createButton(1200 - 150, 160, buttonClearAll, textClearAll, "Clear All");
}

void Game::run() {
    while (window.isOpen()) {
        processEvents();
        update();
        render();
    }
}

void Game::processEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            window.close();

        if (event.type == sf::Event::MouseWheelScrolled) {
            float zoomFactor = 1.1f; // Adjust zoom sensitivity
            if (event.mouseWheelScroll.delta > 0) {
                // Zoom in
                view.zoom(1.0f / zoomFactor);
            }
            else if (event.mouseWheelScroll.delta < 0) {
                // Zoom out
                view.zoom(zoomFactor);
            }

            // Get the mouse position in pixel coordinates
            sf::Vector2i pixelMousePosition = sf::Mouse::getPosition(window);

            // Convert the mouse position to world coordinates
            sf::Vector2f beforeZoomMouseWorldPosition = window.mapPixelToCoords(pixelMousePosition);

            // Adjust the view's center to keep the mouse position consistent
            sf::Vector2f afterZoomMouseWorldPosition = window.mapPixelToCoords(pixelMousePosition);
            sf::Vector2f offset = beforeZoomMouseWorldPosition - afterZoomMouseWorldPosition;

            view.move(offset);
        }

        handleKeyPress(event);  // Check for key press to add a new circle

        if (buttonDfs.getGlobalBounds().contains(static_cast<float>(newMousePosition.x), static_cast<float>(newMousePosition.y))) {
            buttonDfs.setOutlineColor(sf::Color::Green); 
            if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                selectDFS = true;
            }
        }
        else buttonDfs.setOutlineColor(sf::Color::White);


        if (buttonBfs.getGlobalBounds().contains(static_cast<float>(newMousePosition.x), static_cast<float>(newMousePosition.y))) {
            buttonBfs.setOutlineColor(sf::Color::Green);
            if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                selectBFS = true;
            }
        }
        else buttonBfs.setOutlineColor(sf::Color::White);

        if (buttonClearTraversal.getGlobalBounds().contains(static_cast<float>(newMousePosition.x), static_cast<float>(newMousePosition.y))) {
            buttonClearTraversal.setOutlineColor(sf::Color::Green);
            if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                clearTraversal();
            }
        }
        else buttonClearTraversal.setOutlineColor(sf::Color::White);

        if (buttonClearAll.getGlobalBounds().contains(static_cast<float>(newMousePosition.x), static_cast<float>(newMousePosition.y))) {
            buttonClearAll.setOutlineColor(sf::Color::Red); 
            if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                clearGraph();
            }
        }
        else buttonClearAll.setOutlineColor(sf::Color::White);


        if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) && sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
            for (int i = 0; i < circles.size(); i++) {
                if (circles[i]->getBounds().contains(static_cast<float>(newMousePosition.x), static_cast<float>(newMousePosition.y))) { 
                    std::vector<makeCircle*> visitedNodes;
                    if (selectDFS) {
                        doDFS(circles[i].get(), visitedNodes);

                    }
                    else if (selectBFS) {
                        
                        doBFS(circles[i].get(), circles[0].get(), visitedNodes);
                    }
                    
                }
            }
        }
    }
}

void Game::update() {
    newMousePosition = sf::Mouse::getPosition(window);

    // Update all circles to handle dragging
    for (size_t i = 0; i < circles.size(); ++i) {
        circles[i]->update(newMousePosition, sf::Mouse::isButtonPressed(sf::Mouse::Left), selectedCircleIndex, i);
    }

    // Toggle snapping mode with "S" key
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) && selectedNode == nullptr) {
        snappingMode = true;
    }

    // Handle snapping mode for connecting nodes
    if (snappingMode) {
        processSnappingMode();
    }
    
}

void Game::processSnappingMode() {
    // First left-click to select the starting circle
    if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && !selectedNode) {
        selectStartingCircle();
    }

    // Second left-click to find the nearest circle to connect
    if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && selectedNode) {
        selectNearestCircleAndConnect();
    }
}

void Game::selectStartingCircle() {
    for (size_t i = 0; i < circles.size(); ++i) {
        if (circles[i]->getBounds().contains(static_cast<float>(newMousePosition.x), static_cast<float>(newMousePosition.y))) {
            selectedNode = circles[i].get();
            selectedCircleIndex = static_cast<int>(i);
            break;
        }
    }
}

void Game::selectNearestCircleAndConnect() {
    for (size_t i = 0; i < circles.size(); ++i) {
        if (i != selectedCircleIndex) {
            auto& otherCircle = circles[i];
            if (isNear(selectedNode->getPos(), otherCircle->getPos())) {
                connectNodes(selectedNode, otherCircle.get());
                snappingMode = false;
                selectedNode = nullptr;  // Reset selected node
                break;
            }
        }
    }
}

void Game::render() {
    window.setView(view); 
    window.clear();

    // Draw each circle and connections
    
    for (const auto& circle : circles) {
        circle->render(window);
    }
    if (stop) {
        for (auto& node : circles) {
                if (node->highlighted) {
                    sf::sleep(sf::milliseconds(10));
                    node->setcolor(sf::Color::Yellow);
                }
                else {
                    node->setcolor(sf::Color(255, 255, 0, 80));
                }
        }

    }
    drawConnections(window);  // Draw connections between circles
    window.draw(buttonDfs); 
    window.draw(buttonBfs); 
    window.draw(textDfs); 
    window.draw(textBfs); 
    window.draw(buttonClearTraversal);
    window.draw(buttonClearAll);
    window.draw(textClearTraversal);
    window.draw(textClearAll);

    window.display();
}

void Game::handleKeyPress(sf::Event& event) {
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::C) {
        addNewCircle();
    }
}

void Game::addNewCircle() {
    sf::Vector2i mousePosition = sf::Mouse::getPosition(window);
    circles.emplace_back(std::make_unique<makeCircle>(10.f, mousePosition.x, mousePosition.y));
}

void Game::connectNodes(makeCircle* node1, makeCircle* node2) {
    if (node1 && node2 && node1 != node2) {
        node1->addConnection(node2);  // Add connection in both directions
        node2->addConnection(node1);
    }
}

void Game::drawConnections(sf::RenderWindow& window) {
    constexpr float RECTANGLE_THICKNESS = 3.0f;
    constexpr float DEG_TO_RAD = 180.f / 3.14159f;

    for (const auto& node : circles) {
        const sf::Vector2f& pos1 = node->getPos();

        // Draw each unique connection
        for (const auto& connectedNode : node->connections) {
            const sf::Vector2f& pos2 = connectedNode->getPos();

            // Avoid drawing duplicate connections
            if (pos1.x < pos2.x || (pos1.x == pos2.x && pos1.y < pos2.y)) {
                sf::Vector2f delta = pos2 - pos1;
                float distance = std::hypot(delta.x, delta.y);
                float angle = std::atan2(delta.y, delta.x) * DEG_TO_RAD;

                sf::RectangleShape rectangle(sf::Vector2f(distance, RECTANGLE_THICKNESS));
                if (node->highlighted && connectedNode->highlighted) {
                    sf::sleep(sf::milliseconds(10));
                    rectangle.setFillColor(sf::Color::Green); 
                }
                else {
                    rectangle.setFillColor(sf::Color(50, 205, 50, 180));
                }
                rectangle.setOrigin(0.f, RECTANGLE_THICKNESS / 2);
                rectangle.setPosition(pos1);
                rectangle.setRotation(angle);
                window.draw(rectangle);
            }
        }
    }
}



// Helper function to check if two positions are close enough to snap
bool Game::isNear(const sf::Vector2f& pos1, const sf::Vector2f& pos2) {
    static constexpr float SNAP_THRESHOLD = 50.0f;
    return std::hypot(pos1.x - pos2.x, pos1.y - pos2.y) < SNAP_THRESHOLD;
}

void Game:: doDFS(makeCircle* node, std::vector<makeCircle*>& visitedNode) {
    if (node == nullptr || stop ) {
        return;
    }
    node->highlighted = true; 
    visitedNode.push_back(node);

    // visit all the node recursively 
    for (auto& neighbour : node->connections) {
        if (std::find(visitedNode.begin(), visitedNode.end(), neighbour) == visitedNode.end()) {
            doDFS(neighbour, visitedNode);
        }
        //sf::sleep(sf::milliseconds(200)); 
        stop = true;
    }
}

void Game::doBFS(makeCircle* startNode, makeCircle* parentNode, std::vector<makeCircle*>& visitedNodes) {
    if (startNode == nullptr || stop) {
        return;
    }

    // Queue for BFS
    std::queue<makeCircle*> queue;

    // Mark the start node as visited and enqueue it
    parentNode->highlighted = true;
    visitedNodes.push_back(parentNode);
    queue.push(parentNode);

    while (!queue.empty() && !stop) {
        // Dequeue a node from the queue
        makeCircle* currentNode = queue.front();
        //makeCircle* targetNode = queue.back();
        queue.pop();
        
        //if (currentNode == parentNode) break; 
        // Visit all the neighbors of the current node
        
        for (auto& neighbor : currentNode->connections) {
            // If the neighbor hasn't been visited
            if (std::find(visitedNodes.begin(), visitedNodes.end(), neighbor) == visitedNodes.end()) {
                neighbor->highlighted = true;          // Mark as visited (for visualization)
                visitedNodes.push_back(neighbor);     // Add to visited list
                queue.push(neighbor);                 // Enqueue for further traversal
                if (neighbor == startNode) stop = true; 
            } 
        }
    }
}


void Game::createButton(int x, int y, sf::RectangleShape& button, sf::Text& text, const std::string& title) {
    button = sf::RectangleShape(sf::Vector2f(105, 30)); 
    button.setPosition(x, y); 
    button.setOutlineColor(sf::Color::White); 
    button.setOutlineThickness(3.f); 
    button.setFillColor(sf::Color::Transparent); 

    text.setFont(font); 
    text.setString(title); 
    text.setCharacterSize(15); 
    text.setFillColor(sf::Color::White);
    text.setPosition(button.getPosition().x + 5, button.getPosition().y + 5); 
}

void Game::clearTraversal() {
    for (auto& circle : circles) {
        circle->highlighted = false;
        circle->setcolor(sf::Color(255, 255, 0, 210));
    }
    stop = false;
}

void Game::clearGraph() {
    circles.clear();
    stop = false;   
}


