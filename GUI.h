#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include "Tokenizer.h"
#include "Parser.h"
#include "TruthTable.h"
#include "CircuitDrawer.h"
#include "KMapDrawer.h"

class GUI {
public:
    GUI();
    void run();

private:
    sf::RenderWindow window;
    sf::Font font;

    // ── Top bar ──
    sf::Text titleText;

    // ── Theme toggle ──
    sf::RectangleShape themeToggleBg;
    sf::CircleShape    themeToggleKnob;
    sf::Text           themeToggleLabel;
    bool               darkMode;

    sf::RectangleShape inputBox;
    sf::Text           inputText;
    std::string        inputString;
    bool               inputFocused;

    sf::RectangleShape runButton;
    sf::Text           runButtonText;
    sf::RectangleShape exportButton;
    sf::Text           exportButtonText;
    sf::RectangleShape screenshotButton;
    sf::Text           screenshotButtonText;
    sf::RectangleShape kmapButton;
    sf::Text           kmapButtonText;
    sf::RectangleShape circuitPanel;
    sf::RectangleShape tablePanel;
    sf::RectangleShape kmapPanel;
    sf::Text circuitLabel;
    sf::Text tableLabel;
    sf::Text kmapLabel;
    sf::Text statusText;
    sf::RectangleShape statsBox;
    sf::Text           statsText;
    Tokenizer     tokenizer;
    Parser        parser;
    TruthTable    truthTable;
    CircuitDrawer circuitDrawer;
    KMapDrawer    kmapDrawer;
    std::shared_ptr<Node> root;
    bool hasResult;
    bool showKmap;

    std::vector<sf::Text>           tableTexts;
    std::vector<sf::Vertex>         gridLines;
    std::vector<sf::RectangleShape> rowHighlights;

    sf::Text mintermLabel;

    int   selectedRow;
    float rowH_cache;
    float tableStartY_cache;

    // ── Theme colors ──
    sf::Color cBg, cPanel, cInputBg, cInputFocus,
              cBorder, cText, cDim, cGrid;

    void handleEvents();
    void update();
    void draw();
    void setupUI();
    void applyTheme();

    void onRunClicked();
    void rebuildCircuit();
    void buildTableTexts();
    void buildStatsText();
    void buildKMap();
    void onTableClick(float mouseY);
    void onExportClicked();
    void onScreenshotClicked();
    void onThemeToggle();
    void onKmapToggle();
};