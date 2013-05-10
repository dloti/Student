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

State& Instance::operator [](const int nIndex) {
	return states[nIndex];
}
