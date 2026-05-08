#include "GUI.h"
#include <fstream>
#include <functional>
#include <iostream>
#include <iomanip>
#include <sstream>

GUI::GUI()
    : window(sf::VideoMode(1200, 720), "Digital Logic Simulator",
             sf::Style::Default),
      inputFocused(false), hasResult(false),
      showKmap(false), darkMode(true),
      selectedRow(-1), rowH_cache(20), tableStartY_cache(0)
{
    window.setFramerateLimit(60);
    if (!font.loadFromFile("assets/fonts/DejaVuSans.ttf"))
        font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf");
    applyTheme();
    setupUI();
}

void GUI::applyTheme() {
    if (darkMode) {
        cBg         = sf::Color(28,  28,  38 );
        cPanel      = sf::Color(42,  42,  58 );
        cInputBg    = sf::Color(55,  55,  75 );
        cInputFocus = sf::Color(75,  75,  105);
        cBorder     = sf::Color(85,  85,  115);
        cText       = sf::Color(220, 220, 220);
        cDim        = sf::Color(130, 130, 155);
        cGrid       = sf::Color(65,  65,  90 );
    } else {
        cBg         = sf::Color(240, 242, 248);
        cPanel      = sf::Color(255, 255, 255);
        cInputBg    = sf::Color(225, 228, 238);
        cInputFocus = sf::Color(200, 210, 235);
        cBorder     = sf::Color(170, 175, 200);
        cText       = sf::Color(30,  30,  50 );
        cDim        = sf::Color(100, 105, 130);
        cGrid       = sf::Color(190, 195, 215);
    }
}

static void makeBtn(sf::RectangleShape& btn, sf::Text& lbl,
                    sf::Font& font, const std::string& txt,
                    float x, float y, float w, float h,
                    sf::Color fill, unsigned charSize=14) {
    btn.setSize({w, h});
    btn.setPosition(x, y);
    btn.setFillColor(fill);
    lbl.setFont(font);
    lbl.setString(txt);
    lbl.setCharacterSize(charSize);
    lbl.setFillColor(sf::Color::White);
    lbl.setStyle(sf::Text::Bold);
    auto b = lbl.getLocalBounds();
    lbl.setOrigin(b.left+b.width/2, b.top+b.height/2);
    lbl.setPosition(x+w/2, y+h/2);
}

// ── setupUI ──
void GUI::setupUI() {
    float W = window.getSize().x;
    float H = window.getSize().y;
    float pad   = 12;
    float topH  = 44;  
    float rowH2 = 38;   
    float footH = 26;
    float btnH  = 30;

    float kmapH  = showKmap ? 320.f : 0.f;
    float panelY = topH + rowH2 + pad;
    float panelH = H - panelY - footH - pad - kmapH - (showKmap ? pad : 0);
    float halfW  = (W - pad*3) / 2.f;

    // ── Title ──
    titleText.setFont(font);
    titleText.setString("DIGITAL LOGIC SIMULATOR");
    titleText.setCharacterSize(19);
    titleText.setFillColor(sf::Color(100, 180, 255));
    titleText.setStyle(sf::Text::Bold);
    titleText.setPosition(pad, 12);

    // ── Theme toggle ── 
    float tgW=52, tgH=22, tgX=W-tgW-pad, tgY=11;
    themeToggleBg.setSize({tgW, tgH});
    themeToggleBg.setPosition(tgX, tgY);
    themeToggleBg.setFillColor(darkMode ? sf::Color(70,100,180) : sf::Color(180,180,100));
    themeToggleBg.setOutlineThickness(1);
    themeToggleBg.setOutlineColor(cBorder);

    float knobX = darkMode ? tgX+tgW-tgH+2 : tgX+2;
    themeToggleKnob.setRadius(tgH/2-2);
    themeToggleKnob.setPosition(knobX, tgY+2);
    themeToggleKnob.setFillColor(sf::Color::White);

    themeToggleLabel.setString(""); 
    float iy = topH + 4;
    float bx = W - pad;

    // RUN
    bx -= 90;
    makeBtn(runButton, runButtonText, font, "RUN", bx, iy, 90, btnH,
            sf::Color(70,130,200));

    // EXPORT
    bx -= pad + 100;
    makeBtn(exportButton, exportButtonText, font, "EXPORT", bx, iy, 100, btnH,
            sf::Color(50,130,80));

    // SCREENSHOT
    bx -= pad + 110;
    makeBtn(screenshotButton, screenshotButtonText, font, "SCREENSHOT", bx, iy, 110, btnH,
            sf::Color(110,80,160));

    // KMAP toggle
    bx -= pad + 80;
    makeBtn(kmapButton, kmapButtonText, font,
            showKmap ? "KMAP [^]" : "KMAP [v]",
            bx, iy, 80, btnH,
            showKmap ? sf::Color(160,120,40) : sf::Color(120,100,50));

    // Input box (fills remaining left)
    inputBox.setSize({bx - pad*2, btnH});
    inputBox.setPosition(pad, iy);
    inputBox.setFillColor(inputFocused ? cInputFocus : cInputBg);
    inputBox.setOutlineThickness(1);
    inputBox.setOutlineColor(cBorder);

    inputText.setFont(font);
    inputText.setCharacterSize(15);
    inputText.setFillColor(cText);
    inputText.setPosition(pad+8, iy+8);

    // ── Circuit panel ──
    circuitPanel.setSize({halfW, panelH});
    circuitPanel.setPosition(pad, panelY);
    circuitPanel.setFillColor(cPanel);
    circuitPanel.setOutlineThickness(1);
    circuitPanel.setOutlineColor(cBorder);

    circuitLabel.setFont(font);
    circuitLabel.setString("Circuit Diagram");
    circuitLabel.setCharacterSize(11);
    circuitLabel.setFillColor(cDim);
    circuitLabel.setPosition(pad+5, panelY+3);

    // ── Stats box (bottom of circuit panel) ──
    float sbH = 36, sbW = halfW - 20;
    statsBox.setSize({sbW, sbH});
    statsBox.setPosition(pad+10, panelY+panelH-sbH-8);
    statsBox.setFillColor(sf::Color(0,0,0,120));
    statsBox.setOutlineColor(cBorder);
    statsBox.setOutlineThickness(1);

    statsText.setFont(font);
    statsText.setCharacterSize(11);
    statsText.setFillColor(sf::Color(180,220,180));
    statsText.setPosition(pad+16, panelY+panelH-sbH-4);

    // ── Truth table panel ──
    tablePanel.setSize({halfW, panelH});
    tablePanel.setPosition(pad*2+halfW, panelY);
    tablePanel.setFillColor(cPanel);
    tablePanel.setOutlineThickness(1);
    tablePanel.setOutlineColor(cBorder);

    tableLabel.setFont(font);
    tableLabel.setString("Truth Table");
    tableLabel.setCharacterSize(11);
    tableLabel.setFillColor(cDim);
    tableLabel.setPosition(pad*2+halfW+5, panelY+3);

    // ── KMap panel (below, full width) ──
    if (showKmap) {
        float ky = panelY + panelH + pad;
        kmapPanel.setSize({W - pad*2, kmapH});
        kmapPanel.setPosition(pad, ky);
        kmapPanel.setFillColor(cPanel);
        kmapPanel.setOutlineThickness(1);
        kmapPanel.setOutlineColor(cBorder);
    }

    // ── Status bar ──
    statusText.setFont(font);
    statusText.setCharacterSize(12);
    statusText.setFillColor(cDim);
    statusText.setPosition(pad, H-footH+5);
    if (statusText.getString().isEmpty())
        statusText.setString("Enter expression (& | ! ^ parentheses) --> RUN");
}

// ── run ───
void GUI::run() {
    while (window.isOpen()) {
        handleEvents();
        update();
        draw();
    }
}

// ── handleEvents ──
void GUI::handleEvents() {
    sf::Event e;
    while (window.pollEvent(e)) {
        if (e.type == sf::Event::Closed) window.close();

        if (e.type == sf::Event::Resized) {
            window.setView(sf::View(sf::FloatRect(0,0,e.size.width,e.size.height)));
            setupUI();
            if (hasResult) { rebuildCircuit(); buildTableTexts(); buildStatsText(); if(showKmap) buildKMap(); }
        }

        if (e.type == sf::Event::MouseButtonPressed) {
            sf::Vector2f m = {(float)e.mouseButton.x, (float)e.mouseButton.y};
            inputFocused = inputBox.getGlobalBounds().contains(m);

            if (runButton.getGlobalBounds().contains(m))        onRunClicked();
            if (exportButton.getGlobalBounds().contains(m))     onExportClicked();
            if (screenshotButton.getGlobalBounds().contains(m)) onScreenshotClicked();
            if (kmapButton.getGlobalBounds().contains(m))       onKmapToggle();
            if (themeToggleBg.getGlobalBounds().contains(m))    onThemeToggle();

            if (hasResult && tablePanel.getGlobalBounds().contains(m))
                onTableClick(m.y);
        }

        if (e.type == sf::Event::TextEntered && inputFocused) {
            if (e.text.unicode == '\b') {
                if (!inputString.empty()) inputString.pop_back();
            } else if (e.text.unicode=='\r' || e.text.unicode=='\n') {
                onRunClicked();
            } else if (e.text.unicode < 128) {
                inputString += (char)e.text.unicode;
            }
        }
    }
}

// ── update ───
void GUI::update() {
    inputText.setString(inputString);
    inputBox.setFillColor(inputFocused ? cInputFocus : cInputBg);

    sf::Vector2f m = (sf::Vector2f)sf::Mouse::getPosition(window);
    auto hover = [&](sf::RectangleShape& b, sf::Color base, sf::Color hov){
        b.setFillColor(b.getGlobalBounds().contains(m) ? hov : base);
    };
    hover(runButton,        sf::Color(70,130,200),  sf::Color(100,160,240));
    hover(exportButton,     sf::Color(50,130,80),   sf::Color(70,170,110));
    hover(screenshotButton, sf::Color(110,80,160),  sf::Color(140,110,200));
    hover(kmapButton,
          showKmap ? sf::Color(160,120,40) : sf::Color(120,100,50),
          showKmap ? sf::Color(200,160,60) : sf::Color(160,140,80));
}

// ── draw ───
void GUI::draw() {
    window.clear(cBg);

    window.draw(titleText);
    window.draw(themeToggleBg);
    window.draw(themeToggleKnob);
    window.draw(themeToggleLabel);

    window.draw(inputBox);
    window.draw(inputText);
    window.draw(runButton);        window.draw(runButtonText);
    window.draw(exportButton);     window.draw(exportButtonText);
    window.draw(screenshotButton); window.draw(screenshotButtonText);
    window.draw(kmapButton);       window.draw(kmapButtonText);

    window.draw(circuitPanel);
    window.draw(circuitLabel);
    window.draw(tablePanel);
    window.draw(tableLabel);

    if (showKmap) {
        window.draw(kmapPanel);
        kmapDrawer.draw(window);
    }

    // Grid + highlights + table
    if (!gridLines.empty())
        window.draw(gridLines.data(), gridLines.size(), sf::Lines);
    for (auto& r : rowHighlights) window.draw(r);
    for (auto& t : tableTexts)    window.draw(t);
    window.draw(mintermLabel);

    // Circuit + stats
    circuitDrawer.draw(window);
    if (hasResult) {
        window.draw(statsBox);
        window.draw(statsText);
    }

    window.draw(statusText);
    window.display();
}

// ── onRunClicked ───
void GUI::onRunClicked() {
    if (inputString.empty()) {
        statusText.setString("Please enter an expression first.");
        statusText.setFillColor(sf::Color(220,100,100));
        hasResult=false; tableTexts.clear(); gridLines.clear();
        return;
    }
    try {
        auto tokens = tokenizer.tokenize(inputString);
        root        = parser.parse(tokens);
        truthTable.generate(root, parser.varMap);
        hasResult=true; selectedRow=-1;

        rebuildCircuit();
        buildTableTexts();
        buildStatsText();
        if (showKmap) buildKMap();

        statusText.setString("OK: " + inputString +
            "  |  Vars: " + std::to_string(parser.varMap.size()) +
            "  |  Rows: " + std::to_string(truthTable.rows.size()));
        statusText.setFillColor(sf::Color(100,220,100));
    } catch (std::exception& ex) {
        tableTexts.clear(); gridLines.clear(); hasResult=false;
        statusText.setString(std::string("Error: ") + ex.what());
        statusText.setFillColor(sf::Color(220,100,100));
    }
}

void GUI::rebuildCircuit() {
    auto pos  = circuitPanel.getPosition();
    auto size = circuitPanel.getSize();
    circuitDrawer.build(root, font, pos.x, pos.y+18, size.x, size.y-60);
}

// ── Stats text ───
void GUI::buildStatsText() {
    if (!hasResult) return;

    int andC=0, orC=0, notC=0, xorC=0;
    std::function<void(std::shared_ptr<Node>)> count = [&](std::shared_ptr<Node> n){
        if (!n) return;
        if (dynamic_cast<AndGate*>(n.get())) { andC++; auto* g=dynamic_cast<AndGate*>(n.get()); count(g->left); count(g->right); }
        else if (dynamic_cast<OrGate*>(n.get()))  { orC++;  auto* g=dynamic_cast<OrGate*>(n.get());  count(g->left); count(g->right); }
        else if (dynamic_cast<NotGate*>(n.get()))  { notC++; auto* g=dynamic_cast<NotGate*>(n.get()); count(g->child); }
        else if (dynamic_cast<XorGate*>(n.get()))  { xorC++; auto* g=dynamic_cast<XorGate*>(n.get()); count(g->left); count(g->right); }
    };
    count(root);

    std::function<int(std::shared_ptr<Node>)> depth = [&](std::shared_ptr<Node> n) -> int {
        if (!n || dynamic_cast<InputNode*>(n.get())) return 0;
        if (auto* g=dynamic_cast<NotGate*>(n.get())) return 1+depth(g->child);
        if (auto* g=dynamic_cast<AndGate*>(n.get())) return 1+std::max(depth(g->left),depth(g->right));
        if (auto* g=dynamic_cast<OrGate*>(n.get()))  return 1+std::max(depth(g->left),depth(g->right));
        if (auto* g=dynamic_cast<XorGate*>(n.get())) return 1+std::max(depth(g->left),depth(g->right));
        return 0;
    };

    // Count minterms
    int minterms=0;
    for (auto& r : truthTable.rows) if (r.output) minterms++;

    int totalGates = andC+orC+notC+xorC;
    int vars = (int)parser.varMap.size();
    int totalRows = (int)truthTable.rows.size();

    std::string s =
        "Gates: " + std::to_string(totalGates) +
        "  (AND:" + std::to_string(andC) +
        " OR:"   + std::to_string(orC)  +
        " NOT:"  + std::to_string(notC) +
        " XOR:"  + std::to_string(xorC) + ")" +
        "   Depth: " + std::to_string(depth(root)) +
        "   Inputs: " + std::to_string(vars) +
        "   Minterms: " + std::to_string(minterms) + "/" + std::to_string(totalRows);

    statsText.setString(s);
}

// ── K-Map ───
void GUI::buildKMap() {
    if (!hasResult || !showKmap) return;
    auto pos  = kmapPanel.getPosition();
    auto size = kmapPanel.getSize();
    kmapDrawer.build(truthTable, font, pos.x, pos.y + 18, size.x, size.y - 18);
}

// ── Truth table ───
void GUI::buildTableTexts() {
    tableTexts.clear(); gridLines.clear(); rowHighlights.clear();

    auto& vars = truthTable.vars;
    int n    = (int)vars.size();
    int rows = (int)truthTable.rows.size();

    auto tpos  = tablePanel.getPosition();
    auto tsize = tablePanel.getSize();

    float marginTop  = 20;
    float marginSide = 8;
    float availW = tsize.x - marginSide*2;
    float availH = tsize.y - marginTop - 24;

    int   totalCols = n+1;
    float colW      = availW / totalCols;
    int   totalRows = rows+1;
    float rowH      = std::min(std::max(availH/totalRows, 10.f), 22.f);
    unsigned cs     = std::max(8u, (unsigned)(rowH*0.58f));

    float sx = tpos.x + marginSide;
    float sy = tpos.y + marginTop;
    rowH_cache = rowH; tableStartY_cache = sy;

    for (int c=0; c<n; c++) {
        sf::Text t; t.setFont(font); t.setString(vars[c]);
        t.setCharacterSize(cs); t.setFillColor(sf::Color(150,210,255)); t.setStyle(sf::Text::Bold);
        auto b=t.getLocalBounds(); t.setOrigin(b.left+b.width/2,b.top+b.height/2);
        t.setPosition(sx+colW*c+colW/2, sy+rowH/2);
        tableTexts.push_back(t);
    }
    { sf::Text t; t.setFont(font); t.setString("OUT");
      t.setCharacterSize(cs); t.setFillColor(sf::Color(255,180,80)); t.setStyle(sf::Text::Bold);
      auto b=t.getLocalBounds(); t.setOrigin(b.left+b.width/2,b.top+b.height/2);
      t.setPosition(sx+colW*n+colW/2, sy+rowH/2);
      tableTexts.push_back(t); }

    for (int r=0; r<rows; r++) {
        auto& row = truthTable.rows[r];
        float ry  = sy + rowH*(r+1);
        sf::Color outCol = row.output ? sf::Color(80,200,120) : sf::Color(190,190,200);
        for (int c=0; c<n; c++) {
            sf::Text t; t.setFont(font);
            t.setString(std::to_string(row.inputs.at(vars[c])));
            t.setCharacterSize(cs); t.setFillColor(cText);
            auto b=t.getLocalBounds(); t.setOrigin(b.left+b.width/2,b.top+b.height/2);
            t.setPosition(sx+colW*c+colW/2, ry+rowH/2);
            tableTexts.push_back(t);
        }
        sf::Text t; t.setFont(font); t.setString(std::to_string(row.output));
        t.setCharacterSize(cs); t.setFillColor(outCol); t.setStyle(sf::Text::Bold);
        auto b=t.getLocalBounds(); t.setOrigin(b.left+b.width/2,b.top+b.height/2);
        t.setPosition(sx+colW*n+colW/2, ry+rowH/2);
        tableTexts.push_back(t);
    }

    if (selectedRow>=0 && selectedRow<rows) {
        float ry = sy+rowH*(selectedRow+1);
        sf::RectangleShape hl({availW,rowH});
        hl.setPosition(sx,ry);
        hl.setFillColor(sf::Color(80,100,150,120));
        rowHighlights.push_back(hl);
    }

    float totalH = rowH*(rows+1);
    for (int r=0; r<=rows+1; r++) {
        float gy=sy+rowH*r;
        sf::Color lc=(r<=1)?sf::Color(120,140,180):cGrid;
        gridLines.push_back({{sx,gy},lc}); gridLines.push_back({{sx+availW,gy},lc});
    }
    for (int c=0; c<=totalCols; c++) {
        float gx=sx+colW*c;
        sf::Color lc=(c==n)?sf::Color(160,120,60):cGrid;
        gridLines.push_back({{gx,sy},lc}); gridLines.push_back({{gx,sy+totalH},lc});
    }

    std::vector<int> mts;
    for (int i=0;i<rows;i++) if(truthTable.rows[i].output) mts.push_back(i);
    std::string ms = "Sm(";
    for (int i=0;i<(int)mts.size();i++) { if(i) ms+=","; ms+=std::to_string(mts[i]); }
    ms += ")";
    mintermLabel.setFont(font);
    mintermLabel.setString(ms);
    mintermLabel.setCharacterSize(11);
    mintermLabel.setFillColor(sf::Color(160,210,130));
    float labelY = sy + totalH + 3;
    float panelBottom = tpos.y + tsize.y - 14;
    labelY = std::min(labelY, panelBottom);
    mintermLabel.setPosition(sx, labelY);
}

// ── Row click ───
void GUI::onTableClick(float mouseY) {
    float dataY = tableStartY_cache + rowH_cache;
    int clicked = (int)((mouseY - dataY) / rowH_cache);
    int rows    = (int)truthTable.rows.size();

    if (clicked<0 || clicked>=rows || clicked==selectedRow) {
        selectedRow=-1;
        buildTableTexts(); rebuildCircuit();
        statusText.setString("Row deselected.");
        statusText.setFillColor(cDim);
        return;
    }
    selectedRow = clicked;
    buildTableTexts();
    auto& row = truthTable.rows[selectedRow];
    std::map<std::string,bool> vals(row.inputs.begin(),row.inputs.end());
    circuitDrawer.highlight(root, vals);

    std::string msg="Row "+std::to_string(selectedRow+1)+":  ";
    for (auto& v:truthTable.vars) msg+=v+"="+std::to_string(vals[v])+"  ";
    msg+="->  OUT="+std::to_string(row.output);
    statusText.setString(msg);
    statusText.setFillColor(row.output?sf::Color(80,220,100):sf::Color(220,100,80));
}

// ── Theme toggle ───
void GUI::onThemeToggle() {
    darkMode = !darkMode;
    applyTheme();
    setupUI();
    if (hasResult) { buildTableTexts(); rebuildCircuit(); buildStatsText(); if(showKmap) buildKMap(); }
}

// ── KMap toggle ───
void GUI::onKmapToggle() {
    showKmap = !showKmap;
    setupUI();
    if (hasResult) { rebuildCircuit(); buildTableTexts(); buildStatsText(); if(showKmap) buildKMap(); }
}

void GUI::onExportClicked() {
    if (!hasResult) {
        statusText.setString("Nothing to export. Run first.");
        statusText.setFillColor(sf::Color(220,100,100)); return;
    }
    std::string fname="truthtable";
    for (char c:inputString) fname+=(isalnum(c)?c:'_');
    fname+=".txt";
    std::ofstream f(fname);
    if (!f.is_open()) {
        statusText.setString("Error: cannot create "+fname);
        statusText.setFillColor(sf::Color(220,100,100)); return;
    }
    auto& vars=truthTable.vars; int n=(int)vars.size(); int cw=6;
    f<<"========================================\n";
    f<<"  DIGITAL LOGIC SIMULATOR - Truth Table\n";
    f<<"========================================\n";
    f<<"  Expression: "<<inputString<<"\n";
    f<<"  Variables : "; for(auto& v:vars) f<<v<<" "; f<<"\n";
    f<<"  Rows      : "<<truthTable.rows.size()<<"\n";
    f<<"========================================\n\n";
    for(auto& v:vars) f<<std::setw(cw)<<v;
    f<<std::setw(cw)<<"| OUT\n";
    f<<std::string(n*cw+cw+2,'-')<<"\n";
    for(auto& row:truthTable.rows){
        for(auto& v:vars) f<<std::setw(cw)<<row.inputs.at(v);
        f<<std::setw(cw)<<("| "+std::to_string(row.output))<<"\n";
    }

    f<<"\n  Minterms (Sm): ";
    for(auto& row:truthTable.rows) if(row.output) { /* collect */ }
    std::vector<int> mts;
    for(int i=0;i<(int)truthTable.rows.size();i++) if(truthTable.rows[i].output) mts.push_back(i);
    for(int i=0;i<(int)mts.size();i++){if(i) f<<","; f<<mts[i];}
    f<<"\n\n========================================\n";
    f<<"  Generated by Digital Logic Simulator\n";
    f<<"========================================\n";
    f.close();
    statusText.setString("Exported: "+fname);
    statusText.setFillColor(sf::Color(100,220,100));
}

void GUI::onScreenshotClicked() {
    if (!hasResult) {
        statusText.setString("Run an expression first.");
        statusText.setFillColor(sf::Color(220,100,100)); return;
    }
    auto cpos  = circuitPanel.getPosition();
    auto csize = circuitPanel.getSize();

    sf::RenderTexture rt;
    rt.create((unsigned)csize.x, (unsigned)csize.y);
    rt.clear(sf::Color(42,42,58));

    sf::View v(sf::FloatRect(cpos.x, cpos.y, csize.x, csize.y));
    rt.setView(v);

    circuitDrawer.draw(rt);
    sf::Text exprLabel;
    exprLabel.setFont(font);
    exprLabel.setString(inputString);
    exprLabel.setCharacterSize(13);
    exprLabel.setFillColor(sf::Color(180,220,255));
    exprLabel.setPosition(cpos.x+8, cpos.y+4);
    rt.draw(exprLabel);

    rt.display();

    std::string fname="circuit";
    for(char c:inputString) fname+=(isalnum(c)?c:'_');
    fname+=".png";
    if (rt.getTexture().copyToImage().saveToFile(fname)) {
        statusText.setString("Screenshot saved: "+fname);
        statusText.setFillColor(sf::Color(100,220,100));
    } else {
        statusText.setString("Screenshot failed.");
        statusText.setFillColor(sf::Color(220,100,100));
    }
}