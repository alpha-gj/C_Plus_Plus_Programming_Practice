#include<stdlib.h>
#include<stdio.h>
#include"CExample.h"

using namespace std;

int main(void)
{
	printf("Hello World!\n");
	CExample  A(100);
	CExample B = A;
	B.Show();
	return 0;
}
