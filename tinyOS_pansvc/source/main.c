#include <stdio.h>

#define NVIC_INIT_CTRL	0xE000ED04 //中断控制状态寄存器
#define NVIC_PENDSVSET	0x10000000 //设置挂起 NMI
#define NVIC_SYSPRI2		0xE000ED22 //系统处理器 12-15 优先级寄存器
#define NVIC_PENDSV_PRI	0x000000FF

#define MEM32(addr)	*(volatile unsigned long *)(addr)
#define MEM8(addr)  *(volatile unsigned char *)(addr)
	
void triggerPendSVC(void)
{
	MEM8(NVIC_SYSPRI2) = NVIC_PENDSV_PRI;
	MEM32(NVIC_INIT_CTRL) = NVIC_PENDSVSET;
}
void delay(int count);

/************** 结构 *********/
typedef struct _BlockType_t
{
	unsigned long *stackPtr;//堆栈地址
}BlockType_t;

BlockType_t *blockPtr,block;

//缓存区保存r4-r7
unsigned long stackBuffer[1024];

int flag = 0;

int main()
{
	//指向最后一个单元
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