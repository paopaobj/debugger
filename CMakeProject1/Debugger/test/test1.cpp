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

	
	vector<pair<string, string>> t1 = gettoken("D:\\zhuomian\\de\\CMakeProject1\\Debugger\\test\\test3.txt");
	vector<pair<string, string>> t2 = gettoken("D:\\zhuomian\\de\\CMakeProject1\\Debugger\\test\\test2.txt");

	for (auto i : t1) {
		cout << i.first << " " << i.second << endl;
	}

	cout << endl;

	for (auto i : t2) {
		cout << i.first << " " << i.second << endl;
	}

	/*
	Debug d = Debug("D:\\zhuomian\\de\\CMakeProject1\\Debugger\\test\\test2.txt");
	VTs result = d.run();

	for (int i = 0; i < result.size(); i++) {

		cout << "We are at line: " << d.nowline[i] << endl;
		std::vector<std::array<std::string, 4>> table = result.getstep(i).table;
		for (int j = 0; j < table.size(); j++) {
			cout << table[j][0] << " " << table[j][1] << " " << table[j][2] << " " << table[j][3] << endl;
		}
		cout << endl;
		
	}
	*/
	return 0;
}

