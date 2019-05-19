#pragma once
#include<vector>;
#include<string>;
using namespace std;
class CipherDescription
{
private:
	
	
	
	
	vector<string> values;
	
public:
	vector<int> index;
	vector<int> constantindex;
	vector<string> temporaries;
	vector<vector<string>> transition;
	string descriptions;
	int state_size;
	int rounds;
	void apply_xor(string source_1, string source_2, string target, bool description = true);
	void apply_and(string source_1, string source_2, string target, bool description = true);
	void apply_permutation(vector<string> permutation);
	void check_permutation(vector<string> permutation);
	void check_target(string target);
	void check_source(string source);
	void set_rounds(int round);
	void apply_mov(string source, string target, string target_1, bool description = true);
	void apply_Sbox(vector<string> inpyt, vector<string> output, string sname);
	void print_descriptions();
	CipherDescription(int size);
	CipherDescription();
	void apply_modAdd(vector<int> input1, vector<int> input2, vector<int> &output, int startindex, int &endindex);
	void apply_threemov(string source, string source_1, string source_2, string target, bool description=true);
	void apply_threexor(string source_1, string source_2, string source_3, string target, bool description=true);
	void apply_fourmov(string source, string source_1, string source_2, string source_3, string target, bool description = true);
	void apply_ConstantmodAdd(vector<int> a_inputvalue, vector<int> &output, int startindex, int &endindex);
	//¹¹Ôìº¯Êý

	
};
