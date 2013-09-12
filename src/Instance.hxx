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
	State goal;
public:
	Instance();
	virtual ~Instance();
	inline void AddState(State s) {
		states.push_back(s);
	}
	inline void SetGoal(State s) {
		s.SetGoal();
		goal = s;
	}
	inline std::vector<State>* GetStates()  {
		return &states;
	}
	inline State GetGoal() {
		return goal;
	}
	inline void SetNumPlan(int num) {
		numPlan = num;
	}
	inline int SetNumPlan() {
		return numPlan;
	}
	inline void SetNumActions(int num) {
		numActions = num;
	}
	inline int GetNumPlan() {
		return numPlan;
	}
	inline int GetNumActions() {
		return numActions;
	}
	State& operator[](const int nIndex);
};

#endif /* PLAN_HXX_ */
