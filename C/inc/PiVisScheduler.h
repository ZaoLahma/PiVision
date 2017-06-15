#ifndef HEADER_PI_VIS_SCHEDULER
#define HEADER_PI_VIS_SCHEDULER

typedef void (*SchdRunFunc)(void);

typedef struct SchdRunFuncEntry
{
	SchdRunFunc run;
	struct SchdRunFuncEntry* next;
} SchdRunFuncEntry;

void SCHED_init(void);
void SCHED_run(void);
int SCHED_registerCallback(SchdRunFuncEntry* schdRunFuncEntry);

#endif
