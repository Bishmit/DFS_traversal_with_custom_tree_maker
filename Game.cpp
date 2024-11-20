#include "Game.h"

Game::Game()
    : window(sf::VideoMode(1300, 775), "Graph Traversal",
      sf::Style::Titlebar | sf::Style::Close), drawLine(false), snappingMode(false), selectedNode(nullptr) {
    window.setFramerateLimit(60);
    if (!font.loadFromFile("TimesNewRoman.ttf")) {
        std::cout << "Font not loaded\n";
    }
    createButton(1300 - 150, 10, buttonDfs, textDfs, "DFS");
    createButton(1300 - 150, 60, buttonBfs, textBfs, "BFS");
    createButton(1300 - 150, 110, buttonClearTraversal, textClearTraversal, "Reset Traversal");
    createButton(1300 - 150, 160, buttonClearAll, textClearAll, "Clear All");
}

void Game::run() {
    while (window.isOpen()) {
        updateMousePosition(); 
        processEvents();
        update();
        render();
    }
}

void Game::updateMousePosition() {
    sf::Vector2i _mousePosition = sf::Mouse::getPosition(window);
    newMousePosition = sf::Vector2i(window.mapPixelToCoords(_mousePosition));
}

void Game::processEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            window.close();

        if (event.type == sf::Event::MouseWheelScrolled) {
            AdjustViewCoordinates = true; 
            float zoomFactor = 1.1f; // Adjust zoom sensitivity

            // Adjust zoom factor based on scroll direction
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

            // Convert the pixel position to world coordinates
            sf::Vector2f worldMousePositionBefore = window.mapPixelToCoords(pixelMousePosition);

            // Apply the view changes (zoom)
            window.setView(view);

            // Recalculate world coordinates after the zoom
            sf::Vector2f worldMousePositionAfter = window.mapPixelToCoords(pixelMousePosition);

            // Calculate the offset caused by zooming and adjust the view
            sf::Vector2f offset = worldMousePositionBefore - worldMousePositionAfter;
            view.move(offset);

            // Update the window view
            window.setView(view);
        }

        handleKeyPress(event);  // Check for key press to add a new circle

        // We should not change the button according to the viewport when scrolling so we pass the new value of mouse which is tempMousePos which just take the co-ordinates of default mouse vieport
        
        sf::Vector2i tempMousePos = sf::Mouse::getPosition(window);
        if (buttonDfs.getGlobalBounds().contains(static_cast<float>(tempMousePos.x), static_cast<float>(tempMousePos.y))) {
            updateMousePosition();
            buttonDfs.setOutlineColor(sf::Color::Green); 
            if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                selectDFS = true;
                selectBFS = false; 
            }
        }
        else buttonDfs.setOutlineColor(sf::Color::White);


        if (buttonBfs.getGlobalBounds().contains(static_cast<float>(tempMousePos.x), static_cast<float>(tempMousePos.y))) {
            buttonBfs.setOutlineColor(sf::Color::Green);
            if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                selectBFS = true;
                selectDFS = false; 
            }
        }
        else buttonBfs.setOutlineColor(sf::Color::White);

        if (buttonClearTraversal.getGlobalBounds().contains(static_cast<float>(tempMousePos.x), static_cast<float>(tempMousePos.y))) {
            buttonClearTraversal.setOutlineColor(sf::Color::Green);
            if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                clearTraversal();
            }
        }
        else buttonClearTraversal.setOutlineColor(sf::Color::White);

        if (buttonClearAll.getGlobalBounds().contains(static_cast<float>(tempMousePos.x), static_cast<float>(tempMousePos.y))) {
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
                        doDFS(circles[i].get(), circles[0].get(), visitedNodes);

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
    //newMousePosition = sf::Mouse::getPosition(window);
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

    if (stop) {
        for (auto& node : circles) {
                if (node->highlighted) {
                    node->setcolor(sf::Color(255, 255, 0, 255));
                }
                else {
                    node->setcolor(sf::Color(255, 255, 255, 180));
                }
        }

    }
    drawConnections(window);  // Draw connections between circles

    // Draw each circle and connections
    for (const auto& circle : circles) {
        circle->render(window);
    }

    window.setView(window.getDefaultView());
    window.draw(buttonDfs); 
    window.draw(buttonBfs); 
    window.draw(textDfs); 
    window.draw(textBfs); 
    window.draw(buttonClearTraversal);
    window.draw(buttonClearAll);
    window.draw(textClearTraversal);
    window.draw(textClearAll);
    window.setView(view); 

    window.display();
}

void Game::handleKeyPress(sf::Event& event) {
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::C) {
        addNewCircle();
    }
}

void Game::addNewCircle() {
        circles.emplace_back(std::make_unique<makeCircle>(8.f, newMousePosition.x, newMousePosition.y));

}

void Game::connectNodes(makeCircle* node1, makeCircle* node2) {
    if (node1 && node2 && node1 != node2) {
        node1->addConnection(node2);  // Add connection in both directions
        node2->addConnection(node1);
    }
}

void Game::drawConnections(sf::RenderWindow& window) {
    constexpr float RECTANGLE_THICKNESS = 2.0f;
    constexpr float DEG_TO_RAD = 180.f / 3.14159f;

    for (const auto& node : circles) {
        const sf::Vector2f& pos1 = sf::Vector2f(node->getPos().x + 4, node->getPos().y + 4); 

        // Draw each unique connection
        for (const auto& connectedNode : node->connections) {
            const sf::Vector2f& pos2 = sf::Vector2f(connectedNode->getPos().x + 4, connectedNode->getPos().y + 4);

            // Avoid drawing duplicate connections
            if (pos1.x < pos2.x || (pos1.x == pos2.x && pos1.y < pos2.y)) {
                sf::Vector2f delta = pos2 - pos1;
                float distance = std::hypot(delta.x, delta.y);
                float angle = std::atan2(delta.y, delta.x) * DEG_TO_RAD;

                sf::RectangleShape rectangle(sf::Vector2f(distance, RECTANGLE_THICKNESS));
                if (node->highlighted && connectedNode->highlighted) {
                    rectangle.setFillColor(sf::Color::Green); 
                }
                else {
                    rectangle.setFillColor(sf::Color(255, 255, 255, 200));
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

void Game:: doDFS(makeCircle* startnode, makeCircle* parentNode, std::vector<makeCircle*>& visitedNode) {
    if (startnode == nullptr || parentNode == nullptr || stop ) {
        return;
    }
    parentNode->highlighted = true; 
    visitedNode.push_back(parentNode);

    if (parentNode == startnode) {
        stop = true;
        return;
    }

    // visit all the node recursively 
    for (auto& neighbour : parentNode->connections) {
        if (std::find(visitedNode.begin(), visitedNode.end(), neighbour) == visitedNode.end()) {
            doDFS(startnode, neighbour, visitedNode);
        }
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
                neighbor->highlighted = true;         
                visitedNodes.push_back(neighbor);     
                queue.push(neighbor);                
                if (neighbor == startNode) {
                    stop = true;
                    break;
                }
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
        circle->setcolor(sf::Color::White);
    }
    stop = false;
}

void Game::clearGraph() {
    circles.clear();
    stop = false;   
}


