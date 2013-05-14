/*
 * Join.cxx
 *
 *  Created on: Feb 27, 2013
 *      Author: dloti
 */
#include<algorithm>
#include<iostream>
#include "Join.hxx"
namespace expression {
Join::Join(Expression* left, Expression* right) :
		BinaryOperator('^') {
	this->SetLeft(left);
	this->SetRight(right);
}

void Join::UpdateDenotations() {
	std::vector<std::vector<int> > lDenot = this->left->GetDenotationVec();
	std::vector<std::vector<int> > rDenot = this->right->GetDenotationVec();
	if (lDenot.size() != rDenot.size()) {
		std::cout << "ERR Join";
		return;
	}
	for (unsigned i = 0; i < lDenot.size(); ++i) {
		std::vector<int> tmpInterpretation;
		std::vector<int>::iterator first1 =
				lDenot[i].begin();
		std::vector<int>::iterator last1 =
				lDenot[i].end();
		std::vector<int>::iterator first2 =
				rDenot[i].begin();
		std::vector<int>::iterator last2 =
				rDenot[i].end();
		std::set_intersection(first1, last1, first2, last2,
					std::back_inserter(tmpInterpretation));
		if(tmpInterpretation.size()>0) this->nonEmptyDenot++;
		this->denotations.push_back(tmpInterpretation);
	}
}

std::vector<int>* Join::GetInterpretation() {
	//this->UpdateInterpretation();
	return &(this->interpretation);
}

void Join::UpdateInterpretation() {
	this->left->UpdateInterpretation();
	this->right->UpdateInterpretation();
	std::vector<int>::iterator first1 =
			this->left->GetInterpretation()->begin();
	std::vector<int>::iterator last1 = this->left->GetInterpretation()->end();
	std::vector<int>::iterator first2 =
			this->right->GetInterpretation()->begin();
	std::vector<int>::iterator last2 = this->right->GetInterpretation()->end();

	this->ClearInterpretation();

	std::set_intersection(first1, last1, first2, last2,
			std::back_inserter(this->interpretation));
	//std::sort(this->interpretation.begin(),this->interpretation.end());
}

Join::~Join() {
	// TODO Auto-generated destructor stub
}
}

