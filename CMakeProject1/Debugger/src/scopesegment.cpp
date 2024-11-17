#include "scopesegment.h"
#include "rawprocessing.h"
#include "APIOfLexical_debugver.h"

ScopeSegment::ScopeSegment(std::string filename){
	scopes = findALLscope(filename);
	tokens = gettoken(filename);

	allloop= findloops();
	allif = findifs();
	allfunction = findfuns();
	allstruct = findstrs();
	allglobalvars = findglobalvars();
}

int ScopeSegment::getpos(int line) {
	int i = 0;
	while (i < scopes.size() && scopes[i++].second != line); 
	int stk = 1;
	int pos = i - 1;
	for (; i < scopes.size(); i++) {
		if (scopes[i].first == scopes[pos].first) stk++;
		else if(scopes[i].first.substr(3) == scopes[pos].first.substr(5)) stk--;
		if (stk == 0) return scopes[i].second;
	}
	return -1;
}

int ScopeSegment::tokenpos(int line) {
	for (int i = 0; i < tokens.size(); i++) {
		if (tokens[i].first == "startofline" && tokens[i].second == to_string(line)) return i;
	}
	return -1;
}

vector<loops> ScopeSegment::findloops() {
	vector<loops> r;
	for (int i = 0; i < scopes.size(); i++) {
		loops result;
		if (scopes[i].first == "startfor") {
			result.startline = scopes[i].second;
			result.endline = getpos(result.startline);
			int pos=tokenpos(result.startline);

			pos += 3;
			if (tokens[pos].first == "KeyWord" ) {
				result.newvariable = tokens[pos].second;
				pos++;
			}
			else {
				result.newvariable = "";
			}
			while(tokens[pos].second!=";") {
				result.initialization += tokens[pos++].second;
			}
			pos++;

			while(tokens[pos].second!=";") {
				result.loopcondition += tokens[pos++].second;
			}
			pos++;

			while(tokens[pos].second!=")") {
				result.loopstep += tokens[pos++].second;
			}
			r.push_back(result);
		}
		else if (scopes[i].first == "startwhile") {
			result.startline = scopes[i].second;
			result.endline = getpos(result.startline);
			int pos = tokenpos(result.startline);
			pos += 3;
			while (tokens[pos].second != ")") {
				result.loopcondition += tokens[pos++].second;
			}
			result.initialization = "";
			result.loopstep = "";
			result.newvariable = "";
			r.push_back(result);
		}
	}

	return r;

}

vector<ifs> ScopeSegment::findifs() {
	vector<ifs> r;
	for (int i = 0; i < scopes.size(); i++) {
		ifs result;
		if (scopes[i].first == "startif") {
			result.startline = scopes[i].second;
			result.endline = getpos(result.startline);
			int pos = tokenpos(result.startline);
			pos += 3;
			while (tokens[pos].second != ")") {
				result.condition += tokens[pos++].second;
			}
			r.push_back(result);
		}
		else if (scopes[i].first == "startelse") {
			int corrif = 0;
			int stk = 1;
			for (int j = i - 1; j >= 0; j--) {
				if (scopes[j].first == "startelse") stk++;
				if (scopes[j].first == "startif") stk--;
				if (stk == 0) {
					corrif = j;
					break;
				}
			}
			int index = 0;
			for (int j = 0; j < r.size(); j++) {
				if (r[j].startline == scopes[corrif].second) {
					index = j;
					break;
				}
			}
			r[index].haselse = true;
			r[index].elsestartline = scopes[i].second;
			r[index].elseendline = getpos(scopes[i].second);
		}
	}
	for (int i = 0; i < r.size(); i++) {
		if (r[i].haselse == false) {
			r[i].elsestartline = -1;
			r[i].elseendline = -1;
		}
	}
	return r;
}

vector<funs> ScopeSegment::findfuns() {
	vector<funs> r;
	for (int i = 0; i < scopes.size(); i++) {
		funs result;
		if (scopes[i].first == "startfun") {
			result.startline = scopes[i].second;
			result.endline = getpos(result.startline);
			int pos = tokenpos(result.startline);
			pos += 2; 

			result.functionname = tokens[pos].second;

			pos += 2;

			while (tokens[pos].second != ")") {
				if (tokens[pos].second != ",") {
					result.funparameters.push_back(make_pair(tokens[pos].second,tokens[pos+1].second));
					pos++;
				}
				pos++;
			}
			r.push_back(result);
		}
	}
	return r;
}

vector<strs> ScopeSegment::findstrs() {
	vector<strs> r;
	const unordered_set<string> typeword = { "void", "int", "char", "float", "double", "bool", "string", "long", "short", "signed", "unsigned" };

	for (int i = 0; i < scopes.size(); i++) {
		strs result;
		if (scopes[i].first == "startstruct") {
			result.startline = scopes[i].second;
			result.endline = getpos(result.startline);
			int pos = tokenpos(result.startline);
			pos += 2;

			result.strname = tokens[pos].second;

			pos += 2;

			while (tokens[pos].second != "}"||tokens[pos+1].second!=";") {
				if (typeword.contains(tokens[pos].second)&&tokens[pos+1].first=="IDEN") {
					pair<string, string> addment = make_pair(tokens[pos].second, tokens[pos + 1].second);
					result.strcontent.push_back(addment);
				}
				pos++;
			}
			r.push_back(result);
		}
	}
	return r;
}

vector<globalvars> ScopeSegment::findglobalvars() {
	vector<globalvars> r;
	const unordered_set<string> typeword = { "void", "int", "char", "float", "double", "bool", "string", "long", "short", "signed", "unsigned" };

	for (int i = 0; i < tokens.size(); i++) {
		globalvars result;
		if (tokens[i].second == "main") break;
		if ((typeword.contains(tokens[i].second)&&tokens[i+1].first=="IDEN"&&tokens[i+2].second=="(")||tokens[i].second=="struct") {
			int stk = 0;
			for (; i < tokens.size(); i++) {
				if (tokens[i].second == "{") stk++;
				if (tokens[i].second == "}") {
					stk--;
					if (stk == 0) break;
				}
			}
		}
		else if (typeword.contains(tokens[i].second)&&tokens[i+1].first=="IDEN"&&tokens[i+2].second!="(") {
			string type = tokens[i].second;
			while (tokens[i].second != ";") {
				i++;
				if (tokens[i].first == "IDEN") {
					result.varname = tokens[i].second;
				}
				else if (tokens[i].second == "=") {
					i++;
					while (tokens[i].second != ","&&tokens[i].second!=";") {
						result.varvalue.push_back(tokens[i]);
						i++;
					}
					result.vartype = type;
					r.push_back(result);
				}
			}
		}
	}
	return r;
}