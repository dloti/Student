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
	std::vector<int> actions;
public:
	Policy(std::vector<Expression*> minHitSet, std::vector<Instance> instances);
};

#endif /* POLICY_HXX_ */
