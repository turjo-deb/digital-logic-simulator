#include "CircuitDrawer.h"
#include <cmath>
#include <algorithm>

static const float PI = 3.14159265f;

static const sf::Color COL_WIRE     = sf::Color(180, 200, 220); 
static const sf::Color COL_HIGH     = sf::Color(80,  220, 100);  // signal=1 green
static const sf::Color COL_LOW      = sf::Color(220, 80,  80);   // signal=0 red

static const sf::Color COL_GATE     = sf::Color(220, 220, 220);
static const sf::Color COL_INPUT    = sf::Color(60,  180, 120);
static const sf::Color COL_OUT_OFF  = sf::Color(180, 90,  40);
static const sf::Color COL_OUT_ON   = sf::Color(80,  220, 100);

// ── Tree helpers ───
int CircuitDrawer::treeDepth(std::shared_ptr<Node> n) {
    if (!n) return 0;
    if (dynamic_cast<InputNode*>(n.get())) return 1;
    if (auto* ng = dynamic_cast<NotGate*>(n.get()))
        return 1 + treeDepth(ng->child);
    if (auto* ag = dynamic_cast<AndGate*>(n.get()))
        return 1 + std::max(treeDepth(ag->left), treeDepth(ag->right));
    if (auto* og = dynamic_cast<OrGate*>(n.get()))
        return 1 + std::max(treeDepth(og->left), treeDepth(og->right));
    if (auto* xg = dynamic_cast<XorGate*>(n.get()))
        return 1 + std::max(treeDepth(xg->left), treeDepth(xg->right));
    return 1;
}

int CircuitDrawer::treeLeaves(std::shared_ptr<Node> n) {
    if (!n) return 0;
    if (dynamic_cast<InputNode*>(n.get())) return 1;
    if (auto* ng = dynamic_cast<NotGate*>(n.get()))
        return treeLeaves(ng->child);
    if (auto* ag = dynamic_cast<AndGate*>(n.get()))
        return treeLeaves(ag->left) + treeLeaves(ag->right);
    if (auto* og = dynamic_cast<OrGate*>(n.get()))
        return treeLeaves(og->left) + treeLeaves(og->right);
    if (auto* xg = dynamic_cast<XorGate*>(n.get()))
        return treeLeaves(xg->left) + treeLeaves(xg->right);
    return 1;
}


void CircuitDrawer::build(std::shared_ptr<Node> root, sf::Font& f,
                           float px, float py, float pw, float ph) {
    font=&f; PX=px; PY=py; PW=pw; PH=ph;
    hlMode=false; hlVals=nullptr;
    gates.clear(); wires.clear();

    int depth  = treeDepth(root);
    int leaves = treeLeaves(root);


    float maxGW = (PW - 40) / (depth + 1.5f) - 6;
    GW = std::min(60.f, std::max(16.f, maxGW));
    float maxGH = (PH - 50) / std::max(leaves, 1) - 6;
    GH = std::min(40.f, std::max(12.f, maxGH));
    GAP = GW * 0.45f;

    float step = GW + GAP;
    float outBlockW = GW * 0.85f;
    float leftmostX = PX + 10;
    float rootX = leftmostX + (depth - 1) * step;
    float maxRootX = PX + PW - GW - outBlockW - GAP - 20;
    rootX = std::min(rootX, maxRootX);

    float rootY = PY + PH / 2.f;

    auto res = place(root, rootX, rootY, (PH - 50) / 2.f);

    DrawnGate outG = makeOutput(res.x + GAP*0.6f + outBlockW/2, res.y, false);
    gates.push_back(outG);
    wire(res.x, res.y, outG.leftX, res.y, false, false);
}
void CircuitDrawer::highlight(std::shared_ptr<Node> root,
                               std::map<std::string, bool>& inputVals) {
    hlMode = true;
    hlVals = &inputVals;
    gates.clear(); wires.clear();

    float outBlockW = GW * 0.85f;
    int depth = treeDepth(root);
    float step = GW + GAP;
    float leftmostX = PX + 10;
    float rootX = std::min(leftmostX + (depth-1)*step, PX + PW - GW - outBlockW - GAP - 20);
    float rootY = PY + PH / 2.f;

    auto res = place(root, rootX, rootY, (PH - 50) / 2.f);
    DrawnGate outG = makeOutput(res.x + GAP*0.6f + outBlockW/2, res.y, res.val);
    gates.push_back(outG);
    wire(res.x, res.y, outG.leftX, res.y, res.val, true);

    hlMode = false; hlVals = nullptr;
}
CircuitDrawer::PlaceResult CircuitDrawer::place(
    std::shared_ptr<Node> node, float x, float y, float ySpan)
{
    if (!node) return {x, y, false};
    float cx = x + GW/2, cy = y;

    if (auto* inp = dynamic_cast<InputNode*>(node.get())) {
        bool val = hlMode && hlVals ? (*hlVals)[inp->name] : false;

        DrawnGate g = makeInput(inp->name, cx, cy);
        if (hlMode) {
            sf::Color bc = val ? COL_HIGH : COL_LOW;
            for (auto& s : g.shapes) s.setOutlineColor(bc);
        }
        gates.push_back(g);
        return {g.rightX, cy, val};
    }

    // ── NOT ──
    if (auto* ng = dynamic_cast<NotGate*>(node.get())) {
        auto co = place(ng->child, x - GW - GAP, y, ySpan);
        bool val = !co.val;
        DrawnGate g = makeNot(cx, cy);
        gates.push_back(g);
        wire(co.x, co.y, g.leftX, cy, co.val, hlMode);
        return {g.rightX, cy, val};
    }

    std::shared_ptr<Node> left=nullptr, right=nullptr;
    bool isAnd=false, isOr=false, isXor=false;

    if (auto* ag = dynamic_cast<AndGate*>(node.get()))
        { left=ag->left; right=ag->right; isAnd=true; }
    else if (auto* og = dynamic_cast<OrGate*>(node.get()))
        { left=og->left; right=og->right; isOr=true; }
    else if (auto* xg = dynamic_cast<XorGate*>(node.get()))
        { left=xg->left; right=xg->right; isXor=true; }

    if (left && right) {
        float childX = x - GW - GAP;
        float half   = ySpan / 2.f;
        auto lo = place(left,  childX, y - half, half*0.8f);
        auto ro = place(right, childX, y + half, half*0.8f);

        bool val = isAnd ? (lo.val && ro.val)
                 : isOr  ? (lo.val || ro.val)
                 : (lo.val ^ ro.val);

        DrawnGate g = isAnd ? makeAnd(cx,cy)
                    : isOr  ? makeOr(cx,cy)
                    : makeXor(cx,cy);
        gates.push_back(g);

        wire(lo.x, lo.y, g.leftX, g.inputY1, lo.val, hlMode);
        wire(ro.x, ro.y, g.leftX, g.inputY2, ro.val, hlMode);
        return {g.rightX, cy, val};
    }

    return {x+GW, y, false};
}

static sf::Vector2f bezier2(sf::Vector2f p0, sf::Vector2f p1, sf::Vector2f p2, float t) {
    float it=1-t;
    return {it*it*p0.x+2*it*t*p1.x+t*t*p2.x,
            it*it*p0.y+2*it*t*p1.y+t*t*p2.y};
}

DrawnGate CircuitDrawer::makeInput(const std::string& name, float cx, float cy) {
    DrawnGate g; g.cx=cx; g.cy=cy;
    float hw=GW*0.44f, hh=GH*0.40f, tip=hh*0.55f;
    sf::ConvexShape box(6);
    box.setPoint(0,{cx-hw,cy-hh}); box.setPoint(1,{cx+hw-tip,cy-hh});
    box.setPoint(2,{cx+hw,cy});    box.setPoint(3,{cx+hw-tip,cy+hh});
    box.setPoint(4,{cx-hw,cy+hh}); box.setPoint(5,{cx-hw,cy});
    box.setFillColor(COL_INPUT);
    box.setOutlineColor(sf::Color(150,255,180));
    box.setOutlineThickness(1.5f);
    g.shapes.push_back(box);
    g.label.setFont(*font);
    g.label.setString(name);
    g.label.setCharacterSize(std::max(9u,(unsigned)(GH*0.46f)));
    g.label.setFillColor(sf::Color::White);
    g.label.setStyle(sf::Text::Bold);
    auto lb=g.label.getLocalBounds();
    g.label.setOrigin(lb.left+lb.width/2,lb.top+lb.height/2);
    g.label.setPosition(cx,cy);
    g.leftX=cx-hw; g.rightX=cx+hw-tip;
    g.inputY1=g.inputY2=cy;
    return g;
}

DrawnGate CircuitDrawer::makeAnd(float cx, float cy) {
    DrawnGate g; g.cx=cx; g.cy=cy;
    float hw=GW*0.44f, hh=GH*0.44f;
    int N=16;
    sf::ConvexShape shape(N+2);
    shape.setPoint(0,{cx-hw,cy-hh});
    for (int i=0;i<N;i++) {
        float a=PI/2.f-PI*(float)i/(N-1);
        shape.setPoint(1+i,{cx+hw*std::cos(a),cy-hh*std::sin(a)});
    }
    shape.setPoint(N+1,{cx-hw,cy+hh});
    shape.setFillColor(COL_GATE);
    shape.setOutlineColor(sf::Color(100,150,240));
    shape.setOutlineThickness(1.5f);
    g.shapes.push_back(shape);
    g.leftX=cx-hw; g.rightX=cx+hw;
    g.inputY1=cy-hh*0.5f; g.inputY2=cy+hh*0.5f;
    return g;
}

DrawnGate CircuitDrawer::makeOr(float cx, float cy) {
    DrawnGate g; g.cx=cx; g.cy=cy;
    float hw=GW*0.44f, hh=GH*0.44f;
    int N=12;
    sf::ConvexShape shape(N*2+2); int idx=0;
    sf::Vector2f tP0={cx-hw,cy-hh},tP1={cx+hw*0.3f,cy-hh},tP2={cx+hw,cy};
    for (int i=0;i<N;i++) { auto p=bezier2(tP0,tP1,tP2,(float)i/(N-1)); shape.setPoint(idx++,p); }
    sf::Vector2f bP0={cx+hw,cy},bP1={cx+hw*0.3f,cy+hh},bP2={cx-hw,cy+hh};
    for (int i=0;i<N;i++) { auto p=bezier2(bP0,bP1,bP2,(float)i/(N-1)); shape.setPoint(idx++,p); }
    shape.setPoint(idx++,{cx-hw+hw*0.25f,cy});
    shape.setPoint(idx++,{cx-hw,cy-hh});
    shape.setFillColor(COL_GATE);
    shape.setOutlineColor(sf::Color(230,150,60));
    shape.setOutlineThickness(1.5f);
    g.shapes.push_back(shape);
    g.leftX=cx-hw+hw*0.12f; g.rightX=cx+hw;
    g.inputY1=cy-hh*0.48f; g.inputY2=cy+hh*0.48f;
    return g;
}

DrawnGate CircuitDrawer::makeNot(float cx, float cy) {
    DrawnGate g; g.cx=cx; g.cy=cy;
    float hw=GW*0.40f, hh=GH*0.42f, bR=hh*0.20f;
    sf::ConvexShape tri(3);
    tri.setPoint(0,{cx-hw,cy-hh}); tri.setPoint(1,{cx-hw,cy+hh});
    tri.setPoint(2,{cx+hw-bR*2.2f,cy});
    tri.setFillColor(COL_GATE);
    tri.setOutlineColor(sf::Color(210,100,210));
    tri.setOutlineThickness(1.5f);
    g.shapes.push_back(tri);
    sf::CircleShape bubble(bR);
    bubble.setOrigin(bR,bR); bubble.setPosition(cx+hw-bR,cy);
    bubble.setFillColor(sf::Color(35,35,50));
    bubble.setOutlineColor(sf::Color(210,100,210));
    bubble.setOutlineThickness(1.5f);
    g.circles.push_back(bubble);
    g.leftX=cx-hw; g.rightX=cx+hw;
    g.inputY1=g.inputY2=cy;
    return g;
}

DrawnGate CircuitDrawer::makeXor(float cx, float cy) {
    DrawnGate g=makeOr(cx,cy);
    for (auto& s:g.shapes) s.setOutlineColor(sf::Color(230,220,50));
    float hw=GW*0.44f, hh=GH*0.44f;
    float offset=hw*0.22f;
    int N=10; float thick=std::max(1.5f,hh*0.09f);
    sf::ConvexShape arc(N*2);
    for (int i=0;i<N;i++) {
        float t=(float)i/(N-1);
        float bow=hh*0.25f*std::sin(PI*t);
        float y=cy-hh+hh*2*t;
        arc.setPoint(i,{cx-hw-offset+bow,y});
    }
    for (int i=0;i<N;i++) {
        float t=(float)(N-1-i)/(N-1);
        float bow=hh*0.25f*std::sin(PI*t);
        float y=cy-hh+hh*2*t;
        arc.setPoint(N+i,{cx-hw-offset+bow-thick,y});
    }
    arc.setFillColor(COL_GATE);
    arc.setOutlineColor(sf::Color(230,220,50));
    arc.setOutlineThickness(1);
    g.shapes.push_back(arc);
    g.leftX=cx-hw+hw*0.12f+offset*0.5f;
    return g;
}

DrawnGate CircuitDrawer::makeOutput(float cx, float cy, bool val) {
    DrawnGate g; g.cx=cx; g.cy=cy;
    float hw=GW*0.38f, hh=GH*0.38f;
    sf::ConvexShape box(4);
    box.setPoint(0,{cx-hw,cy-hh}); box.setPoint(1,{cx+hw,cy-hh});
    box.setPoint(2,{cx+hw,cy+hh}); box.setPoint(3,{cx-hw,cy+hh});
    sf::Color fill = val ? COL_OUT_ON : COL_OUT_OFF;
    box.setFillColor(fill);
    box.setOutlineColor(sf::Color(255,180,80));
    box.setOutlineThickness(1.5f);
    g.shapes.push_back(box);
    g.label.setFont(*font);
    g.label.setString("OUT");
    g.label.setCharacterSize(std::max(9u,(unsigned)(GH*0.38f)));
    g.label.setFillColor(sf::Color::White);
    g.label.setStyle(sf::Text::Bold);
    auto lb=g.label.getLocalBounds();
    g.label.setOrigin(lb.left+lb.width/2,lb.top+lb.height/2);
    g.label.setPosition(cx,cy);
    g.leftX=cx-hw; g.rightX=cx+hw;
    g.inputY1=g.inputY2=cy;
    return g;
}

// ── Wire ───
void CircuitDrawer::wire(float x1, float y1, float x2, float y2,
                          bool signal, bool colored) {
    sf::Color c = colored ? (signal ? COL_HIGH : COL_LOW) : COL_WIRE;
    float midX=(x1+x2)/2;
    wires.push_back({{x1,   y1}, c});
    wires.push_back({{midX, y1}, c});
    wires.push_back({{midX, y1}, c});
    wires.push_back({{midX, y2}, c});
    wires.push_back({{midX, y2}, c});
    wires.push_back({{x2,   y2}, c});
}

// ── Draw ──
void CircuitDrawer::draw(sf::RenderTarget& window) {
    if (!wires.empty())
        window.draw(wires.data(), wires.size(), sf::Lines);
    for (auto& g:gates) {
        for (auto& s:g.shapes)  window.draw(s);
        for (auto& c:g.circles) window.draw(c);
        if (g.label.getFont())  window.draw(g.label);
    }
}