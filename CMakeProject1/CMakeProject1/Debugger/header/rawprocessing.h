// rawprocessing.h

#ifndef RAWPROCESSING_H
#define RAWPROCESSING_H

#include <vector>
#include <unordered_set>
#include <string>

using namespace std;

//�ҵ����������ֹλ��
int pairing(vector<pair<string, string> > tokens, int start, int n);

//�ҵ����еĺ�������
unordered_set<string> findfuns(vector<pair<string, string> > tokens, int n);

//�ҵ����е�������
vector<pair<string, int> > findallscope(vector<pair<string, string> > tokens, unordered_set<string> functionname, int n);

//���������к����� 
vector<pair<string, int> > findALLscope(string filename);

#endif 