#ifndef _scopesegment_h
#define _scopesegment_h

#include "rawprocessing.h"
#include <vector>

struct loops {
	int startline=-1;
	int endline=-1;
	std::string newvariable="";
	std::string initialization="";
	std::string loopcondition="";
	std::string loopstep="";
};

struct ifs {
	int startline=-1;
	int endline=-1;
	bool haselse = false;
	int elsestartline=-1;
	int elseendline=-1;
	string condition="";
};

struct funs {
	int startline=-1;
	int endline=-1;
	std::string functionname="";
	std::vector<std::pair<std::string, std::string>> funparameters = vector<std::pair<std::string, std::string>>();
};

struct strs {
	int startline = -1;
	int endline = -1;
	std::string strname = "";
	vector<pair<string,string>> strcontent;
};

struct globalvars {
	std::string varname = "";
	std::string vartype = "";
	std::vector<pair<std::string,std::string>> varvalue;
};

class ScopeSegment{
	public:
		std::vector<std::pair<std::string, int>> scopes;
		std::vector<std::pair<std::string, std::string>> tokens;

		vector<loops> allloop;
		vector<ifs> allif;
		vector<funs> allfunction;
		vector<strs> allstruct;
		vector<globalvars> allglobalvars;

		ScopeSegment(std::string filename);
	
		vector<loops> findloops();
	
		vector<ifs> findifs();
	
		vector<funs> findfuns();

		vector<strs> findstrs();

		vector<globalvars> findglobalvars();

		int getpos(int line);

		int tokenpos(int line);
};
#endif