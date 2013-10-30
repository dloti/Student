/*
 * Policy.hxx
 *
 *  Created on: Oct 29, 2013
 *      Author: dloti
 */

#ifndef POLICY_HXX_
#define POLICY_HXX_
#include"Expression.hxx"
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
	std::map<std::string,int> policy;
	std::map<std::string,int> minimalPolicy;
	std::vector<std::pair<std::string,int> > decisionList;
	std::vector<Expression*> minHitSet;
	std::vector<int> actions;

	bool isSignatureMatch(std::string signature, std::string signature1);
	bool isValidSignature(std::string signature, int action);
	bool goesToDifferentAction(std::string signature, int action, std::string signature1, int action1);
public:
	Policy(std::vector<Expression*> minHitSet, std::vector<Instance> instances);
	void MinimizePolicy();
	void MakeDecisionList();
	void Print();
	void PrintMinimalPolicy();
};

#endif /* POLICY_HXX_ */
