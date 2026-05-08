#include "KMapDrawer.h"
#include <cmath>
#include <sstream>

static const sf::Color CELL_ONE  = sf::Color(60, 180, 100, 200);  // green = 1
static const sf::Color CELL_ZERO = sf::Color(40, 40, 60,  200);   // dark  = 0
static const sf::Color CELL_LINE = sf::Color(100, 100, 140);
static const sf::Color HDR_COLOR = sf::Color(100, 160, 220);
std::vector<int> KMapDrawer::grayOrder(int bits) {
    int n = 1 << bits;
    std::vector<int> order(n);
    for (int i = 0; i < n; i++)
        order[i] = i ^ (i >> 1);  
    return order;
}

int KMapDrawer::getOutput(TruthTable& t, int rowIndex) {
    if (rowIndex < 0 || rowIndex >= (int)t.rows.size()) return 0;
    return t.rows[rowIndex].output ? 1 : 0;
}

void KMapDrawer::build(TruthTable& table, sf::Font& f,
                        float px, float py, float pw, float ph) {
    font = &f;
    cells.clear(); cellTexts.clear();
    gridLines.clear(); headerTexts.clear();

    int n = (int)table.vars.size();
    if (n < 2 || n > 4) {
      
        titleText.setFont(*font);
        titleText.setString("K-Map: supports 2-4 variables only.");
        titleText.setCharacterSize(13);
        titleText.setFillColor(sf::Color(220,100,100));
        titleText.setPosition(px+10, py+10);
        return;
    }

    titleText.setFont(*font);
    titleText.setString("Karnaugh Map");
    titleText.setCharacterSize(13);
    titleText.setFillColor(HDR_COLOR);
    titleText.setStyle(sf::Text::Bold);
    titleText.setPosition(px+10, py+4);

    if      (n == 2) build2var(table, px, py+24, pw, ph-24);
    else if (n == 3) build3var(table, px, py+24, pw, ph-24);
    else             build4var(table, px, py+24, pw, ph-24);
}

void KMapDrawer::build2var(TruthTable& t, float px, float py, float pw, float ph) {
    auto gc = grayOrder(2);  // [0,1,3,2]
    float cellW = std::min(pw/6.f, 80.f);
    float cellH = std::min(ph*0.5f, 60.f);
    float startX = px + 40;
    float startY = py + 30;

    // Column headers
    std::vector<std::string> colHdr = {"00","01","11","10"};
    std::string varLabel = t.vars[0] + t.vars[1];

    sf::Text vl; vl.setFont(*font); vl.setString(varLabel);
    vl.setCharacterSize(13); vl.setFillColor(HDR_COLOR);
    vl.setPosition(startX, startY - 22);
    headerTexts.push_back(vl);

    for (int c = 0; c < 4; c++) {
        sf::Text h; h.setFont(*font); h.setString(colHdr[c]);
        h.setCharacterSize(12); h.setFillColor(HDR_COLOR);
        auto b=h.getLocalBounds();
        h.setOrigin(b.left+b.width/2, b.top+b.height/2);
        h.setPosition(startX + cellW*c + cellW/2, startY - 10);
        headerTexts.push_back(h);

        int rowIdx = gc[c];
        int val = getOutput(t, rowIdx);

        sf::RectangleShape cell({cellW-2, cellH-2});
        cell.setPosition(startX + cellW*c + 1, startY + 1);
        cell.setFillColor(val ? CELL_ONE : CELL_ZERO);
        cell.setOutlineColor(CELL_LINE);
        cell.setOutlineThickness(1);
        cells.push_back(cell);

        sf::Text ct; ct.setFont(*font);
        ct.setString(std::to_string(val));
        ct.setCharacterSize(16); ct.setStyle(sf::Text::Bold);
        ct.setFillColor(sf::Color::White);
        auto b2=ct.getLocalBounds();
        ct.setOrigin(b2.left+b2.width/2, b2.top+b2.height/2);
        ct.setPosition(startX + cellW*c + cellW/2, startY + cellH/2);
        cellTexts.push_back(ct);
    }

    buildMintermString(t, startX, startY + cellH + 12);
}

// ── 3 variable K-Map──
void KMapDrawer::build3var(TruthTable& t, float px, float py, float pw, float ph) {
    auto colGC = grayOrder(2);  // BC: [0,1,3,2]
    float cellW = std::min((pw-60)/4.f, 80.f);
    float cellH = std::min((ph-60)/2.f, 60.f);
    float startX = px + 50;
    float startY = py + 30;

    std::vector<std::string> colHdr = {"00","01","11","10"};
    std::vector<std::string> rowHdr = {"0","1"};

    std::string colVar = t.vars[1] + t.vars[2];
    sf::Text cl; cl.setFont(*font); cl.setString(colVar);
    cl.setCharacterSize(13); cl.setFillColor(HDR_COLOR);
    cl.setPosition(startX + cellW*0.5f, startY - 22);
    headerTexts.push_back(cl);

    sf::Text rl; rl.setFont(*font); rl.setString(t.vars[0]);
    rl.setCharacterSize(13); rl.setFillColor(HDR_COLOR);
    rl.setPosition(startX - 38, startY + cellH*0.5f);
    headerTexts.push_back(rl);

    for (int c = 0; c < 4; c++) {
        sf::Text h; h.setFont(*font); h.setString(colHdr[c]);
        h.setCharacterSize(12); h.setFillColor(HDR_COLOR);
        auto b=h.getLocalBounds(); h.setOrigin(b.left+b.width/2,b.top+b.height/2);
        h.setPosition(startX+cellW*c+cellW/2, startY-10);
        headerTexts.push_back(h);
    }
    for (int r = 0; r < 2; r++) {
        sf::Text h; h.setFont(*font); h.setString(rowHdr[r]);
        h.setCharacterSize(12); h.setFillColor(HDR_COLOR);
        auto b=h.getLocalBounds(); h.setOrigin(b.left+b.width/2,b.top+b.height/2);
        h.setPosition(startX-14, startY+cellH*r+cellH/2);
        headerTexts.push_back(h);
    }

    for (int r = 0; r < 2; r++) {
        for (int c = 0; c < 4; c++) {
            // row=A value, col=BC gray code value
            int aVal  = r;
            int bcVal = colGC[c];
            int rowIdx = (aVal << 2) | bcVal;
            int val = getOutput(t, rowIdx);

            sf::RectangleShape cell({cellW-2, cellH-2});
            cell.setPosition(startX+cellW*c+1, startY+cellH*r+1);
            cell.setFillColor(val ? CELL_ONE : CELL_ZERO);
            cell.setOutlineColor(CELL_LINE); cell.setOutlineThickness(1);
            cells.push_back(cell);

            sf::Text ct; ct.setFont(*font);
            ct.setString(std::to_string(val));
            ct.setCharacterSize(15); ct.setStyle(sf::Text::Bold);
            ct.setFillColor(sf::Color::White);
            auto b=ct.getLocalBounds(); ct.setOrigin(b.left+b.width/2,b.top+b.height/2);
            ct.setPosition(startX+cellW*c+cellW/2, startY+cellH*r+cellH/2);
            cellTexts.push_back(ct);
        }
    }
    buildMintermString(t, startX, startY + cellH*2 + 12);
}

void KMapDrawer::build4var(TruthTable& t, float px, float py, float pw, float ph) {
    auto gc = grayOrder(2);  // [0,1,3,2] for both axes
    float cellW = std::min((pw-70)/4.f, 72.f);
    float cellH = std::min((ph-70)/4.f, 56.f);
    float startX = px + 60;
    float startY = py + 34;

    std::vector<std::string> hdr = {"00","01","11","10"};
    std::string colVar = t.vars[2] + t.vars[3];
    std::string rowVar = t.vars[0] + t.vars[1];

    sf::Text cl; cl.setFont(*font); cl.setString(colVar);
    cl.setCharacterSize(13); cl.setFillColor(HDR_COLOR);
    cl.setPosition(startX+cellW, startY-22);
    headerTexts.push_back(cl);

    sf::Text rl; rl.setFont(*font); rl.setString(rowVar);
    rl.setCharacterSize(13); rl.setFillColor(HDR_COLOR);
    rl.setPosition(startX-52, startY+cellH);
    headerTexts.push_back(rl);

    for (int i = 0; i < 4; i++) {
        // Col headers
        sf::Text ch; ch.setFont(*font); ch.setString(hdr[i]);
        ch.setCharacterSize(11); ch.setFillColor(HDR_COLOR);
        auto b=ch.getLocalBounds(); ch.setOrigin(b.left+b.width/2,b.top+b.height/2);
        ch.setPosition(startX+cellW*i+cellW/2, startY-11);
        headerTexts.push_back(ch);
        // Row headers
        sf::Text rh; rh.setFont(*font); rh.setString(hdr[i]);
        rh.setCharacterSize(11); rh.setFillColor(HDR_COLOR);
        auto b2=rh.getLocalBounds(); rh.setOrigin(b2.left+b2.width/2,b2.top+b2.height/2);
        rh.setPosition(startX-16, startY+cellH*i+cellH/2);
        headerTexts.push_back(rh);
    }

    for (int r = 0; r < 4; r++) {
        for (int c = 0; c < 4; c++) {
            int abVal = gc[r];
            int cdVal = gc[c];
            int rowIdx = (abVal << 2) | cdVal;
            int val = getOutput(t, rowIdx);

            sf::RectangleShape cell({cellW-2, cellH-2});
            cell.setPosition(startX+cellW*c+1, startY+cellH*r+1);
            cell.setFillColor(val ? CELL_ONE : CELL_ZERO);
            cell.setOutlineColor(CELL_LINE); cell.setOutlineThickness(1);
            cells.push_back(cell);

            sf::Text mn; mn.setFont(*font);
            mn.setString(std::to_string(rowIdx));
            mn.setCharacterSize(9); mn.setFillColor(sf::Color(160,160,180));
            mn.setPosition(startX+cellW*c+3, startY+cellH*r+2);
            cellTexts.push_back(mn);

            sf::Text ct; ct.setFont(*font);
            ct.setString(std::to_string(val));
            ct.setCharacterSize(15); ct.setStyle(sf::Text::Bold);
            ct.setFillColor(sf::Color::White);
            auto b=ct.getLocalBounds(); ct.setOrigin(b.left+b.width/2,b.top+b.height/2);
            ct.setPosition(startX+cellW*c+cellW/2, startY+cellH*r+cellH/2+4);
            cellTexts.push_back(ct);
        }
    }
    buildMintermString(t, startX, startY + cellH*4 + 12);
}

// ──implified SOP ────────────────
void KMapDrawer::buildMintermString(TruthTable& t, float x, float y) {
    std::vector<int> minterms;
    for (int i = 0; i < (int)t.rows.size(); i++)
        if (t.rows[i].output) minterms.push_back(i);

    std::string s = "Sm(";
    for (int i = 0; i < (int)minterms.size(); i++) {
        if (i) s += ",";
        s += std::to_string(minterms[i]);
    }
    s += ")";

    mintermText.setFont(*font);
    mintermText.setString(s);
    mintermText.setCharacterSize(13);
    mintermText.setFillColor(sf::Color(180, 220, 140));
    mintermText.setPosition(x, y);

    // Simplified SOP expression via Tabluar method
    std::string simp = t.simplify();
    simplifiedText.setFont(*font);
    simplifiedText.setString("Simplified: " + simp);
    simplifiedText.setCharacterSize(13);
    simplifiedText.setFillColor(sf::Color(255, 200, 100));
    simplifiedText.setStyle(sf::Text::Bold);
    simplifiedText.setPosition(x, y + 20);
}

void KMapDrawer::draw(sf::RenderTarget& window) {
    window.draw(titleText);
    for (auto& c : cells)       window.draw(c);
    for (auto& t : cellTexts)   window.draw(t);
    for (auto& h : headerTexts) window.draw(h);
    window.draw(mintermText);
    window.draw(simplifiedText);
}