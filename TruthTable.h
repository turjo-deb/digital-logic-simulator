#pragma once
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <iostream>
#include <iomanip>
#include "Node.h"

template<typename T>
void printCell(T val, int width = 4) {
    std::cout << std::setw(width) << val;
}

class TruthTable {
public:
  
    struct Row {
        std::map<std::string, bool> inputs;  
        bool output;                         

        bool operator==(const Row& other) const {
            return inputs == other.inputs && output == other.output;
        }
     
        friend std::ostream& operator<<(std::ostream& os, const Row& r) {
            for (auto& [k, v] : r.inputs) os << k << "=" << v << " ";
            os << "-> " << r.output;
            return os;
        }
    };

    std::vector<Row> rows;          
    std::vector<std::string> vars;

    void generate(
        std::shared_ptr<Node> root,
        std::map<std::string, std::shared_ptr<InputNode>>& varMap
    );

    void print();
    std::string simplify() const;
};