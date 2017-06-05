#include "tinyOS.h"
#include "ARMCm3.h"

	
tTask *currentTask;
tTask *nextTask;
tTask *taskTable[2];
tTask *idleTask;

void delay(int count);
void tTaskSched();//任务调度函数
void tSetSysTickPeriod (uint32_t ms);//初始化定时器
void tTaskSystemTickHandler();//时间

void tSetSysTickPeriod (uint32_t ms)//初始化定时器
{
	SysTick->LOAD = ms * SystemCoreClock / 1000 -1; //初始化重载
	NVIC_SetPriority(SysTick_IRQn,((1 << __NVIC_PRIO_BITS) - 1));
	SysTick->VAL = 0;
	SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk |   //控制寄存器
								  SysTick_CTRL_TICKINT_Msk |
									SysTick_CTRL_ENABLE_Msk;
}

void SysTick_Handler()//时间到
{
//	tTaskSched();//任务调度函数
   	tTaskSystemTickHandler();//时间到调用
}
/************** 结构 *********/
typedef struct _BlockType_t
{
	unsigned long *stackPtr;//堆栈地址
}BlockType_t;

BlockType_t *blockPtr,block;

//缓存区保存r4-r7
unsigned long stackBuffer[1024];

int flag = 0;

tTask tTask1;
tTask tTask2;

tTaskStack task1Env[1024];
tTaskStack task2Env[1024];

tTask tTaskIdle;
tTaskStack idletaskEnv[1024];

void tTaskSystemTickHandler()//时间到调用
{
	int i;
	for(i = 0;i < 2;i++)
	{
		if(taskTable[i]->delayTicks > 0)
		{
			taskTable[i]->delayTicks--;
		}
	}
	tTaskSched();
}

void tTaskDelay(uint32_t delay)
{
	currentTask->delayTicks = delay;
	tTaskSched();//切换任务
}
void tTaskInit (tTask * task,void (*entry)(void *),void * param,tTaskStack * stack)
{
	*(--stack) = (unsigned long)(1 << 24);
	*(--stack) = (unsigned long)entry;//PC
	*(--stack) = (unsigned long)0x14;//LR
	*(--stack) = (unsigned long)0x12;//R12
	*(--stack) = (unsigned long)0x3;//R3
	*(--stack) = (unsigned long)0x2;//R2
	*(--stack) = (unsigned long)0x1;//R1
	*(--stack) = (unsigned long)param;//R0
	
	*(--stack) = (unsigned long)0x11;//R11
	*(--stack) = (unsigned long)0x10;//R10
	*(--stack) = (unsigned long)0x9;//R9
	*(--stack) = (unsigned long)0x8;//R8
	*(--stack) = (unsigned long)0x7;//R7
	*(--stack) = (unsigned long)0x6;//R6
	*(--stack) = (unsigned long)0x5;//R5
	*(--stack) = (unsigned long)0x4;//R4
	
	task->stack = stack;
	task->delayTicks = 0;
}

int task1Flag;
int task2Flag;

void tTaskSched()//任务调度函数
{
//	if(currentTask == taskTable[0])
//	{
//		nextTask = taskTable[1];
//	}
//	else
//	{
//		nextTask = taskTable[0];
//	}
	
	if(currentTask == idleTask)//如果是空闲任务
	{
		if (taskTable[0]->delayTicks == 0)//第0个任务时间到
		{
			nextTask = taskTable[0];
		}
		else if (taskTable[1]->delayTicks == 0)//第一个任务时间到
		{
			nextTask = taskTable[1];
		}
		else//延时完毕
		{
			return;
		}
	}
	else//如果不是空闲任务
	{
		if (currentTask == taskTable[0])//当前任务为第0个任务
		{
			if(taskTable[1]->delayTicks == 0)
			{
				nextTask = taskTable[1];
			}
			else if(currentTask->delayTicks != 0)//如果自己设置有延时
			{
				nextTask = idleTask;
			}
			else
			{
				return;
			}
		}
		else if (currentTask == taskTable[1])//当前任务为第1个任务
		{
			if(taskTable[1]->delayTicks == 0)
			{
				nextTask = taskTable[0];
			}
			else if(currentTask->delayTicks != 0)//如果自己设置有延时
			{
				nextTask = idleTask;
			}
			else
			{
				return;
			}
		}
	}
	tTaskSwitch();
}

void task1Entry (void * param)
{
	tSetSysTickPeriod (10);
	for(;;)
	{
		task1Flag = 0;
		tTaskDelay(1);
		task1Flag = 1;
		tTaskDelay(1);
		
//		tTaskSched();
	}
}

void task2Entry (void * param)
{
	for(;;)
	{
		task2Flag = 0;
		tTaskDelay(1);
		task2Flag = 1;
		tTaskDelay(1);
		
//		tTaskSched();
	}
}

void idleTaskEntry (void * param)
{
	for (;;)
	{
		
	}
}
int main()
{
	tTaskInit(&tTask1,task1Entry,(void *)0x11111111,&task1Env[1024]);
	tTaskInit(&tTask2,task2Entry,(void *)0x22222222,&task2Env[1024]);

	
	taskTable[0] = &tTask1;
	taskTable[1] = &tTask2;
	

	
	tTaskInit(&tTaskIdle,idleTaskEntry,(void *)0,&idletaskEnv[1024]);
	idleTask = &tTaskIdle;
	
	nextTask = &tTask1;
	
	tTaskRunFirst();
	
	return 0;
}

void delay(int count)
{
	while(--count > 0);
}
