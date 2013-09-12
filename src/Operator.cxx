/*
 * Operator.cxx
 *
 *  Created on: Feb 26, 2013
 *      Author: dloti
 */

#include "Operator.hxx"
namespace expression {
Operator::Operator(char op) :
		Expression() {
	this->op = op;
	this->level = 0;
}

void Operator::ClearInterpretation() {
	this->interpretation.clear();
	this->roleInterpretation.clear();
}

std::vector<int>* Operator::GetInterpretation() {
	if (isRole)
		this->interpretation.clear();
	return &(this->interpretation);
}

std::vector<std::pair<int, int> >* Operator::GetRoleInterpretation() {
	if (!isRole)
		this->roleInterpretation.clear();
	return &(this->roleInterpretation);
}

bool Operator::EqualDenotationVec(Expression* exp) {
	if (exp->GetNonEmptyDenotationNum() != this->nonEmptyDenot)
		return false;

	if (exp->GetNonEmptyDenotationNum() == 0)
		std::cout << "ERR nula";

	std::vector<std::vector<int> > cDenot = exp->GetDenotationVec();
	if (cDenot.size() != this->denotations.size()) {
		std::cout << "ERR EqualDenotationVec";
		return false;
	}
	std::vector<int> intersect;

	for (unsigned i = 0; i < cDenot.size(); i++) {
		if (cDenot[i].size() != this->denotations[i].size())
			return false;
		set_intersection(cDenot[i].begin(), cDenot[i].end(), this->denotations[i].begin(), this->denotations[i].end(),
				back_inserter(intersect));

		if (intersect.size() != cDenot[i].size())
			return false;

		intersect.clear();
	}
	return true;
}

Operator::~Operator() {
}
}

