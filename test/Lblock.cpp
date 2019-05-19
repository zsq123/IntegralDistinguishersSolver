

#include <cryptominisat5/cryptominisat.h>
#include <assert.h>
#include <vector>
#include<climits>
#include <fstream>
#include<iostream>
#include<string>
#include<sstream>
#include<cstdio>  
#include <algorithm>
#include<set>
#include"CipherDescription.h"
#include"Solver.h"
#include"Sbox.h"
#include"Cipher.h"
#include <windows.h>
using std::vector;
using namespace CMSat;
using namespace std;
bool LblockBalancedOrNot(CipherDescription cipher, vector<int> k, int conbit);
void LblockLAndKVectors(CipherDescription cipher, vector<vector<int>> &K, vector<vector<int>>&L, vector<int> Qindex, bool isChange, int bitsize);
int LblockResult(int rounds, int bitsizes, int conbit, vector<vector<int>>K, vector<vector<int>>L);
CipherDescription generate_one_Lblock(int index);
CipherDescription generate_new_LBlock(int rounds, int n, int startbit, vector<int>&Qindex);

int CipherBalancedBit::LblockBalancedBit(){
	//起始时间
		SYSTEMTIME st = { 0 };
		GetLocalTime(&st);  //获取当前时间 可精确到ms
		printf("%d-%02d-%02d %02d:%02d:%02d\n", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
		cout << "lblock" << endl;
		
		vector<int>Lactive_bits;
		vector<int>Kactive_bits;
		for (int j = 0; j < 64; j++)
		{
			if (j == 46)
			{
				Lactive_bits.push_back(0);
			}
			else
			{
				Lactive_bits.push_back(1);
			}
			Kactive_bits.push_back(1);
		}
		vector<vector<int>>L;
		vector<vector<int>>K;
		L.push_back(Lactive_bits);
		K.push_back(Kactive_bits);

		for (int i = 0; i < 64; i++) {
			cout << i << ",";
			int result = LblockResult(17, 64, i, K, L);
			if (result == 0)
			{
				cout << "balance" << endl;
			}
			else if (result == 2)
			{
				cout << "unknown" << endl;
			}
			else {
				cout << "odd" << endl;

			}
		}

		//截止时间	
		GetLocalTime(&st);  //获取当前时间 可精确到ms
		printf("%d-%02d-%02d %02d:%02d:%02d\n", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
		return 0;
}

int LblockResult(int rounds, int bitsizes, int conbit, vector<vector<int>>K, vector<vector<int>>L)
{
	bool isChange = false;
	vector<int> Qindex;
	vector<vector<int>> nextL;
	for (int i = 0; i <rounds; i++)
	{
		//cout << i << endl;
		for (int j = 0; j < 8; j++) {
			Qindex.clear();
			//CipherDescription simon = generate_new_simon(rounds - i, bitsizes, j, Qindex);
			CipherDescription Lblock = generate_new_LBlock(rounds - i, bitsizes, j, Qindex);
			vector<int> nullvalue;
			CipherDescription lblocks = generate_new_LBlock(1, bitsizes, 0, nullvalue);
			solver sols;
			sols.load_cipher(lblocks);
			for (int i = 0; i < L.size(); i++) {
				for (int j = 0; j < 4; j++) {
					if (L[i][Qindex[j]] == 0)
					{
						L[i][Qindex[j]] = 1;
						K.push_back(L[i]);
						L[i][Qindex[j]] = 0;
					}
				}
			}
			sols.Redundant(K);
			L = sols.RedundantLset(K, L);
			//CipherDescription re = generate_LBlock_versions(17,64);
			for (int k = K.size() - 1; k >= 0; k--)
			{
				if (LblockBalancedOrNot(Lblock, K[k], conbit))
				{				
					return 2;
				}
				else
				{
					K.erase(K.begin() + k);
				}
			}
			nextL.clear();
			for (int z = 0; z < L.size(); z++)
			{
				if (LblockBalancedOrNot(Lblock, L[z], conbit))
				{				
					nextL.push_back(L[z]);
				}
			}
			if (nextL.size() == 0)
			{
				return 0;
			}
			L = nextL;
			if (j == 7)
			{
				isChange = true;
			}
			else
			{
				isChange = false;
			}

			LblockLAndKVectors(generate_one_Lblock(j), K, L, Qindex, isChange, bitsizes);
		}
	}

	if (nextL.size() != 0)
	{
		cout << "new dis 1" << endl;
		return 1;
	}
}
bool LblockBalancedOrNot(CipherDescription cipher, vector<int> k, int conbit)
{
	bool returnValue = false;
	CipherDescription simon = cipher; //generate_simon_version(rounds, bitsizes, 8, 1, 2);
	solver sol;
	sol.load_cipher(simon);
	if (sol.Balance_bit(k, conbit))
	{
		return returnValue = false;
	}
	else
	{
		return returnValue = true;
	}

}

void LblockLAndKVectors(CipherDescription cipher, vector<vector<int>> &K, vector<vector<int>>&L, vector<int> Qindex, bool isChange, int bitsize)
{
	CipherDescription cip = cipher; //generate_simon_version(rounds, bitsizes, 8, 1, 2);
	solver sol;
	sol.load_cipher(cip);
	
	
	vector<vector<int>> resultL;

	for (int i = 0; i<L.size(); i++)
	{
		vector<vector<int>> tmpL;
		vector<int>tmpQ;
		tmpQ.push_back(L[i][Qindex[0]]);
		tmpQ.push_back(L[i][Qindex[1]]);
		tmpQ.push_back(L[i][Qindex[2]]);
		tmpQ.push_back(L[i][Qindex[3]]);
		tmpQ.push_back(L[i][Qindex[4]]);
		tmpQ.push_back(L[i][Qindex[5]]);
		tmpQ.push_back(L[i][Qindex[6]]);
		tmpQ.push_back(L[i][Qindex[7]]);
		tmpL = sol.LReturnAllSolu(tmpQ);
		vector<vector<int>> ss = tmpL;
		for (int j = 0; j < tmpL.size(); j++)
		{
			vector<vector<int>>::iterator rets;
			L[i][Qindex[0]] = tmpL[j][0];
			L[i][Qindex[1]] = tmpL[j][1];
			L[i][Qindex[2]] = tmpL[j][2];
			L[i][Qindex[3]] = tmpL[j][3];
			L[i][Qindex[4]] = tmpL[j][4];
			L[i][Qindex[5]] = tmpL[j][5];
			L[i][Qindex[6]] = tmpL[j][6];
			L[i][Qindex[7]] = tmpL[j][7];

			rets = std::find(resultL.begin(), resultL.end(), L[i]);
			if (rets == resultL.end())
				resultL.push_back(L[i]);
			else {
				resultL.erase(rets);
			}
		}
	}

	if (isChange)
	{
		if (resultL.size()> 0) {

			for (int z = 0; z < resultL.size(); z++)
			{
				vector<int> tmpsize;
				int tmpn = 0;//中间变量
				tmpsize = resultL[z];

				for (int j = 0; j < bitsize / 2; j++)
				{
					int tmp = 0;
					tmp = resultL[z][j];
					tmpsize[j] = resultL[z][j + bitsize / 2];
					resultL[z][j + bitsize / 2] = tmp;
				}

				resultL[z] = tmpsize;
				//tmpsize.clear();
			}
			//K = sols.LSetToKSet(K, resultL, "simon");
			//K = sols.LSetToKSet(K, resultL, "lblock");
			//sols.Redundant(K);
			//L = sols.RedundantLset(K, resultL);

			L = resultL;
		}
	}
	else
	{
		//L = sols.RedundantLset(K, resultL);
		//L = resultL;
		//K.clear();
		L = resultL;
	}
}

CipherDescription generate_new_LBlock(int rounds, int n, int startbit, vector<int>&Qindex)
{
	CipherDescription LBlock(n);
	vector<int> size;
	int index;
	vector<int> arry;
	//vector<int> inputS_box;
	vector<int> tmp_inputS_box;
	vector<string> S_input;
	vector<string> S_output;
	vector<string> inputS_box;
	string input1, input2, copy1, copy2, target;
	int permution[] = { 2,0,3,1,6,4,7,5 };
	int Q;
	Q = startbit;
	startbit = startbit - 1;
	int startvalue;
	for (int i = 0; i < n; i++)
	{
		size.push_back(i);
	}
	for (int r = 0; r < rounds; r++)
	{
		auto maxPosition = max_element(size.begin(), size.end());
		startvalue = *maxPosition;
		index = startvalue;

		vector<int> tmpsize = size;
		for (int i = 0; i < n / 2; i++)
		{
			int input;
			if ((i - 8) % n < 0)
			{
				input = n / 2 + (i - 8) % n;
			}
			else
			{
				input = ((i - 8) % n);
			}
			//arry.push_back(input);
			size[i] = tmpsize[input];
		}


		if (r == 0)
		{
			for (int bit = 0; bit < n / 8; bit++)
			{
				if (bit <= startbit)
				{

				}
				else
				{
					inputS_box.clear();
					for (int j = 4 * bit + 32; j < 4 * bit + 36; j++)
					{
						startvalue = index;
						input1 = "s" + to_string(size[j]);
						copy1 = "c" + to_string(startvalue + 1);
						copy2 = "c" + to_string(startvalue + 2);
						LBlock.apply_mov(copy1, copy2, input1);
						size[j] = startvalue + 1;
						inputS_box.push_back("s" + to_string(startvalue + 2));
						index = startvalue + 2;

					}
					S_output.clear();
					S_output.push_back("t" + to_string(index + 1));
					S_output.push_back("t" + to_string(index + 2));
					S_output.push_back("t" + to_string(index + 3));
					S_output.push_back("t" + to_string(index + 4));
					LBlock.apply_Sbox(inputS_box, S_output, "S" + to_string(bit));//输入S盒的变量
					startvalue = index + 4;
					int rightStart = permution[bit] * 4;
					index = startvalue;

					for (int j = 0; j < 4; j++)
					{
						startvalue = index;
						input1 = "s" + to_string(size[rightStart + j]);
						input2 = S_output[j];
						target = "t" + to_string(startvalue + 1);
						LBlock.apply_xor(input1, input2, target);
						size[rightStart + j] = startvalue + 1;
						index = startvalue + 1;
					}
				}

				if (Q == bit)
				{
					int rightStart = permution[bit] * 4;
					Qindex.push_back(4 * bit + 32);
					Qindex.push_back(4 * bit + 33);
					Qindex.push_back(4 * bit + 34);
					Qindex.push_back(4 * bit + 35);
					Qindex.push_back(rightStart);
					Qindex.push_back(rightStart + 1);
					Qindex.push_back(rightStart + 2);
					Qindex.push_back(rightStart + 3);

				}
			}

		}
		else
		{
			for (int bit = 0; bit < n / 8; bit++)
			{
				inputS_box.clear();
				for (int j = 4 * bit + 32; j < 4 * bit + 36; j++)
				{
					startvalue = index;
					input1 = "s" + to_string(size[j]);
					copy1 = "c" + to_string(startvalue + 1);
					copy2 = "c" + to_string(startvalue + 2);
					LBlock.apply_mov(copy1, copy2, input1);
					size[j] = startvalue + 1;
					inputS_box.push_back("s" + to_string(startvalue + 2));
					index = startvalue + 2;

				}
				S_output.clear();
				S_output.push_back("t" + to_string(index + 1));
				S_output.push_back("t" + to_string(index + 2));
				S_output.push_back("t" + to_string(index + 3));
				S_output.push_back("t" + to_string(index + 4));
				LBlock.apply_Sbox(inputS_box, S_output, "S" + to_string(bit));//输入S盒的变量
				startvalue = index + 4;
				int rightStart = permution[bit] * 4;
				index = startvalue;
				for (int j = 0; j < 4; j++)
				{
					startvalue = index;
					input1 = "s" + to_string(size[rightStart + j]);
					input2 = S_output[j];
					target = "t" + to_string(startvalue + 1);
					LBlock.apply_xor(input1, input2, target);
					size[rightStart + j] = startvalue + 1;
					index = startvalue + 1;
				}
			}
		}

		for (int i = 0; i < n / 2; i++)
		{
			int tmp = 0;
			tmp = size[i];
			size[i] = size[i + n / 2];
			size[i + n / 2] = tmp;
		}



	}
	string ss = LBlock.descriptions;
	vector<int> sizess = size;
	LBlock.index = size;
	//simon.set_rounds(rounds);
	return LBlock;

}

CipherDescription generate_one_Lblock(int index)
{
	CipherDescription LBlock(8);
	string input1, input2, copy1, copy2, target;
	int startvalue;
	vector<string> inputS_box;
	vector<string> S_input;
	vector<string> S_output;
	startvalue = 7;
	int indexs = 7;
	vector<int> size;
	for (int i = 0; i < 8; i++)
	{
		size.push_back(i);
	}

	for (int j = 0; j < 4; j++)
	{
		startvalue = indexs;
		input1 = "s" + to_string(size[j]);
		copy1 = "c" + to_string(startvalue + 1);
		copy2 = "c" + to_string(startvalue + 2);
		LBlock.apply_mov(copy1, copy2, input1);
		size[j] = startvalue + 1;
		inputS_box.push_back("s" + to_string(startvalue + 2));
		indexs = startvalue + 2;

	}
	S_output.clear();
	S_output.push_back("t" + to_string(indexs + 1));
	S_output.push_back("t" + to_string(indexs + 2));
	S_output.push_back("t" + to_string(indexs + 3));
	S_output.push_back("t" + to_string(indexs + 4));
	LBlock.apply_Sbox(inputS_box, S_output, "S" + to_string(index));//输入S盒的变量
	startvalue = indexs + 4;
	indexs = startvalue;
	//int rightStart = permution[bit] * 4;
	for (int j = 4; j < 8; j++)
	{
		startvalue = indexs;
		input1 = "s" + to_string(size[j]);
		input2 = S_output[j - 4];
		target = "t" + to_string(startvalue + 1);
		LBlock.apply_xor(input1, input2, target);
		size[j] = startvalue + 1;
		indexs = startvalue + 1;
	}
	string ss = LBlock.descriptions;
	vector<int> sizess = size;
	LBlock.index = size;
	//simon.set_rounds(rounds);
	return LBlock;

}
