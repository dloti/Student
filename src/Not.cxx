/*
 * Not.cxx
 *
 *  Created on: Mar 1, 2013
 *      Author: dloti
 */

#include "Not.hxx"
namespace expression {
Not::Not(Expression* child, std::vector<int>* allObjects, PreOps* preops) :
		UnaryOperator(child, '!') {
	this->allObjects = allObjects;
	this->preops = preops;
	this->nonEmptyDenot = 0;
	this->UpdateSimpleDenotations();
}

std::vector<int>* Not::GetInterpretation() {
	//this->UpdateInterpretation();
	return &(this->interpretation);
}
void Not::UpdateDenotations() {
	if (typeid(*child) == typeid(Not))
		return;
	std::vector<std::vector<int> > cDenot = this->child->GetDenotationVec();
	std::vector<int> tmpInterpretation;
	for (unsigned i = 0; i < cDenot.size(); ++i) {
		int k = 0;
		for (unsigned j = 0; j < allObjects->size(); ++j) {
			if (k < cDenot[i].size() && j == cDenot[i][k]) {
				k++;
				continue;
			} else
				tmpInterpretation.push_back(j);
		}
		if (tmpInterpretation.size() > 0)
			this->nonEmptyDenot++;
		this->denotations.push_back(tmpInterpretation);
		tmpInterpretation.clear();
	}
}

void Not::UpdateSimpleDenotations() {
	if (typeid(*child) == typeid(Not))
		return;
	std::vector<int>* cDenot = this->child->GetSimpleDenotationVec();
	std::map<int, int>* notMap = preops->GetNotMap();
	for (unsigned i = 0; i < cDenot->size(); ++i) {
		int notElem = (*notMap)[(*cDenot)[i]];
		if (notElem != 0)
			this->nonEmptyDenot++;
		this->simpleDenotations.push_back(notElem);
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

