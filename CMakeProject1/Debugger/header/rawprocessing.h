// rawprocessing.h

#ifndef RAWPROCESSING_H
#define RAWPROCESSING_H

#include <vector>
#include <unordered_set>
#include <string>

using namespace std;

//找到作用域的终止位置
int pairing(vector<pair<string, string> > tokens, int start, int n);

//找到所有的函数名称
unordered_set<string> findfuns(vector<pair<string, string> > tokens, int n);

//找到所有的作用域
vector<pair<string, int> > findallscope(vector<pair<string, string> > tokens, unordered_set<string> functionname, int n);

//将作用域按行号排序 
vector<pair<string, int> > findALLscope(string filename);

#endif 