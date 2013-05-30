/*
 * ActionDenotations.cxx
 *
 *  Created on: May 30, 2013
 *      Author: dloti
 */

#include "ActionDenotations.hxx"

ActionDenotations::ActionDenotations(std::vector<Instance>* instances, std::vector<std::string>* actions) {
	this->instances = instances;
	this->actions = actions;
	for (unsigned i = 0; i < actions->size(); ++i) {
		std::vector<bool> tmpVec;
		for (unsigned j = 0; j < instances->size(); ++j) {
			for (unsigned k = 0; k < (*instances)[j].GetStates().size(); ++k) {
				if ((*actions)[i].compare((*instances)[j][k].GetAction()) == 0)
					tmpVec.push_back(true);
				else
					tmpVec.push_back(false);
			}
		}
		actionDenotations.push_back(tmpVec);
	}
}

std::vector<bool>& ActionDenotations::operator [](const int nIndex) {
	return actionDenotations[nIndex];
}
