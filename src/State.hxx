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
	bool covered;
public:
	State();
	virtual ~State();
	inline void AddConceptInterpretation(std::string concept, std::vector<int> interpretation) {
		conceptInterpretation[concept] = interpretation;
	}

	inline void AddRoleInterpretation(std::string role, std::vector<std::pair<int, int> > interpretation) {
		roleInterpretation[role] = interpretation;
	}

	inline std::vector<int> GetConceptInterpretation(std::string name) {
		return conceptInterpretation[name];
	}

	inline std::vector<std::pair<int, int> > GetRoleInterpretation(std::string name) {
		return roleInterpretation[name];
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

	inline void Covered() {
		covered = true;
	}

	inline bool IsCovered() {
		return covered;
	}

	inline std::string GetAction() {
		return action;
	}

	void Print(std::vector<std::string> allObjects);
};

#endif /* STATE_HXX_ */
