/*
 * CompoundConcept.cxx
 *
 *  Created on: Feb 26, 2013
 *      Author: dloti
 */

#include "Expression.hxx"

namespace expression {
Expression::Expression() {
	this->level = 0;
	this->isRole = false;
}

void Expression::SimplifyDenotations() {
	if (simpleDenotations.size() > 0)
		return;
	std::vector<std::vector<int> >* subsets = preops->GetSubsets();
	std::vector<std::vector<int> >::iterator it;
	for (unsigned i = 0; i < this->denotations.size(); ++i) {
		it = std::find(subsets->begin(), subsets->end(), denotations[i]);
		if (it == subsets->end())
			std::cout << "ERR denotation simplifier"<<std::endl;
//		for (unsigned j = 0; j < denotations[i].size(); ++j)
//			std::cout << denotations[i][j] << " ";
//		std::cout << std::endl;
		simpleDenotations.push_back((it - subsets->begin()));
	}
}

Expression::~Expression() {
}
}
