/*
 * Instance.hxx
 *
 *  Created on: May 10, 2013
 *      Author: dloti
 */

#ifndef INSTANCE_HXX_
#define INSTANCE_HXX_
#include <vector>
#include "State.hxx"

class Instance {
	int numPlan;
	int numActions;
	std::vector<State> states;
public:
	Instance();
	virtual ~Instance();
	inline void AddState(State s){
		states.push_back(s);
	}
	inline void SetNumPlan(int num){ numPlan = num;}
	inline int SetNumPlan(){ return numPlan;}
	inline void SetNumActions(int num){ numActions = num;}
	inline int GetNumActions(){ return numActions;}
	State& operator[] (const int nIndex);
};

#endif /* PLAN_HXX_ */
