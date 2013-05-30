/*
 * State.cxx
 *
 *  Created on: May 10, 2013
 *      Author: dloti
 */

#include "State.hxx"

State::State() {
	type = 1;
	number = 0;
	covered = false;
}

State::~State() {
	// TODO Auto-generated destructor stub
}

void State::Print(std::vector<std::string> allObjects) {
	std::map<std::string, std::vector<int> >::iterator it;
	std::map<std::string, std::vector<std::pair<int, int> > >::iterator itr;

	if (!this->IsGoal())
		std::cout << "State " << number << std::endl;
	else
		std::cout << "Goal state" << std::endl;

	for (it = conceptInterpretation.begin(); it != conceptInterpretation.end();
			++it) {
		std::cout << it->first;
		std::cout << ": ";
		for (unsigned i = 0; i < it->second.size(); ++i)
			std::cout << allObjects[it->second[i]] << "-";
		std::cout << std::endl;
	}

	for (itr = roleInterpretation.begin(); itr != roleInterpretation.end();
			++itr) {
		std::cout << itr->first;
		std::cout << ": ";
		for (unsigned i = 0; i < itr->second.size(); ++i)
			std::cout << allObjects[itr->second[i].first] << ","
					<< allObjects[itr->second[i].second] << "-";
		std::cout << std::endl;
	}
	if (!(this->IsGoal()))
		std::cout << "Action: " << action << std::endl;
}
