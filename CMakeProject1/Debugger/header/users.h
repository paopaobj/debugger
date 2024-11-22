#ifndef USER_H
#define USER_H

#include "debug.h"

class User {
	Debug debug;
	int step = 0;
public:
	User(std::string filename) : debug(filename) {
		debug.run();
	}
	std::pair<VT, int> show() {
		VT vt = debug.result.getstep(step);
		int line = debug.nowline[step];
		return std::make_pair(vt, line);
	}
	bool next() {
		if (step == debug.nowline.size() - 1) return false;
		step++;
		return true;
	}
	bool previous() {
		if (step == 0) return false;
		step--;
		return true;
	}
	bool jump(int line) {
		std::vector<int> lines = debug.nowline;
		for (int i = 0; i < lines.size(); i++) {
			if (lines[i] == line) {
				step = i;
				return true;
			}
		}
		return false;
	}
};

#endif