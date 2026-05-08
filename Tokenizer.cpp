#include "Tokenizer.h"

std::vector<std::string> Tokenizer::tokenize(const std::string& expr) {
    std::vector<std::string> tokens;

    for (int i = 0; i < (int)expr.size(); i++) {
        char c = expr[i];

       
        if (c == ' ') continue;
        if (c == '&' || c == '|' || c == '!' || c == '^' ||
            c == '(' || c == ')') {
            tokens.push_back(std::string(1, c));  
        }
      
        else if (isalpha(c)) {
            std::string name = "";
            while (i < (int)expr.size() && isalpha(expr[i])) {
                name += expr[i];
                i++;
            }
            i--;  
            tokens.push_back(name);
        }
     
    }

    return tokens;
}
