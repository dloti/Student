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
void Policy::Print() {
	std::map<std::string, int>::iterator it;
	std::cout<<"Policy: "<<std::endl;
	for (it = policy.begin(); it != policy.end(); ++it) {
		std::cout << it->first << ": " << it->second << std::endl;
	}
}

void Policy::PrintMinimalPolicy() {
	std::map<std::string, int>::iterator it;
	std::cout<<"Minimal policy: "<<std::endl;
	for (it = minimalPolicy.begin(); it != minimalPolicy.end(); ++it) {
		std::cout << it->first << ": " << it->second << std::endl;
	}
}

bool Policy::goesToDifferentAction(std::string signature, int action, std::string signature1, int action1) {
	if (action1 == action)
		return false;
	for (int i = 0; i < signature.length(); ++i) {
		if (signature[i] == signature1[i])
			continue;
		if (signature[i] == '*' || signature1[i] == '*')
			continue;
		if (signature[i] != signature1[i]) {
			//std::cout << "FALSE: " << signature << " " << signature1 << std::endl;
			return false;
		}

	}
	//std::cout << "TRUE: " << signature << " " << signature1 << std::endl;
	return true;
}

bool Policy::isValidSignature(std::string signature, int action) {
	std::map<std::string, int>::iterator it;

	for (it = policy.begin(); it != policy.end(); ++it) {
		if (goesToDifferentAction(signature, action, it->first, it->second))
			return false;
	}
	return true;
}
void Policy::MinimizePolicy() {
	std::map<std::string, int>::iterator it;
	for (it = policy.begin(); it != policy.end(); ++it) {
		std::string signature = it->first;
		for (int i = 0; i < signature.length(); ++i) {
			char backup = signature[i];
			signature[i] = '*';
			if (!isValidSignature(signature, it->second))
				signature[i] = backup;
		}
		minimalPolicy[signature] = it->second;
	}
}

