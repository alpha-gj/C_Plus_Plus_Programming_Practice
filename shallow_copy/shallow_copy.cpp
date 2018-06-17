#include<stdlib.h>
#include<stdio.h>

using namespace std;


class CExample {

	public:
		CExample(int paramter):val(paramter)
		{

		}
		~CExample() 
		{

		}
		void Show();

	private:
		int val;
};

void CExample::Show()
{
	printf("val is %d\n", val);
}

int main(void)
{
	printf("Hello World!\n");
	CExample  A(100);
	CExample B = A;
	B.Show();
	return 0;
}
