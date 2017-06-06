#include "tinyOS.h"
#include "ARMCm3.h"

	
tTask *currentTask;
tTask *nextTask;
tTask *taskTable[2];
tTask *idleTask;
uint32_t tickCounter;
//����������r4-r7
unsigned long stackBuffer[1024];

int flag = 0;

tTask tTask1;
tTask tTask2;

tTaskStack task1Env[1024];
tTaskStack task2Env[1024];

tTask tTaskIdle;
tTaskStack idletaskEnv[1024];

void tSetSysTickPeriod (uint32_t ms);//��ʼ����ʱ��
void SysTick_Handler(void);//�ж�ʱ�䵽����
void tTaskSystemTickHandler(void);//ʱ�䴦����
void tTaskSched(void);//������Ⱥ���
void tTaskDelay(uint32_t delay);//��ʱ����
void tTaskInit (tTask * task,void (*entry)(void *),void * param,tTaskStack * stack);//��ջ��ʼ��
/****************������� ****************/
void task1Entry (void * param);
void task2Entry (void * param);
void idleTaskEntry (void * param);


void tTaskInit (tTask * task,void (*entry)(void *),void * param,tTaskStack * stack)//��ջ��ʼ��
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
	uint32_t status = tTaskEnterCritical();//�����ٽ���
	currentTask->delayTicks = delay;
	
	tTaskExitCritical(status);//�ָ��ٽ���
	tTaskSched();//������Ⱥ���
}
/****************������� ****************/
int task1Flag;
int task2Flag;
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
		uint32_t i;
		
		uint32_t status = tTaskEnterCritical();//�����ٽ���
		
		uint32_t counter = tickCounter;
		for(i = 0; i < 0xFFFF;i++)
		{
			;
		}
		tickCounter = counter + 1;
		
		tTaskExitCritical(status);//�ָ��ٽ���
		
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
/****************end ****************/

void SysTick_Handler()//�ж�ʱ�䵽����
{
//	tTaskSched();//������Ⱥ���
   	tTaskSystemTickHandler();//ʱ�䴦����
}

void tTaskSystemTickHandler()//ʱ�䴦����
{
	int i;
	
	uint32_t status = tTaskEnterCritical();
	for(i = 0;i < 2;i++)
	{
		if(taskTable[i]->delayTicks > 0)
		{
			taskTable[i]->delayTicks--;
		}
	}
	tickCounter++;
	tTaskExitCritical(status);
	
	tTaskSched();//������Ⱥ���
}

void tTaskSched()//������Ⱥ���
{
//	if(currentTask == taskTable[0])
//	{
//		nextTask = taskTable[1];
//	}
//	else
//	{
//		nextTask = taskTable[0];
//	}
	uint32_t status = tTaskEnterCritical();//�����ٽ���
	
	if(currentTask == idleTask)//����ǿ�������
	{
		if (taskTable[1]->delayTicks == 0)//��0������ʱ�䵽
		{
			nextTask = taskTable[0];
		}
		else if (taskTable[1]->delayTicks == 0)//��һ������ʱ�䵽
		{
			nextTask = taskTable[1];
		}
		else//��ʱ���
		{
			tTaskExitCritical(status);
			return;
		}
	}
	else//������ǿ�������
	{
		if (currentTask == taskTable[0])//��ǰ����Ϊ��0������
		{
			if(taskTable[1]->delayTicks == 0)//����ʱ��ʱ�䵽 �л�����
			{
				nextTask = taskTable[1];
			}
			else if(currentTask->delayTicks != 0)//����ʱִ�п�������
			{
				nextTask = idleTask;
			}
			else
			{
				tTaskExitCritical(status);
				return;
			}
		}
		else if (currentTask == taskTable[1])//��ǰ����Ϊ��1������
		{
			if(taskTable[1]->delayTicks == 0)//����ʱ��ʱ�䵽 �л�����
			{
				nextTask = taskTable[0];
			}
			else if(currentTask->delayTicks != 0)//����Լ���������ʱ
			{
				nextTask = idleTask;
			}
			else
			{
				tTaskExitCritical(status);
				return;
			}
		}
	}
	tTaskSwitch();//����penSV�쳣
	tTaskExitCritical(status);
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
