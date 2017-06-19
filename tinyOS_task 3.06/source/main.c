#include "tinyOS.h"
#include "ARMCm3.h"
#include "config.h"
	
tTask *currentTask;
tTask *nextTask;
//tTask *taskTable[2];
tTask *idleTask;
//缓存区保存r4-r7
unsigned long stackBuffer[1024];
int shareCount;
int flag = 0;

tTask tTask1;
tTask tTask2;

tTaskStack task1Env[1024];
tTaskStack task2Env[1024];

tTask tTaskIdle;
tTaskStack idletaskEnv[1024];
tBitmap taskPrioBitmap;
tTask * taskTable[TINYOS_PRO_COUNT];

uint8_t schedLockCount;//锁定计数器

void tSetSysTickPeriod (uint32_t ms);//初始化定时器
void SysTick_Handler(void);//中断时间到调用
void tTaskSystemTickHandler(void);//时间处理函数
void tTaskSched(void);//任务调度函数
void tTaskDelay(uint32_t delay);//延时函数
void tTaskInit (tTask * task,void (*entry)(void *),void * param,uint32_t prio,tTaskStack * stack);//堆栈初始化
/****************入口任务 ****************/
void task1Entry (void * param);
void task2Entry (void * param);
void idleTaskEntry (void * param);
void tTaskSchedInit (void);
void tTaskSchedRdy (tTask *task);
void tTimeTaskWait(tTask *task,uint32_t ticks);

void tTaskSchedInit (void)
{
	schedLockCount = 0;
	tBitmapInit(&taskPrioBitmap);
}

void tTaskSchedDisable (void)
{
	uint32_t status = tTaskEnterCritical();
	
	if (schedLockCount < 255)
	{
		schedLockCount++;
	}
	
	tTaskExitCritical(status);
}

void tTaskSchedEnable(void)
{
	uint32_t status = tTaskEnterCritical();
	
	if (schedLockCount > 0)
	{
		if (--schedLockCount == 0)
		{
			tTaskSched();
		}
	}
	
	tTaskExitCritical(status);
}

tList tTaskDelayedList;

void tTaskInit (tTask * task,void (*entry)(void *),void * param,uint32_t prio,tTaskStack * stack)//堆栈初始化
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
	task->prio = prio;
	task->state = TINYOS_TASK_STATE_RDY;
	
	tNodeInit(&(task->delayNode));
	taskTable[prio] = task;
	tBitmapSet(&taskPrioBitmap,prio);
}

//查找位图
tTask * tTaskHighestReady (void)//获取最高优先级任务
{
	uint32_t highestPrio = tBitmapGetFirstSet(&taskPrioBitmap);
	return taskTable[highestPrio];
}


void tSetSysTickPeriod (uint32_t ms)//初始化定时器
{
	SysTick->LOAD = ms * SystemCoreClock / 1000 -1; //初始化重载
	NVIC_SetPriority(SysTick_IRQn,((1 << __NVIC_PRIO_BITS) - 1));
	SysTick->VAL = 0;
	SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk |   //控制寄存器
								  SysTick_CTRL_TICKINT_Msk |
									SysTick_CTRL_ENABLE_Msk;
}

void tTaskDelay(uint32_t delay)//延时函数
{
	uint32_t status =tTaskEnterCritical();
	
	tTimeTaskWait(currentTask,delay);
	
	tTaskSchedRdy(currentTask);
	
	tTaskExitCritical(status);
	
	tTaskSched();//任务调度函数
}
/****************入口任务 ****************/
int task1Flag;
int task2Flag;
int firstSet;

tList list;
tNode node[8];
void task1Entry (void * param)
{
	tSetSysTickPeriod (10);

	for(;;)
	{
		task1Flag = 0;
		tTaskDelay(1);
		task1Flag = 1;
		tTaskDelay(1);
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
	}
}

void idleTaskEntry (void * param)
{
	for (;;)
	{
		
	}
}
/****************end ****************/

void SysTick_Handler()//中断时间到调用
{
//	tTaskSched();//任务调度函数
   	tTaskSystemTickHandler();//时间处理函数
}

void tTaskDelayedInit (void)
{
	tListInit(&tTaskDelayedList);
}

void tTimeTaskWait(tTask *task,uint32_t ticks)
{
	task->delayTicks = ticks;
	tListAddFirst(&tTaskDelayedList,&(task->delayNode));
	task->state |= TINYOS_TASK_STATE_DELAYD;
}

void tTimeWakeUp (tTask *task)
{
	tListRemove(&tTaskDelayedList,&(task->delayNode));
	task->state &= TINYOS_TASK_STATE_DELAYD;
}

void tTaskSystemTickHandler()//时间处理函数
{
	tNode * node;
	
	uint32_t status =tTaskEnterCritical();
	
	for (node = tTaskDelayedList.headNode.nextNode; node != &(tTaskDelayedList.headNode) ;node = node->nextNode)
	{
		tTask * task = tNodeParent(node,tTask,delayNode);
		
		if (--task->delayTicks == 0)
		{
			tTimeWakeUp(task);
			
			tTaskSchedRdy(task);
		}
	}

	tTaskExitCritical(status);
	
	tTaskSched();//任务调度函数
	
}

void tTaskSchedRdy (tTask *task)
{
	taskTable[task->prio] = task;
	tBitmapSet(&taskPrioBitmap,task->prio);
}

void tTaskschedUnRdy (tTask *task)
{
	taskTable[task->prio] = (tTask *)0;
	tBitmapClear(&taskPrioBitmap,task->prio);
}

void tTaskSched()//任务调度函数
{
	tTask *tempTask;
	
	uint32_t status = tTaskEnterCritical();//保护临界区
	
	if (schedLockCount > 0)//是否上锁
	{
		tTaskExitCritical(status);
		return;
	}
	
	tempTask = tTaskHighestReady();
	if(tempTask != currentTask)
	{
		nextTask = tempTask;
		tTaskSwitch();
	}
	
	tTaskExitCritical(status);
}



int main()
{
	tTaskSchedInit();
	
	tTaskDelayedInit();
	
	tTaskInit(&tTask1,task1Entry,(void *)0x11111111,0,&task1Env[1024]);
	tTaskInit(&tTask2,task2Entry,(void *)0x22222222,1,&task2Env[1024]);

	
	taskTable[0] = &tTask1;
	taskTable[1] = &tTask2;
	

	
	tTaskInit(&tTaskIdle,idleTaskEntry,(void *)0,TINYOS_PRO_COUNT - 1,&idletaskEnv[1024]);
	idleTask = &tTaskIdle;
	
	nextTask = tTaskHighestReady();
	
	tTaskRunFirst();
	
	return 0;
}
