#include "std_lib_facilities.h"

void skip_to_int()
{
	if (cin.fail())
	{
		cin.clear();
		for (char ch;cin>>ch;)
		{
			if (isdigit(ch)||ch=='-')
			{
				cin.unget();
				return;
			}
		}
	}
	error("no input");
}
int main()
{
	cout << "please enter an integer in the range 1 to 10 (inclusive): \n";
	int c = 0;
	while (true)
	{
		if (cin >> c)
		{
			if (c >= 0 && c <= 10)
			{
				break;
			}
			else
			{
				cout << "sorry"
					<< c
					<< "is not in the [1:10] range;please try aganin\n";
			}
		}
		else
		{
			cout << "sorry,that was not a number;please try again\n";
			skip_to_int();
		}
	}

}