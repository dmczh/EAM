#include "std_lib_facilities.h"

int main()
{
	int n;
	double a, sum;
	string u;
	vector<double> v;
	while (cin>>a>>u)
	{
		if (u=="cm")
		{
			v.push_back(a);
		}
		else if (u=="in")
		{
			a = a * 2.54;
			v.push_back(a);
		}
		else if (u=="ft")
		{
			a = 12 * a;
			a = a * 2.54;
			v.push_back(a);

		}
		else if (u=="m")
		{
			a = 1000 * a;
			v.push_back(a);

		}
		else
		{
			cout << "your input is error !" << endl;
			break;
		}
		sort(v);
		n = v.size();
		sum = 0;
		for (double b:v)
		{
			sum += b;
		}
		cout << v[0] <<"cm" << '\n' << v[v.size() - 1]<<"cm"<<'\n' << n << '\n' << sum <<"cm"  << endl;
	}
}
