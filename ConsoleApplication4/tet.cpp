#include <iostream>
#include <math.h>
#include <vector>
#include <algorithm>
#include <limits>
#include <std_lib_facilities.h>
using namespace std;

enum class Month {
	jan = 1, feb, mar, par, may, jun, jul, aug, sep, oct, nov, dec
};
class Date
{
public:
	class Invalid {};
	Date(int y,int m,int d);

private:
	int y, m, d;
	bool is_valid();
};

Date::Date(int yy, int mm, int dd)
	:y{yy}, m(mm), d{dd}
{
	if (!is_valid())
	{
		throw Invalid{};
	}
}

bool Date::is_valid()
{
	if (m < 1 || 12 < m)
	{
		return false;
	}
	return true;
}
struct  Reading
{
	int hour;
	double temperature;
};

int main()
{
	cout << "";
	string iname;
	cin >> iname;
	ifstream ist{iname,ios_base::binary};
	ofstream ofs{iname,ios_base::app};
	bool t=ist.fail();
	ist.clear(ios_base::failbit);
	error("printf error");
	if (!ist)
	{
		//
	}
	int i;
	ist.read(as_bytes(i),sizeof(int));
	ist.seekg('c');
	istringstream u;

	for (string word;ist>>word;)
	{
	}
}
void end_of_loop(istream& ist,char term,const string& message)
{
	if (ist.fail())
	{
		ist.clear();
		char ch;
		if (ist>>ch&&ch==term)
		{
			return;
		}
		error("message");
		ist.exceptions(ist.exceptions()|ios_base::badbit);
		
	}
}
