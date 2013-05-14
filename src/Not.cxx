/*
 * Not.cxx
 *
 *  Created on: Mar 1, 2013
 *      Author: dloti
 */

#include "Not.hxx"
namespace expression {
Not::Not(Expression* child, std::vector<int>* allObjects) :
		UnaryOperator(child, '!') {
	this->allObjects = allObjects;
	this->UpdateDenotations();
}

std::vector<int>* Not::GetInterpretation() {
	//this->UpdateInterpretation();
	return &(this->interpretation);
}
void Not::UpdateDenotations() {
	this->nonEmptyDenot = 0;
	if (typeid(*child) == typeid(Not))
		return;
	std::vector<std::vector<int> > cDenot = this->child->GetDenotationVec();
	std::vector<int> tmpInterpretation;
	tmpInterpretation.reserve(cDenot[0].size());
	for (unsigned i = 0; i < cDenot.size(); ++i) {
		for (unsigned j = 0; j < allObjects->size(); ++j) {
			if (std::find(cDenot[i].begin(), cDenot[i].end(), j) == cDenot[i].end()) {
				tmpInterpretation.push_back((*allObjects)[j]);
			}
		}
		std::sort(tmpInterpretation.begin(), tmpInterpretation.end());
		if (tmpInterpretation.size() > 0)
			this->nonEmptyDenot++;
		this->denotations.push_back(tmpInterpretation);
		tmpInterpretation.clear();
	}
}

void Not::UpdateInterpretation() {
//	this->ClearInterpretation();
//	this->child->UpdateInterpretation();
//	std::vector<int>* childInterpretation = this->child->GetInterpretation();
//	if (childInterpretation == NULL && childInterpretation->size() > 0)
//		return;
//
//	for (unsigned i = 0; i < allObjects->size(); i++) {
//		if (std::find(childInterpretation->begin(), childInterpretation->end(), i) != childInterpretation->end())
//			continue;
//		interpretation.push_back(i);
//	}
//	std::sort(this->interpretation.begin(), this->interpretation.end());
}

Not::~Not() {
}
}

