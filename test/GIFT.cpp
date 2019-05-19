
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
bool GiftBalancedOrNot(CipherDescription cipher, vector<int> k, int conbit);
void GiftLAndKVectors(CipherDescription cipher, vector<vector<int>> &K, vector<vector<int>>&L, vector<int> Qindex, bool isChange, int bitsize);
int GiftResult(int rounds, int bitsizes, int conbit, vector<vector<int>>K, vector<vector<int>>L);
CipherDescription generate_new_gift(int rounds, int n, int startbit, vector<int>&Qindex);
CipherDescription generate_oneSbox(string sname);
int CipherBalancedBit::GiftBalancedBit()
{

	//起始时间
	SYSTEMTIME st = { 0 };
	GetLocalTime(&st);  //获取当前时间 可精确到ms
	printf("%d-%02d-%02d %02d:%02d:%02d\n", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
	cout << "gift" << endl;
	vector<int>Lactive_bits;
	vector<int>Kactive_bits;
	for (int j = 0; j < 64; j++)
	{
		if (j == 1)
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
		int result = GiftResult(9, 64, i, K, L);
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

int GiftResult(int rounds, int bitsizes, int conbit, vector<vector<int>>K, vector<vector<int>>L)
{
	bool isChange = false;
	vector<int> Qindex;
	vector<vector<int>> nextL;
	for (int i = 0; i <rounds; i++)
	{
		//cout << i << endl;
		for (int j = 0; j < bitsizes / 4; j++) {
			Qindex.clear();
			//	CipherDescription simon = generate_Gift_version(11,128);
			CipherDescription retangle = generate_new_gift(rounds - i, bitsizes, j, Qindex);
			for (int k = K.size() - 1; k >= 0; k--)
			{
				if (GiftBalancedOrNot(retangle, K[k], conbit))
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
				if (GiftBalancedOrNot(retangle, L[z], conbit))
				{					
					nextL.push_back(L[z]);
				}
				
			}
			if (nextL.size() == 0)
			{
				return 0;
			}
			L = nextL;
			if (j == bitsizes / 4 - 1)
			{
				isChange = true;
			}
			else
			{
				isChange = false;
			}
			GiftLAndKVectors(generate_oneSbox("Sgift"), K, L, Qindex, isChange, bitsizes);
		}
	}

	if (nextL.size() != 0)
	{
		cout << "new dis 1" << endl;
		return 1;
	}
}


/*
true 代表balance
false 代表单位向量
*/
bool GiftBalancedOrNot(CipherDescription cipher, vector<int> k, int conbit)
{
	bool returnValue = false;
	CipherDescription ciphers = cipher; //generate_simon_version(rounds, bitsizes, 8, 1, 2);
	solver sol;
	sol.load_cipher(ciphers);
	if (sol.Balance_bit(k, conbit))
	{
		return returnValue = false;
	}
	else
	{
		return returnValue = true;
	}

}

void GiftLAndKVectors(CipherDescription cipher, vector<vector<int>> &K, vector<vector<int>>&L, vector<int> Qindex, bool isChange, int bitsize)
{
	CipherDescription cip = cipher; //generate_simon_version(rounds, bitsizes, 8, 1, 2);
	solver sol;
	sol.load_cipher(cip);
	vector<int> nullvalue;
	CipherDescription gift = generate_new_gift(1, bitsize, 0, nullvalue);
	solver sols;
	sols.load_cipher(gift);
	vector<vector<int>> resultL;

	for (int i = 0; i<L.size(); i++)
	{
		vector<vector<int>> tmpL;
		vector<int>tmpQ;
		tmpQ.push_back(L[i][Qindex[0]]);
		tmpQ.push_back(L[i][Qindex[1]]);
		tmpQ.push_back(L[i][Qindex[2]]);
		tmpQ.push_back(L[i][Qindex[3]]);
		tmpL = sol.LReturnAllSolu(tmpQ);
		vector<vector<int>> ss = tmpL;
		for (int j = 0; j < tmpL.size(); j++)
		{
			vector<vector<int>>::iterator rets;
			L[i][Qindex[0]] = tmpL[j][0];
			L[i][Qindex[1]] = tmpL[j][1];
			L[i][Qindex[2]] = tmpL[j][2];
			L[i][Qindex[3]] = tmpL[j][3];

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
		int permutation[] = { 0 ,17, 34, 51, 48, 1, 18, 35, 32, 49 ,2 ,19 ,16 ,33 ,50, 3,4 ,21, 38, 55, 52, 5, 22, 39, 36 ,53, 6, 23, 20 ,37, 54, 7, 8, 25, 42, 59, 56, 9, 26, 43, 40, 57, 10, 27, 24, 41, 58, 11,12, 29, 46 ,63 ,60, 13, 30, 47 ,44, 61, 14, 31, 28, 45, 62, 15 };
		/*int permutation[] = { 0, 33, 66, 99 ,96, 1, 34, 67, 64 ,97, 2 ,35, 32, 65, 98 ,3,
		4, 37, 70, 103, 100, 5, 38, 71, 68, 101 ,6, 39, 36, 69, 102,7,
		8, 41, 74 ,107 ,104, 9, 42, 75, 72, 105, 10 ,43, 40 ,73 ,106 ,11,
		12 ,45, 78, 111, 108 ,13 ,46, 79, 76, 109, 14, 47, 44, 77, 110, 15,
		16, 49 ,82 ,115 ,112 ,17, 50, 83, 80, 113, 18 ,51, 48, 81, 114 ,19,
		20, 53 ,86, 119, 116, 21, 54, 87, 84, 117, 22, 55, 52, 85, 118, 23,
		24, 57 ,90, 123, 120 ,25, 58, 91, 88, 121, 26, 59, 56, 89, 122, 27,
		28, 61, 94, 127, 124, 29, 62, 95, 92 ,125 ,30 ,63, 60, 93, 126, 31 };*/

		if (resultL.size()> 0) {
			vector<int>tmpsize;
			for (int z = 0; z < resultL.size(); z++)
			{

				tmpsize = resultL[z];
				for (int i = 0; i<64; i++)
				{
					resultL[z][i] = tmpsize[permutation[i]];
				}

				tmpsize.clear();
			}

			K = sols.LSetToKSet(K, resultL, "gift");
			sols.Redundant(K);
			L = sols.RedundantLset(K, resultL);
		}
	}
	else
	{
		L = sols.RedundantLset(K, resultL);
		//L = resultL;
		//K.clear();
	}
}
CipherDescription generate_new_gift(int rounds, int n, int startbit, vector<int>&Qindex)
{
	CipherDescription Gift(n);
	vector<int> size;
	vector<int> tmpsize;
	int index;
	int permutation[] = { 0 ,17, 34, 51, 48, 1, 18, 35, 32, 49 ,2 ,19 ,16 ,33 ,50, 3,4 ,21, 38, 55, 52, 5, 22, 39, 36 ,53, 6, 23, 20 ,37, 54, 7, 8, 25, 42, 59, 56, 9, 26, 43, 40, 57, 10, 27, 24, 41, 58, 11,12, 29, 46 ,63 ,60, 13, 30, 47 ,44, 61, 14, 31, 28, 45, 62, 15 };
	/*int permutation[] = { 0, 33, 66, 99 ,96, 1, 34, 67, 64 ,97, 2 ,35, 32, 65, 98 ,3,
	4, 37, 70, 103, 100, 5, 38, 71, 68, 101 ,6, 39, 36, 69, 102,7,
	8, 41, 74 ,107 ,104, 9, 42, 75, 72, 105, 10 ,43, 40 ,73 ,106 ,11,
	12 ,45, 78, 111, 108 ,13 ,46, 79, 76, 109, 14, 47, 44, 77, 110, 15,
	16, 49 ,82 ,115 ,112 ,17, 50, 83, 80, 113, 18 ,51, 48, 81, 114 ,19,
	20, 53 ,86, 119, 116, 21, 54, 87, 84, 117, 22, 55, 52, 85, 118, 23,
	24, 57 ,90, 123, 120 ,25, 58, 91, 88, 121, 26, 59, 56, 89, 122, 27,
	28, 61, 94, 127, 124, 29, 62, 95, 92 ,125 ,30 ,63, 60, 93, 126, 31 };*/

	vector<int> inputS_box;
	vector<int> tmp_inputS_box;
	vector<string> S_input;
	vector<string> S_output;
	for (int i = 0; i < n; i++)
	{
		size.push_back(i);
	}

	int startvalue = 0;
	int Q;
	Q = startbit;
	startbit = startbit - 1;
	for (int r = 0; r < rounds; r++)
	{
		auto maxPosition = max_element(size.begin(), size.end());
		startvalue = *maxPosition;
		index = startvalue;
		if (r == 0)
		{
			for (int bit = 0; bit < n / 4; bit++)
			{
				if (bit <= startbit)
				{
					//	S_input.clear();
					//	S_output.clear();
					//	S_input.push_back("s" + to_string(size[4 * bit]));
					//	S_input.push_back("s" + to_string(size[4 * bit + 1]));
					//	S_input.push_back("s" + to_string(size[4 * bit + 2]));
					//	S_input.push_back("s" + to_string(size[4 * bit + 3]));

					//	S_output.push_back("t" + to_string(size[4 * bit]));
					//	S_output.push_back("t" + to_string(size[4 * bit + 1]));
					//	S_output.push_back("t" + to_string(size[4 * bit + 2]));
					//	S_output.push_back("t" + to_string(size[4 * bit + 3]));
					//	Gift.apply_Sbox(S_input, S_output, "Sgift");//输入S盒的变量		
					//	
					//	index = startvalue;

				}
				else
				{
					startvalue = index;
					S_input.clear();
					S_output.clear();
					S_input.push_back("s" + to_string(size[4 * bit]));
					S_input.push_back("s" + to_string(size[4 * bit + 1]));
					S_input.push_back("s" + to_string(size[4 * bit + 2]));
					S_input.push_back("s" + to_string(size[4 * bit + 3]));

					S_output.push_back("t" + to_string(startvalue + 1));
					S_output.push_back("t" + to_string(startvalue + 2));
					S_output.push_back("t" + to_string(startvalue + 3));
					S_output.push_back("t" + to_string(startvalue + 4));
					Gift.apply_Sbox(S_input, S_output, "Sgift");//输入S盒的变量


					size[4 * bit] = startvalue + 1;
					size[4 * bit + 1] = startvalue + 2;
					size[4 * bit + 2] = startvalue + 3;
					size[4 * bit + 3] = startvalue + 4;

					index = startvalue + 4;
				}

				if (bit == Q)
				{
					Qindex.push_back(4 * Q);
					Qindex.push_back(4 * Q + 1);
					Qindex.push_back(4 * Q + 2);
					Qindex.push_back(4 * Q + 3);
				}
			}

		}
		else
		{

			for (int bit = 0; bit < n / 4; bit++)
			{
				startvalue = index;
				S_input.clear();
				S_output.clear();
				S_input.push_back("s" + to_string(size[4 * bit]));
				S_input.push_back("s" + to_string(size[4 * bit + 1]));
				S_input.push_back("s" + to_string(size[4 * bit + 2]));
				S_input.push_back("s" + to_string(size[4 * bit + 3]));

				S_output.push_back("t" + to_string(startvalue + 1));
				S_output.push_back("t" + to_string(startvalue + 2));
				S_output.push_back("t" + to_string(startvalue + 3));
				S_output.push_back("t" + to_string(startvalue + 4));
				Gift.apply_Sbox(S_input, S_output, "Sgift");//输入S盒的变量


				size[4 * bit] = startvalue + 1;
				size[4 * bit + 1] = startvalue + 2;
				size[4 * bit + 2] = startvalue + 3;
				size[4 * bit + 3] = startvalue + 4;

				index = startvalue + 4;
			}

		}

		int tmpn = 0;
		tmpsize = size;
		for (int i = 0; i<n; i++)
		{
			//cout << n << endl;
			//tmpsize[i] = size[permutation[i]];
			size[i] = tmpsize[permutation[i]];
		}

		//size = tmpsize;
		tmpsize.clear();

	}

	string ss = Gift.descriptions;
	//vector<int> sizess = size;
	Gift.index = size;
	//simon.set_rounds(rounds);
	return Gift;


}
CipherDescription generate_oneSbox(string sname)
{
	CipherDescription ciper(4);
	vector<int> size;
	string input1, input2, input3, copy1, copy2, copy3, copy4;
	for (int i = 0; i < 4; i++)
	{
		size.push_back(i);
	}
	vector<string> S_input;
	vector<string> S_output;
	//startvalue = index;
	S_input.push_back("s" + to_string(size[0]));
	S_input.push_back("s" + to_string(size[1]));
	S_input.push_back("s" + to_string(size[2]));
	S_input.push_back("s" + to_string(size[3]));

	S_output.push_back("t" + to_string(4));
	S_output.push_back("t" + to_string(5));
	S_output.push_back("t" + to_string(6));
	S_output.push_back("t" + to_string(7));
	ciper.apply_Sbox(S_input, S_output, sname);//输入S盒的变量


	size[0] = 4;
	size[1] = 5;
	size[2] = 6;
	size[3] = 7;
	S_input.clear();
	S_output.clear();
	string ss = ciper.descriptions;
	vector<int> sizess = size;
	ciper.index = size;
	//simon.set_rounds(rounds);
	return ciper;
}

