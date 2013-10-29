/*
 * Policy.cxx
 *
 *  Created on: Oct 29, 2013
 *      Author: dloti
 */

#include "Policy.hxx"

Policy::Policy(std::vector<Expression*> minHitSet, std::vector<Instance> instances) {
	for (unsigned i = 0; i < instances.size(); ++i) {
		std::vector<int> plan = instances[i].GetPlanNums();
		actions.insert(actions.end(), plan.begin(), plan.end());
	}

	std::vector<std::string> signatures;
	for (unsigned i = 0; i < minHitSet.size(); ++i) {
		signatures.push_back(minHitSet[i]->GetSignature());
	}

	for (unsigned i = 0; i < signatures[0].size(); ++i) {
		std::string signature = "";
		for (unsigned j = 0; j < signatures.size(); ++j) {
			signature += signatures[j][i];
		}
		policy[signature] = actions[i];
	}
}

