#ifndef USER_H
#define USER_H

#include "debug.h"

class User {
	Debug debug;
	int step = 0;
public:
	User(std::string filename) : debug(filename) {
	}
	std::pair<VT, int> show() {
		VT vt = debug.result.getstep(step);
		int line = debug.nowline[step];
		return std::make_pair(vt, line);
	}
	void next() {
		step++;
	}
	void previous() {
		step--;
	}
	void jump(int line) {
		std::vector<int> lines = debug.nowline;
		for (int i = 0; i < lines.size(); i++) {
			if (lines[i] == line) {
				step = i;
				break;
			}
		}
	}
};

#endif