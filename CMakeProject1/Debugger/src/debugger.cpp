#include "debugger.h"
#include "HelperFunction.h"
#include <vector>
#include <string>
#include <array>
#include <algorithm>
#include <iostream>

void VT::add(std::array<std::string,4>& v){
	std::array<std::string, 4> temp = get(v[2]);
	if (temp[0]=="this"&&temp[1]=="is"&&temp[2]=="empty"&&temp[3]=="!") {
		table.push_back(v);
	}
	else {
		for (int i = 0; i < table.size(); i++) {
			if (v[2] == table[i][2]) {
				table[i] = v;
			}
		}
	}
}

void VT::add(std::array<std::string, 2>& v) {
	std::array<std::string, 4> temp = get(v[0]);
	if (temp[0] == "this" && temp[1] == "is" && temp[2] == "empty" && temp[3] == "!") {
		std::cerr<< "Variable not found";
	}
	else {
		for (int i = 0; i < table.size(); i++) {
			if (v[0] == table[i][2]) {
				table[i][3] = v[1];
			}
		}
	}
}

std::array<std::string, 4> VT::get(const std::string& variable) const {
	for (int i = 0; i < table.size(); i++) {
		if (variable == table[i][2]) {
			return table[i];
		}
	}
	std::array<std::string, 4> r = {"this","is","empty","!"};
    return r;
}

void VT::del(const std::string& scope){
	if (scope == "ALLScope") {
		table.erase(std::remove_if(table.begin(), table.end(),
			[&scope](const std::array<std::string, 4>& item) {
				return item[0] != "global";
			}),
			table.end());
	}
	else {
		table.erase(std::remove_if(table.begin(), table.end(),
			[&scope](const std::array<std::string, 4>& item) {
				return item[0] == scope;
			}),
			table.end());
	}
}

VT& VT::operator=(const VT& other) {
    if (this != &other) {
        table = other.table; // 按值传递 table 的内容
    }
    return *this;
}

VTs::VTs(){
}

void VTs::setstep(int line,VT vt){
	sz++;
	map[line]=vt;
}

VT VTs::getstep(int line){
	auto result = map.find(line);
	if(result != map.end()) {
		return result->second;
	}
	else std::cerr << "nosuchline";
}

int VTs::size() {
	return sz;
}
