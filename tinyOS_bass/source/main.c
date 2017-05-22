#include <stdio.h>

void delay(int count)
{
	while(--count > 0);
}
int main()
{
	int flag = 0;
	
	for(;;)
	{
		flag = 0;
		delay(100);
		flag = 1;
		delay(100);
	}
	return 0;
}