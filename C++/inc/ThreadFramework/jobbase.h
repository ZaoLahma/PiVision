/*
 * job.h
 *
 *  Created on: Feb 3, 2016
 *      Author: janne
 */

#ifndef INC_JOBBASE_H_
#define INC_JOBBASE_H_

#include <vector>
#include <memory>

class JobDataBase
{
public:
	virtual ~JobDataBase() {}
protected:

private:
};

class JobBase
{
public:
	virtual ~JobBase();

	void SetJobData(std::shared_ptr<JobDataBase> _dataPtr);

	virtual void Execute() = 0;
protected:
	std::shared_ptr<JobDataBase> dataPtr;

private:

};

typedef std::vector<std::shared_ptr<JobBase>> JobBasePtrVectorT;

#endif /* INC_JOBBASE_H_ */
