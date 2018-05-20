#include<stdlib.h>
#include<stdio.h>
#include<iostream>

using namespace std;

class CExample 
{
	private:
		int a;

	public:
		CExample();
		~CExample();
		CExample(int b);
		CExample(const CExample &C);
		void Show();
};

