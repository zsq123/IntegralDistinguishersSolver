#include"Sbox.h"

sbox::sbox() 
{

}

void sbox:: RedeceMono(vector<string> &mon)
{
	sort(mon.begin(), mon.end());
	int i;
	for (i = mon.size() - 1; i > 0; i--)
	{
		if (mon[i] == mon[i - 1])
		{
			mon.erase(mon.begin() + i);
			mon.erase(mon.begin() + i - 1);
			i = mon.size();
		}

	}

}

vector<string> sbox:: Monomials(vector<vector<int>> S)
{
	vector<string> monomial;
	vector<string> result;
	vector<string> Re;
	string f;
	vector<string> x0;
	vector<string> x1;
	vector<string> x2;
	vector<string> x3;
	for (int i = 0; i < S.size(); i++)
	{
		//for (int j = 0; j < S[0].size(); j++)
		//{

		if (S[i][3] == 1)
		{
			x0.push_back("x0");
		}
		else
		{
			x0.push_back("1");
			x0.push_back("x0");

		}
		if (S[i][2] == 1)
		{
			x1.push_back("x1");
		}
		else
		{
			x1.push_back("1");
			x1.push_back("x1");
		}
		if (S[i][1] == 1)
		{
			x2.push_back("x2");
		}
		else
		{
			x2.push_back("1");
			x2.push_back("x2");
		}
		if (S[i][0] == 1)
		{
			x3.push_back("x3");
		}
		else
		{
			x3.push_back("1");
			x3.push_back("x3");
		}

		for (int i = 0; i < x0.size(); i++)
		{
			result.push_back(x0[i]);
		}
		monomial = result;
		result.clear();

		for (int i = 0; i < x1.size(); i++)
		{
			for (int j = 0; j<monomial.size(); j++)
			{
				if (x1[i] == "1")
				{
					result.push_back(monomial[j]);
				}
				else
				{
					if (monomial[j] == "1")
					{
						result.push_back(x1[i]);
					}
					else
					{
						result.push_back(monomial[j]+x1[i]);
					}
				}
			}
		}

		monomial = result;
		result.clear();

		for (int i = 0; i < x2.size(); i++)
		{
			for (int j = 0; j<monomial.size(); j++)
			{
				if (x2[i] == "1")
				{
					result.push_back(monomial[j]);
				}
				else
				{
					if (monomial[j] == "1")
					{
						result.push_back(x2[i]);
					}
					else
					{
						result.push_back(monomial[j]+x2[i]);
					}
				}
			}
		}

		monomial = result;
		result.clear();

		for (int i = 0; i < x3.size(); i++)
		{
			for (int j = 0; j<monomial.size(); j++)
			{
				if (x3[i] == "1")
				{
					result.push_back(monomial[j]);
				}
				else
				{
					if (monomial[j] == "1")
					{
						result.push_back(x3[i]);
					}
					else
					{
						result.push_back(monomial[j]+x3[i] );
					}
				}
			}
		}
		for (int j = 0; j < result.size(); j++)
		{
			Re.push_back(result[j]);
		}
		monomial.clear();
		result.clear();
		x0.clear();
		x1.clear();
		x2.clear();
		x3.clear();
	}
	return Re;
}

vector<vector<int>> sbox:: fullPerm_01(int num)
{
	vector<int> tmp;
	vector<vector<int>> alltmp;
	if (num < 1)
		return alltmp;

	int tol = 1;
	for (int i = num; i >= 0; i--)
		tol = tol << 1;
	tol = tol - 2;
	for (int i = tol >> 1; i <= tol; i++) {
		int p = i;
		for (int j = 0; j < num; j++) {
			tmp.push_back((p % 2 == 0) ? 1 : 0);
			p = (p - 1) >> 1;
		}
		alltmp.push_back(tmp);
		tmp.clear();

	}

	return alltmp;
}


vector<vector<string>> sbox:: getANF(int sbox[])
{
//	vector<string>tmp;
	vector<vector<string>> result;

	vector<vector<int>> ss;
	vector<string> mon;
	vector<vector<int>> alltmp = fullPerm_01(4);
	sort(alltmp.begin(), alltmp.end());
	vector<vector<int>> trueValue;
	vector<int> tV;
	string bin;
	for (int i = 0; i < 16; i++)
	{
		tV.push_back(0);
		tV.push_back(0);
		tV.push_back(0);
		tV.push_back(0);
		int j = 0;
		while (sbox[i])
		{
			tV[j] = (sbox[i] % 2);
			sbox[i] /= 2;
			j++;
		}
		trueValue.push_back(tV);
		tV.clear();

	}

	for (int i = 0; i < trueValue.size(); i++)
	{
		if (trueValue[i][0] == 1)
		{
			ss.push_back(alltmp[i]);
		}
	}
	mon = Monomials(ss);

	RedeceMono(mon);
	//cout << "y0=";
	/*for (int i = 0; i < mon.size(); i++)
	{
		cout << mon[i] << "+";
	}
	cout << endl;*/
	result.push_back(mon);
	ss.clear();

	for (int i = 0; i < trueValue.size(); i++)
	{
		if (trueValue[i][1] == 1)
		{
			ss.push_back(alltmp[i]);
		}
	}
	mon = Monomials(ss);

	RedeceMono(mon);
	result.push_back(mon);
	//cout << "y1=";
	/*for (int i = 0; i < mon.size(); i++)
	{
		cout << mon[i] << "+";
	}
	cout << endl;*/
	ss.clear();

	for (int i = 0; i < trueValue.size(); i++)
	{
		if (trueValue[i][2] == 1)
		{
			ss.push_back(alltmp[i]);
		}
	}
	mon = Monomials(ss);

	RedeceMono(mon);
	result.push_back(mon);
	/*cout << "y2=";
	for (int i = 0; i < mon.size(); i++)
	{
		cout << mon[i] << "+";
	}
	cout << endl;*/
	ss.clear();
	for (int i = 0; i < trueValue.size(); i++)
	{
		if (trueValue[i][3] == 1)
		{
			ss.push_back(alltmp[i]);
		}
	}
	mon = Monomials(ss);

	RedeceMono(mon);
	result.push_back(mon);
	/*cout << "y3=";
	for (int i = 0; i < mon.size(); i++)
	{
		cout << mon[i] << "+";
	}
	cout << endl;*/
	ss.clear();

	return result;
}

vector<vector<int>>sbox:: S_box(vector<string>y0, vector<string>y1, vector<string>y2, vector<string>y3, vector<int> inputK)
{
	bool my0 = false, my1 = false, my2 = false, my3 = false;
	vector<vector<int>> result;
	vector<vector<int>> alltmp = fullPerm_01(4);
	vector<vector<int>> S = S_vector(inputK);


	vector<string> monomial = Monomial(S);

	vector<string> monoMiddResult;
	vector<string> monoResult;

	for (int i = 0; i < alltmp.size(); i++)
	{

		if (alltmp[i][0] == 1)
		{
			my0 = true;
		}
		if (alltmp[i][1] == 1)
		{
			my1 = true;
		}
		if (alltmp[i][2] == 1)
		{
			my2 = true;
		}
		if (alltmp[i][3] == 1)
		{
			my3 = true;
		}

		if (my0 != true && my1 != true && my2 != true && my3 != true)
		{
			monoResult.push_back("1");
		}
		if (my0)
		{
			for (int i = 0; i < y3.size(); i++)
			{
				monoMiddResult.push_back(y3[i]);
			}
			monoResult = monoMiddResult;
		}
		if (my1)
		{
			monoMiddResult = monoResult;
			monoResult.clear();
			if (monoMiddResult.size() == 0)
			{
				for (int i = 0; i < y2.size(); i++)
				{
					monoMiddResult.push_back(y2[i]);
				}
				monoResult = monoMiddResult;
			}
			else
			{
				//monoResult.clear();
				for (int i = 0; i < y2.size(); i++)
				{
					for (int j = 0; j < monoMiddResult.size(); j++)
					{
						if (y2[i] == "1")
						{
							monoResult.push_back(monoMiddResult[j]);
						}
						else
						{
							if (monoMiddResult[j] == "1")
							{
								monoResult.push_back(y2[i]);
							}
							else
							{
								monoResult.push_back(arryTmp(y2[i], monoMiddResult[j]));
							}
						}

					}
				}
			}
		}
		if (my2)
		{
			monoMiddResult = monoResult;
			monoResult.clear();

			if (monoMiddResult.size() == 0)
			{
				for (int i = 0; i < y1.size(); i++)
				{
					monoMiddResult.push_back(y1[i]);
				}
				monoResult = monoMiddResult;
			}
			else
			{
				for (int i = 0; i < y1.size(); i++)
				{
					for (int j = 0; j < monoMiddResult.size(); j++)
					{
						if (y1[i] == "1")
						{
							monoResult.push_back(monoMiddResult[j]);
						}
						else
						{
							if (monoMiddResult[j] == "1")
							{
								monoResult.push_back(y1[i]);
							}
							else
							{
								monoResult.push_back(arryTmp(y1[i], monoMiddResult[j]));
							}
						}
					}
				}
			}
		}
		if (my3)
		{
			monoMiddResult = monoResult;
			monoResult.clear();
			if (monoMiddResult.size() == 0)
			{
				for (int i = 0; i < y0.size(); i++)
				{
					monoMiddResult.push_back(y0[i]);
				}
				monoResult = monoMiddResult;
			}
			else
			{
				for (int i = 0; i < y0.size(); i++)
				{
					for (int j = 0; j < monoMiddResult.size(); j++)
					{
						if (y0[i] == "1")
						{
							monoResult.push_back(monoMiddResult[j]);
						}
						else
						{
							if (monoMiddResult[j] == "1")
							{
								monoResult.push_back(y0[i]);
							}
							else
							{
								monoResult.push_back(arryTmp(y0[i], monoMiddResult[j]));
							}

						}
					}
				}
			}
		}


		vector<string> ss = monoResult;
		RedeceMono(monoResult);
		for (int z = 0; z < monoResult.size(); z++)
		{
			bool br = false;
			for (int y = 0; y < monomial.size(); y++)
			{
				if (monoResult[z] == monomial[y])
				{
					result.push_back(alltmp[i]);
					br = true;
					break;
				}
			}
			if (br) break;
		}

		monoResult.clear();
		monoMiddResult.clear();
		my0 = false;
		my1 = false;
		my2 = false;
		my3 = false;


	}
	return Redundant(result);

}

vector<vector<int>>sbox:: LS_box(vector<string>y0, vector<string>y1, vector<string>y2, vector<string>y3, vector<int> inputK)
{
	bool my0 = false, my1 = false, my2 = false, my3 = false;
	vector<vector<int>> result;
	vector<vector<int>> alltmp = fullPerm_01(4);
	vector<vector<int>> S;
	S.push_back(inputK);

	vector<string> monomial = Monomial(S);

	vector<string> monoMiddResult;
	vector<string> monoResult;

	for (int i = 0; i < alltmp.size(); i++)
	{

		if (alltmp[i][0] == 1)
		{
			my0 = true;
		}
		if (alltmp[i][1] == 1)
		{
			my1 = true;
		}
		if (alltmp[i][2] == 1)
		{
			my2 = true;
		}
		if (alltmp[i][3] == 1)
		{
			my3 = true;
		}

		if (my0 != true && my1 != true && my2 != true && my3 != true)
		{
			monoResult.push_back("1");
		}
		if (my0)
		{
			for (int i = 0; i < y3.size(); i++)
			{
				monoMiddResult.push_back(y3[i]);
			}
			monoResult = monoMiddResult;
		}
		if (my1)
		{
			monoMiddResult = monoResult;
			monoResult.clear();
			if (monoMiddResult.size() == 0)
			{
				for (int i = 0; i < y2.size(); i++)
				{
					monoMiddResult.push_back(y2[i]);
				}
				monoResult = monoMiddResult;
			}
			else
			{
				//monoResult.clear();
				for (int i = 0; i < y2.size(); i++)
				{
					for (int j = 0; j < monoMiddResult.size(); j++)
					{
						if (y2[i] == "1")
						{
							monoResult.push_back(monoMiddResult[j]);
						}
						else
						{
							if (monoMiddResult[j] == "1")
							{
								monoResult.push_back(y2[i]);
							}
							else
							{
								monoResult.push_back(arryTmp(y2[i], monoMiddResult[j]));
							}
						}

					}
				}
			}
		}
		if (my2)
		{
			monoMiddResult = monoResult;
			monoResult.clear();

			if (monoMiddResult.size() == 0)
			{
				for (int i = 0; i < y1.size(); i++)
				{
					monoMiddResult.push_back(y1[i]);
				}
				monoResult = monoMiddResult;
			}
			else
			{
				for (int i = 0; i < y1.size(); i++)
				{
					for (int j = 0; j < monoMiddResult.size(); j++)
					{
						if (y1[i] == "1")
						{
							monoResult.push_back(monoMiddResult[j]);
						}
						else
						{
							if (monoMiddResult[j] == "1")
							{
								monoResult.push_back(y1[i]);
							}
							else
							{
								monoResult.push_back(arryTmp(y1[i], monoMiddResult[j]));
							}
						}
					}
				}
			}
		}
		if (my3)
		{
			monoMiddResult = monoResult;
			monoResult.clear();
			if (monoMiddResult.size() == 0)
			{
				for (int i = 0; i < y0.size(); i++)
				{
					monoMiddResult.push_back(y0[i]);
				}
				monoResult = monoMiddResult;
			}
			else
			{
				for (int i = 0; i < y0.size(); i++)
				{
					for (int j = 0; j < monoMiddResult.size(); j++)
					{
						if (y0[i] == "1")
						{
							monoResult.push_back(monoMiddResult[j]);
						}
						else
						{
							if (monoMiddResult[j] == "1")
							{
								monoResult.push_back(y0[i]);
							}
							else
							{
								monoResult.push_back(arryTmp(y0[i], monoMiddResult[j]));
							}

						}
					}
				}
			}
		}


		vector<string> ss = monoResult;
		RedeceMono(monoResult);
		for (int z = 0; z < monoResult.size(); z++)
		{
			bool br = false;
			for (int y = 0; y < monomial.size(); y++)
			{
				if (monoResult[z] == monomial[y])
				{
					result.push_back(alltmp[i]);
					br = true;
					break;
				}
			}
			if (br) break;
		}

		monoResult.clear();
		monoMiddResult.clear();
		my0 = false;
		my1 = false;
		my2 = false;
		my3 = false;


	}
	return result;

}


vector<string> sbox:: Monomial(vector<vector<int>> S)
{
	vector<string> monomial;
	string f;
	for (int i = 0; i < S.size(); i++)
	{
		//for (int j = 0; j < S[0].size(); j++)
		//{

		if (S[i][3] == 0 && S[i][2] == 0 && S[i][1] == 0 && S[i][0] == 0)
		{
			f = "1";
		}
		if (S[i][3] == 1)
		{
			f += "x" + to_string(0);
		}
		if (S[i][2] == 1)
		{
			f += "x" + to_string(1);
		}
		if (S[i][1] == 1)
		{
			f += "x" + to_string(2);
		}
		if (S[i][0] == 1)
		{
			f += "x" + to_string(3);
		}
		//}
		monomial.push_back(f);
		f = "";
	}
	return monomial;
}

//字符串排序
string sbox:: arryTmp(string a, string b)
{
	string result;
	string ss = a;

	if (b[0] != '1')
	{
		ss += b;
	}
	else
	{
		ss += b.erase(0, 1);
	}

	vector<string> aarry;
	if (ss[0] != '1')
	{
		for (int i = 0; i < ss.length(); i += 2)
		{
			aarry.push_back(ss.substr(i, 2));
		}
	}
	else
	{
		aarry.push_back("1");
		for (int i = 1; i < ss.length(); i += 2)
		{
			aarry.push_back(ss.substr(i, 2));
		}
	}
	sort(aarry.begin(), aarry.end());
	//去除重复的单项

	aarry.erase(unique(aarry.begin(), aarry.end()), aarry.end());

	for (int i = 0; i < aarry.size(); i++)
	{
		result += aarry[i];

	}



	return result;

}
//去掉重复的单项式


vector<vector<int>> sbox:: S_vector(vector<int> k)
{
	vector<int> tmp;
	vector<vector<int>> result;
	vector<vector<int>> alltmp = fullPerm_01(4);
	for (int i = 0; i < alltmp.size(); i++)
	{

		if (alltmp[i][0] >= k[0] && alltmp[i][1] >= k[1] && alltmp[i][2] >= k[2] && alltmp[i][3] >= k[3])
		{
			result.push_back(alltmp[i]);
		}

	}
	return result;
}

vector<vector<int>>sbox:: Redundant(vector<vector<int>> clause)
{


	vector<int> interSolution;
	vector<int> comSolution;
	int index = 0;
	for (int i = clause.size() - 1; i >= 0; i--)
	{
		interSolution = clause[i];

		for (int j = clause.size() - 1; j >= 0; j--)
		{
			comSolution = clause[j];
			for (int z = 0; z<interSolution.size(); z++)
			{
				if (interSolution[z] >= comSolution[z])
				{
					index++;
				}
			}

			if (index == interSolution.size() && i != j)
			{
				clause.erase(clause.begin() + i);
				break;


			}
			index = 0;

		}
	}
	return clause;

}


vector<string>sbox:: getKTrueValue(int sbox[])
{

	vector<string> result;
	//所有组合
	vector<vector<int>> Va = fullPerm_01(8);
	sort(Va.begin(), Va.end());
	//八个中间变量
	vector<int> middvalue;
	vector<vector<int>> Resmid;


	vector<vector<string>> anf = getANF(sbox);

	vector<vector<int>> value;
	vector<vector<int>> re;

	value = fullPerm_01(4);
	sort(value.begin(), value.end());
	vector<int> tmp;

	for (int i = 0; i < value.size(); i++)
	{
		for (int j = 0; j<4; j++) {
			tmp.push_back(value[i][j]);
			//cout << value[i][j] << "，";
		}
		re = S_box(anf[0], anf[1], anf[2], anf[3], tmp);

		int ss = re.size();
		vector<vector<int>> ress=re;

		tmp.clear();
		//cout << "-------";
		for (int z = 0; z < re.size(); z++)
		{
			
			middvalue.push_back(value[i][0]);
			middvalue.push_back(value[i][1]);
			middvalue.push_back(value[i][2]);
			middvalue.push_back(value[i][3]);
			middvalue.push_back(re[z][0]);
			middvalue.push_back(re[z][1]);
			middvalue.push_back(re[z][2]);
			middvalue.push_back(re[z][3]);
			Resmid.push_back(middvalue);
			middvalue.clear();
			//cout << re[z][0]<< re[z][1]<< re[z][2]<< re[z][3];
			//cout << "，";
		}
		//cout << endl;

	}

	bool istrue = false;
	for (int i = Va.size() - 1; i >= 0; i--)
	{
		for (int j = 0; j < Resmid.size(); j++)
		{
			if (Va[i] == Resmid[j])
			{
				//cout << "T" << " ";
				result.push_back("T");
				istrue = true;
			}
		}
		if (!istrue)
			result.push_back("F");
		istrue = false;
	}

	//cout << endl;

	return result;
}

vector<string> sbox::getLTrueValue(int sbox[]) 
{
	vector<string> result;
	//所有组合
	vector<vector<int>> Va = fullPerm_01(8);
	sort(Va.begin(), Va.end());
	//八个中间变量
	vector<int> middvalue;
	vector<vector<int>> Resmid;


	vector<vector<string>> anf = getANF(sbox);

	vector<vector<int>> value;
	vector<vector<int>> re;

	value = fullPerm_01(4);
	sort(value.begin(), value.end());
	vector<int> tmp;

	for (int i = 0; i < value.size(); i++)
	{
		for (int j = 0; j<4; j++) {
			tmp.push_back(value[i][j]);
			//cout << value[i][j] << "，";
		}
		re = LS_box(anf[0], anf[1], anf[2], anf[3], tmp);


		tmp.clear();
		//cout << "-------";
		for (int z = 0; z < re.size(); z++)
		{
			
			//cout << re[z][0] << re[z][1] << re[z][2] << re[z][3]  ;
			middvalue.push_back(value[i][0]);
			middvalue.push_back(value[i][1]);
			middvalue.push_back(value[i][2]);
			middvalue.push_back(value[i][3]);
			middvalue.push_back(re[z][0]);
			middvalue.push_back(re[z][1]);
			middvalue.push_back(re[z][2]);
			middvalue.push_back(re[z][3]);
			Resmid.push_back(middvalue);
			middvalue.clear();

			//cout << "，";
		}
		//cout << endl;

	}

	bool istrue = false;
	for (int i = Va.size() - 1; i >= 0; i--)
	{
		for (int j = 0; j < Resmid.size(); j++)
		{
			if (Va[i] == Resmid[j])
			{
				//cout << "T" << " ";
				result.push_back("T");
				istrue = true;
			}
		}
		if (!istrue)
			result.push_back("F");
		istrue = false;
	}

	//cout << endl;

	return result;
}