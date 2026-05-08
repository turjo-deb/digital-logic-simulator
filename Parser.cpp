#include "Parser.h"
#include <stdexcept>

std::string Parser::peek() {
    if (pos < (int)toks.size()) return toks[pos];
    return ""; 
}

std::string Parser::consume() {
    return toks[pos++];
}

std::shared_ptr<Node> Parser::parse(const std::vector<std::string>& tokens) {
    toks = tokens;
    pos = 0;
    varMap.clear();
    return parseOr(); 
}

std::shared_ptr<Node> Parser::parseOr() {
    auto left = parseXor();

    while (peek() == "|") {
        consume(); 
        auto right = parseXor();
        left = std::make_shared<OrGate>(left, right);
    }
    return left;
}

std::shared_ptr<Node> Parser::parseXor() {
    auto left = parseAnd();

    while (peek() == "^") {
        consume(); 
        auto right = parseAnd();
        left = std::make_shared<XorGate>(left, right);
    }
    return left;
}

std::shared_ptr<Node> Parser::parseAnd() {
    auto left = parseNot();

    while (peek() == "&") {
        consume(); 
        auto right = parseNot();
        left = std::make_shared<AndGate>(left, right);
    }
    return left;
}

std::shared_ptr<Node> Parser::parseNot() {
    if (peek() == "!") {
        consume();
        auto child = parseNot(); 
        return std::make_shared<NotGate>(child);
    }
    return parsePrimary();
}

std::shared_ptr<Node> Parser::parsePrimary() {
    std::string tok = peek();
    if (tok == "(") {
        consume(); 
        auto node = parseOr();  
        if (peek() != ")") {
            throw std::runtime_error("Expected closing ')'");
        }
        consume(); 
        return node;
    }

    if (!tok.empty() && isalpha(tok[0])) {
        consume();  

        if (varMap.find(tok) == varMap.end()) {
            varMap[tok] = std::make_shared<InputNode>(tok, false);
        }
        return varMap[tok];
    }

    throw std::runtime_error("Unexpected token: '" + tok + "'");
}
