#include "Game.h"

Game::Game()
    : window(sf::VideoMode(800, 600), "Graph Demo"), drawLine(false), snappingMode(false), selectedNode(nullptr) {
    window.setFramerateLimit(60);
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

        handleKeyPress(event);  // Check for key press to add a new circle

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) && sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
            for (int i = 0; i < circles.size(); i++) {
                if (circles[i]->getBounds().contains(static_cast<float>(newMousePosition.x), static_cast<float>(newMousePosition.y))) { 
                    std::vector<makeCircle*> visitedNodes;
                    doDFS(circles[i].get(), visitedNodes); 
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
    window.clear();

    // Draw each circle and connections
    
    for (const auto& circle : circles) {
        circle->render(window);
    }
    if (stop) {
        for (auto& node : circles) {
            if (node->highlighted) {
                node->setcolor(sf::Color::Yellow);
            }
            else {
                node->setcolor(sf::Color(255, 255, 0, 80)); 
            }
        }
    }
    drawConnections(window);  // Draw connections between circles

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
        stop = true;
    }
}



