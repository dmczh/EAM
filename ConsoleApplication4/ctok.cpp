#include "std_lib_facilities.h"
#include <iostream>
using namespace std;

template<typename Elem> struct Link
{
	Link* prev;
	Link* succ;
	Elem val;
}; 
//template<typename Elem> struct list
//{
//	Link<Elem>* first;
//	Link<Elem>* last;
//};

template<typename Elem> class list
{
public:
	class iterator;
	iterator begin();
	iterator end();

	iterator insert(iterator p,const Elem& v);
	iterator erase(iterator p);

	void push_back(const Elem& v);
	void push_front(const Elem& v);
	void pop_front();
	void pop_back();

	Elem& front();
	Elem& back();
};
bool is_palindrome(const char s[],int n)
{
	int first=0;
	int last = n - 1;
	while (first<last)
	{
		if (s[first]!=s[last])
		{
			return false;
		}
		++first;
		--last;
	}
	return true;
}

istream& read_word(istream& is,char*buffer,int max)
{
	is.width(max);
	is >> buffer;
	return is;
}

int main()
{
	constexpr int max = 128;
	for (char s[max];read_word(cin,s,max);)
	{
		cout << s << " is ";
		if (!is_palindrome(s,strlen(s)))
		{
			cout << "not" ;
		}
		cout << " a palindrome"<<'\n';
	}
}