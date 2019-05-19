#pragma once
#include <iostream>
#include <algorithm>
#include<string>
#include<vector>
using namespace std;

class sbox 
{
public:
	sbox();
	int sBox[];
	void RedeceMono(vector<string> &mon);
	vector<string> Monomials(vector<vector<int>> S);
	vector<vector<int>> fullPerm_01(int num);
	vector<vector<string>> getANF(int sbox[]);

	//vector<vector<int>> fullPerm_01(int num);
	vector<vector<int>> S_vector(vector<int> k);
	vector<string> Monomial(vector<vector<int>> S);
	vector<vector<int>> LS_box(vector<string>y0, vector<string>y1, vector<string>y2, vector<string>y3, vector<int> inputK);
	vector<vector<int>> S_box(vector<string>y0, vector<string>y1, vector<string>y2, vector<string>y3, vector<int> inputK);
	vector<vector<int>> Redundant(vector<vector<int>> clause);
	string arryTmp(string a, string b);
	vector<string>getKTrueValue(int sbox[]);
	vector<string>getLTrueValue(int sbox[]);
	//void RedeceMono(vector<string> &mon);

private:

};
