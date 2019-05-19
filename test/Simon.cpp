

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
bool SimonAndSimeckBalancedOrNot(CipherDescription cipher, vector<int> k, int conbit);
void SimonAndSimeckLAndKVectors(CipherDescription cipher, vector<vector<int>> &K, vector<vector<int>>&L, vector<int> Qindex,bool isChange,int bitsize);
int SimonAndSimeckResult(int rounds, int bitsizes, int conbit, vector<vector<int>>K, vector<vector<int>>L);
CipherDescription generate_new_simon(int rounds, int n, int startbit, vector<int>&Qindex);
CipherDescription simon_four_bit();

int CipherBalancedBit:: SimonANDSimeckBalancedBit()
{	
	SYSTEMTIME st = { 0 };
	GetLocalTime(&st);  //获取当前时间 可精确到ms
	printf("%d-%02d-%02d %02d:%02d:%02d\n",st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);
	cout << "simon" << endl;
	vector<int>Lactive_bits;
	vector<int>Kactive_bits;
	for (int j = 0; j < 32; j++)
	{
		if (j ==31)
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

	for (int i = 0; i <32; i++) {
		cout << i << ",";
		int result = SimonAndSimeckResult(19, 16, i, K, L);
		if (result ==0)
		{
			cout << "balance" << endl;
		}
		else if(result == 2)
		{
			cout << "unknown" << endl;
		}
		else {
			//cout << "odd" << endl;
		}
	}
	//截止时间	
	GetLocalTime(&st);  //获取当前时间 可精确到ms
	printf("%d-%02d-%02d %02d:%02d:%02d\n",st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);
	return 0;
	
}

int SimonAndSimeckResult(int rounds ,  int bitsizes,int conbit,vector<vector<int>>K, vector<vector<int>>L)
{
	bool isChange=false;
	vector<int> Qindex;
	vector<vector<int>> nextL;
	for (int i = 0; i <rounds; i++) 
	{	
		//cout << i << endl;
		for (int j = 0; j < bitsizes; j++) {
			Qindex.clear();   
			CipherDescription simon = generate_new_simon(rounds - i, bitsizes, j, Qindex);			
			for (int k = K.size()-1; k >=0; k--)
			{
				if (SimonAndSimeckBalancedOrNot(simon, K[k], conbit))
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
				if (SimonAndSimeckBalancedOrNot(simon, L[z], conbit))
				{
					nextL.push_back(L[z]);
				}				
			}
			if (nextL.size() == 0)
			{
				return 0;
			}
			L = nextL;
			if (j ==bitsizes-1) 
			{
				isChange = true;
			}
			else 
			{
				isChange = false;
			}
			SimonAndSimeckLAndKVectors(simon_four_bit(), K, L,Qindex,isChange, bitsizes);
			//isChange = false;
		}
	}

	if (nextL.size() != 0) 
	{
		cout << "party of bit is odd" << endl;
		for (int i = 0; i < nextL.size(); i++) {
			for (int j = 0; j < nextL[i].size(); j++)
			{
				if (j < 16) {
					cout << nextL[i][15-j] << " ";
				}
				else {
					cout << nextL[i][47- j] << " ";
				}
				
			}
			cout << endl;
		}
		return 1;
	}
}

bool SimonAndSimeckBalancedOrNot(CipherDescription cipher ,vector<int> k,int conbit)
{
	bool returnValue=false;
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

void SimonAndSimeckLAndKVectors(CipherDescription cipher, vector<vector<int>> &K,vector<vector<int>>&L, vector<int> Qindex,bool isChange,int bitsize)
{
	CipherDescription simon = cipher; //generate_simon_version(rounds, bitsizes, 8, 1, 2);
	solver sol;
	sol.load_cipher(simon);
	vector<int> nullvalue;
	CipherDescription simons = generate_new_simon(1, bitsize, 0, nullvalue);
	solver sols;
	sols.load_cipher(simons);
	vector<vector<int>> resultL;
	
	for(int i=0;i<L.size();i++)
	{
		vector<vector<int>> tmpL;
		vector<int>tmpQ;
		tmpQ.push_back(L[i][Qindex[0]]);
		tmpQ.push_back(L[i][Qindex[1]]);
		tmpQ.push_back(L[i][Qindex[2]]);
		tmpQ.push_back(L[i][Qindex[3]]);	
		tmpL=sol.LReturnAllSolu(tmpQ);
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

	if(isChange)
	{
		if (resultL.size()> 0) {
			int n = resultL[0].size()/2;
			for (int i = 0; i < resultL.size(); i++)
			{
				for (int j = 0; j < n; j++)
				{
					int tmp = 0;
					tmp = resultL[i][j];
					resultL[i][j] = resultL[i][j + n];
					resultL[i][j + n] = tmp;

				}
			}
			K = sols.LSetToKSet(K, resultL, "simon");			
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

CipherDescription generate_new_simon(int rounds, int n, int startbit,vector<int>&Qindex)
{
	CipherDescription simon(2 * n);
	vector<int> size;

	//simon或者simeck的移位数
	int a = 0, b = 1, c = 2;
	//vector<int> Qindex;
	int Q;
	int startvalue = 0;	
	string input_1, input_2, copy_1, copy_2, copy_3, copy_4, copy_5, copy_6, product, input_3, xor, xor_1, right_side;
	vector<int>arry_a;
	vector<int>arry_b;
	vector<int>arry_c;
	int index;
	Q = startbit;
	startbit = startbit - 1;
	

	for (int i = 0; i < 2 * n; i++)
	{
		size.push_back(i);
	}
	for (int i = 0; i<n; i++)
	{
		int input;
		if ((i - a) % n<0)
		{
			input = n + (i - a) % n + n;
		}
		else
		{
			input = ((i - a) % n + n);
		}
		arry_a.push_back(input);
	}
	for (int i = 0; i<n; i++)
	{
		int input;
		if ((i - b) % n<0)
		{
			input = n + (i - b) % n + n;
		}
		else
		{
			input = ((i - b) % n + n);
		}
		arry_b.push_back(input);
	}
	for (int i = 0; i<n; i++)
	{
		int input;
		if ((i - c) % n<0)
		{
			input = n + (i - c) % n + n;
		}
		else
		{
			input = ((i - c) % n + n);
		}
		arry_c.push_back(input);
	}

	for (int r = 0; r < rounds; r++) 
	{
		auto maxPosition = max_element(size.begin(), size.end());
		startvalue = *maxPosition + 1;
		
		if (r == 0) 
		{
			for (int bit = 0; bit < n; bit++) 
			{				
				if (bit <= startbit) 
				{

					input_1 = "s" + to_string(size[arry_a[bit]]);
					input_2 = "s" + to_string(size[arry_b[bit]]);

					copy_1 = "c" + to_string((startvalue));
					copy_2 = "c" + to_string((startvalue +1));
					simon.apply_mov(copy_1, copy_2, input_1);
					size[arry_a[bit]] = (startvalue + 1);

					copy_3 = "c" + to_string((startvalue  + 2));
					copy_4 = "c" + to_string((startvalue  + 3));
					simon.apply_mov(copy_3, copy_4, input_2);
					size[arry_b[bit]] = (startvalue  + 3);

					product = "t" + to_string((startvalue+ 4));
					simon.apply_and(copy_1, copy_3, product);

					//simon.apply_and(input_1, input_2, product);

					input_3 = "s" + to_string(size[arry_c[bit]]);
					copy_5 = "c" + to_string((startvalue  + 5));
					copy_6 = "c" + to_string((startvalue  + 6));
					simon.apply_mov(copy_5, copy_6, input_3);
					size[arry_c[bit]] = (startvalue + 6);

					xor = "t" + to_string((startvalue + 7));
					simon.apply_xor(product, copy_5, xor);

					right_side = "s" + to_string(size[bit]);
					//xor_1 = "t" + to_string((startvalue + 9 * i + 8));
					simon.apply_xor(xor, right_side, right_side);
					size[bit] = size[bit];
					startvalue = startvalue + 8;

				}
				else 
				{
					input_1 = "s" + to_string(size[arry_a[bit]]);
					input_2 = "s" + to_string(size[arry_b[bit]]);

					copy_1 = "c" + to_string((startvalue));
					copy_2 = "c" + to_string((startvalue + 1));
					simon.apply_mov(copy_1, copy_2, input_1);
					size[arry_a[bit]] = (startvalue + 1);

					copy_3 = "c" + to_string((startvalue + 2));
					copy_4 = "c" + to_string((startvalue + 3));
					simon.apply_mov(copy_3, copy_4, input_2);
					size[arry_b[bit]] = (startvalue + 3);

					product = "t" + to_string((startvalue + 4));
					simon.apply_and(copy_1, copy_3, product);

					//simon.apply_and(input_1, input_2, product);

					input_3 = "s" + to_string(size[arry_c[bit]]);
					copy_5 = "c" + to_string((startvalue + 5));
					copy_6 = "c" + to_string((startvalue + 6));
					simon.apply_mov(copy_5, copy_6, input_3);
					size[arry_c[bit]] = (startvalue + 6);

					xor = "t" + to_string((startvalue + 7));
					simon.apply_xor(product, copy_5, xor);

					right_side = "s" + to_string(size[bit]);
					xor_1 = "t" + to_string((startvalue + 8));
					simon.apply_xor(xor, right_side, xor_1);
					size[bit] = (startvalue + 8);
					startvalue = startvalue +9;
				}
				//记录需要进行L操作的bit位置
				if (bit == Q) 
				{
					
					Qindex.push_back(arry_a[bit]);	
					Qindex.push_back(arry_b[bit]);
					Qindex.push_back(arry_c[bit]);
					Qindex.push_back(bit);
				}
			}
		}
		else 
		{
			for (int bit = 0; bit < n; bit++) 
			{
				input_1 = "s" + to_string(size[arry_a[bit]]);
				input_2 = "s" + to_string(size[arry_b[bit]]);

				copy_1 = "c" + to_string((startvalue));
				copy_2 = "c" + to_string((startvalue + 1));
				simon.apply_mov(copy_1, copy_2, input_1);
				size[arry_a[bit]] = (startvalue + 1);

				copy_3 = "c" + to_string((startvalue + 2));
				copy_4 = "c" + to_string((startvalue + 3));
				simon.apply_mov(copy_3, copy_4, input_2);
				size[arry_b[bit]] = (startvalue + 3);

				product = "t" + to_string((startvalue + 4));
				simon.apply_and(copy_1, copy_3, product);

				//simon.apply_and(input_1, input_2, product);

				input_3 = "s" + to_string(size[arry_c[bit]]);
				copy_5 = "c" + to_string((startvalue + 5));
				copy_6 = "c" + to_string((startvalue + 6));
				simon.apply_mov(copy_5, copy_6, input_3);
				size[arry_c[bit]] = (startvalue + 6);

				xor = "t" + to_string((startvalue + 7));
				simon.apply_xor(product, copy_5, xor);

				right_side = "s" + to_string(size[bit]);
				xor_1 = "t" + to_string((startvalue + 8));
				simon.apply_xor(xor, right_side, xor_1);
				size[bit] = (startvalue + 8);
				startvalue = startvalue + 9;

			}

			
		}

		//左右交换
		
		for (int i = 0; i < n; i++)
		{
			int tmp = 0;
			tmp = size[i];
			size[i] = size[i + n];
			size[i + n] = tmp;

		}


	}
	string ss = simon.descriptions;
	vector<int> sizess = size;
	simon.index = size;
	//simon.set_rounds(rounds);
	return simon;
}
//计算四bit的运算规则
CipherDescription simon_four_bit() 
{
	CipherDescription simon(4);
	vector<int> size;
	string input1, input2, input3, copy1, copy2, copy3, copy4;
	for (int i = 0; i < 4; i++) 
	{
		size.push_back(i);
	}
	input1 = "s" + to_string(size[0]);
	copy1 = "c" + to_string(4);
	copy2 = "c" + to_string(5);
	size[0] = 4;
	simon.apply_mov(copy1,copy2,input1);

	input1 = "s" + to_string(size[1]);
	copy1 = "c" + to_string(6);
	copy3 = "c" + to_string(7);
	size[1] = 6;
	simon.apply_mov(copy1, copy3, input1);
	input2 = "s" + to_string(8);
	simon.apply_and(copy2,copy3,input2);

	input1 = "s" + to_string(size[2]);
	copy1 = "c" + to_string(9);
	copy2 = "c" + to_string(10);
	size[2] = 9;
	simon.apply_mov(copy1, copy2, input1);

	input3 = "s" + to_string(11);
	simon.apply_xor(input2,copy2,input3);

	input1 = "s" + to_string(12);
	input2 = "s" + to_string(size[3]);
	simon.apply_xor(input2, input3,input1);
	size[3] = 12;


	//size[3] = 11;

	
	simon.index = size;
	//simon.set_rounds(rounds);
	return simon;


}








