#include "tinyOS.h"
#include "ARMCm3.h"
#include "config.h"
	
tTask *currentTask;
tTask *nextTask;
//tTask *taskTable[2];
tTask *idleTask;
//����������r4-r7
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

uint8_t schedLockCount;//����������

void tSetSysTickPeriod (uint32_t ms);//��ʼ����ʱ��
void SysTick_Handler(void);//�ж�ʱ�䵽����
void tTaskSystemTickHandler(void);//ʱ�䴦����
void tTaskSched(void);//������Ⱥ���
void tTaskDelay(uint32_t delay);//��ʱ����
void tTaskInit (tTask * task,void (*entry)(void *),void * param,uint32_t prio,tTaskStack * stack);//��ջ��ʼ��
/****************������� ****************/
void task1Entry (void * param);
void task2Entry (void * param);
void idleTaskEntry (void * param);
void tTaskSchedInit (void);



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
void tTaskInit (tTask * task,void (*entry)(void *),void * param,uint32_t prio,tTaskStack * stack)//��ջ��ʼ��
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
	
	taskTable[prio] = task;
	tBitmapSet(&taskPrioBitmap,prio);
}

//����λͼ
tTask * tTaskHighestReady (void)//��ȡ������ȼ�����
{
	uint32_t highestPrio = tBitmapGetFirstSet(&taskPrioBitmap);
	return taskTable[highestPrio];
}


void tSetSysTickPeriod (uint32_t ms)//��ʼ����ʱ��
{
	SysTick->LOAD = ms * SystemCoreClock / 1000 -1; //��ʼ������
	NVIC_SetPriority(SysTick_IRQn,((1 << __NVIC_PRIO_BITS) - 1));
	SysTick->VAL = 0;
	SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk |   //���ƼĴ���
								  SysTick_CTRL_TICKINT_Msk |
									SysTick_CTRL_ENABLE_Msk;
}

void tTaskDelay(uint32_t delay)//��ʱ����
{

	currentTask->delayTicks = delay;
	
	tBitmapClear(&taskPrioBitmap,currentTask->prio);
	
	tTaskSched();//������Ⱥ���
}
/****************������� ****************/
int task1Flag;
int task2Flag;
int firstSet;

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

void SysTick_Handler()//�ж�ʱ�䵽����
{
//	tTaskSched();//������Ⱥ���
   	tTaskSystemTickHandler();//ʱ�䴦����
}

void tTaskSystemTickHandler()//ʱ�䴦����
{
	int i;
	
	for(i = 0;i < TINYOS_PRO_COUNT;i++)
	{
		if(taskTable[i]->delayTicks > 0)
		{
			taskTable[i]->delayTicks--;
		}
		else
		{
			tBitmapSet(&taskPrioBitmap,i);
		}
	}
	
	tTaskSched();//������Ⱥ���
}

void tTaskSched()//������Ⱥ���
{
	tTask *tempTask;
	
	uint32_t status = tTaskEnterCritical();//�����ٽ���
	
	if (schedLockCount > 0)//�Ƿ�����
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
