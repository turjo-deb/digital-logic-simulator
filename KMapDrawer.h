#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include "TruthTable.h"


class KMapDrawer {
public:
    void build(TruthTable& table, sf::Font& font,
               float px, float py, float pw, float ph);
    void draw(sf::RenderTarget& window);

    bool visible = false;

private:
    std::vector<sf::RectangleShape> cells;
    std::vector<sf::Text>           cellTexts;
    std::vector<sf::Vertex>         gridLines;
    std::vector<sf::Text>           headerTexts;
    sf::Text                        titleText;
    sf::Text                        mintermText;
    sf::Text                        simplifiedText;

    sf::Font* font = nullptr;
    void build2var(TruthTable& t, float px, float py, float pw, float ph);
    void build3var(TruthTable& t, float px, float py, float pw, float ph);
    void build4var(TruthTable& t, float px, float py, float pw, float ph);

    static std::vector<int> grayOrder(int bits);
    static int getOutput(TruthTable& t, int rowIndex);

    void buildMintermString(TruthTable& t, float x, float y);
};