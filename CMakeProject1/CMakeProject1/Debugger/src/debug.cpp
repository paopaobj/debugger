#include "debug.h"
#include "debugger.h"
#include "scopesegment.h"
#include "HelperFunction.h"
#include "APIOfLexical_debugver.h"

#include <vector>
#include <unordered_set>
#include <stack>

//建议不要看具体实现，因为本人也看不下去，但为了让自己看懂还是加上了注释

const unordered_set<string> typeword = { "void", "int", "char", "float", "double", "bool", "string", "long", "short", "signed", "unsigned" };


    Debug::Debug(string filename) 
        : information(ScopeSegment(filename)), 
          scopes(information.scopes), 
          tokens(information.tokens), 
          result(VTs()), 
          countif(0), 
          countloop(0), 
          count(0), 
          nowline(vector<int>()),
		  strnames(unordered_set<string>())
    {}

	VTs Debug::run() {
		VT vt;
		vector<string> p;
		for (int i = 0; i < information.allstruct.size(); i++) {
			strnames.insert(information.allstruct[i].strname);
		}
		for (int i = 0; i < information.allglobalvars.size(); i++) {
			vector<pair<string,string>> t=replacement(information.allglobalvars[i].varvalue, 0, information.allglobalvars[i].varvalue.size() - 1, vt);
			string expr = "";
			for (int j = 0; j < t.size(); j++) {
				expr += t[j].second;
			}
			string value = to_string(evaluateExpression(expr).first);
			array<string, 4> addment = { "global", information.allglobalvars[i].vartype, information.allglobalvars[i].varname, value };
			vt.add(addment);
		}
		executefunction(vt, "main", p);
		return result;
	}

	//忘记干嘛的了，万一有用，先留着
	//11.10 我想起来了，这个函数是用来找具体是哪种类型的scope的

	pair<string, int> Debug::find(vector<pair<string, int>> scopes, int line) {
		for (int i = 0; i < scopes.size(); i++) {
			if (scopes[i].second == line) {
				return scopes[i];
			}
		}
		return make_pair("",-1);
	}

	bool Debug::isfunction(vector<funs> functionnames, string target) {
		for (int i = 0; i < functionnames.size(); i++) {
			if (functionnames[i].functionname == target) return true;
		}
		return false;
	}
	//执行某个函数块，从左到右依次是：总变量表，调用函数时的变量表，要执行的函数名，参数列表，总信息，源文件

	std::string Debug::executefunction(VT& current, const std::string& functionname, const std::vector<string>& valuesofparameters) {
		int startline=-1, endline=-1;
		std::vector<std::pair<std::string, std::string>> funparameters;

		for (int i = 0; i < information.allfunction.size(); i++) {
			if (information.allfunction[i].functionname == functionname) {
				startline = information.allfunction[i].startline;
				endline = information.allfunction[i].endline;
				funparameters = information.allfunction[i].funparameters;
				break;
			}
		}
		if (startline == -1 || endline == -1) {
			throw "Function not found";
		}
		std::stack<std::string> scope;
		scope.push(functionname);
		VT temp = current;
		temp.del("ALLScope");

		for (int i = 0; i < funparameters.size(); i++) {
			array<string, 4> addment = { functionname, funparameters[i].first, funparameters[i].second, valuesofparameters[i] };
			temp.add(addment);
		}
		result.setstep(count++, temp);
		nowline.push_back(startline);

		for (int i = startline + 1; i <= endline; i++) {
			std::vector<std::pair<std::string, std::string>> token = oneline(tokens, i);
			std::pair<std::string, int> flag = find(information.scopes, i);
			int countloop = 0;
			int countif = 0;

			if (token[0].second == "break") {
				loops thisloop;
				for (int j = 0; j < information.allloop.size(); j++) {
					if (information.allloop[j].startline <= i&&information.allloop[j].endline>=i) {
						thisloop = information.allloop[j];
					}
				}
				i = thisloop.endline;
				while (scope.top().substr(0, 3) != "for" && scope.top().substr(0, 5) != "while"){
					temp.del(scope.top());
					scope.pop();
				}
				temp.del(scope.top());
				scope.pop();
				result.setstep(count++, temp);
				nowline.push_back(i);
				continue;
			}

			if (token[0].second == "continue") {
				loops thisloop;
				for (int j = 0; j < information.allloop.size(); j++) {
					if (information.allloop[j].startline <= i) {
						thisloop = information.allloop[j];
					}
				}
				while (scope.top().substr(0, 3) != "for" && scope.top().substr(0, 5) != "while") {
					temp.del(scope.top());
					scope.pop();
				}
				i = thisloop.endline-1;
				result.setstep(count++, temp);
				nowline.push_back(i);
				continue;
			}

			if (token[0].second == "return") {
				result.setstep(count++, temp);
				nowline.push_back(i);
				scope.pop();
				string expression;
				int j;
				for (j = 1; token[j].second != ";"; j++);
				token = replacement(token, 1, j - 1, temp);
				for (int k = 1; token[k].second!=";"; k++) {
					expression += token[k].second;
				}
				pair<double, string> value = evaluateExpression(expression);
				return to_string(value.first);
			}

			//以下是处理简单语句

			if (flag.second == -1) { //太好了，这只是个普通语句

				if (token[0].first == "KeyWord"&&typeword.contains(token[0].second)) { //这是个声明语句
					std::string type = token[0].second;
					for (int j = 1; j<token.size(); j++) { //处理每个变量
						std::string variable;
						std::string expression;
						if (token[j].second == "=") { //这个变量有初始化
							variable = token[j - 1].second;
							int k;
							int stk = 0;
							for (k = j + 1; k < token.size(); k++) {
								if ((token[k].second == "," || token[k].second == ";") && stk == 0) break;
								if (token[k].second == "(") stk++;
								if (token[k].second == ")") stk--;
							}
							token = replacement(token, j + 1, k - 1, temp);
							for (k = j + 1; token[k].second != "," && token[k].second != ";"; k++) {
								expression += token[k].second;
							}
							j = k;
							pair<double, string> value = evaluateExpression(expression);
							array<string,4> addment = { scope.top(), type, variable, std::to_string(value.first) };
							temp.add(addment);
						}
						else if (j<token.size()&&(token[j].second == ","|| token[j].second == ";") &&token[j-1].first=="IDEN") { //这个变量没有初始化
							variable = token[j - 1].second;
							array<string,4> addment = { scope.top(), type, variable, "" };
							temp.add(addment);
						}
					}
				}
				else if (strnames.contains(token[0].second)) { //这是个结构体声明
					string structname = token[0].second;
					strs thisstr;
					for (int j = 0; j < information.allstruct.size(); j++) {
						if (information.allstruct[j].strname == structname) {
							thisstr = information.allstruct[j];
							break;
						}
					}
					for (int j = 1; j < token.size(); j++) { //处理每个变量
						std::string variable;
						if (token[j].second == "=") { //这个变量有初始化
							variable = token[j - 1].second;
							array<string, 4> addment = { scope.top(), token[0].second, variable, "As follows"};
							temp.add(addment);
							int k;
							int stk = 0;
							for (k = j + 1; k < token.size(); k++) {
								if (token[k].second == "{") stk++;
								if (token[k].second == "}") {
									stk--;
									if (stk == 0) break;
								}
							}
							token = replacement(token, j + 1, k - 1, temp);
							vector<string> e;
							for (k = j + 1; k < token.size(); k++) {
								if (token[k].second == "{") stk++;
								if (token[k].second == "}") {
									stk--;
									if (stk == 0) break;
								}
							}
							for (int l = j + 2; l < k; l++) {
								string tp;
								int end = l;
								while (token[end].second != "," && token[end].second != "}") {
									tp += token[end].second;
									end++;
								}
								replacement(token, l, end - 1, temp);
								l = end;
								e.push_back(tp);
							}
							while (e.size() < thisstr.strcontent.size())
								e.push_back("");
							for (int l = 0; l < e.size(); l++) {
								pair<double, string> value = evaluateExpression(e[l]);
								array<string, 4> addment = { scope.top(), thisstr.strcontent[l].first, variable + "." + thisstr.strcontent[l].second, to_string(value.first) };
								temp.add(addment);
							}
						}
						else if (j < token.size() && (token[j].second == "," || token[j].second == ";") && token[j - 1].first == "IDEN") { //这个变量没有初始化
							for (int l = 0; l < thisstr.strcontent.size(); l++) {
								array<string, 4> addment = { scope.top(),thisstr.strcontent[l].first, variable + "." + thisstr.strcontent[l].second, ""};
								temp.add(addment);
							}
						}
					}
				}

				else { //这是个赋值语句，那更好了
					bool haseq=false;
					string expression;
					string variable;
					string op;
					int eq = -1;
					unordered_set<string> operators = { "+=", "-=", "*=", "/=", "%=" ,"++","--"};
					for (int j = 0; j < token.size(); j++) {
						if (token[j].second == "="||operators.contains(token[j].second)) {
							haseq = true;
							eq = j;
							op = token[j].second;
							std::array<std::string, 4> inf = temp.get(token[j-1].second);
							variable = token[j - 1].second;
							if (operators.contains(token[j].second)) {
								expression += inf[3];
								if (token[j].second == "+=") expression += "+";
								else if (token[j].second == "-=") expression += "-";
								else if (token[j].second == "*=") expression += "*";
								else if (token[j].second == "/=") expression += "/";
								else if (token[j].second == "%=") expression += "%";
								else if (token[j].second == "++") expression += "+1";
								else if (token[j].second == "--") expression += "-1";
							}
							break;
						}
					}
					if (!haseq) {
						result.setstep(count++, temp);
						nowline.push_back(i);
						continue;
					}
					else{
						int j;
						for (j = 2; token[j].second != ";"; j++);
						token = replacement(token, 2, j - 1, temp);

					    if (token[eq + 1].second == "{") {
							int s, e;
							strs thisstr;
							array<string, 4> inf = temp.get(variable);
							for (int j = 0; j < information.allstruct.size(); j++) {
								if (information.allstruct[j].strname == inf[1]) {
									thisstr = information.allstruct[j];
									break;
								}
							}
							for (s = eq + 1; token[s].second != "{"; s++);
							for (e = s; token[e].second != "}"; e++);
							token = replacement(token, s + 1, e - 1, temp);
							vector<string> express;
							for (int j = s + 1; j < e; j++) {
								string tp;
								while (token[j].second != "," && token[j].second != "}") {
									tp += token[j].second;
									j++;
								}
								express.push_back(tp);
							}
							while (express.size() < thisstr.strcontent.size())
								express.push_back("");
							for (int j = 0; j < express.size(); j++) {
								pair<double, string> value = evaluateExpression(express[j]);
								array<string, 2> addment = { variable + "." + thisstr.strcontent[j].second, to_string(value.first)};
								temp.add(addment);
							}
						}
						else {
							if (op != "++" && op != "--") {
								for (j = 2; token[j].second != ";"; j++) {
									expression += token[j].second;
								}
							}
							pair<double, string> value = evaluateExpression(expression);
							array<string, 2> addment = { variable, std::to_string(value.first) };
							temp.add(addment);
						}
					}
				}
				//有人会问，这里如果在不赋值的情况下调用函数，那么这个函数的返回值会怎么处理？答案是没人会这么干
				//因为我们的编译器甚至不支持array，所以所有自定义函数都是按值传递的，不存在类似sort(array)这种会更改传入参数的方法
				//所以不赋值，仅调用是没有任何意义的，这里就不考虑这种情况

				result.setstep(count++, temp);
				nowline.push_back(i);
				continue;
			}

			//单行语句处理结束

			else if (flag.first == "startif") { //这是个if
				string condition="";
				ifs thisif;
				for (int j = 0; j < information.allif.size(); j++) {
					if (information.allif[j].startline == i) {
						thisif = information.allif[j];
						break;
					}
				}
				int s, e;
				for (s = 0; token[s].second != "("; s++);
				for (e = s; token[e].second != ")"; e++);
				token = replacement(token, s + 1, e - 1, temp);
				for (int j = s + 1; j <= e - 1; j++) {
					condition += token[j].second;
				}
				pair<double, string> value = evaluateExpression(condition);
				if (value.first <= 0) {
					if (thisif.haselse) {
						scope.push("else"+to_string(countif++));
						i = thisif.elsestartline;
					}
					else {
						i = thisif.endline;
					}
				}
				else {
					scope.push("if"+to_string(countif++));
				}
				result.setstep(count++, temp);
				nowline.push_back(i);
				continue;
			}

			//对于else，我们直接跳过，因为如果else成立，那么在if段会直接跳到else的下一行，根本不会读取到这个else
			else if (flag.first == "startelse") { 
				for (int j = 0; j < information.allif.size(); j++) {
					if (information.allif[j].elsestartline == i) {
						i = information.allif[j].elseendline;
						break;
					}
				}
				result.setstep(count++, temp);
				nowline.push_back(i);
				continue;
			}

			else if (flag.first == "endif") {
				temp.del(scope.top());
				scope.pop();
				result.setstep(count++, temp);
				nowline.push_back(i);
				continue;
			}

			else if (flag.first == "endelse") {
				temp.del(scope.top());
				scope.pop();
				result.setstep(count++, temp);
				nowline.push_back(i);
				continue;
			}

			else if (flag.first == "startfor") { //这是个for
				string initialization;
				loops thisloop;
				for (int j = 0; j < information.allloop.size(); j++) {
					if (information.allloop[j].startline == i) {
						initialization = information.allloop[j].initialization;
						thisloop = information.allloop[j];
						break;
					}
				}
				scope.push("for"+to_string(countloop++));

				if (initialization != "") {
					int s, e;
					for (s = 0; token[s].second != "="; s++);
					for (e = s; token[e].second != ";"; e++);
					token = replacement(token, s + 1, e - 1, temp);
					initialization = "";
					for (int j = s-1; j <= e - 1; j++) {
						initialization += token[j].second;
					}
				}

				if (initialization != "" && thisloop.newvariable != "") {
					string variable = initialization.substr(0, initialization.find("="));
					string expression = initialization.substr(initialization.find("=") + 1);
					pair<double, string> value = evaluateExpression(expression);
					array<string, 4> addment = { scope.top(),thisloop.newvariable , variable, to_string(value.first) };
					temp.add(addment);
				}
				else if (initialization != "") {
					string variable = initialization.substr(0, initialization.find("="));
					string expression = initialization.substr(initialization.find("=") + 1);
					pair<double, string> value = evaluateExpression(expression);
					array<string, 2> addment = { variable, std::to_string(value.first) };
					temp.add(addment);
				}
				result.setstep(count++, temp);
				nowline.push_back(i);
				continue;
			}
			else if (flag.first == "endfor") { //这是个endfor
				string loopcondition;
				string loopstep;
				loops thisloop;
				unordered_set<string> operators = { "+=", "-=", "*=", "/=", "%=" ,"++","--" };

				for (int j = 0; j < information.allloop.size(); j++) {
					if (information.allloop[j].endline == i) {
						thisloop = information.allloop[j];
						break;
					}
				}
				int s, e;
				std::vector<std::pair<std::string, std::string>> fortoken = oneline(tokens,thisloop.startline);
				for (s = 0; fortoken[s].second != ";"; s++);
				for (e = s+1; fortoken[e].second != ";"; e++);
				s = e + 1;
				for (e = s; e<fortoken.size()&&fortoken[e].second != ")"; e++); 
				fortoken = replacement(fortoken, s+1,e-1, temp);
				for (int j = s + 1; j <= e - 1; j++) {
					loopstep += fortoken[j].second;
				}
				if (loopstep != "") {
					string expression;
					for (int j = s + 1; j < e; j++) {
						if (fortoken[j].second == "=" || operators.contains(fortoken[j].second)) {
							std::array<std::string, 4> inf = temp.get(fortoken[j - 1].second);
							if (operators.contains(fortoken[j].second)) {
								expression += inf[3];
								if (fortoken[j].second == "+=") expression += "+";
								else if (fortoken[j].second == "-=") expression += "-";
								else if (fortoken[j].second == "*=") expression += "*";
								else if (fortoken[j].second == "/=") expression += "/";
								else if (fortoken[j].second == "%=") expression += "%";
								else if (fortoken[j].second == "++") expression += "+1";
								else if (fortoken[j].second == "--") expression += "-1";
							}
							break;
						}
					}
					string variable = fortoken[s].second;
					if (loopstep != "++" && loopstep != "--")
						expression += loopstep;
					pair<double, string> value = evaluateExpression(expression);
					array<string, 2> addment = { variable, std::to_string(value.first) };
					temp.add(addment);
				}
				for (s = 0; fortoken[s].second != ";"; s++);
				for (e = s + 1; fortoken[e].second != ";"; e++);
				fortoken = replacement(fortoken, s + 1, e - 1, temp);
				for (int j = s + 1; j <= e - 1; j++) {
					loopcondition += fortoken[j].second;
				}
				if (loopcondition == "") loopcondition = "1"; //如果没有条件，那么就默认为真（无限循环)
				if (evaluateExpression(loopcondition).first > 0) { //如果条件成立
					result.setstep(count++, temp);
					nowline.push_back(i);
					i = thisloop.startline;
					continue;
				}
				else {
					temp.del(scope.top());
					scope.pop();
					result.setstep(count++, temp);
					nowline.push_back(i);
					continue;
				}
			}

			else if (flag.first == "startwhile") { //这是个while
				string condition;
				loops thisloop;
				for (int j = 0; j < information.allloop.size(); j++) {
					if (information.allloop[j].startline == i) {
						thisloop = information.allloop[j];
						break;
					}
				}
				scope.push("while" + to_string(countloop++));

				int s, e;
				for (s = 0; token[s].second != "("; s++);
				for (e = s; token[e].second != ")"; e++);
				token = replacement(token, s + 1, e - 1, temp);
				for (int j = s + 1; j <= e - 1; j++) {
					condition += token[j].second;
				}

				if (evaluateExpression(condition).first <= 0) {
					i = thisloop.endline;
				}
				result.setstep(count++, temp);
				nowline.push_back(i);
				continue;
			}

			else if (flag.first == "endwhile") { //这是个endwhile
				string condition;
				loops thisloop;
				for (int j = 0; j < information.allloop.size(); j++) {
					if (information.allloop[j].endline == i) {
						thisloop = information.allloop[j];
						break;
					}
				}
				vector<pair<string, string>> fortoken = oneline(tokens, thisloop.startline);
				int s, e;
				for (s = 0; fortoken[s].second != "("; s++);
				for (e = s; fortoken[e].second != ")"; e++);
				fortoken = replacement(fortoken, s + 1, e - 1, temp);
				for (int j = s + 1; j <= e - 1; j++) {
					condition += fortoken[j].second;
				}
				if (evaluateExpression(condition).first > 0) {
					result.setstep(count++, temp);
					nowline.push_back(i);
					i = thisloop.startline;
					continue;
				}
				else {
					temp.del(scope.top());
					scope.pop();
					result.setstep(count++, temp);
					nowline.push_back(i);
					continue;
				}
			}
		}
		return "";
	}

	vector<pair<string, string> > Debug::replacement(vector<pair<string, string> > token,int start, int end, VT vt) {
		if (start > end) return token;
		for (int i = start; i <= end; i++) {
			if (token[i].first == "IDEN" && !isfunction(information.allfunction, token[i].second)) {
				if (strnames.contains(vt.get(token[i].second)[1])) {
					strs thisstr;
					for (int j = 0; j < information.allstruct.size(); j++) {
						if (information.allstruct[j].strname == vt.get(token[i].second)[1]) {
							thisstr = information.allstruct[j];
							break;
						}
					}
					string variable = token[i].second;
					token[i].first = "Delimiter";
					token[i].second = "{";
					int j;
					for (j = 0; j < thisstr.strcontent.size()-1; j++) {
						array<string, 4> inf = vt.get(variable + "." + thisstr.strcontent[j].second);
						token.insert(token.begin() + i + 2*j + 1, make_pair("Num", inf[3]));
						token.insert(token.begin() + i + 2*j + 2, make_pair("Delimiter", ","));
					}
					array<string, 4> inf = vt.get(variable + "." + thisstr.strcontent[j].second);
					token.insert(token.begin() + i + 2 * j + 1, make_pair("Num", inf[3]));
					token.insert(token.begin() + i + 2*j+2, make_pair("Delimiter", "}"));
					end =end+ 2 * thisstr.strcontent.size();
				}
				else{
					std::array<std::string, 4> inf = vt.get(token[i].second);
					if (!inf[0].empty())
						token[i].second = inf[3];
				}
			}
			else if (token[i].first == "IDEN" && isfunction(information.allfunction, token[i].second)) {
				std::string functionname = token[i].second;
				std::vector<string> valuesofparameters;
				int s=i+1, e=i+2;
				int stk = 1;
				for (e = i + 2; e<=end; e++) {
					if (token[e].second == "(") stk++;
					else if (token[e].second == ")") stk--;
					if (stk == 0) break;
				}
				end = end - (e-s+1);
				token=replacement(token, s+1, e - 1, vt);
				for (e = i + 2; token[e].second != ")"; e++);
				for (int j = s + 1; j <= e - 1; j++) {
					if (token[j].second != ",")
						valuesofparameters.push_back(token[j].second);
				}
				token[i].second = executefunction(vt, functionname, valuesofparameters);
				token[i].first = "NUM";
				end -= (e - s + 1);
                token.erase(token.begin() + s, token.begin() + e + 1);
			}
		}
		
		return token;
	}

	vector<pair<string, string> > Debug::oneline(vector<pair<string, string> > tokens, int line) {
		vector<pair<string, string> > result;
		for(int i=0;i<tokens.size();i++) {
			if(tokens[i].first == "startofline" && tokens[i].second == to_string(line)) {
				while(i<tokens.size()&&tokens[++i].first != "startofline") {
					result.push_back(tokens[i]);
				}
				break;
			}
		}
		return result;
	}