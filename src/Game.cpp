#include "../Include/Game.h"

Game::Game()
    : window(sf::VideoMode(1300, 775), 
      "Graph Traversal",
      sf::Style::Titlebar | sf::Style::Close),
      drawLine(false),
      snappingMode(false),
      selectedNode(nullptr) 
 {
    window.setFramerateLimit(60);
    if (!font.loadFromFile("Assets/TimesNewRoman.ttf")) {
        std::cout << "Font not loaded\n";
    }
    createButton(1300 - 150, 10, buttonDfs, textDfs, "DFS");
    createButton(1300 - 150, 60, buttonBfs, textBfs, "BFS");
    createButton(1300 - 150, 110, buttonClearTraversal, textClearTraversal, "Reset Traversal");
    createButton(1300 - 150, 160, buttonClearAll, textClearAll, "Clear All");
}

void Game::run() 
{
    while (window.isOpen()) {
        updateMousePosition(); 
        processEvents();
        update();
        render();
    }
}

void Game::updateMousePosition() 
{
    sf::Vector2i _mousePosition = sf::Mouse::getPosition(window);
    newMousePosition = sf::Vector2i(window.mapPixelToCoords(_mousePosition));
}

void Game::processEvents() 
{
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            window.close();


        if (event.type == sf::Event::MouseWheelScrolled) {

            /*
            This code handles the mouse position adjustment when zooming in and out of the view.

            newMousePosition.x = (OriginalMousePosition.x - width) / ZoomFactor + width;
            newMousePosition.y = (OriginalMousePosition.y - height) / ZoomFactor + height;


            The process is a combination of **scaling** and **translation**:

            Translation: Subtract the center coordinates from the current mouse position to shift the origin to the center of the view.
            adjustedMousePosition = mousePosition - sf::Vector2i(width, height);

            Scaling: Divide the translated mouse position by the zoom factor to adjust the position relative to the zoom level.
            scaledMousePosition = adjustedMousePosition / zoomFactor;

            Translation (again): Add the center coordinates back to the scaled position to restore the mouse position relative to the original view center.
            finalMousePosition = scaledMousePosition + sf::Vector2i(width, height);

            The result is that the mouse position adapts correctly to the zooming action, ensuring that the zoom is centered around the point (width, height) without causing disorientation in the user interface.
            
            SFML has inbuilt method for this so i am doing that but the priciple lies the same as above. 
            */
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
            isbuttonChecked = true; 
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


void Game::update() 
{
    st.update(newMousePosition); 

    // Update all circles to handle dragging
    for (size_t i = 0; i < circles.size(); ++i) {
        circles[i]->update(newMousePosition, sf::Mouse::isButtonPressed(sf::Mouse::Left), selectedCircleIndex, i, isDragging);    
    }

    // Toggle snapping mode with "S" key
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) && selectedNode == nullptr) {
        snappingMode = true;
    }

    // Handle snapping mode for connecting nodes
    if (snappingMode) {
        processSnappingMode();
    }

    makeConnections(window, clock);  // make connections between circles 
}

void Game::processSnappingMode() 
{
    // First left-click to select the starting circle
    if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && !selectedNode) {
        selectStartingCircle();
    }

    // Second left-click to find the nearest circle to connect
    if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && selectedNode) {
        selectNearestCircleAndConnect();
    }
}

void Game::selectStartingCircle() 
{
    for (size_t i = 0; i < circles.size(); ++i) {
        if (circles[i]->getBounds().contains(static_cast<float>(newMousePosition.x), static_cast<float>(newMousePosition.y))) {
            selectedNode = circles[i].get();
            selectedCircleIndex = static_cast<int>(i);
            break;
        }
    }
}

void Game::selectNearestCircleAndConnect() 
{
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

void Game::render() 
{
    window.setView(view);
    window.clear();

    st.render(window); 

    if (selectBFS && isbuttonChecked) { 
        if (clock.getElapsedTime().asSeconds() >= colorUpdateInterval) {
            if (nodecoloring < circles.size()) {
                if (circles[nodecoloring]->highlighted) {
                    circles[nodecoloring]->setcolor(sf::Color::Cyan);
                }
                ++nodecoloring; 
            }
            clock.restart();
        }

    }

    if (selectDFS && isbuttonChecked) {
        if (clock.getElapsedTime().asSeconds() >= colorUpdateInterval) {
            if (nodecoloring < visitedNodesDFS.size()) {
                visitedNodesDFS[nodecoloring]->setcolor(sf::Color::Cyan);
                ++nodecoloring;
            }
            clock.restart();
        }
    }
   
   /* if (selectDFS && isbuttonChecked) {
        if (clock.getElapsedTime().asSeconds() >= colorUpdateInterval) {
            if (nodecoloring < rectangles.size()) {
                if (!visited[nodecoloring] &&something == true) {
                    rectangles[nodecoloring].setFillColor(sf::Color::Green); 
                    visited[nodecoloring] = true; 
                }  
                else {
                    rectangles[nodecoloring].setFillColor(sf::Color(255, 255, 255, 210));
                }
                ++nodecoloring;     
            }
          clock.restart();
       }
    }*/

    // Draw all rectangles in the window
    for (auto& rect: rectangles) {
        window.draw(rect);
    }

    if (stop) {
        for (auto& node : circles) {
            if (!node->highlighted) {
                node->setcolor(sf::Color(255, 255, 255, 180));
            }
        }
    }

    displacePosition(); 

    // Draw each circle and connections
    for (const auto& circle : circles) {
        circle->render(window);
    }

    // logic to print the numbers associated with nodes
    if (circles.size() > lastSize) {
        lastSize = circles.size(); 
        std::cout << lastSize << "\n"; 
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

void Game::handleKeyPress(sf::Event& event) 
{
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::C) {
        addNewCircle();
    }
}

void Game::addNewCircle() 
{
    circles.emplace_back(std::make_unique<makeCircle>(8.f, newMousePosition.x, newMousePosition.y, font, lastSize + 1));
}

void Game::connectNodes(makeCircle* node1, makeCircle* node2) 
{
    if (node1 && node2 && node1 != node2) {
        node1->addConnection(node2);  // Add connection in both directions
        node2->addConnection(node1);
    }
}

void Game::makeConnections(sf::RenderWindow& window, sf::Clock& animationClock) 
{
    constexpr float RECTANGLE_THICKNESS = 2.0f;
    constexpr float DEG_TO_RAD = 180.f / 3.14159f;

    rectangles.clear();

    for (const auto& node : circles) {
        const sf::Vector2f pos1(node->getPos().x + 4, node->getPos().y + 4);

        // unique connection
        for (const auto& connectedNode : node->connections) {
            const sf::Vector2f pos2(connectedNode->getPos().x + 4, connectedNode->getPos().y + 4);

            // Avoid duplicate connections
            if (pos1.x < pos2.x || (pos1.x == pos2.x && pos1.y < pos2.y)) {
                sf::Vector2f delta = pos2 - pos1;
                float distance = std::hypot(delta.x, delta.y);
                float angle = std::atan2(delta.y, delta.x) * DEG_TO_RAD;

                // Create and configure the rectangle for this connection
                sf::RectangleShape rectangle(sf::Vector2f(distance, RECTANGLE_THICKNESS));
                rectangle.setOrigin(0.f, RECTANGLE_THICKNESS / 2);
                rectangle.setPosition(pos1);
                rectangle.setRotation(angle);

                // Set color based on highlighting
               
                if (node->highlighted && connectedNode->highlighted) {
                    something = true; 
                    rectangle.setFillColor(sf::Color(0, 255, 0, 240)); // Highlight color
                }
                else {
                    //something = false; 
                    rectangle.setFillColor(sf::Color(255, 255, 255, 210)); // Default color
                }

                // Store the rectangle
                rectangles.push_back(rectangle);
            }
        }
    }
}


// Helper function to check if two positions are close enough to snap
bool Game::isNear(const sf::Vector2f& pos1, const sf::Vector2f& pos2) 
{
    static constexpr float SNAP_THRESHOLD = 30.0f;
    return std::hypot(pos1.x - pos2.x, pos1.y - pos2.y) < SNAP_THRESHOLD;
}

void Game:: doDFS(makeCircle* startnode, makeCircle* parentNode, std::vector<makeCircle*>& visitedNode) 
{
    if (startnode == nullptr || parentNode == nullptr || stop ) {
        return;
    }

    parentNode->highlighted = true;
       
    visitedNodesDFS.push_back(parentNode);
      
    if (parentNode == startnode) {
        stop = true;
        return;
    }

    // visit all the node recursively 
    for (auto& neighbour : parentNode->connections) {
        if (std::find(visitedNode.begin(), visitedNode.end(), neighbour) == visitedNode.end()) {
            doDFS(startnode, neighbour, visitedNodesDFS);
        }
    }
   
}

void Game::doBFS(makeCircle* startNode, makeCircle* parentNode, std::vector<makeCircle*>& visitedNodes) 
{
    if (startNode == nullptr || stop) {
        return;
    }

    // Queue for BFS
    std::queue<makeCircle*> queue;

    // Mark the start node as visited and enqueue it
    parentNode->highlighted = true;
    visitedNodesBFS.push_back(parentNode);
    queue.push(parentNode);

    while (!queue.empty() && !stop) {
        // Dequeue a node from the queue
        makeCircle* currentNode = queue.front();
        queue.pop();
        
        for (auto& neighbor : currentNode->connections) {
            // If the neighbor hasn't been visited
            if (std::find(visitedNodesBFS.begin(), visitedNodesBFS.end(), neighbor) == visitedNodesBFS.end()) {
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


void Game::createButton(int x, int y, sf::RectangleShape& button, sf::Text& text, const std::string& title) 
{
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

void Game::clearTraversal() 
{
    visitedNodesDFS.clear(); 
    visitedNodesBFS.clear(); 
    for (auto& circle : circles) {
        circle->highlighted = false;
        circle->setcolor(sf::Color::White);
    }
    nodecoloring = 0; 
    stop = false;
    isbuttonChecked = false; 
}

void Game::clearGraph() 
{
    circles.clear();
    stop = false;   
    isbuttonChecked = false; 
    lastSize = 0; 
}

void Game::coverNodeOnSelect() 
{
    for (int i = 0; i < circles.size(); i++) {
       if(st.selectionTool.getGlobalBounds().contains(circles[i]->getPos().x, circles[i]->getPos().y))
        {
           circles[i]->isUnderSelection = true; 
        }
    }
}

void Game::displacePosition() 
{
    static sf::Vector2f initialMousePosition;
    sf::Vector2f displacement;
    sf::Vector2f _newMousePosition = static_cast<sf::Vector2f>(sf::Mouse::getPosition(window));

    static bool wasMiddleButtonPressed = false; // tracks the previous state of the middle button

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)) {
        isDragging = true;
        initialMousePosition = _newMousePosition;
        coverNodeOnSelect();
    }

    // Handle selection and displacement with Middle Mouse
    if (sf::Mouse::isButtonPressed(sf::Mouse::Middle)) {
        if (!wasMiddleButtonPressed) {
            // initialize the dragging state
            isDragging = true;
            moveSelection = true;
            initialMousePosition = _newMousePosition;
        }

        wasMiddleButtonPressed = true;
        displacement = _newMousePosition - initialMousePosition;
        initialMousePosition = _newMousePosition;

        for (auto& node : circles) {
            if (node->isUnderSelection) {
                node->changeDisplacement(displacement);
            }
        }
    }

    // Handle Middle Mouse release
    if (!sf::Mouse::isButtonPressed(sf::Mouse::Middle) && wasMiddleButtonPressed) {
        wasMiddleButtonPressed = false; 
        isDragging = false;             
        moveSelection = false;         

        for (auto& node : circles) {
            node->isUnderSelection = false; 
        }
    }
}




