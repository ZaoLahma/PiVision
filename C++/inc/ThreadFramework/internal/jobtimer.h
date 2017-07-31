/*
 * jobtimer.h
 *
 *  Created on: Apr 27, 2016
 *      Author: janne
 */

#ifndef INC_INTERNAL_JOBTIMER_H_
#define INC_INTERNAL_JOBTIMER_H_

#include <cinttypes>

#include "../jobbase.h"
#include "../defaultexecgroups.h"
#include "timerbase.h"

class JobTimer : public TimerBase
{
public:
	JobTimer(std::shared_ptr<JobBase> _jobPtr, const uint32_t _ms, uint32_t groupId = DEFAULT_EXEC_GROUP_ID);

	void TimerFunction();

protected:

private:
	JobTimer();
	std::shared_ptr<JobBase> jobPtr;
	uint32_t groupId;
};


#endif /* INC_INTERNAL_JOBTIMER_H_ */
