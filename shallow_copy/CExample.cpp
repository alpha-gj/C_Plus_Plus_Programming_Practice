#include"CExample.h"

CExample::CExample()
{

}

CExample::~CExample()
{

}
CExample::CExample(int b) 
{
	a = b;
}

CExample::CExample(const CExample &C)
{
	a = C.a;
}

void CExample::Show()
{
	cout << a << endl;
}
