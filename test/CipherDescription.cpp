#include<vector>;
#include<string>;
#include<iostream>;
#include"CipherDescription.h"
using namespace std;

CipherDescription::CipherDescription(int size)
{
	state_size = size;
	rounds = 1;
	descriptions = "";
}

CipherDescription::CipherDescription()
{
	rounds = 1;
	descriptions = "";
}

void CipherDescription::apply_xor(string source_1, string source_2, string target, bool description ) 
{
	
		values.push_back(source_1);
		values.push_back(source_2);
		values.push_back(target);
		values.push_back("XOR");

		transition.push_back(values);
		values.clear();
		if (description){
			descriptions += target+"=" +source_1+"XOR"+source_2+"\n";
		}

}
void CipherDescription::apply_threexor(string source_1, string source_2,string source_3, string target, bool description)
{


	values.push_back(source_1);
	values.push_back(source_2);
	values.push_back(source_3);
	values.push_back(target);
	values.push_back("LXOR");

	transition.push_back(values);
	values.clear();
	if (description) {
		descriptions += target + "=" + source_1 + "LXOR" + source_2 +"LXOR"+source_3+ "\n";
	}

}

void CipherDescription::apply_and(string source_1, string source_2, string target, bool description ) {
	
		
		values.push_back(source_1);
		values.push_back(source_2);
		values.push_back(target);
		values.push_back("AND");

		transition.push_back(values);
		values.clear();
		if (description) {
			descriptions += target + "=" + source_1 + "AND" + source_2 + "\n";
		}	
}

void CipherDescription::apply_permutation(vector<string> permutation) 
{
	
	check_permutation(permutation);
	permutation.push_back("'PERM'");
	transition.push_back(permutation);
	descriptions += permutation[2] + " -> ";
	descriptions += " -> "+ permutation[0]+","+ permutation[1] + '\n';

}

void CipherDescription::apply_mov(string source, string source_1,string target , bool description)
{	
	values.push_back(source);
	values.push_back(source_1);
	values.push_back(target);
	values.push_back("COPY");

	transition.push_back(values);
	values.clear();
	if (description) {
		descriptions += target + "COPY:" + source +":" + source_1+"\n";
	}
		
}
void CipherDescription::apply_threemov(string source, string source_1,string source_2, string target, bool description)
{
	values.push_back(source);
	values.push_back(source_1);
	values.push_back(source_2);
	values.push_back(target);
	values.push_back("LCOPY");

	transition.push_back(values);
	values.clear();
	if (description) {
		descriptions += target + "LCOPY:" + source + ":" + source_1 + ":"+source_2+"\n";
	}

}
void CipherDescription::apply_fourmov(string source, string source_1, string source_2,string source_3, string target, bool description)
{
	values.push_back(source);
	values.push_back(source_1);
	values.push_back(source_2);
	values.push_back(source_3);
	values.push_back(target);
	values.push_back("FOURCOPY");

	transition.push_back(values);
	values.clear();
	if (description) {
		descriptions += target + "FOURCOPY:" + source + ":" + source_1 + ":" + source_2 + ":" + source_3 + "\n";
	}
}
void CipherDescription::apply_Sbox(vector<string> input,vector<string> output,string sname) 
{
	for (int i = 0; i < input.size(); i++) 
	{
		values.push_back(input[i]);
	}
	for (int i = 0; i < output.size(); i++)
	{
		values.push_back(output[i]);
	}
	values.push_back(sname);
	transition.push_back(values);
	values.clear();
	descriptions += sname + input[0]+","+input[1] + "," + input[2] + "," + input[3] + ","  + ":input ---"+output[0] + "," + output[1] + "," + output[2] + "," + output[3] + ",""---output \n";
}
void CipherDescription::apply_modAdd(vector<int> a_inputvalue,vector<int> b_inputvalue,vector<int> &output,int startindex,int &endindex) 
{
	output.clear();
	string input1,input2, a1, a2, a3, b1, b2, b3, d, v, g, r, m, q, w;
	input1 = "s" + to_string(a_inputvalue[0]);
	a1 = "c" + to_string(startindex+1);
	a2 = "c" + to_string(startindex + 2);
	apply_mov(a1,a2,input1);

	input2 = "s" + to_string(b_inputvalue[0]);
	b1 = "c" + to_string(startindex + 3);
	b2 = "c" + to_string(startindex + 4);
	apply_mov(b1, b2, input2);

	d="t"+ to_string(startindex + 5);
	apply_xor(a1,b1,d);
	output.push_back(startindex + 5);
	v="t"+ to_string(startindex + 6);
	apply_and(a2,b2,v);

	g="c"+ to_string(startindex + 7);
	r = "c" + to_string(startindex + 8);
	apply_mov(g,r,v);

	a1 = "c" + to_string(startindex + 9);
	a2 = "c" + to_string(startindex + 10);
	a3 = "c" + to_string(startindex + 11);
	input1= "s" + to_string(a_inputvalue[1]);
	apply_threemov(a1,a2, a3,input1);

	input2 = "s" + to_string(b_inputvalue[1]);
	b1 = "c" + to_string(startindex + 12);
	b2 = "c" + to_string(startindex + 13);
	b3="c" + to_string(startindex + 14);
	apply_threemov(b1, b2, b3, input2);

	int startindexvalue = 0;//生成变量的起始值
	int starttmp = startindex + 15;
	int lasttmp = 0;
	for (int i = 2; i <=a_inputvalue.size() - 2;i++)
	{
		d="t" + to_string(13* startindexvalue +starttmp);
		apply_threexor(a1, b1, g, d);
		output.push_back(13 * startindexvalue + starttmp);
		v="t"+ to_string(13 * startindexvalue + starttmp+1);
		apply_and(a2, b2, v);
		m="t" + to_string(13 * startindexvalue + starttmp + 2);
		apply_xor(a3,b3,m);
		q="t" + to_string(13 * startindexvalue + starttmp + 3);
		apply_and(m,r,q);
		w="t" + to_string(13 * startindexvalue + starttmp + 4);
		apply_xor(v,q,w);
		g="t" + to_string(13 * startindexvalue + starttmp + 5);
		r="t" + to_string(13 * startindexvalue + starttmp + 6);
		apply_mov(g,r,w);
		
		input1= "s" + to_string(a_inputvalue[i]);
		a1 = "c" + to_string(13 * startindexvalue + starttmp + 7);
		a2 = "c" + to_string(13 * startindexvalue + starttmp + 8);
		a3 = "c" + to_string(13 * startindexvalue + starttmp + 9);
		apply_threemov(a1,a2,a3,input1);

		input2 = "s" + to_string(b_inputvalue[i]);
		b1 = "c" + to_string(13 * startindexvalue + starttmp + 10);
		b2 = "c" + to_string(13 * startindexvalue + starttmp + 11);
		b3 = "c" + to_string(13 * startindexvalue + starttmp + 12);
		apply_threemov(b1, b2, b3, input2);	
		lasttmp = 13 * startindexvalue + starttmp + 12;
		startindexvalue++;
	}
	d = "t" + to_string(lasttmp + 1);
	apply_threexor(a1, b1, g, d);
	output.push_back(lasttmp + 1);
	v= "t" + to_string(lasttmp + 2);
	apply_and(a2,b2,v);
	m= "t" + to_string(lasttmp + 3);
	apply_xor(a3, b3, m);
	q= "t" + to_string(lasttmp + 4);
	apply_and(m,r,q);
	w= "t" + to_string(lasttmp + 5);
	apply_xor(v, q, w);
	d= "t" + to_string(lasttmp + 6);
	input1 ="t"+to_string(a_inputvalue[a_inputvalue.size() - 1]);
	input2 = "t" + to_string(b_inputvalue[b_inputvalue.size() - 1]);
	//apply_xor(input1, input2, d);
	apply_threexor(input1, input2, w,d);
	output.push_back(lasttmp + 6);
	endindex = lasttmp + 6;
}

void CipherDescription::apply_ConstantmodAdd(vector<int> a_inputvalue, vector<int> &output, int startindex, int &endindex) 
{
	output.clear();
	string input1, input2, a1, a2, a3, b1, b2, b3, d, v, g, r, m, q, w;
	input1 = "s" + to_string(a_inputvalue[0]);
	a1 = "c" + to_string(startindex + 1);
	a2 = "c" + to_string(startindex + 2);
	apply_mov(a1, a2, input1);

	//d = "t" + to_string(startindex + 5);
	//apply_xor(a1, b1, d);
	output.push_back(startindex + 1);
	v = a2;
	//apply_and(a2, b2, v);

	g = "c" + to_string(startindex + 3);
	r = "c" + to_string(startindex + 4);
	apply_mov(g, r, v);

	a1 = "c" + to_string(startindex + 5);
	a2 = "c" + to_string(startindex + 6);
	a3 = "c" + to_string(startindex + 7);
	input1 = "s" + to_string(a_inputvalue[1]);
	apply_threemov(a1, a2, a3, input1);

	

	int startindexvalue = 0;//生成变量的起始值
	int starttmp = startindex + 8;
	int lasttmp = 0;
	for (int i = 2; i <= a_inputvalue.size() - 2; i++)
	{
		d = "t" + to_string(8 * startindexvalue + starttmp);
		apply_xor(a1,g, d);
		output.push_back(8 * startindexvalue + starttmp);

		v = a2;
		//apply_and(a2, b2, v);
		m = a3;
		//apply_xor(a3, b3, m);

		q = "t" + to_string(8 * startindexvalue + starttmp + 1);
		apply_and(m, r, q);
		w = "t" + to_string(8 * startindexvalue + starttmp + 2);
		apply_xor(v, q, w);

		g = "t" + to_string(8 * startindexvalue + starttmp + 3);
		r = "t" + to_string(8 * startindexvalue + starttmp + 4);
		apply_mov(g, r, w);

		input1 = "s" + to_string(a_inputvalue[i]);
		a1 = "c" + to_string(8 * startindexvalue + starttmp + 5);
		a2 = "c" + to_string(8 * startindexvalue + starttmp + 6);
		a3 = "c" + to_string(8 * startindexvalue + starttmp + 7);
		apply_threemov(a1, a2, a3, input1);		
		lasttmp = 8 * startindexvalue + starttmp + 7;
		startindexvalue++;
	}
	d = "t" + to_string(lasttmp + 1);
	apply_xor(a1,g, d);
	output.push_back(lasttmp + 1);
	v = a2;
	//apply_and(a2, b2, v);
	m = a3;
	//apply_xor(a3, b3, m);
	q = "t" + to_string(lasttmp + 2);
	apply_and(m, r, q);
	w = "t" + to_string(lasttmp + 3);
	apply_xor(v, q, w);
	d = "t" + to_string(lasttmp + 4);
	input1 = "t" + to_string(a_inputvalue[a_inputvalue.size() - 1]);
	//input2 = "t" + to_string(b_inputvalue[b_inputvalue.size() - 1]);
	//apply_xor(input1, input2, d);
	apply_xor(input1,w,d);
	output.push_back(lasttmp + 4);
	endindex = lasttmp + 4;
}


void  CipherDescription::check_permutation(vector<string> permutation) 
{
	/*
	Check that permutation is correctly specified
	'''
	*/
	if(permutation.size()>0)
	{
		for (int var = 0; var < permutation.size(); var++) 
			{
			string va = permutation[var];
			if (va[0] == 's') 
			{
				va.erase(0);
				int number = stoi(va);
				if (number >= state_size)
					return;
			}

			}
	}
	
}

void CipherDescription::set_rounds(int round) {
	/*
	Set the number of transitions used in the cipher
	'''*/
	rounds = round;
}
void CipherDescription::print_descriptions()
{
	cout << descriptions << endl;
}