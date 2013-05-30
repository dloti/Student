/*
 * ActionDenotations.hxx
 *
 *  Created on: May 30, 2013
 *      Author: dloti
 */

#ifndef ACTIONDENOTATIONS_HXX_
#define ACTIONDENOTATIONS_HXX_
#include "Instance.hxx"
#include "State.hxx"
class ActionDenotations {
	std::vector<Instance>* instances;
	std::vector<std::string>* actions;
	std::vector<std::vector<bool> > actionDenotations;
	std::vector<std::vector<int> > actionCoverage;
public:
	ActionDenotations(std::vector<Instance>* instances, std::vector<std::string>* actions);
	std::vector<bool>& operator[](const int nIndex);
};

#endif /* ACTIONDENOTATIONS_HXX_ */
