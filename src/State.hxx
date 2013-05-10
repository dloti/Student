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
	int type;
	std::map<std::string, std::vector<int> > conceptInterpretation;
	std::map<std::string, std::vector<std::pair<int, int> > > roleInterpretation;
	std::string action;
public:
	State();
	virtual ~State();
	inline void AddConceptInterpretation(std::string concept, std::vector<int> interpretation) {
		conceptInterpretation[concept] = interpretation;
	}

	inline void AddRoleInterpretation(std::string role, std::vector<std::pair<int, int> > interpretation) {
		roleInterpretation[role] = interpretation;
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

	void Print(std::vector<int> allObjects) {
		std::map<std::string, std::vector<int> >::iterator it;
		std::map<std::string, std::vector<std::pair<int, int> > >::iterator itr;

		for (it = conceptInterpretation.begin(); it != conceptInterpretation.end(); ++it) {
			std::cout << it->first;
			std::cout << ": ";
			for (unsigned i = 0; i < it->second.size(); ++i)
				std::cout << it->second[i] << " ";
			std::cout << std::endl;
		}

		for (itr = roleInterpretation.begin(); itr != roleInterpretation.end(); ++itr) {
			std::cout << itr->first;
			std::cout << ": ";
			for (unsigned i = 0; i < itr->second.size(); ++i)
				std::cout << itr->second[i].first << "," << itr->second[i].second << " ";
			std::cout << std::endl;
		}
	}
};

#endif /* STATE_HXX_ */
