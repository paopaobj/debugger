#include "debug.h"
#include "debugger.h"
#include "scopesegment.h"
#include "HelperFunction.h"
#include "APIOfLexical_debugver.h"

#include <vector>
#include <unordered_set>
#include <stack>

//���鲻Ҫ������ʵ�֣���Ϊ����Ҳ������ȥ����Ϊ�����Լ��������Ǽ�����ע��

const unordered_set<string> typeword = { "void", "int", "char", "float", "double", "bool", "string", "long", "short", "signed", "unsigned" };


    Debug::Debug(string filename) 
        : information(ScopeSegment(filename)), 
          scopes(information.scopes), 
          tokens(information.tokens), 
          result(VTs()), 
          countif(0), 
          countloop(0), 
          count(0), 
          nowline(vector<int>()) 
    {}

	VTs Debug::run() {
		VT vt;
		vector<string> p;
		executefunction(vt, "main", p);
		return result;
	}

	//���Ǹ�����ˣ���һ���ã�������
	//11.10 ���������ˣ���������������Ҿ������������͵�scope��

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
	//ִ��ĳ�������飬�����������ǣ��ܱ��������ú���ʱ�ı�����Ҫִ�еĺ������������б�����Ϣ��Դ�ļ�

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

			//�����Ǵ�������

			if (flag.second == -1) { //̫���ˣ���ֻ�Ǹ���ͨ���

				if (token[0].first == "KeyWord"&&typeword.contains(token[0].second)) { //���Ǹ��������
					std::string type = token[0].second;
					for (int j = 1; j<token.size(); j++) { //����ÿ������
						std::string variable;
						std::string expression;
						if (token[j].second == "=") { //��������г�ʼ��
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
						else if (j<token.size()&&(token[j].second == ","|| token[j].second == ";") &&token[j-1].first=="IDEN") { //�������û�г�ʼ��
							variable = token[j - 1].second;
							array<string,4> addment = { scope.top(), type, variable, "" };
							temp.add(addment);
						}
					}
				}

				else { //���Ǹ���ֵ��䣬�Ǹ�����
					bool haseq=false;
					string expression;
					string variable;
					string op;
					unordered_set<string> operators = { "+=", "-=", "*=", "/=", "%=" ,"++","--"};
					for (int j = 0; j < token.size(); j++) {
						if (token[j].second == "="||operators.contains(token[j].second)) {
							haseq = true;
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
					int j;
					for (j = 2; token[j].second != ";"; j++);
					token=replacement(token, 2, j - 1, temp);
					if(op!="++"&&op!="--"){
						for (j = 2; token[j].second != ";"; j++) {
							expression += token[j].second;
						}
					}
					pair<double, string> value = evaluateExpression(expression);
					array<string, 2> addment = { variable, std::to_string(value.first) };
					temp.add(addment);
				}
				//���˻��ʣ���������ڲ���ֵ������µ��ú�������ô��������ķ���ֵ����ô��������û�˻���ô��
				//��Ϊ���ǵı�����������֧��array�����������Զ��庯�����ǰ�ֵ���ݵģ�����������sort(array)���ֻ���Ĵ�������ķ���
				//���Բ���ֵ����������û���κ�����ģ�����Ͳ������������

				result.setstep(count++, temp);
				nowline.push_back(i);
				continue;
			}

			//������䴦�����

			else if (flag.first == "startif") { //���Ǹ�if
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

			//����else������ֱ����������Ϊ���else��������ô��if�λ�ֱ������else����һ�У����������ȡ�����else
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

			else if (flag.first == "startfor") { //���Ǹ�for
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
			else if (flag.first == "endfor") { //���Ǹ�endfor
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
				if (loopcondition == "") loopcondition = "1"; //���û����������ô��Ĭ��Ϊ�棨����ѭ��)
				if (evaluateExpression(loopcondition).first > 0) { //�����������
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

			else if (flag.first == "startwhile") { //���Ǹ�while
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

			else if (flag.first == "endwhile") { //���Ǹ�endwhile
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
				std::array<std::string, 4> inf = vt.get(token[i].second);
				if (!inf[0].empty())
					token[i].second = inf[3];
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