#ifndef _debugger_h
#define _debugger_h

#include <vector>
#include <string>
#include <unordered_map>
#include <array>

class VT{
	public:
	std::vector<std::array<std::string,4>> table;
	
    VT(){}
	void add(std::array<std::string,4>& v); //加入一行变量信息 
	void add(std::array<std::string, 2>& v); //修改一行变量信息
	void del(const std::string& scope); // 删除某个作用域内的变量 
	std::array<std::string, 4> get(const std::string& variable) const;
	VT& operator=(const VT& other); // 重载赋值运算符
};

class VTs{
	public:
	std::unordered_map<int,VT> map;
	int sz = 0;
	
	VTs();
	void setstep(int line,VT vt); //将第line步的状态设置为vt 
	VT getstep(int line); //获取第line步的VT 
	int size(); //获取VTs的大小
};

#endif
