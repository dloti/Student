/*
 * Instance.cxx
 *
 *  Created on: May 10, 2013
 *      Author: dloti
 */

#include "Instance.hxx"

Instance::Instance() {
	numPlan = 0;
	numActions = 0;
}

Instance::~Instance() {
	// TODO Auto-generated destructor stub
}

std::vector<int> Instance::GetPlanNums(){
	std::vector<int> ret;
	for(int i=0;i<states.size();++i){
		ret.push_back(states[i].GetActionNum());
	}
	return ret;
}
State& Instance::operator [](const int nIndex) {
	return states[nIndex];
}

