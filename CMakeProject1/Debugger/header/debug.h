// debug.h
#ifndef DEBUG_H
#define DEBUG_H

#include <string>
#include <vector>
#include <unordered_set>
#include "scopesegment.h"
#include "debugger.h"
#include "APIOfLexical_debugver.h"
#include "rawprocessing.h"
#include "HelperFunction.h"

class Debug {
public:
    ScopeSegment information;
    std::vector<std::pair<std::string, int>> scopes;
    std::vector<std::pair<std::string, std::string>> tokens;
    VTs result;
    int countif;
    int countloop;
    int count;
    std::vector<int> nowline;
private:
    std::pair<std::string, int> find(std::vector<std::pair<std::string, int>> scopes, int line);
    bool isfunction(std::vector<funs> functionnames, std::string target);
    std::string executefunction(VT& current, const std::string& functionname, const std::vector<std::string>& valuesofparameters);
    std::vector<std::pair<std::string, std::string>> replacement(std::vector<std::pair<std::string, std::string>> token, int start, int end, VT vt);
    std::vector<std::pair<std::string, std::string>> oneline(std::vector<std::pair<std::string, std::string>> tokens, int line);

public:
    Debug(std::string filename);
    VTs run();
};

#endif // DEBUG_H