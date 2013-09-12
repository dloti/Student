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
	this->numCoverage = 0;
	for (unsigned i = 0; i < actions->size(); ++i) {
		std::vector<bool> tmpVec;
		std::vector<int> coverageVec;
		for (unsigned j = 0; j < instances->size(); ++j) {
			for (unsigned k = 0; k < (*instances)[j].GetStates()->size(); ++k) {
				if ((*actions)[i].compare((*instances)[j][k].GetAction()) == 0){
					tmpVec.push_back(true);
					coverageVec.push_back(1);
				}
				else{
					tmpVec.push_back(false);
					coverageVec.push_back(0);
				}

			}
		}
		actionCoverage.push_back(coverageVec);
		actionDenotations.push_back(tmpVec);
	}
}

void ActionDenotations::SetCovered(int i, int j) {
	if(actionCoverage[i][j] != 1)
		std::cout<<"ERR coverage";

	actionCoverage[i][j] = 2;
}

std::vector<int>& ActionDenotations::operator [](const int nIndex) {
	return actionCoverage[nIndex];
}
