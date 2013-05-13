/*
 * Not.cxx
 *
 *  Created on: Mar 1, 2013
 *      Author: dloti
 */

#include "Not.hxx"
namespace expression {
Not::Not(Expression* child) :
		UnaryOperator(child, '!') {
	this->UpdateDenotations();
}

std::vector<int>* Not::GetInterpretation() {
	//this->UpdateInterpretation();
	return &(this->interpretation);
}
void Not::UpdateDenotations() {
	std::vector<std::vector<int> > cDenot = this->child->GetDenotationVec();
	for (unsigned i = 0; i < cDenot.size(); ++i) {
		std::vector<int> tmpInterpretation;
		for (unsigned j = 0;j < allObjects->size(); ++j) {
			if (std::find(cDenot[i].begin(), cDenot[i].end(), j) != cDenot[i].end())
				continue;
			tmpInterpretation.push_back(j);
		}
		this->denotations.push_back(tmpInterpretation);
	}
}

void Not::UpdateInterpretation() {
	this->ClearInterpretation();
	this->child->UpdateInterpretation();
	std::vector<int>* childInterpretation = this->child->GetInterpretation();
	if (childInterpretation == NULL && childInterpretation->size() > 0)
		return;

	for (unsigned i = 0; i < allObjects->size(); i++) {
		if (std::find(childInterpretation->begin(), childInterpretation->end(), i) != childInterpretation->end())
			continue;
		interpretation.push_back(i);
	}
	std::sort(this->interpretation.begin(), this->interpretation.end());
}

Not::~Not() {
}
}

