#include "TruthTable.h"
#include <iostream>
#include <iomanip>
#include <cmath>
#include <algorithm>

void TruthTable::generate(
    std::shared_ptr<Node> root,
    std::map<std::string, std::shared_ptr<InputNode>>& varMap
) {
    rows.clear();
    vars.clear();

    for (auto& [name, node] : varMap) {
        vars.push_back(name);
    }
    int n = vars.size();
    int totalRows = 1 << n; 
    for (int i = 0; i < totalRows; i++) {
        Row row;
        for (int j = 0; j < n; j++) {
            bool val = (i >> (n - 1 - j)) & 1;
            varMap[vars[j]]->value = val; 
            row.inputs[vars[j]] = val;
        }
        row.output = root->evaluate();
        rows.push_back(row);
    }
}

void TruthTable::print() {
    for (auto& v : vars) {
        printCell<std::string>(v);
    }
    std::cout << "  | OUT\n";
    int width = vars.size() * 4 + 8;
    std::cout << std::string(width, '-') << "\n";
    for (auto& row : rows) {
        for (auto& v : vars) {
            printCell<bool>(row.inputs.at(v));
        }
        std::cout << "  |  " << row.output << "\n";
    }
}

struct Implicant {
    int  minterm;  
    int  mask;     
    std::vector<int> covered; 

    bool operator==(const Implicant& o) const {
        return minterm == o.minterm && mask == o.mask;
    }
};

static bool canMerge(const Implicant& a, const Implicant& b) {
    if (a.mask != b.mask) return false;
    int diff = (a.minterm ^ b.minterm) & ~a.mask;
    return diff && (diff & (diff-1)) == 0; 
}

static Implicant merge(const Implicant& a, const Implicant& b) {
    int diff = (a.minterm ^ b.minterm) & ~a.mask;
    Implicant r;
    r.minterm = a.minterm & ~diff;
    r.mask    = a.mask | diff;
    r.covered = a.covered;
    for (int m : b.covered)
        if (std::find(r.covered.begin(), r.covered.end(), m) == r.covered.end())
            r.covered.push_back(m);
    return r;
}

std::string TruthTable::simplify() const {
    int n = (int)vars.size();
    if (n == 0 || rows.empty()) return "";

 
    std::vector<int> minterms;
    for (int i = 0; i < (int)rows.size(); i++)
        if (rows[i].output) minterms.push_back(i);

    if (minterms.empty()) return "0";
    if ((int)minterms.size() == (int)rows.size()) return "1";

    std::vector<Implicant> implicants;
    for (int m : minterms) {
        Implicant imp; imp.minterm = m; imp.mask = 0; imp.covered = {m};
        implicants.push_back(imp);
    }

    std::vector<Implicant> primes;
    while (!implicants.empty()) {
        std::vector<Implicant> next;
        std::vector<bool> merged(implicants.size(), false);
        for (int i = 0; i < (int)implicants.size(); i++) {
            for (int j = i+1; j < (int)implicants.size(); j++) {
                if (canMerge(implicants[i], implicants[j])) {
                    auto m = merge(implicants[i], implicants[j]);
                    bool dup = false;
                    for (auto& x : next) if (x == m) { dup = true; break; }
                    if (!dup) next.push_back(m);
                    merged[i] = merged[j] = true;
                }
            }
        }
        for (int i = 0; i < (int)implicants.size(); i++)
            if (!merged[i]) primes.push_back(implicants[i]);
        implicants = next;
    }

    std::vector<bool> covered(1 << n, false);
    for (int m : minterms) covered[m] = false; 

    std::vector<int> tocover = minterms;
    std::vector<Implicant> selected;
    for (int m : tocover) {
        std::vector<int> covering;
        for (int i = 0; i < (int)primes.size(); i++)
            if (std::find(primes[i].covered.begin(), primes[i].covered.end(), m) != primes[i].covered.end())
                covering.push_back(i);
        if (covering.size() == 1) {
            auto& p = primes[covering[0]];
            bool dup = false;
            for (auto& s : selected) if (s == p) { dup = true; break; }
            if (!dup) selected.push_back(p);
        }
    }
    std::vector<int> remaining;
    for (int m : tocover) {
        bool cov = false;
        for (auto& s : selected)
            if (std::find(s.covered.begin(), s.covered.end(), m) != s.covered.end()) { cov = true; break; }
        if (!cov) remaining.push_back(m);
    }
    while (!remaining.empty()) {
        int best = -1, bestCount = 0;
        for (int i = 0; i < (int)primes.size(); i++) {
            int cnt = 0;
            for (int m : remaining)
                if (std::find(primes[i].covered.begin(), primes[i].covered.end(), m) != primes[i].covered.end()) cnt++;
            if (cnt > bestCount) { bestCount = cnt; best = i; }
        }
        if (best < 0) break;
        selected.push_back(primes[best]);
        std::vector<int> newrem;
        for (int m : remaining) {
            bool cov = false;
            for (auto& s : selected)
                if (std::find(s.covered.begin(), s.covered.end(), m) != s.covered.end()) { cov = true; break; }
            if (!cov) newrem.push_back(m);
        }
        remaining = newrem;
    }
    if (selected.empty()) return "0";
    std::string result;
    for (auto& imp : selected) {
        if (!result.empty()) result += " | ";
        std::string term;
        for (int i = n-1; i >= 0; i--) {
            int bit = n-1-i;
            if (!((imp.mask >> bit) & 1)) {
                bool val = (imp.minterm >> bit) & 1;
                if (!term.empty()) term += "&";
                if (!val) term += "!";
                term += vars[i];
            }
        }
        if (term.empty()) term = "1";
        result += term;
    }
    return result;
}