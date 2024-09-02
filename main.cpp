#include <iostream>
#include <Windows.h>
#include "CKernel.h"
using namespace std;

int main()
{
	CKernel kernal;
	if (!kernal.startServer())
	{
		cout << "startServer false" << endl;
		return 1;
	}
	

	while (1)
	{
		Sleep(50000);
		cout << "server is running" << endl;
	}
	return 0;
}