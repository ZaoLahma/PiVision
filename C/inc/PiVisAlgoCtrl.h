#ifndef HEADER_ALGO_CONTROL
#define HEADER_ALGO_CONTROL

typedef enum
{
	ALGO_INIT = 0,
	ALGO_RUNNING,
	ALGO_COMPLETE
} PiVisAlgoState;

typedef void* (*AlgoExecFunc)(void*);

typedef struct
{
	PiVisAlgoState state;
	char* inputData;
	char* outputData;
	unsigned int outputDataSize;
  AlgoExecFunc exec;
} PiVisAlgoContext;

void ALGOCTRL_init(void);

#endif
