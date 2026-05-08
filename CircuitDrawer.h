#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>
#include <map>
#include <string>
#include "Node.h"

struct DrawnGate {
    std::vector<sf::ConvexShape> shapes;
    std::vector<sf::CircleShape> circles;
    sf::Text label;
    float cx, cy;
    float inputY1, inputY2;
    float leftX, rightX;
};

class CircuitDrawer {
public:
    void build(std::shared_ptr<Node> root, sf::Font& font,
               float px, float py, float pw, float ph);

   
    void highlight(std::shared_ptr<Node> root,
                   std::map<std::string, bool>& inputVals);

    void draw(sf::RenderTarget& target);

private:
    std::vector<DrawnGate> gates;
    std::vector<sf::Vertex> wires;
    sf::Font* font = nullptr;

    float GW, GH, GAP;
    float PX, PY, PW, PH;

    bool  hlMode = false;
    std::map<std::string, bool>* hlVals = nullptr;

    int treeDepth (std::shared_ptr<Node> n);
    int treeLeaves(std::shared_ptr<Node> n);
    struct PlaceResult { float x, y; bool val; };
    PlaceResult place(std::shared_ptr<Node> node, float x, float y, float ySpan);

    DrawnGate makeInput (const std::string& name, float cx, float cy);
    DrawnGate makeAnd   (float cx, float cy);
    DrawnGate makeOr    (float cx, float cy);
    DrawnGate makeNot   (float cx, float cy);
    DrawnGate makeXor   (float cx, float cy);
    DrawnGate makeOutput(float cx, float cy, bool val=false);
    void wire(float x1, float y1, float x2, float y2, bool signal, bool colored);
};