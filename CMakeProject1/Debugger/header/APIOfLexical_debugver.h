//
// Created by 马宸泽 on 2024/10/21.
//

#ifndef APIOFLEXICAL_DEBUGVER_H
#define APIOFLEXICAL_DEBUGVER_H

#include <iostream>
#include <fstream>
#include <set>
#include <string>
#include <vector>
#include <utility>

using namespace std;

// 全局变量声明
extern bool hasBeenComment;
extern vector<pair<string, string>> output;

// 函数声明
void getpair(string first, string second);

bool isKeyWord(string word);
bool isComment(string& input, int pos, char peek, bool& hasBeenComment);
bool isWhiteSpace(char ch);
bool isLetter(char ch);
bool isDigit(char ch);
bool isOperator(char ch);
bool isDelimiter(char ch);
bool isString(char ch);
bool isChar(char ch);

void dealWithLetter(string& input, int& pos, char peek);
void dealWithDigit(string& input, int& pos, char peek);
void dealWithOperator(string& input, int& pos, char peek);
void dealWithDelimiter(int& pos, char peek);
void dealWithString(string& input, int& pos, char peek);
void dealWithChar(string& input, int& pos, char peek);

void words(string input, bool& hasBeenComment);
vector<pair<string, string>> gettoken(string filename);

#endif // APIOFLEXICAL_DEBUGVER_H