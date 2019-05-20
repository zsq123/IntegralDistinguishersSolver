#pragma once
#include"CipherDescription.h"
#include<stddef.h>
#include<set>
#include <map>
#include <cryptominisat5/cryptominisat.h>
using namespace CMSat;
class solver
{
public:
	solver();
	
	void load_cipher(CipherDescription ciper);
	void set_rounds(int round);
	void create_conditions(SATSolver &solvers);
	void Lcreate_conditions(SATSolver &solvers);
	vector<vector<int>>KReturnAllSolu(vector<int> active);
	vector<vector<int>> LReturnAllSolu(vector<int> active);
	vector<vector<int>> khudraReturnAllSolu(vector<int> active);
	vector<vector<int>> LSetToKSet(vector<vector<int>> K, vector<vector<int>>L,string name);
	vector<vector<int>> RedundantLset(vector<vector<int>> K, vector<vector<int>>L);
	void AddVector(vector<vector<int>> &inter, vector<vector<int>> &midd);
	
	void apply_xor(SATSolver &solvers,int target_1, int target_2, int source);
	int get_variables(string bit);
	void apply_copy(SATSolver &solvers,int target, int source_1, int source_2);
	void apply_and(SATSolver &solvers,int target_1,int target_2, int source);
	void is_balanced(vector<int> startBit, set<int>& result);
	
	bool Is_exist(int bit, vector<int> active);
	void Redundant(vector<vector<int>> &clause);
	void apply_Lcopy(SATSolver &solvers, int target_1, int target_2, int source);
	void apply_Land(SATSolver &solvers, int target_1, int target_2, int source);
	void apply_Lxor(SATSolver &solvers, int target_1, int target_2, int source);
	void apply_Sbox(SATSolver &solvers,  string sname, vector<int> input, vector<int> output);
	void apply_LSbox(SATSolver &solvers, string sname, vector<int> input, vector<int> output);
	vector<vector<int>> getSboxCNF(vector<string> value,int num);
	void getClause(SATSolver &slover, vector<vector<int>> value, vector<int> input, vector<int> output);
	bool Balance_bit(vector<int> activebit, int bit);
	void apply_three_xor(SATSolver &solvers, int target_1, int target_2, int target_3, int source);
	void apply_three_copy(SATSolver &solvers, int target, int source_1, int source_2, int source_3);
	void apply_three_Lxor(SATSolver &solvers, int target_1, int target_2, int target_3, int source);
	void apply_three_Lcopy(SATSolver &solvers, int target, int source_1, int source_2, int source_3);
	void apply_four_copy(SATSolver &solvers, int target, int source_1, int source_2, int source_3, int source_4);
	bool LBalance_bit(vector<int> activebit, int bit);
private:
	CipherDescription cipher ;
	int state_size;
	int next_variable = 1;
	vector<int> state_bit;
	vector<Lit> clause;
	vector<int> endIndex;
	vector<int> conIndex;
	map<string, string>  temporary;
	vector<vector<int>>round_states;
	 // string round_states [];
		int rounds = 0;
		bool fresh_conditions = false;
		int model_size = 0;

};

