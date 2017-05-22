#include <stdio.h>

#define NVIC_INIT_CTRL	0xE000ED04 //�жϿ���״̬�Ĵ���
#define NVIC_PENDSVSET	0x10000000 //���ù��� NMI
#define NVIC_SYSPRI2		0xE000ED22 //ϵͳ������ 12-15 ���ȼ��Ĵ���
#define NVIC_PENDSV_PRI	0x000000FF

#define MEM32(addr)	*(volatile unsigned long *)(addr)
#define MEM8(addr)  *(volatile unsigned char *)(addr)
	
void triggerPendSVC(void)
{
	MEM8(NVIC_SYSPRI2) = NVIC_PENDSV_PRI;
	MEM32(NVIC_INIT_CTRL) = NVIC_PENDSVSET;
}
void delay(int count);

/************** �ṹ *********/
typedef struct _BlockType_t
{
	unsigned long *stackPtr;//��ջ��ַ
}BlockType_t;

BlockType_t *blockPtr,block;

//����������r4-r7
unsigned long stackBuffer[1024];

int flag = 0;

int main()
{
	//ָ�����һ����Ԫ
	block.stackPtr = &stackBuffer[1024];
	blockPtr = &block;
	
	for(;;)
	{
		flag = 0;
		delay(100);
		flag = 1;
		delay(100);
		
		triggerPendSVC();
	}
	return 0;
}

void delay(int count)
{
	while(--count > 0);
}