/*
 * Equality.cxx
 *
 *  Created on: Mar 11, 2013
 *      Author: dloti
 */

#include "Equality.hxx"
namespace expression {
Equality::Equality(Expression* left, Expression* right,  PreOps* preops) :
		BinaryOperator('=') {
	this->SetLeft(left);
	this->SetRight(right);
	this->preops = preops;
	this->UpdateDenotations();
}

std::vector<int>* Equality::GetInterpretation() {
	//this->UpdateInterpretation();
	return &(this->interpretation);
}

void Equality::UpdateDenotations() {
	this->nonEmptyDenot = 0;
	std::vector<std::vector<std::pair<int, int> > > lDenot = this->left->GetDenotationRoleVec();
	std::vector<std::vector<std::pair<int, int> > > rDenot = this->right->GetDenotationRoleVec();
	if (lDenot.size() != rDenot.size()) {
		std::cout << "ERR Equality";
		return;
	}
	std::vector<int> tmpInterpretation;
	for (unsigned i = 0; i < lDenot.size(); ++i) {

		std::vector<std::pair<int, int> >::iterator it = rDenot[i].begin(), last = rDenot[i].end(), it1 =
				lDenot[i].begin(), last1 = lDenot[i].end();

		if (lDenot[i].size() < rDenot[i].size()) {
			it1 = rDenot[i].begin();
			last1 = rDenot[i].end();
			it = lDenot[i].begin();
			last = lDenot[i].end();
		}

		const std::vector<std::pair<int, int> >::iterator tmp = it1;
		for (; it != last; ++it) {
			for (it1 = tmp; it1 != last1; ++it1) {
				if (it->first == it1->first && it->second == it1->second
						&& std::find(tmpInterpretation.begin(), tmpInterpretation.end(), it->first)
								== tmpInterpretation.end()) {
					tmpInterpretation.push_back(it->first);
					break;
				}
			}
		}

		if (tmpInterpretation.size() > 0)
			this->nonEmptyDenot++;
		std::sort(tmpInterpretation.begin(), tmpInterpretation.end());
		this->denotations.push_back(tmpInterpretation);
		tmpInterpretation.clear();
	}
}

void Equality::UpdateSimpleDenotations(){
	this->SimplifyDenotations();
}

void Equality::UpdateInterpretation() {
	this->left->UpdateInterpretation();
	this->right->UpdateInterpretation();
	ClearInterpretation();
	std::vector<std::pair<int, int> >* leftInterpretation = (this->left)->GetRoleInterpretation();
	std::vector<std::pair<int, int> >* rightInterpretation = (this->right)->GetRoleInterpretation();

	if (leftInterpretation == NULL || leftInterpretation->size() == 0 || rightInterpretation == NULL
			|| rightInterpretation->size() == 0)
		return;

	std::vector<std::pair<int, int> >::iterator it = rightInterpretation->begin(), last = rightInterpretation->end(),
			it1 = leftInterpretation->begin(), last1 = leftInterpretation->end();
	if (leftInterpretation->size() < rightInterpretation->size()) {
		it1 = rightInterpretation->begin();
		last1 = rightInterpretation->end();
		it = leftInterpretation->begin();
		last = leftInterpretation->end();
	}
	const std::vector<std::pair<int, int> >::iterator tmp = it1;
	for (; it != last; ++it) {
		for (it1 = tmp; it1 != last1; ++it1) {
			if (it->first == it1->first && it->second == it1->second
					&& std::find(this->interpretation.begin(), this->interpretation.end(), it->first)
							== this->interpretation.end()) {
				this->interpretation.push_back(it->first);
				break;
			}
		}
	}

	std::sort(this->interpretation.begin(), this->interpretation.end());
}

Equality::~Equality() {
	// TODO Auto-generated destructor stub
}
}

