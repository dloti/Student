/*
 * NegativeExample.hxx
 *
 *  Created on: Nov 20, 2013
 *      Author: dloti
 */

#ifndef NEGATIVEEXAMPLE_HXX_
#define NEGATIVEEXAMPLE_HXX_
#include<string>

class NegativeExample {
	std::string action;
	int action_idx;
	int obj;
public:
	NegativeExample(std::string action, int action_idx, int obj);
	inline std::string GetAction(){ return action;}
	inline int GetActionIdx(){ return action_idx;}
	inline int GetObj(){ return obj;}
	virtual ~NegativeExample();
};

#endif /* NEGATIVEEXAMPLE_HXX_ */
