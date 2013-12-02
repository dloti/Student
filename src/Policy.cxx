/*
 * Policy.cxx
 *
 *  Created on: Oct 29, 2013
 *      Author: dloti
 */

#include "Policy.hxx"

Policy::Policy(std::vector<Expression*> minHitSet, std::vector<Instance> instances, std::vector<int> significantObjects) {
	this->minHitSet = minHitSet;
	this->significantObjects = significantObjects;
	for (unsigned i = 0; i < instances.size(); ++i) {
		std::vector<int> plan = instances[i].GetPlanNums();
		actions.insert(actions.end(), plan.begin(), plan.end());
	}

	std::vector<std::string> signatures;
	for (unsigned i = 0; i < minHitSet.size(); ++i) {
		signatures.push_back(minHitSet[i]->GetSignificantObjectSignature(&significantObjects));
	}

	std::cout << "Full policy:" << std::endl;
	for (unsigned i = 0; i < signatures[0].size(); ++i) {
		std::string signature = "";
		for (unsigned j = 0; j < signatures.size(); ++j) {
			signature += signatures[j][i];
		}
		std::cout << signature << ":" << actions[i] << std::endl;
		std::pair<std::string,int> p(signature,actions[i]);
		stateSignatures.push_back(p);
		if (policy.find(signature) != policy.end() && policy[signature] != actions[i])
			std::cout << "ERR policy not correct!" << std::endl;
		policy[signature] = actions[i];
	}
}
void Policy::Print() {
	std::map<std::string, int>::iterator it;
	std::cout << "Policy: " << std::endl;
	for (it = policy.begin(); it != policy.end(); ++it) {
		std::cout << it->first << ": " << it->second << std::endl;
	}
}

void Policy::PrintMinimalPolicy() {
	std::map<std::string, int>::iterator it;
	std::cout << "Minimal policy: " << std::endl;
	for (it = minimalPolicy.begin(); it != minimalPolicy.end(); ++it) {
		std::cout << it->first << ": " << it->second << std::endl;
	}
}
void Policy::PrintDecisionList(std::ostream& out){
	for(int i=0;i<decisionList.size();++i){
		out << decisionList[i].first <<std::endl;
		out << decisionList[i].second << std::endl;
	}
}
bool Policy::isSignatureMatch(std::string signature, std::string signature1) {
	for (int i = 0; i < signature.length(); ++i) {
		if (signature[i] == signature1[i])
			continue;
		if (signature[i] == '*' || signature1[i] == '*')
			continue;
		if (signature[i] != signature1[i]) {
			return false;
		}
	}
	return true;
}

bool Policy::goesToDifferentAction(std::string signature, int action, std::string signature1, int action1) {
	if (action1 == action)
		return false;
	return isSignatureMatch(signature, signature1);
}

bool Policy::isValidSignature(std::string signature, int action) {
	std::map<std::string, int>::iterator it;

	for (it = policy.begin(); it != policy.end(); ++it) {
		if (goesToDifferentAction(signature, action, it->first, it->second))
			return false;
	}
	return true;
}

bool Policy::moreCorrect(std::pair<std::string, int> p, std::pair<std::string, int> p1){
	return (p.second > p1.second);
}

void Policy::MakeDecisionList() {
	std::vector<std::pair<std::string, int> > coverage_vec;
	std::map<std::string, int>::iterator it;
	std::map<std::string,int> coveredSignatures;
	for (it = minimalPolicy.begin(); it != minimalPolicy.end(); ++it) {
		int correct = 0;
		for (int i = 0; i < stateSignatures.size(); ++i) {
			if (isSignatureMatch(it->first, stateSignatures[i].first)) {
				if (it->second == stateSignatures[i].second &&
						coveredSignatures.find(it->first)== coveredSignatures.end()){
					++correct;
					coveredSignatures[it->first] = 1;
				}
			}
		}
		std::pair<std::string, int> p(it->first, correct);
		coverage_vec.push_back(p);
	}
	std::sort(coverage_vec.begin(), coverage_vec.end(), moreCorrect);
	for (int i = 0; i < coverage_vec.size(); ++i) {
		std::string signature = coverage_vec[i].first;
		std::pair<std::string, int> p(signature, minimalPolicy[signature]);
		decisionList.push_back(p);
	}
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

