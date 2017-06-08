
#ifndef TINYOS_H
#define TINYOS_H

#include <stdint.h>
#include "tl_ib.h"

typedef uint32_t tTaskStack;

typedef struct _tTask{
	
	tTaskStack * stack;
	uint32_t delayTicks;
}tTask;

extern tTask *currentTask;
extern tTask *nextTask;

void tTaskRunFirst(void);
void tTaskSwitch(void);
uint32_t tTaskEnterCritical (void);//�����ٽ���

uint32_t tTaskExitCritical (uint32_t status);//�˳��ٽ���
#endif
