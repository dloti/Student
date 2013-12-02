/*
 * Policy.hxx
 *
 *  Created on: Oct 29, 2013
 *      Author: dloti
 */

#ifndef POLICY_HXX_
#define POLICY_HXX_
#include"Expression.hxx"
#include"State.hxx"
#include <iostream>
#include <sstream>
#include <cstring>
#include <fstream>
#include <typeinfo>
#include <cmath>
#include <map>
#include <vector>
#include <algorithm>
#include <stdlib.h>
using namespace expression;
class Policy {
	std::map<std::string, int> policy;
	std::map<std::string, int> minimalPolicy;
	std::vector<std::pair<std::string, int> > decisionList;
	std::vector<Expression*> minHitSet;
	std::vector<std::pair<std::string, int> > stateSignatures;
	std::vector<int> actions;
	std::vector<int> significantObjects;

	static bool moreCorrect(std::pair<std::string, int> p, std::pair<std::string, int> p1);
	bool isSignatureMatch(std::string signature, std::string signature1);
	bool isValidSignature(std::string signature, int action);
	bool goesToDifferentAction(std::string signature, int action, std::string signature1, int action1);

public:
	Policy(std::vector<Expression*> minHitSet, std::vector<Instance> instances, std::vector<int> significantObjects);
	void MinimizePolicy();
	void MakeDecisionList();
	void Print();
	void PrintMinimalPolicy();
	void PrintDecisionList(std::ostream& out);
};

#endif /* POLICY_HXX_ */
