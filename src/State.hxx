/*
 * State.hxx
 *
 *  Created on: May 10, 2013
 *      Author: dloti
 */

#ifndef STATE_HXX_
#define STATE_HXX_
#include <map>
#include <vector>
#include <string>
#include <iostream>
class State {
	int type, number;
	std::map<std::string, std::vector<int> > conceptInterpretation;
	std::map<std::string, std::vector<std::pair<int, int> > > roleInterpretation;
	std::string action;
public:
	State();
	virtual ~State();
	inline void AddConceptInterpretation(std::string concept,
			std::vector<int> interpretation) {
		conceptInterpretation[concept] = interpretation;
	}

	inline void AddRoleInterpretation(std::string role,
			std::vector<std::pair<int, int> > interpretation) {
		roleInterpretation[role] = interpretation;
	}

	inline void SetNumState(int stateNum) {
		number = stateNum;
	}

	inline void SetGoal() {
		type = 2;
	}

	inline bool IsGoal() {
		return (type == 2);
	}

	inline void SetAction(std::string a) {
		action = a;
	}

	void Print(std::vector<std::string> allObjects);
};

#endif /* STATE_HXX_ */
