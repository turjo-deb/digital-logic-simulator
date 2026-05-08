#pragma once
#include <string>
#include <memory>

// ── Base Node ─────────────────────────────────────────────
struct Node {
    virtual ~Node() = default;
    virtual bool evaluate() const = 0;
};

// ── Input variable ────────────────────────────────────────
struct InputNode : Node {
    std::string name;
    bool value;
    InputNode(const std::string& n, bool v) : name(n), value(v) {}
    bool evaluate() const override { return value; }
};

// ── AND gate ──────────────────────────────────────────────
struct AndGate : Node {
    std::shared_ptr<Node> left, right;
    AndGate(std::shared_ptr<Node> l, std::shared_ptr<Node> r) : left(l), right(r) {}
    bool evaluate() const override { return left->evaluate() && right->evaluate(); }
};

// ── OR gate ───────────────────────────────────────────────
struct OrGate : Node {
    std::shared_ptr<Node> left, right;
    OrGate(std::shared_ptr<Node> l, std::shared_ptr<Node> r) : left(l), right(r) {}
    bool evaluate() const override { return left->evaluate() || right->evaluate(); }
};

// ── XOR gate ──────────────────────────────────────────────
struct XorGate : Node {
    std::shared_ptr<Node> left, right;
    XorGate(std::shared_ptr<Node> l, std::shared_ptr<Node> r) : left(l), right(r) {}
    bool evaluate() const override { return left->evaluate() ^ right->evaluate(); }
};

// ── NOT gate ──────────────────────────────────────────────
struct NotGate : Node {
    std::shared_ptr<Node> child;
    explicit NotGate(std::shared_ptr<Node> c) : child(c) {}
    bool evaluate() const override { return !child->evaluate(); }
};
