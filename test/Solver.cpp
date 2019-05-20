#include"Solver.h"
#include"Sbox.h"
#include <cryptominisat5/cryptominisat.h>
#include<vector>
#include<numeric>
using namespace CMSat;
using namespace std;
	
solver::solver()
{
	state_size = 0;
	rounds = 1;
}

void solver::load_cipher(CipherDescription cip)
{
	//ciper.model_size = 0
		cipher = cip;
		state_size = cip.state_size;
		//self.fresh_conditions = False
		set_rounds(cip.rounds);
		endIndex = cip.index;
		conIndex = cip.constantindex;
		//self.sbox_cnfs = {}
}



void solver::set_rounds(int round) 
{
		/*
		Specify the number of times the round transition is applied
		'''
		*/
	rounds = int(round);
	if (rounds < 0) { return; }

	fresh_conditions = false;
		
}

void solver::create_conditions(SATSolver &solvers)
{
	//solvers.set_num_threads(4);
	auto maxPosition = max_element(endIndex.begin(), endIndex.end());
	int max = *maxPosition;
	solvers.new_vars(max + 1);
	
		for (int step = 0; step < cipher.transition.size(); step++)
		{
			int size = cipher.transition[step].size();
			string sboxname = cipher.transition[step][size - 1];
			if (cipher.transition[step][size-1] == "XOR") 
			{
				//xor
				apply_xor(solvers,get_variables(cipher.transition[step][0]), get_variables( cipher.transition[step][1]), get_variables( cipher.transition[step][2]));
			}
			else if (cipher.transition[step][size-1] == "AND") 
			{
				//and
				apply_and(solvers,get_variables(cipher.transition[step][0]), get_variables(cipher.transition[step][1]), get_variables(cipher.transition[step][2]));
			}
			else if (cipher.transition[step][size-1] == "PERM") 
			{
				//perm
			}
			else if (cipher.transition[step][size - 1] == "COPY") 
			{				
				apply_copy(solvers,get_variables(cipher.transition[step][2]), get_variables(cipher.transition[step][0]), get_variables(cipher.transition[step][1]));
			}
			else if (cipher.transition[step][size - 1] == "LCOPY")
			{
				apply_three_copy(solvers, get_variables(cipher.transition[step][3]), get_variables(cipher.transition[step][2]), get_variables(cipher.transition[step][1]), get_variables(cipher.transition[step][0]));
				//apply_copy(solvers, get_variables(cipher.transition[step][2]), get_variables(cipher.transition[step][0]), get_variables(cipher.transition[step][1]));
			}
			else if (cipher.transition[step][size - 1] == "FOURCOPY")
			{
				apply_four_copy(solvers, get_variables(cipher.transition[step][4]), get_variables(cipher.transition[step][3]), get_variables(cipher.transition[step][2]), get_variables(cipher.transition[step][1]),get_variables(cipher.transition[step][0]));
				//apply_copy(solvers, get_variables(cipher.transition[step][2]), get_variables(cipher.transition[step][0]), get_variables(cipher.transition[step][1]));
			}
			else if (cipher.transition[step][size - 1] == "LXOR")
			{
				//xor
				apply_three_xor(solvers, get_variables(cipher.transition[step][0]), get_variables(cipher.transition[step][1]), get_variables(cipher.transition[step][2]), get_variables(cipher.transition[step][3]));
				//apply_xor(solvers, get_variables(cipher.transition[step][0]), get_variables(cipher.transition[step][1]), get_variables(cipher.transition[step][2]));
			}
			else if (sboxname.substr(0,1) == "S")
			{
				vector<int> input;
				vector<int> output;
				input.push_back(get_variables(cipher.transition[step][0]));
				input.push_back(get_variables(cipher.transition[step][1]));
				input.push_back(get_variables(cipher.transition[step][2]));
				input.push_back(get_variables(cipher.transition[step][3]));
				output.push_back(get_variables(cipher.transition[step][4]));
				output.push_back(get_variables(cipher.transition[step][5]));
				output.push_back(get_variables(cipher.transition[step][6]));
				output.push_back(get_variables(cipher.transition[step][7]));
				apply_Sbox(solvers,cipher.transition[step][size - 1],input,output);
				input.clear();
				output.clear();
			}
		}					
							
}

void solver::Lcreate_conditions(SATSolver &solvers)
{
	//solvers.set_num_threads(4);
	
	auto maxPosition = max_element(endIndex.begin(), endIndex.end());
	int max = *maxPosition;
	solvers.new_vars(max + 1);
	for (int step = 0; step < cipher.transition.size(); step++)
	{
		int size = cipher.transition[step].size();
		string sboxname = cipher.transition[step][size - 1];
		if (cipher.transition[step][size - 1] == "XOR")
		{
			//xor
			apply_Lxor(solvers, get_variables(cipher.transition[step][0]), get_variables(cipher.transition[step][1]), get_variables(cipher.transition[step][2]));
		}
		else if (cipher.transition[step][size - 1] == "AND")
		{
			//and
			apply_Land(solvers, get_variables(cipher.transition[step][0]), get_variables(cipher.transition[step][1]), get_variables(cipher.transition[step][2]));
		}
		else if (cipher.transition[step][size - 1] == "PERM")
		{
			//perm
		}
		else if (cipher.transition[step][size - 1] == "COPY")
		{
			apply_Lcopy(solvers, get_variables(cipher.transition[step][2]), get_variables(cipher.transition[step][0]), get_variables(cipher.transition[step][1]));
		}
		else if (sboxname.substr(0, 1) == "S")
		{
			vector<int> input;
			vector<int> output;
			input.push_back(get_variables(cipher.transition[step][0]));
			input.push_back(get_variables(cipher.transition[step][1]));
			input.push_back(get_variables(cipher.transition[step][2]));
			input.push_back(get_variables(cipher.transition[step][3]));
			output.push_back(get_variables(cipher.transition[step][4]));
			output.push_back(get_variables(cipher.transition[step][5]));
			output.push_back(get_variables(cipher.transition[step][6]));
			output.push_back(get_variables(cipher.transition[step][7]));
			apply_LSbox(solvers, cipher.transition[step][size - 1], input, output);
			input.clear();
			output.clear();

		}

	}
}



void solver::apply_three_Lxor(SATSolver &solvers, int target_1, int target_2, int target_3, int source) 
{
	clause.clear();
	clause.push_back(Lit(target_1, true));//true is negated
	clause.push_back(Lit(target_2, true));
	clause.push_back(Lit(target_3, true));
	clause.push_back(Lit(source, true));
	solvers.add_clause(clause);

	clause.clear();
	clause.push_back(Lit(target_1, true));//true is negated
	clause.push_back(Lit(target_2, true));
	clause.push_back(Lit(target_3, true));
	clause.push_back(Lit(source, false));
	solvers.add_clause(clause);

	clause.clear();
	clause.push_back(Lit(target_1, true));//true is negated
	clause.push_back(Lit(target_2, true));
	clause.push_back(Lit(target_3, false));
	clause.push_back(Lit(source, true));
	solvers.add_clause(clause);

	clause.clear();
	clause.push_back(Lit(target_1, true));//true is negated
	clause.push_back(Lit(target_2, true));
	clause.push_back(Lit(target_3, false));
	clause.push_back(Lit(source, false));
	solvers.add_clause(clause);

	clause.clear();
	clause.push_back(Lit(target_1, true));//true is negated
	clause.push_back(Lit(target_2, false));
	clause.push_back(Lit(target_3, true));
	clause.push_back(Lit(source, true));
	solvers.add_clause(clause);

	clause.clear();
	clause.push_back(Lit(target_1, true));//true is negated
	clause.push_back(Lit(target_2, false));
	clause.push_back(Lit(target_3, true));
	clause.push_back(Lit(source, false));
	solvers.add_clause(clause);

	clause.clear();
	clause.push_back(Lit(target_1, true));//true is negated
	clause.push_back(Lit(target_2, false));
	clause.push_back(Lit(target_3, false));
	clause.push_back(Lit(source, false));
	solvers.add_clause(clause);

	clause.clear();
	clause.push_back(Lit(target_1, false));//true is negated
	clause.push_back(Lit(target_2, true));
	clause.push_back(Lit(target_3, true));
	clause.push_back(Lit(source, true));
	solvers.add_clause(clause);

	clause.clear();
	clause.push_back(Lit(target_1, false));//true is negated
	clause.push_back(Lit(target_2, true));
	clause.push_back(Lit(target_3, true));
	clause.push_back(Lit(source, false));
	solvers.add_clause(clause);

	clause.clear();
	clause.push_back(Lit(target_1, false));//true is negated
	clause.push_back(Lit(target_2, true));
	clause.push_back(Lit(target_3, false));
	clause.push_back(Lit(source, false));
	solvers.add_clause(clause);

	clause.clear();
	clause.push_back(Lit(target_1, false));//true is negated
	clause.push_back(Lit(target_2, false));
	clause.push_back(Lit(target_3, true));
	clause.push_back(Lit(source, false));
	solvers.add_clause(clause);

	clause.clear();
	clause.push_back(Lit(target_1, false));//true is negated
	clause.push_back(Lit(target_2, false));
	clause.push_back(Lit(target_3, false));
	clause.push_back(Lit(source, true));
	solvers.add_clause(clause);
}
void solver::apply_three_Lcopy(SATSolver &solvers, int target, int source_1, int source_2, int source_3) 
{
	clause.clear();
	clause.push_back(Lit(target, true));//true is negated
	clause.push_back(Lit(source_1, false));
	clause.push_back(Lit(source_2, false));
	clause.push_back(Lit(source_3, false));
	solvers.add_clause(clause);

	clause.clear();
	clause.push_back(Lit(target, false));//true is negated
	clause.push_back(Lit(source_1, true));
	clause.push_back(Lit(source_2, true));
	clause.push_back(Lit(source_3, true));
	solvers.add_clause(clause);

	clause.clear();
	clause.push_back(Lit(target, false));//true is negated
	clause.push_back(Lit(source_1, true));
	clause.push_back(Lit(source_2, true));
	clause.push_back(Lit(source_3, false));
	solvers.add_clause(clause);

	clause.clear();
	clause.push_back(Lit(target, false));//true is negated
	clause.push_back(Lit(source_1, true));
	clause.push_back(Lit(source_2, false));
	clause.push_back(Lit(source_3, true));
	solvers.add_clause(clause);

	clause.clear();
	clause.push_back(Lit(target, false));//true is negated
	clause.push_back(Lit(source_1, true));
	clause.push_back(Lit(source_2, false));
	clause.push_back(Lit(source_3, false));
	solvers.add_clause(clause);

	clause.clear();
	clause.push_back(Lit(target, false));//true is negated
	clause.push_back(Lit(source_1, false));
	clause.push_back(Lit(source_2, true));
	clause.push_back(Lit(source_3, true));
	solvers.add_clause(clause);

	clause.clear();
	clause.push_back(Lit(target, false));//true is negated
	clause.push_back(Lit(source_1, false));
	clause.push_back(Lit(source_2, true));
	clause.push_back(Lit(source_3, false));
	solvers.add_clause(clause);

	clause.clear();
	clause.push_back(Lit(target, false));//true is negated
	clause.push_back(Lit(source_1, false));
	clause.push_back(Lit(source_2, false));
	clause.push_back(Lit(source_3, true));
	solvers.add_clause(clause);

}
void solver::apply_three_xor(SATSolver &solvers, int target_1, int target_2,int target_3, int source)
{
	clause.clear();
	clause.push_back(Lit(target_1, true));//true is negated
	clause.push_back(Lit(target_2, true));
	clause.push_back(Lit(target_3, true));
	clause.push_back(Lit(source, true));
	solvers.add_clause(clause);

	clause.clear();
	clause.push_back(Lit(target_1, true));//true is negated
	clause.push_back(Lit(target_2, true));
	clause.push_back(Lit(target_3, true));
	clause.push_back(Lit(source, false));
	solvers.add_clause(clause);

	clause.clear();
	clause.push_back(Lit(target_1, true));//true is negated
	clause.push_back(Lit(target_2, true));
	clause.push_back(Lit(target_3, false));
	clause.push_back(Lit(source, true));
	solvers.add_clause(clause);

	clause.clear();
	clause.push_back(Lit(target_1, true));//true is negated
	clause.push_back(Lit(target_2, true));
	clause.push_back(Lit(target_3, false));
	clause.push_back(Lit(source, false));
	solvers.add_clause(clause);

	clause.clear();
	clause.push_back(Lit(target_1, true));//true is negated
	clause.push_back(Lit(target_2, false));
	clause.push_back(Lit(target_3, true));
	clause.push_back(Lit(source, true));
	solvers.add_clause(clause);

	clause.clear();
	clause.push_back(Lit(target_1, true));//true is negated
	clause.push_back(Lit(target_2, false));
	clause.push_back(Lit(target_3, true));
	clause.push_back(Lit(source, false));
	solvers.add_clause(clause);

	clause.clear();
	clause.push_back(Lit(target_1, true));//true is negated
	clause.push_back(Lit(target_2, false));
	clause.push_back(Lit(target_3, false));
	clause.push_back(Lit(source, false));
	solvers.add_clause(clause);

	clause.clear();
	clause.push_back(Lit(target_1, false));//true is negated
	clause.push_back(Lit(target_2, true));
	clause.push_back(Lit(target_3, true));
	clause.push_back(Lit(source, true));
	solvers.add_clause(clause);

	clause.clear();
	clause.push_back(Lit(target_1, false));//true is negated
	clause.push_back(Lit(target_2, true));
	clause.push_back(Lit(target_3, true));
	clause.push_back(Lit(source, false));
	solvers.add_clause(clause);

	clause.clear();
	clause.push_back(Lit(target_1, false));//true is negated
	clause.push_back(Lit(target_2, true));
	clause.push_back(Lit(target_3, false));
	clause.push_back(Lit(source, false));
	solvers.add_clause(clause);

	clause.clear();
	clause.push_back(Lit(target_1, false));//true is negated
	clause.push_back(Lit(target_2, false));
	clause.push_back(Lit(target_3, true));
	clause.push_back(Lit(source, false));
	solvers.add_clause(clause);

	clause.clear();
	clause.push_back(Lit(target_1, false));//true is negated
	clause.push_back(Lit(target_2, false));
	clause.push_back(Lit(target_3, false));
	clause.push_back(Lit(source, true));
	solvers.add_clause(clause);
}
void solver::apply_three_copy(SATSolver &solvers, int target, int source_1, int source_2, int source_3) 
{
	clause.clear();
	clause.push_back(Lit(target, true));//true is negated
	clause.push_back(Lit(source_1, true));
	clause.push_back(Lit(source_2, true));
	clause.push_back(Lit(source_3, true));
	solvers.add_clause(clause);

	clause.clear();
	clause.push_back(Lit(target, true));//true is negated
	clause.push_back(Lit(source_1, true));
	clause.push_back(Lit(source_2, true));
	clause.push_back(Lit(source_3, false));
	solvers.add_clause(clause);

	clause.clear();
	clause.push_back(Lit(target, true));//true is negated
	clause.push_back(Lit(source_1, true));
	clause.push_back(Lit(source_2, false));
	clause.push_back(Lit(source_3, true));
	solvers.add_clause(clause);

	clause.clear();
	clause.push_back(Lit(target, true));//true is negated
	clause.push_back(Lit(source_1, false));
	clause.push_back(Lit(source_2, true));
	clause.push_back(Lit(source_3, true));
	solvers.add_clause(clause);

	clause.clear();
	clause.push_back(Lit(target, true));//true is negated
	clause.push_back(Lit(source_1, false));
	clause.push_back(Lit(source_2, false));
	clause.push_back(Lit(source_3, false));
	solvers.add_clause(clause);

	clause.clear();
	clause.push_back(Lit(target, false));//true is negated
	clause.push_back(Lit(source_1, true));
	clause.push_back(Lit(source_2, true));
	clause.push_back(Lit(source_3, true));
	solvers.add_clause(clause);

	clause.clear();
	clause.push_back(Lit(target, false));//true is negated
	clause.push_back(Lit(source_1, true));
	clause.push_back(Lit(source_2, true));
	clause.push_back(Lit(source_3, false));
	solvers.add_clause(clause);

	clause.clear();
	clause.push_back(Lit(target, false));//true is negated
	clause.push_back(Lit(source_1, true));
	clause.push_back(Lit(source_2, false));
	clause.push_back(Lit(source_3, true));
	solvers.add_clause(clause);

	clause.clear();
	clause.push_back(Lit(target, false));//true is negated
	clause.push_back(Lit(source_1, true));
	clause.push_back(Lit(source_2, false));
	clause.push_back(Lit(source_3, false));
	solvers.add_clause(clause);

	clause.clear();
	clause.push_back(Lit(target, false));//true is negated
	clause.push_back(Lit(source_1, false));
	clause.push_back(Lit(source_2, true));
	clause.push_back(Lit(source_3, true));
	solvers.add_clause(clause);

	clause.clear();
	clause.push_back(Lit(target, false));//true is negated
	clause.push_back(Lit(source_1, false));
	clause.push_back(Lit(source_2, true));
	clause.push_back(Lit(source_3, false));
	solvers.add_clause(clause);

	clause.clear();
	clause.push_back(Lit(target, false));//true is negated
	clause.push_back(Lit(source_1, false));
	clause.push_back(Lit(source_2, false));
	clause.push_back(Lit(source_3, true));
	solvers.add_clause(clause);
}
void solver::apply_four_copy(SATSolver &solvers, int target, int source_1, int source_2, int source_3, int source_4)
{
	clause.clear();
	clause.push_back(Lit(target, true));//true is negated
	clause.push_back(Lit(source_1, true));
	clause.push_back(Lit(source_2, true));
	clause.push_back(Lit(source_3, true));
	clause.push_back(Lit(source_4, true));
	solvers.add_clause(clause);

	clause.clear();
	clause.push_back(Lit(target, true));//true is negated
	clause.push_back(Lit(source_1, true));
	clause.push_back(Lit(source_2, true));
	clause.push_back(Lit(source_3, true));
	clause.push_back(Lit(source_4, false));
	solvers.add_clause(clause);

	clause.clear();
	clause.push_back(Lit(target, true));//true is negated
	clause.push_back(Lit(source_1, true));
	clause.push_back(Lit(source_2, true));
	clause.push_back(Lit(source_3, false));
	clause.push_back(Lit(source_4, true));
	solvers.add_clause(clause);

	clause.clear();
	clause.push_back(Lit(target, true));//true is negated
	clause.push_back(Lit(source_1, true));
	clause.push_back(Lit(source_2, true));
	clause.push_back(Lit(source_3, false));
	clause.push_back(Lit(source_4, false));
	solvers.add_clause(clause);

	clause.clear();
	clause.push_back(Lit(target, true));//true is negated
	clause.push_back(Lit(source_1, true));
	clause.push_back(Lit(source_2, false));
	clause.push_back(Lit(source_3, true));
	clause.push_back(Lit(source_4, true));
	solvers.add_clause(clause);

	clause.clear();
	clause.push_back(Lit(target, true));//true is negated
	clause.push_back(Lit(source_1, true));
	clause.push_back(Lit(source_2, false));
	clause.push_back(Lit(source_3, true));
	clause.push_back(Lit(source_4, false));
	solvers.add_clause(clause);

	clause.clear();
	clause.push_back(Lit(target, true));//true is negated
	clause.push_back(Lit(source_1, true));
	clause.push_back(Lit(source_2, false));
	clause.push_back(Lit(source_3, false));
	clause.push_back(Lit(source_4, true));
	solvers.add_clause(clause);

	clause.clear();
	clause.push_back(Lit(target, true));//true is negated
	clause.push_back(Lit(source_1, false));
	clause.push_back(Lit(source_2, true));
	clause.push_back(Lit(source_3, true));
	clause.push_back(Lit(source_4, true));
	solvers.add_clause(clause);

	clause.clear();
	clause.push_back(Lit(target, true));//true is negated
	clause.push_back(Lit(source_1, false));
	clause.push_back(Lit(source_2, true));
	clause.push_back(Lit(source_3, true));
	clause.push_back(Lit(source_4, false));
	solvers.add_clause(clause);
	clause.clear();
	clause.push_back(Lit(target, true));//true is negated
	clause.push_back(Lit(source_1, false));
	clause.push_back(Lit(source_2, true));
	clause.push_back(Lit(source_3, false));
	clause.push_back(Lit(source_4, true));
	solvers.add_clause(clause);

	clause.clear();
	clause.push_back(Lit(target, true));//true is negated
	clause.push_back(Lit(source_1, false));
	clause.push_back(Lit(source_2, false));
	clause.push_back(Lit(source_3, true));
	clause.push_back(Lit(source_4, true));
	solvers.add_clause(clause);

	clause.clear();
	clause.push_back(Lit(target, true));//true is negated
	clause.push_back(Lit(source_1, false));
	clause.push_back(Lit(source_2, false));
	clause.push_back(Lit(source_3, false));
	clause.push_back(Lit(source_4, false));
	solvers.add_clause(clause);

	clause.clear();
	clause.push_back(Lit(target, false));//true is negated
	clause.push_back(Lit(source_1, true));
	clause.push_back(Lit(source_2, true));
	clause.push_back(Lit(source_3, true));
	clause.push_back(Lit(source_4, true));
	solvers.add_clause(clause);

	clause.clear();
	clause.push_back(Lit(target, false));//true is negated
	clause.push_back(Lit(source_1, true));
	clause.push_back(Lit(source_2, true));
	clause.push_back(Lit(source_3, true));
	clause.push_back(Lit(source_4, false));
	solvers.add_clause(clause);

	clause.clear();
	clause.push_back(Lit(target, false));//true is negated
	clause.push_back(Lit(source_1, true));
	clause.push_back(Lit(source_2, true));
	clause.push_back(Lit(source_3, false));
	clause.push_back(Lit(source_4, true));
	solvers.add_clause(clause);

	clause.clear();
	clause.push_back(Lit(target, false));//true is negated
	clause.push_back(Lit(source_1, true));
	clause.push_back(Lit(source_2, true));
	clause.push_back(Lit(source_3, false));
	clause.push_back(Lit(source_4, false));
	solvers.add_clause(clause);

	clause.clear();
	clause.push_back(Lit(target, false));//true is negated
	clause.push_back(Lit(source_1, true));
	clause.push_back(Lit(source_2, false));
	clause.push_back(Lit(source_3, true));
	clause.push_back(Lit(source_4, true));
	solvers.add_clause(clause);

	clause.clear();
	clause.push_back(Lit(target, false));//true is negated
	clause.push_back(Lit(source_1, true));
	clause.push_back(Lit(source_2, false));
	clause.push_back(Lit(source_3, true));
	clause.push_back(Lit(source_4, false));
	solvers.add_clause(clause);

	clause.clear();
	clause.push_back(Lit(target, false));//true is negated
	clause.push_back(Lit(source_1, true));
	clause.push_back(Lit(source_2, false));
	clause.push_back(Lit(source_3, false));
	clause.push_back(Lit(source_4, true));
	solvers.add_clause(clause);

	clause.clear();
	clause.push_back(Lit(target, false));//true is negated
	clause.push_back(Lit(source_1, true));
	clause.push_back(Lit(source_2, false));
	clause.push_back(Lit(source_3, false));
	clause.push_back(Lit(source_4, false));
	solvers.add_clause(clause);

	clause.clear();
	clause.push_back(Lit(target, false));//true is negated
	clause.push_back(Lit(source_1, false));
	clause.push_back(Lit(source_2, true));
	clause.push_back(Lit(source_3, true));
	clause.push_back(Lit(source_4, true));
	solvers.add_clause(clause);

	clause.clear();
	clause.push_back(Lit(target, false));//true is negated
	clause.push_back(Lit(source_1, false));
	clause.push_back(Lit(source_2, true));
	clause.push_back(Lit(source_3, true));
	clause.push_back(Lit(source_4, false));
	solvers.add_clause(clause);

	clause.clear();
	clause.push_back(Lit(target, false));//true is negated
	clause.push_back(Lit(source_1, false));
	clause.push_back(Lit(source_2, true));
	clause.push_back(Lit(source_3, false));
	clause.push_back(Lit(source_4, true));
	solvers.add_clause(clause);

	clause.clear();
	clause.push_back(Lit(target, false));//true is negated
	clause.push_back(Lit(source_1, false));
	clause.push_back(Lit(source_2, true));
	clause.push_back(Lit(source_3, false));
	clause.push_back(Lit(source_4, false));
	solvers.add_clause(clause);

	clause.clear();
	clause.push_back(Lit(target, false));//true is negated
	clause.push_back(Lit(source_1, false));
	clause.push_back(Lit(source_2, false));
	clause.push_back(Lit(source_3, true));
	clause.push_back(Lit(source_4, true));
	solvers.add_clause(clause);

	clause.clear();
	clause.push_back(Lit(target, false));//true is negated
	clause.push_back(Lit(source_1, false));
	clause.push_back(Lit(source_2, false));
	clause.push_back(Lit(source_3, true));
	clause.push_back(Lit(source_4, false));
	solvers.add_clause(clause);

	clause.clear();
	clause.push_back(Lit(target, false));//true is negated
	clause.push_back(Lit(source_1, false));
	clause.push_back(Lit(source_2, false));
	clause.push_back(Lit(source_3, false));
	clause.push_back(Lit(source_4, true));
	solvers.add_clause(clause);
}
void solver::apply_xor(SATSolver &solvers,int target_1, int target_2,int source)
{
clause.clear();
clause.push_back(Lit(target_1, true));//true is negated
clause.push_back(Lit(target_2, true));
solvers.add_clause(clause);

//1 4 -6
clause.clear();
clause.push_back(Lit(target_1, false));//true is negated
clause.push_back(Lit(target_2, false));
clause.push_back(Lit(source, true));
solvers.add_clause(clause);

//1 -4 6
clause.clear();
clause.push_back(Lit(target_1, false));//true is negated
clause.push_back(Lit(target_2, true));
clause.push_back(Lit(source, false));
solvers.add_clause(clause);

//-1 4 6
clause.clear();
clause.push_back(Lit(target_1, true));//true is negated
clause.push_back(Lit(target_2, false));
clause.push_back(Lit(source, false));
solvers.add_clause(clause);

}
void solver::apply_copy(SATSolver &solvers,int target, int source_1, int source_2)
{
	clause.clear();
	clause.push_back(Lit(source_1, true));//true is negated
	clause.push_back(Lit(source_2, true));
	solvers.add_clause(clause);
	//1 2 -3
	clause.clear();
	clause.push_back(Lit(target, false));//true is negated
	clause.push_back(Lit(source_1, false));
	clause.push_back(Lit(source_2, true));
	solvers.add_clause(clause);

	//1 -2 3
	clause.clear();
	clause.push_back(Lit(target, false));//true is negated
	clause.push_back(Lit(source_1, true));
	clause.push_back(Lit(source_2, false));
	solvers.add_clause(clause);

	//-1 2 3
	clause.clear();
	clause.push_back(Lit(target, true));//true is negated
	clause.push_back(Lit(source_1, false));
	clause.push_back(Lit(source_2, false));
	solvers.add_clause(clause);
}
void solver::apply_and(SATSolver &solvers,int target_1, int target_2, int source)
{
	clause.clear();
	clause.push_back(Lit(target_1, true));//true is negated
	clause.push_back(Lit(source, false));
	solvers.add_clause(clause);

	clause.clear();
	clause.push_back(Lit(target_1, false));//true is negated
	clause.push_back(Lit(target_2, false));//true is negated
	clause.push_back(Lit(source, true));
	solvers.add_clause(clause);

	clause.clear();
	clause.push_back(Lit(target_2, true));//true is negated
	clause.push_back(Lit(source, false));
	solvers.add_clause(clause);

}
void solver::apply_Lcopy(SATSolver &solvers, int target, int source_1, int source_2)
{
	clause.clear();
	clause.push_back(Lit(target, true));//true is negated
	clause.push_back(Lit(source_1, false));//true is negated
	clause.push_back(Lit(source_2, false));
	solvers.add_clause(clause);
	//1 2 -3
	clause.clear();
	clause.push_back(Lit(target, false));//true is negated
	clause.push_back(Lit(source_1, true));
	clause.push_back(Lit(source_2, true));
	solvers.add_clause(clause);

	//1 -2 3
	clause.clear();
	clause.push_back(Lit(target, false));//true is negated
	clause.push_back(Lit(source_1, true));
	clause.push_back(Lit(source_2, false));
	solvers.add_clause(clause);

	//-1 2 3
	clause.clear();
	clause.push_back(Lit(target, false));//true is negated
	clause.push_back(Lit(source_1, false));
	clause.push_back(Lit(source_2, true));
	solvers.add_clause(clause);
	
}
void solver::apply_Land(SATSolver &solvers, int target_1, int target_2, int source)
{
	clause.clear();
	clause.push_back(Lit(target_2, true));//true is negated
	clause.push_back(Lit(source, false));
	solvers.add_clause(clause);

	clause.clear();
	clause.push_back(Lit(target_1, true));//true is negated
	clause.push_back(Lit(target_2, false));//true is negated
	clause.push_back(Lit(source, false));
	solvers.add_clause(clause);

	

	clause.clear();
	clause.push_back(Lit(target_2, false));//true is negated
	clause.push_back(Lit(source, true));
	solvers.add_clause(clause);

	clause.clear();
	clause.push_back(Lit(target_1, false));//true is negated
	clause.push_back(Lit(target_2, true));//true is negated
	clause.push_back(Lit(source, true));
	solvers.add_clause(clause);

	

}
void solver::apply_Lxor(SATSolver &solvers, int target_1, int target_2, int source)
{
	/* First move source_2 to target, then apply the following
	rules on source_1, target :
	0 0 -> 0 0
	0 1 -> 0 1
	1 0 -> 1 0
	1 0 -> 0 1
	1 1 -> 1 1
	"""

	if source_1 == source_2:
	old_target, new_target = self.get_variables(target)
	self.addclause([-new_target])
	return
	if source_1 != target :
	self.apply_mov(target, source_2)
	source = source_1
	else:
	source = source_2
	old_source, new_source = self.get_variables(source)
	old_target, new_target = self.get_variables(target)
	self.addclause([-new_source, old_source])
	self.addclause([new_target, -old_target])
	self.addclause([-new_source, -new_target, old_target])
	self.addclause([new_source, new_target, -old_source])
	self.addclause([-new_target, old_source, old_target])
	self.addclause([new_source, -old_source, -old_target])*/

	/*int a = target_1;
	int b = target_2;
	int c = source;*/
	clause.clear();
	clause.push_back(Lit(target_1, true));//true is negated
	clause.push_back(Lit(target_2, true));
	solvers.add_clause(clause);

	//1 4 -6
	clause.clear();
	clause.push_back(Lit(target_1, false));//true is negated
	clause.push_back(Lit(target_2, false));
	clause.push_back(Lit(source, true));
	solvers.add_clause(clause);

	//1 -4 6
	clause.clear();
	clause.push_back(Lit(target_1, false));//true is negated
	clause.push_back(Lit(target_2, true));
	clause.push_back(Lit(source, false));
	solvers.add_clause(clause);

	//-1 4 6
	clause.clear();
	clause.push_back(Lit(target_1, true));//true is negated
	clause.push_back(Lit(target_2, false));
	clause.push_back(Lit(source, false));
	solvers.add_clause(clause);

}
void solver::apply_Sbox(SATSolver &solvers,string sname, vector<int> input, vector<int> output) 
{
	if (sname == "S0") 
	{
		int s0[] = { 14, 9, 15, 0, 13, 4, 10, 11, 1, 2, 8, 3, 7, 6, 12, 5 };
		sbox s;
		vector<string>ss = s.getKTrueValue(s0);
		vector<vector<int>> value = getSboxCNF(ss, 8);
		getClause(solvers, value, input, output);
	}
	else if (sname == "S1")
	{
		int s1[] = { 4, 11, 14, 9, 15, 13, 0, 10, 7, 12, 5, 6, 2, 8, 1, 3 };
		sbox s;
		vector<string>ss = s.getKTrueValue(s1);
		vector<vector<int>> value = getSboxCNF(ss, 8);
		getClause(solvers, value, input, output);
	}
	else if (sname == "S2")
	{
		int s2[] = { 1, 14, 7, 12, 15, 13, 0, 6, 11, 5, 9, 3, 2, 4, 8, 10 };
		sbox s;
		vector<string>ss = s.getKTrueValue(s2);
		vector<vector<int>> value = getSboxCNF(ss, 8);
		getClause(solvers, value, input, output);
	}
	else if (sname == "S3")
	{
		int s3[] = { 7, 6, 8, 11, 0, 15, 3, 14, 9, 10, 12, 13, 5, 2, 4, 1 };
		sbox s;
		vector<string>ss = s.getKTrueValue(s3);
		vector<vector<int>> value = getSboxCNF(ss, 8);
		getClause(solvers, value, input, output);
	}
	else if (sname == "S4")
	{
		int s4[] = { 14, 5, 15, 0, 7, 2, 12, 13, 1, 8, 4, 9, 11, 10, 6, 3 };
		sbox s;
		vector<string>ss = s.getKTrueValue(s4);
		vector<vector<int>> value = getSboxCNF(ss, 8);
		getClause(solvers, value, input, output);
	}
	else if (sname == "S5")
	{
		int s5[] = { 2, 13, 11, 12, 15, 14, 0, 9, 7, 10, 6, 3, 1, 8, 4, 5 };
		sbox s;
		vector<string>ss = s.getKTrueValue(s5);
		vector<vector<int>> value = getSboxCNF(ss, 8);
		getClause(solvers, value, input, output);
	}
	else if (sname == "S6")
	{
		int s6[] = { 11, 9, 4, 14, 0, 15, 10, 13, 6, 12, 5, 7, 3, 8, 1, 2 };
		sbox s;
		vector<string>ss = s.getKTrueValue(s6);
		vector<vector<int>> value = getSboxCNF(ss, 8);
		getClause(solvers, value, input, output);
	}
	else if (sname == "S7")
	{
		int s7[] = { 13, 10, 15, 0, 14, 4, 9, 11, 2, 1, 8, 3, 7, 5, 12, 6 };
		sbox s;
		vector<string>ss = s.getKTrueValue(s7);
		vector<vector<int>> value = getSboxCNF(ss, 8);
		getClause(solvers, value,input,output);
	}
	else if (sname=="Spresent") 
	{
		int s7[] = { 12, 5, 6, 11, 9, 0, 10, 13,3, 14, 15, 8, 4, 7, 1, 2 };
		sbox s;
		vector<string>ss = s.getKTrueValue(s7);
		vector<vector<int>> value = getSboxCNF(ss, 8);
		getClause(solvers, value, input, output);
	}
	else if (sname == "Srectangle")
	{
		int srectangle[] = {6,5,12,10,1,14,7,9,11,0,3,13,8,15,4,2};
		sbox s;
		vector<string>ss = s.getKTrueValue(srectangle);
		vector<vector<int>> value = getSboxCNF(ss, 8);
		getClause(solvers, value, input, output);
	}
	else if (sname == "Sgift")
	{
		int sgift[] = {1,10,4,12,6,15,3,9,2,13,11,7,5,0,8,14};
		sbox s;
		vector<string>ss = s.getKTrueValue(sgift);
		vector<vector<int>> value = getSboxCNF(ss, 8);
		getClause(solvers, value, input, output);
	}
	else if (sname == "Stwine")
	{
		int stwine[] = { 12,0,15,10,2,11,9,5,8,3,13,7,1,14,6,4 };
		sbox s;
		vector<string>ss = s.getKTrueValue(stwine);
		vector<vector<int>> value = getSboxCNF(ss, 8);
		getClause(solvers, value, input, output);
	}

}
void solver::getClause(SATSolver &slover,vector<vector<int>> value, vector<int> input, vector<int> output)
{
	
	for (int i = 0; i < value.size(); i++) 
	{
		clause.clear();
		for (int j = 0; j < value[i].size(); j++) 
		{
			if (j < 4)
			{
				if (value[i][j] > 0) 
				{
					clause.push_back(Lit(input[j], false));
				}
				else 
				{
					clause.push_back(Lit(input[j], true));
				}
			}
			else 
			{
				if (value[i][j] > 0)
				{
					clause.push_back(Lit(output[j-4], false));
				}
				else
				{
					clause.push_back(Lit(output[j-4], true));
				}
			}
		}
		slover.add_clause(clause);
		
	}
}
void solver::apply_LSbox(SATSolver &solvers, string sname, vector<int> input, vector<int> output)
{
	if (sname =="S0")
	{
		int s0[] = { 14, 9, 15, 0, 13, 4, 10, 11, 1, 2, 8, 3, 7, 6, 12, 5 };
		sbox s;
		vector<string>ss = s.getLTrueValue(s0);
		vector<vector<int>> value = getSboxCNF(ss, 8);
		getClause(solvers, value, input, output);
	}
	else if (sname =="S1")
	{
		int s1[] = { 4, 11, 14, 9, 15, 13, 0, 10, 7, 12, 5, 6, 2, 8, 1, 3 };
		sbox s;
		vector<string>ss = s.getLTrueValue(s1);
		vector<vector<int>> value = getSboxCNF(ss, 8);
		getClause(solvers, value, input, output);
	}
	else if (sname =="S2")
	{
		int s2[] = { 1, 14, 7, 12, 15, 13, 0, 6, 11, 5, 9, 3, 2, 4, 8, 10 };
		sbox s;
		vector<string>ss = s.getLTrueValue(s2);
		vector<vector<int>> value = getSboxCNF(ss, 8);
		getClause(solvers, value, input, output);
	}
	else if (sname =="S3")
	{
		int s3[] = { 7, 6, 8, 11, 0, 15, 3, 14, 9, 10, 12, 13, 5, 2, 4, 1 };
		sbox s;
		vector<string>ss = s.getLTrueValue(s3);
		vector<vector<int>> value = getSboxCNF(ss, 8);
		getClause(solvers, value, input, output);
	}
	else if (sname =="S4")
	{
		int s4[] = { 14, 5, 15, 0, 7, 2, 12, 13, 1, 8, 4, 9, 11, 10, 6, 3 };
		sbox s;
		vector<string>ss = s.getLTrueValue(s4);
		vector<vector<int>> value = getSboxCNF(ss, 8);
		getClause(solvers, value, input, output);
	}
	else if (sname =="S5")
	{
		int s5[] = { 2, 13, 11, 12, 15, 14, 0, 9, 7, 10, 6, 3, 1, 8, 4, 5 };
		sbox s;
		vector<string>ss = s.getLTrueValue(s5);
		vector<vector<int>> value = getSboxCNF(ss, 8);
		getClause(solvers, value, input, output);
	}
	else if (sname =="S6")
	{
		int s6[] = { 11, 9, 4, 14, 0, 15, 10, 13, 6, 12, 5, 7, 3, 8, 1, 2 };
		sbox s;
		vector<string>ss = s.getLTrueValue(s6);
		vector<vector<int>> value = getSboxCNF(ss, 8);
		getClause(solvers, value, input, output);
	}
	else if (sname =="S7")
	{
		int s7[] = { 13, 10, 15, 0, 14, 4, 9, 11, 2, 1, 8, 3, 7, 5, 12, 6 };
		sbox s;
		vector<string>ss = s.getLTrueValue(s7);
		vector<vector<int>> value = getSboxCNF(ss, 8);
		getClause(solvers, value, input, output);



	}
	else if (sname == "Spresent")
	{
		int spresent[] = { 12, 5, 6, 11, 9, 0, 10, 13,3, 14, 15, 8, 4, 7, 1, 2 };
		sbox s;
		vector<string>ss = s.getLTrueValue(spresent);
		vector<vector<int>> value = getSboxCNF(ss, 8);
		getClause(solvers, value, input, output);
	}
	else if (sname == "Srectangle")
	{
		int srectangle[] = { 6,5,12,10,1,14,7,9,11,0,3,13,8,15,4,2};
		sbox s;
		vector<string>ss = s.getLTrueValue(srectangle);
		vector<vector<int>> value = getSboxCNF(ss, 8);
		getClause(solvers, value, input, output);
	}
	else if (sname == "Sgift")
	{
		int sgift[] = { 1,10,4,12,6,15,3,9,2,13,11,7,5,0,8,14 };
		sbox s;
		vector<string>ss = s.getLTrueValue(sgift);
		vector<vector<int>> value = getSboxCNF(ss, 8);
		getClause(solvers, value, input, output);
	}
	else if (sname == "Stwine")
	{
		int stwine[] = { 12,0,15,10,2,11,9,5,8,3,13,7,1,14,6,4 };
		sbox s;
		vector<string>ss = s.getLTrueValue(stwine);
		vector<vector<int>> value = getSboxCNF(ss, 8);
		getClause(solvers, value, input, output);
	}
}

int solver::get_variables(string bit) 
{
	int values;
	if (bit != "") 
	{
		bit.erase(0,1);
		values = stoi(bit);
	}
		
	return values;
}

vector<vector<int>>solver::KReturnAllSolu(vector<int> active)
{
	SATSolver solvers;
	create_conditions(solvers);
	vector<Lit> ban_solution;
	//存储中间值
	vector<vector<int>> interSolutions;
	//存储过度向量
	vector<vector<int>> middInterSolutions;
	vector<int> interSolution;
	clause.clear();
	for (int i = 0; i < state_size; i++)
	{
		if (active[i]==1)
		{
			clause.push_back(Lit(i, false));
		}
		else
		{
			clause.push_back(Lit(i, true));
		}
	}
	while (true)
	{

		lbool ret = solvers.solve(&clause);
		if (ret != l_True)
		{
			//All solutions found.
			break;
		}


		interSolution.clear();
		for (int out = 0; out<state_size; out++)
		{
			if (solvers.get_model()[endIndex[out]] == l_True)
			{
				//cout<<"1"<<",";
				interSolution.push_back(1);
			}
			else
			{
				//cout<<"0"<<",";
				interSolution.push_back(0);
			}
		}
		//cout << endl;
		middInterSolutions.push_back(interSolution);
		//int sizes = middInterSolutions.size();
		ban_solution.clear();
		for (uint32_t var = 0; var < solvers.nVars(); var++)
		{
			if (solvers.get_model()[var] != l_Undef)
			{
				ban_solution.push_back(
					Lit(var, (solvers.get_model()[var] == l_True) ? true : false));
			}
		}
		solvers.add_clause(ban_solution);
	}

	int sizes = middInterSolutions.size();
	Redundant(middInterSolutions);
	Redundant(middInterSolutions);
	//AddVector(interSolutions, middInterSolutions);
	//middInterSolutions.clear();

	return  middInterSolutions;
}

vector<vector<int>>solver::LReturnAllSolu(vector<int> active)
{
	SATSolver solvers;

	Lcreate_conditions(solvers);
	vector<Lit> ban_solution;
	//存储中间值
	vector<vector<int>> interSolutions;
	//存储过度向量
	vector<vector<int>> middInterSolutions;
	vector<int> interSolution;
	clause.clear();
	for (int i = 0; i < state_size; i++) 
	{
		if (active[i]==1)
		{
			clause.push_back(Lit(i, false));
		}
		else
		{
			clause.push_back(Lit(i, true));
		}
	}
	while (true)
	{

		lbool ret = solvers.solve(&clause);
		if (ret != l_True)
		{
			//All solutions found.
			break;
		}


		interSolution.clear();
		for (int out = 0; out<state_size; out++)
		{
			if (solvers.get_model()[endIndex[out]] == l_True)
			{
				//cout<<"1"<<",";
				interSolution.push_back(1);
			}
			else
			{
				//cout<<"0"<<",";
				interSolution.push_back(0);
			}
		}
		vector<vector<int>>::iterator rets;
		//vector<vector<int>>ssss = middInterSolutions;
		//vector<int>sds = interSolution;
		rets = std::find(middInterSolutions.begin(), middInterSolutions.end(), interSolution);
		if (rets == middInterSolutions.end())
			middInterSolutions.push_back(interSolution);
		else {
			middInterSolutions.erase(rets);
		}
		ban_solution.clear();
		for (uint32_t var = 0; var < solvers.nVars(); var++)
		{
			if (solvers.get_model()[var] != l_Undef)
			{
				ban_solution.push_back(
					Lit(var, (solvers.get_model()[var] == l_True) ? true : false));
			}
		}
		solvers.add_clause(ban_solution);
	}

	//int sizes = middInterSolutions.size();
	//Redundant(middInterSolutions);

	return  middInterSolutions;
}
vector<vector<int>>solver::khudraReturnAllSolu(vector<int> active)
{
	SATSolver solvers;

	Lcreate_conditions(solvers);
	vector<Lit> ban_solution;
	//存储中间值
	vector<vector<int>> interSolutions;
	//存储过度向量
	vector<vector<int>> middInterSolutions;
	vector<int> interSolution;
	clause.clear();
	for (int i = 0; i < state_size; i++) 
	{
		if (active[i]==1)
		{
			clause.push_back(Lit(i, false));
		}
		else
		{
			clause.push_back(Lit(i, true));
		}
	}
	while (true)
	{

		lbool ret = solvers.solve(&clause);
		if (ret != l_True)
		{
			//All solutions found.
			break;
		}

		if(middInterSolutions.size()>10){
			break;
		}
		interSolution.clear();
		for (int out = 0; out<state_size; out++)
		{
			if (solvers.get_model()[endIndex[out]] == l_True)
			{
				//cout<<"1"<<",";
				interSolution.push_back(1);
			}
			else
			{
				//cout<<"0"<<",";
				interSolution.push_back(0);
			}
		}
		vector<vector<int>>::iterator rets;
		//vector<vector<int>>ssss = middInterSolutions;
		//vector<int>sds = interSolution;
		rets = std::find(middInterSolutions.begin(), middInterSolutions.end(), interSolution);
		if (rets == middInterSolutions.end())
			middInterSolutions.push_back(interSolution);
		else {
			middInterSolutions.erase(rets);
		}
		ban_solution.clear();
		for (uint32_t var = 0; var < solvers.nVars(); var++)
		{
			if (solvers.get_model()[var] != l_Undef)
			{
				ban_solution.push_back(
					Lit(var, (solvers.get_model()[var] == l_True) ? true : false));
			}
		}
		solvers.add_clause(ban_solution);
	}

	//int sizes = middInterSolutions.size();
	//Redundant(middInterSolutions);

	return  middInterSolutions;
}



// 将L集合中的元素添加到K集合中
vector<vector<int>>solver::LSetToKSet(vector<vector<int>> K, vector<vector<int>>L,string name )
{
	//vector<vector<int>> tmp;
	//tmp = L;
	int gif[] = { 0,1,3,4,5,7,8,9,11,12,13,15,16,17,19,20,21,23,24,25,28,29,32,33,36,37,40,41,44,45,48,49,52,53,56,57,60,61,63 };
	if (name == "simon")
	{
		for (int i = 0; i < L.size(); i++)
		{
			for (int j = state_size - 1; j >= state_size / 2; j--)
			{
				if (L[i][j] == 0)
				{
					L[i][j] = 1;
					K.push_back(L[i]);
					L[i][j] = 0;
				}
			}
		}
	}
	else if (name == "lblock")
	{
		for (int i = 0; i < L.size(); i++)
		{
			for (int j = state_size - 1; j >= state_size / 2; j--)
			{
				if (L[i][j] == 0)
				{
					L[i][j] = 1;
					K.push_back(L[i]);
					L[i][j] = 0;
				}
			}
		}
	}
	else if (name == "gift")
	{
		for (int i = 0; i < L.size(); i++)
		{
			for (int j = 0; j < 39; j++)
			{
				if (L[i][gif[j]] == 0)
				{
					L[i][gif[j]] = 1;
					K.push_back(L[i]);
					L[i][gif[j]] = 0;
				}
			}
		}
	}
	else if (name == "khudra1") {
		for (int i = 0; i < L.size(); i++) {
			for (int j = 0; j < 16; j++) {
				if (L[i][j] == 0) {
					L[i][j] = 1;
					K.push_back(L[i]);
					L[i][j] = 0;
				}

			}
		}
	}
	else if (name == "khudra2") {
		for (int i = 0; i < L.size(); i++) {
			for (int j = 0; j < 16; j++) {
				if (L[i][j + 48] == 0) {
					L[i][j + 48] = 1;
					K.push_back(L[i]);
					L[i][j + 48] = 0;
				}
			}
		}
	}
	Redundant(K);
	return K;
}

// 将L集合中多余的元素去除
vector<vector<int>>solver::RedundantLset(vector<vector<int>> K, vector<vector<int>>L)
{
	sort(L.begin(), L.end());
	int index = 0;
	for (int i = L.size()-1; i >= 0; i--) 
	{
		for (int j = K.size()-1; j >= 0; j--) 
		{
			for (int z = 0; z < state_size; z++) 
			{
				if (L[i][z] >= K[j][z])
				{
					index++;
				}
			}

			if (index == state_size && i != j)
			{
				L.erase(L.begin() + i);
				index = 0;
				break;


			}
			index = 0;
		}
	}

	return L;

}
//将midd中的值重新赋值给inter
void solver::AddVector(vector<vector<int>> &inter, vector<vector<int>> &midd)
{
	for (int i = 0; i < midd.size(); i++)
	{
		inter.push_back(midd[i]);
	}


}

void solver::is_balanced(vector<int> startBit,set<int> &result) 
{
	SATSolver solvers;
	create_conditions(solvers);
	vector<Lit> tmp;
	//clause.clear();
	vector<Lit> clauses;
	for (int i = 0; i < state_size; i++) 
	{
		//vector<int>::iterator ret;
		//ret = std::find(active.begin(), active.end(), i);
		if(startBit[i]==1)
		{
			clauses.push_back(Lit(i, false));
			//tmp.push_back(Lit(i, false));
		}
		else 
		{
			clauses.push_back(Lit(i, true));
			//tmp.push_back(Lit(i, false));
		}
	}
	if(conIndex.size()>0)
	{
		for (int i = 0; i < conIndex.size(); i++) 
		{
			clauses.push_back(Lit(conIndex[i], true));
		}
	}
	tmp = clauses;
	for (int j = 0; j < state_size; j++) 
	{
		for (int z = 0; z < state_size; z++) 
		{
			if (z == j) 
			{
				clauses.push_back(Lit(endIndex[z], false));
			}
			else 
			{
				clauses.push_back(Lit(endIndex[z], true));
			}
		}
	
		lbool ret = solvers.solve(&clauses);
		clauses.clear();
		clauses = tmp;
		if (ret==l_True) 
		{
			result.insert(j);
		}
		else 
		{
			break;
		}
	}


	//lbool ret = solvers.solve(&clause);
	////clause.clear();
	//if (ret == l_True) 
	//{
	//	return true;
	//}
	//else 
	//{
	//	return false;
	//}


}

bool solver::Balance_bit(vector<int> activebit, int bit) 
{
	SATSolver solvers;
	create_conditions(solvers);
	//vector<Lit> tmp;
	//clause.clear();
	vector<Lit> clauses;
	for (int i = 0; i < state_size; i++)
	{
		//vector<int>::iterator ret;
		//ret = std::find(active.begin(), active.end(), i);
		if (activebit[i] == 1)
		{
			clauses.push_back(Lit(i, false));
			//tmp.push_back(Lit(i, false));
		}
		else
		{
			clauses.push_back(Lit(i, true));
			//tmp.push_back(Lit(i, false));
		}
	}
	if (conIndex.size()>0)
	{
		for (int i = 0; i < conIndex.size(); i++)
		{
			clauses.push_back(Lit(conIndex[i], true));
		}
	}
	for (int z = 0; z < state_size; z++)
	{
		if (z == bit)
		{
			clauses.push_back(Lit(endIndex[z], false));
		}
		else
		{
			clauses.push_back(Lit(endIndex[z], true));
		}
	}
	lbool ret = solvers.solve(&clauses);
	clauses.clear();
	//clauses = tmp;
	if (ret == l_True)
	{
		return false;
	}
	else
	{
		return true;
	}


}



bool solver::LBalance_bit(vector<int> activebit, int bit)
{
	SATSolver solvers;
	Lcreate_conditions(solvers);
	//vector<Lit> tmp;
	//clause.clear();
	vector<Lit> clauses;
	for (int i = 0; i < state_size; i++)
	{
		//vector<int>::iterator ret;
		//ret = std::find(active.begin(), active.end(), i);
		if (activebit[i] == 1)
		{
			clauses.push_back(Lit(i, false));
			//tmp.push_back(Lit(i, false));
		}
		else
		{
			clauses.push_back(Lit(i, true));
			//tmp.push_back(Lit(i, false));
		}
	}
	if (conIndex.size()>0)
	{
		for (int i = 0; i < conIndex.size(); i++)
		{
			clauses.push_back(Lit(conIndex[i], true));
		}
	}
	for (int z = 0; z < state_size; z++)
	{
		if (z == bit)
		{
			clauses.push_back(Lit(endIndex[z], false));
		}
		else
		{
			clauses.push_back(Lit(endIndex[z], true));
		}
	}
	lbool ret = solvers.solve(&clauses);
	clauses.clear();
	//clauses = tmp;
	if (ret == l_True)
	{
		return false;
	}
	else
	{
		return true;
	}


}



//void solver::Check() 
//{
//	vector<vector<int>> middInterSolutions;
//	vector<int> interSolution;
//	vector<Lit> ban_solution;
//	//create_conditions();
//	clause.clear();
//	for (int index = 0; index < state_size; index++)
//	{
//		if (index < 3)
//		{
//			clause.push_back(Lit(index, false));
//		}
//		else 
//		{
//			clause.push_back(Lit(index, true));
//		}
//	}
//
//	
//	while (true) {
//		//lbool ret = solvers.solve(&clause);
//		if (ret != l_True) {
//			//assert(ret == l_False);
//			//All solutions found.
//			break;
//		}
//
//		//Use solution here. print it, for example.
//		interSolution.clear();
//		for (int out = 0; out<8; out++)
//		{
//			
//			if (solvers.get_model()[endIndex[out]] == l_True)
//			{
//				//cout<<"1"<<",";
//				interSolution.push_back(1);
//			}
//			else
//			{
//				//cout<<"0"<<",";
//				interSolution.push_back(0);
//			}
//		}
//
//
//		//cout<<endl;			
//		//interSolutions.push_back(interSolution);
//
//		middInterSolutions.push_back(interSolution);
//		//solution.clear();
//		//Banning found solution
//		ban_solution.clear();
//		for (uint32_t var = 0; var < solvers.nVars(); var++) {
//			if (solvers.get_model()[var] != l_Undef) {
//				ban_solution.push_back(
//					Lit(var, (solvers.get_model()[var] == l_True) ? true : false));
//			}
//		}
//		solvers.add_clause(ban_solution);
//	}
//	Redundant(middInterSolutions);
//	int iii = middInterSolutions.size();
//}

void solver::Redundant(vector<vector<int>> &clause)
{
	clause.erase(unique(clause.begin(), clause.end()), clause.end());
	sort(clause.begin(), clause.end());
	vector<int> interSolution;
	vector<int> comSolution;
	int index = 0;
	for (int i = clause.size() - 1; i >= 0; i--)
	{
		interSolution = clause[i];

		for (int j =i - 1; j >= 0; j--)
		{
			comSolution = clause[j];
			for (int z = 0; z<interSolution.size(); z++)
			{
				if (interSolution[z] >= comSolution[z])
				{
					index++;
				}
				else 
				{
					index=0;
					break;
				}
			}

			if (index == interSolution.size() && i != j)
			{
				clause.erase(clause.begin() + i);
				index = 0;
				break;
			}
			index = 0;

		}
	}


	/*clause.erase(unique(clause.begin(), clause.end()), clause.end());
	sort(clause.begin(), clause.end());
	vector<int> interSolution;
	vector<int> comSolution;
	int index = 0;
	for (int i = clause.size() - 1; i >= 0; i--)
	{
		interSolution = clause[i];
		int sum1 = accumulate(interSolution.begin(), interSolution.end(), 0);
		for (int j = 0; j < i; j++)
		{
			bool flag = false;
			comSolution = clause[j];
			int sum2 = accumulate(comSolution.begin(), comSolution.end(), 0);
			if (sum1>sum2) {
				for (int z = 0; z<interSolution.size(); z++)
				{
					if (interSolution[z] < comSolution[z])
					{
						flag = true;
						break;
					}
				}
				if (flag == false)
				{
					clause.erase(clause.begin() + i);
					break;
				}
			}
		}
	}
	clause.erase(unique(clause.begin(), clause.end()), clause.end());*/
	//return clause;


}


bool solver::Is_exist(int bit, vector<int> active) 
{
	bool isTrue = false;
	for (int i = 0; i < active.size(); i++) 
	{
		if(bit==active[i]){
			isTrue = true;
			break;
		}
	}

	return isTrue;

}

//通过真值表获取合取范式
vector<vector<int>> solver:: getSboxCNF(vector<string> value,int num) 
{
	//char arr[LEN];
	vector<int> tmp;
	vector<vector<int>> result;
	int sta1 = 0, sta2 = 0;
	int brr[300][14];
	int beg = 1;
	for (int i = 0; i <value.size(); i++)
	{
		//cin >> arr[i];
		if (value[i] == "T")
			sta1++;
		else
			sta2++;
	}


	int cnt1 = 0, cnt2 = 0;
	for (int i = value.size() - 1; i >= 0; i--)
	{
		cnt1 = 0;
		int val = i;
		while (cnt1 < num)
		{
			
			brr[cnt2][cnt1] = val % 2;
			val = val / 2;
			cnt1++;
		}
		cnt2++;
	}

	int k = 0;
	for (int i = 0; i < value.size(); i++)
	{
		if (value[i] == "F")
		{
			k++;
			//cout << '(';
			for (int j = num-1; j >= 0; j--)
			{
				if (brr[i][j] == 0)
				{
					//cout << (char)(beg++);
					tmp.push_back(beg++);
				}
				else
				{
					//cout << "┓" << (char)(beg++);
					tmp.push_back(-beg++);
				}
				//if (j != 1)
					//cout << "∨";
			}
			//cout << ')';
			result.push_back(tmp);
			tmp.clear();
			//if (k < sta2)
				//cout << endl;
		}
		beg = 1;
	}


	return result;
}
