
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
bool KhudraBalancedOrNot(CipherDescription cipher, vector<int> k, int conbit);
void KhudraLAndKVectors(CipherDescription cipher, vector<vector<int>> &K, vector<vector<int>>&L, vector<int> Qindex, bool isChange, int bitsize);
int KhudraResult(int rounds, int bitsizes, int conbit, vector<vector<int>>K, vector<vector<int>>L);
CipherDescription generate_new_khudra(int rounds, int n, int startbit, vector<int>&Qindex);
CipherDescription generate_onepart_khudra();
vector<int> KhudraFunction(CipherDescription &khudra, vector<int>  input, int &start);

int CipherBalancedBit:: KhudraBalancedBit() {
	SYSTEMTIME st = { 0 };
	GetLocalTime(&st);  //获取当前时间 可精确到ms
	printf("%d-%02d-%02d %02d:%02d:%02d\n", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
	cout << "khudra" << endl;
	vector<int>Lactive_bits;
	vector<int>Kactive_bits;
	for (int j = 0; j < 64; j++)
	{
		if (j == 63)
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

	for (int i = 62; i < 64; i++) {
		cout << i << ",";
		int result = KhudraResult(9, 64, i, K, L);
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

int KhudraResult(int rounds, int bitsizes, int conbit, vector<vector<int>>K, vector<vector<int>>L)
{
	bool  isChange = false;
	vector<int> Qindex;
	vector<vector<int>> nextL;
	for (int i = 0; i <rounds; i++)
	{
		//cout << i << endl;
		for (int j = 0; j < 2; j++) {
			Qindex.clear();
			CipherDescription khudra = generate_new_khudra(rounds - i, bitsizes, j, Qindex);
			for (int k = K.size() - 1; k >= 0; k--)
			{
				if (KhudraBalancedOrNot(khudra, K[k], conbit))
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
				if (KhudraBalancedOrNot(khudra, L[z], conbit))
				{					
					nextL.push_back(L[z]);
				}				
			}
			if (nextL.size() == 0)
			{
				return 0;
			}
			L = nextL;
			if (j == 1 || j == 0)
			{
				isChange = true;
			}
			else
			{
				isChange = false;
			}
			KhudraLAndKVectors(generate_onepart_khudra(), K, L, Qindex, isChange, bitsizes);
			//isChange = false;
		}
	}

	if (nextL.size() != 0)
	{
		return 1;
	}
}

bool KhudraBalancedOrNot(CipherDescription cipher, vector<int> k, int conbit)
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

void KhudraLAndKVectors(CipherDescription cipher, vector<vector<int>> &K, vector<vector<int>>&L, vector<int> Qindex, bool isChange, int bitsize)
{
	string khudra1 = "0";
	string khudra2 = "0";
	if (Qindex[0] == 0) { khudra1 = "1"; khudra2 = "0"; }
	if (Qindex[0] == 32) { khudra1 = "0"; khudra2 = "1"; }
	CipherDescription simon = cipher; //generate_simon_version(rounds, bitsizes, 8, 1, 2);
	solver sol;
	sol.load_cipher(simon);
	vector<int> nullvalue;
	CipherDescription khudras = generate_new_khudra(1, bitsize, 0, nullvalue);
	solver sols;
	sols.load_cipher(khudras);
	vector<vector<int>> resultL;

	for (int i = 0; i<L.size(); i++)
	{
		vector<vector<int>> tmpL;
		vector<int>tmpQ;
		for (int in = 0; in < Qindex.size(); in++) {
			tmpQ.push_back(L[i][Qindex[in]]);
		}

		//tmpQ.push_back(L[i][Qindex[1]]);
		//tmpQ.push_back(L[i][Qindex[2]]);
		//tmpQ.push_back(L[i][Qindex[3]]);

		tmpL = sol.khudraReturnAllSolu(tmpQ);
		vector<vector<int>> ss = tmpL;
		for (int j = 0; j < tmpL.size(); j++)
		{
			vector<vector<int>>::iterator rets;
			for (int ins = 0; ins < Qindex.size(); ins++) {
				L[i][Qindex[ins]] = tmpL[j][ins];
			}

			////L[i][Qindex[1]] = tmpL[j][1];
			//L[i][Qindex[2]] = tmpL[j][2];
			//L[i][Qindex[3]] = tmpL[j][3];

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
		isChange = false;
		if (resultL.size()> 0) {
			//int n = resultL[0].size()/2;
			//vector<vector<int>> ss = resultL;
			for (int i = 0; i < resultL.size(); i++)
			{
				vector<int> tmpsize;
				int tmpn = 0;

				tmpsize = resultL[i];

				//tmpsize = size;
				if (khudra2 == "1")
					for (int a = 0; a<64; a++)
					{
						int in;
						if ((a - 16) % 64<0)
						{
							in = 64 + (a - 16) % 64;
						}
						else
						{
							in = ((a - 16) % 64);
						}
						resultL[i][a] = tmpsize[in];
					}

			}
			//K = sols.LSetToKSet(K, resultL, "simon");
			if (khudra1 == "1") {
				K = sols.LSetToKSet(K, resultL, "khudra1");
				sols.Redundant(K);
				L = sols.RedundantLset(K, resultL);
			}
			else {
				K = sols.LSetToKSet(K, resultL, "khudra2");
				sols.Redundant(K);
				L = sols.RedundantLset(K, resultL);
			}
		}
	}
	else
	{
		L = sols.RedundantLset(K, resultL);
		//L = resultL;
		//K.clear();
	}
}

CipherDescription generate_onepart_khudra() {
	CipherDescription khudra(32);
	vector<int> size, tmpsize;
	vector<int>L1, L2, tmpL1, tmpL2;
	int startvalue, tmpindex;
	//vector<int> vec_a, vec_b, vec_c, vec_d, vec_f, vec_e;
	//vector<int> output_a, output_b, output_c, right_mod;;
	string input1, input2, input3, copy1, copy2, copy3, copy4, copy5, copy6, target, target1, target2, target3;
	vector<int>input_one_F;
	vector<int>S_input;
	vector<int>S_output;
	vector<string>tmps_input;
	vector<string>tmps_onput;
	//vector<int>
	for (int i = 0; i <32; i++)
	{
		size.push_back(i);
	}
	auto maxPosition = max_element(size.begin(), size.end());
	startvalue = *maxPosition;
	input_one_F.clear();

	for (int i = 16; i < 32; i++) {
		input1 = "s" + to_string(size[i]);
		copy1 = "c" + to_string(startvalue + 2 * (i - 16) + 1);
		copy2 = "c" + to_string(startvalue + 2 * (i - 16) + 2);
		khudra.apply_mov(copy1, copy2, input1);
		size[i] = startvalue + 2 * (i - 16) + 1;
		input_one_F.push_back(startvalue + 2 * i + 2);
		tmpindex = startvalue + 2 * (i - 16) + 2;
	}
	vector<int> ff = KhudraFunction(khudra, input_one_F, tmpindex);
	input_one_F = ff;
	startvalue = tmpindex;
	//	vector<int> gg = size;
	for (int i = 0; i < 16; i++)
	{
		input1 = "s" + to_string(size[i]);
		target = "t" + to_string(startvalue + i + 1);
		size[i] = startvalue + i + 1;
		khudra.apply_xor(input1, "c" + to_string(input_one_F[i]), target);
	}
	string ss = khudra.descriptions;
	vector<int> sizess = size;
	khudra.index = size;
	//katan.constantindex = tmpsize;
	return khudra;
}

CipherDescription generate_new_khudra(int rounds, int n, int startbit, vector<int>&Qindex) {
	CipherDescription khudra(n);
	vector<int> size, tmpsize;
	vector<int>L1, L2, tmpL1, tmpL2;
	int startvalue, tmpindex;
	//vector<int> vec_a, vec_b, vec_c, vec_d, vec_f, vec_e;
	//vector<int> output_a, output_b, output_c, right_mod;;
	string input1, input2, input3, copy1, copy2, copy3, copy4, copy5, copy6, target, target1, target2, target3;
	vector<int>input_one_F;
	vector<int>input_two_F;
	vector<int>S_input;
	vector<int>S_output;
	vector<string>tmps_input;
	vector<string>tmps_onput;
	int index;
	//vector<int>
	for (int i = 0; i <n; i++)
	{
		size.push_back(i);
	}
	int Q;
	Q = startbit;
	startbit = startbit - 1;

	for (int r = 0; r < rounds; r++) {
		auto maxPosition = max_element(size.begin(), size.end());
		startvalue = *maxPosition;
		index = startvalue;
		input_one_F.clear();
		input_two_F.clear();
		if (r == 0) {

			if (startbit == 0) {
				for (int i = 0; i < n / 4; i++) {

					input1 = "s" + to_string(size[i + 48]);
					copy1 = "c" + to_string(startvalue + 2 * i + 1);
					copy2 = "c" + to_string(startvalue + 2 * i + 2);
					khudra.apply_mov(copy1, copy2, input1);
					size[i + 48] = startvalue + 2 * i + 1;
					input_two_F.push_back(startvalue + 2 * i + 2);
					tmpindex = startvalue + 2 * i + 2;

				}
				vector<int> ff = KhudraFunction(khudra, input_two_F, tmpindex);
				input_two_F = ff;
				startvalue = tmpindex;
				for (int i = 0; i < n / 4; i++) {
					input1 = "s" + to_string(size[i + 32]);
					target = "t" + to_string(startvalue + i + 1);
					size[i + 32] = startvalue + i + 1;
					index = startvalue + i + 1;
					khudra.apply_xor(input1, "c" + to_string(input_two_F[i]), target);
				}

				for (int i = 0; i < 32; i++) {
					Qindex.push_back(32 + i);
				}

			}
			else {

				for (int i = 0; i < n / 4; i++)
				{
					input1 = "s" + to_string(size[i + 16]);
					copy1 = "c" + to_string(startvalue + 4 * i + 1);
					copy2 = "c" + to_string(startvalue + 4 * i + 2);
					khudra.apply_mov(copy1, copy2, input1);
					size[i + 16] = startvalue + 4 * i + 1;
					input_one_F.push_back(startvalue + 4 * i + 2);

					input1 = "s" + to_string(size[i + 48]);
					copy1 = "c" + to_string(startvalue + 4 * i + 3);
					copy2 = "c" + to_string(startvalue + 4 * i + 4);
					khudra.apply_mov(copy1, copy2, input1);
					size[i + 48] = startvalue + 4 * i + 3;
					input_two_F.push_back(startvalue + 4 * i + 4);
					tmpindex = startvalue + 4 * i + 4;
				}
				//右半边F函数
				vector<int> ff = KhudraFunction(khudra, input_one_F, tmpindex);

				input_one_F = ff;
				//左半边F函数
				vector<int> ee = KhudraFunction(khudra, input_two_F, tmpindex);
				input_two_F = ee;
				startvalue = tmpindex;

				//	vector<int> gg = size;
				for (int i = 0; i < 16; i++)
				{
					input1 = "s" + to_string(size[i]);
					target = "t" + to_string(startvalue + 2 * i + 1);
					size[i] = startvalue + 2 * i + 1;
					khudra.apply_xor(input1, "c" + to_string(input_one_F[i]), target);

					input1 = "s" + to_string(size[i + 32]);
					target = "t" + to_string(startvalue + 2 * i + 2);
					size[i + 32] = startvalue + 2 * i + 2;
					index = startvalue + 2 * i + 2;
					khudra.apply_xor(input1, "c" + to_string(input_two_F[i]), target);

				}

				for (int i = 0; i < 32; i++) {
					Qindex.push_back(i);
				}

			}

		}
		else {
			startvalue = index;
			for (int i = 0; i < n / 4; i++)
			{
				input1 = "s" + to_string(size[i + 16]);
				copy1 = "c" + to_string(startvalue + 4 * i + 1);
				copy2 = "c" + to_string(startvalue + 4 * i + 2);
				khudra.apply_mov(copy1, copy2, input1);
				size[i + 16] = startvalue + 4 * i + 1;
				input_one_F.push_back(startvalue + 4 * i + 2);

				input1 = "s" + to_string(size[i + 48]);
				copy1 = "c" + to_string(startvalue + 4 * i + 3);
				copy2 = "c" + to_string(startvalue + 4 * i + 4);
				khudra.apply_mov(copy1, copy2, input1);
				size[i + 48] = startvalue + 4 * i + 3;
				input_two_F.push_back(startvalue + 4 * i + 4);
				tmpindex = startvalue + 4 * i + 4;
			}
			//右半边F函数
			vector<int> ff = KhudraFunction(khudra, input_one_F, tmpindex);

			input_one_F = ff;
			//左半边F函数
			vector<int> ee = KhudraFunction(khudra, input_two_F, tmpindex);
			input_two_F = ee;
			startvalue = tmpindex;
			//	vector<int> gg = size;
			for (int i = 0; i < 16; i++)
			{
				input1 = "s" + to_string(size[i]);
				target = "t" + to_string(startvalue + 2 * i + 1);
				size[i] = startvalue + 2 * i + 1;
				khudra.apply_xor(input1, "c" + to_string(input_one_F[i]), target);

				input1 = "s" + to_string(size[i + 32]);
				target = "t" + to_string(startvalue + 2 * i + 2);
				size[i + 32] = startvalue + 2 * i + 2;
				khudra.apply_xor(input1, "c" + to_string(input_two_F[i]), target);
			}

		}
		tmpsize = size;
		for (int a = 0; a<64; a++)
		{
			int in;
			if ((a - 16) % 64<0)
			{
				in = 64 + (a - 16) % 64;
			}
			else
			{
				in = ((a - 16) % 64);
			}
			size[a] = tmpsize[in];
		}
	}
	string ss = khudra.descriptions;
	vector<int> sizess = size;
	khudra.index = size;
	//katan.constantindex = tmpsize;
	return khudra;
}
vector<int> KhudraFunction(CipherDescription &khudra, vector<int>  input, int &start)
{
	vector<int> result;
	int startvalue;
	string input1, input2, input3, copy1, copy2, copy3, copy4, copy5, copy6, target, target1, target2, target3;
	vector<string>tmps_input;
	vector<string>tmps_onput;
	vector<int>S_input;
	vector<int> tmpsize;
	vector<string> tmpSboxsize;
	for (int minr = 0; minr < 6; minr++)
	{
		startvalue = start;
		S_input.clear();
		for (int f = 0; f < input.size() / 4; f++)
		{
			input1 = "s" + to_string(input[f + 4]);
			copy1 = "c" + to_string(startvalue + 2 * f + 1);
			copy2 = "c" + to_string(startvalue + 2 * f + 2);
			khudra.apply_mov(copy1, copy2, input1);
			input[f + 4] = startvalue + 2 * f + 1;
			S_input.push_back(startvalue + 2 * f + 2);
			start = startvalue + 2 * f + 2;
		}
		vector<int>hh = S_input;
		startvalue = start;
		for (int f = 0; f < input.size() / 4; f++)
		{
			input1 = "s" + to_string(input[f + 12]);
			copy1 = "c" + to_string(startvalue + 2 * f + 1);
			copy2 = "c" + to_string(startvalue + 2 * f + 2);
			khudra.apply_mov(copy1, copy2, input1);
			input[f + 12] = startvalue + 2 * f + 1;
			S_input.push_back(startvalue + 2 * f + 2);
			start = startvalue + 2 * f + 2;
		}
		startvalue = start;
		tmpSboxsize.clear();
		for (int j = 0; j < 2; j++)
		{
			tmps_input.clear();
			tmps_onput.clear();
			tmps_input.push_back("s" + to_string(S_input[4 * j]));
			tmps_input.push_back("s" + to_string(S_input[4 * j + 1]));
			tmps_input.push_back("s" + to_string(S_input[4 * j + 2]));
			tmps_input.push_back("s" + to_string(S_input[4 * j + 3]));

			tmps_onput.push_back("t" + to_string(4 * j + startvalue + 1));
			tmps_onput.push_back("t" + to_string(4 * j + startvalue + 2));
			tmps_onput.push_back("t" + to_string(4 * j + startvalue + 3));
			tmps_onput.push_back("t" + to_string(4 * j + startvalue + 4));
			tmpSboxsize.push_back("t" + to_string(4 * j + startvalue + 1));
			tmpSboxsize.push_back("t" + to_string(4 * j + startvalue + 2));
			tmpSboxsize.push_back("t" + to_string(4 * j + startvalue + 3));
			tmpSboxsize.push_back("t" + to_string(4 * j + startvalue + 4));
			khudra.apply_Sbox(tmps_input, tmps_onput, "Spresent");//输入S盒的变量
			start = 4 * j + startvalue + 4;
		}

		startvalue = start;
		tmps_onput = tmpSboxsize;
		for (int s = 0; s < 4; s++)
		{
			input1 = "s" + to_string(input[s]);
			input2 = tmps_onput[s];
			target = "t" + to_string(startvalue + 1 + 2 * s);
			input[s] = startvalue + 1 + 2 * s;
			khudra.apply_xor(input1, input2, target);

			input1 = "s" + to_string(input[s + 8]);
			input2 = tmps_onput[s + 4];
			target = "t" + to_string(startvalue + 2 + 2 * s);
			input[s + 8] = startvalue + 2 + 2 * s;
			khudra.apply_xor(input1, input2, target);
			start = startvalue + 2 + 2 * s;
		}


		tmpsize = input;
		for (int a = 0; a<16; a++)
		{
			int in;
			if ((a - 4) % 16<0)
			{
				in = 16 + (a - 4) % 16;
			}
			else
			{
				in = ((a - 4) % 16);
			}
			input[a] = tmpsize[in];
		}



	}
	return result = input;
}
