
#ifndef TINYOS_H
#define TINYOS_H

#include <stdint.h>
#include "tl_ib.h"

#define TINYOS_TASK_STATE_RDY (0)//就绪状态
#define TINYOS_TASK_STATE_DELAYD (1 << 1)//延时状态

typedef uint32_t tTaskStack;

typedef struct _tTask{
	
	tTaskStack * stack;
	uint32_t delayTicks;
	tNode delayNode;
	uint32_t prio;
	uint32_t state;
}tTask;

extern tTask *currentTask;
extern tTask *nextTask;

void tTaskRunFirst(void);
void tTaskSwitch(void);
uint32_t tTaskEnterCritical (void);//进入临界区

uint32_t tTaskExitCritical (uint32_t status);//退出临界区
#endif
