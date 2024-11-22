#include "users.h"
#include "debug.h"
#include "rawprocessing.h"
#include "APIOFLexical_debugver.h"
#include "HelperFunction.h"
#include <iostream>
#include <string>

using namespace std;

int main(){
	/*
	VT vt;
	array<string,4> a={{"main","int","a","18"}},b={{"fori","int","b","87"}};
	vt.add(a);
	vt.add(b);
	VTs final;
	final.setstep(1,vt);
	vt.del("fori");
	final.setstep(2,vt);
	
	cout<<final.getstep(1).table[0][0] << " " << final.getstep(1).table[0][1] << " " << final.getstep(1).table[0][2] << endl;
	cout<<final.getstep(1).table[1][0]<<" "<<final.getstep(1).table[1][1]<<" "<<final.getstep(1).table[1][2]<<endl;
	cout<<final.getstep(2).table[0][0]<<" "<<final.getstep(2).table[0][1]<<" "<<final.getstep(2).table[0][2]<<endl;
	*/

	/*
	User s = User("D:\\zhuomian\\de\\CMakeProject1\\Debugger\\test\\test2.txt");
	User p = User("D:\\zhuomian\\de\\CMakeProject1\\Debugger\\test\\test3.txt");
	*/

	User ss = User("D:\\zhuomian\\de\\CMakeProject1\\Debugger\\test\\test2.txt");
	User pp = User("D:\\zhuomian\\de\\CMakeProject1\\Debugger\\test\\test3.txt");
	string word;
	while (cin >> word) {
		if (word == "next") {
			ss.next();
		}
		else if (word == "previous") {
			ss.previous();
		}
		else if (word == "jump") {
			int line;
			cin >> line;
			ss.jump(line);
		}
		else if (word == "end") break;
		pair<VT, int> result = ss.show();
		for (auto i : result.first.table) {
			cout << i[0] << " " << i[1] << " " << i[2] << " " << i[3] << endl;
		}
	}

	while (cin >> word) {
		if (word == "next") {
			pp.next();
		}
		else if (word == "previous") {
			pp.previous();
		}
		else if (word == "jump") {
			int line;
			cin >> line;
			pp.jump(line);
		}
		pair<VT, int> result = pp.show();
		for (auto i : result.first.table) {
			cout << i[0] << " " << i[1] << " " << i[2] << " " << i[3] << endl;
		}
	}

	/*
	VTs result = ss.run();

	for (int i = 0; i < result.size(); i++) {
		cout << "We are at line: " << ss.nowline[i] << endl;
		std::vector<std::array<std::string, 4>> table = result.getstep(i).table;
		for (int j = 0; j < table.size(); j++) {
			cout << table[j][0] << " " << table[j][1] << " " << table[j][2] << " " << table[j][3] << endl;
		}
		cout << endl;
		
	}
	*/
	
	return 0;
}

