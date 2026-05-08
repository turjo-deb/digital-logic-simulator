#pragma once
#include <string>
#include <vector>
#include <map>
#include <memory>
#include "Gate.h"

class Parser {
public:
    std::map<std::string, std::shared_ptr<InputNode>> varMap;
    std::shared_ptr<Node> parse(const std::vector<std::string>& tokens);

private:
    std::vector<std::string> toks; 
    int pos;                        
    std::shared_ptr<Node> parseOr();   
    std::shared_ptr<Node> parseXor();
    std::shared_ptr<Node> parseAnd();
    std::shared_ptr<Node> parseNot();  
    std::shared_ptr<Node> parsePrimary(); 
    std::string peek();
    std::string consume();
};
