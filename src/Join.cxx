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
Join::Join(Expression* left, Expression* right, PreOps* preops) :
		BinaryOperator('^') {
	this->SetLeft(left);
	this->SetRight(right);
	this->nonEmptyDenot = 0;
	this->preops = preops;
	this->UpdateSimpleDenotations();
}

void Join::UpdateDenotations() {
	std::vector<std::vector<int> > lDenot = this->left->GetDenotationVec();
	std::vector<std::vector<int> > rDenot = this->right->GetDenotationVec();
	if (lDenot.size() != rDenot.size()) {
		std::cout << "ERR Join";
		return;
	}
	std::vector<int> tmpInterpretation;
	for (unsigned i = 0; i < lDenot.size(); ++i) {
		std::vector<int>::iterator first1 = lDenot[i].begin();
		std::vector<int>::iterator last1 = lDenot[i].end();
		std::vector<int>::iterator first2 = rDenot[i].begin();
		std::vector<int>::iterator last2 = rDenot[i].end();
		std::set_intersection(first1, last1, first2, last2, std::back_inserter(tmpInterpretation));
		if (tmpInterpretation.size() > 0)
			this->nonEmptyDenot++;
		//std::sort(tmpInterpretation.begin(),tmpInterpretation.end());
		this->denotations.push_back(tmpInterpretation);
		tmpInterpretation.clear();
	}
}

void Join::UpdateSimpleDenotations() {
	std::vector<std::vector<int> >* subsets = preops->GetSubsets();
	std::map<std::pair<int, int>, int>* joinMap = preops->GetJoinMap();
	std::vector<int>* lDenot = this->left->GetSimpleDenotationVec();
	std::vector<int>* rDenot = this->right->GetSimpleDenotationVec();
	if (lDenot->size() != rDenot->size()) {
		std::cout << "ERR Join";
		return;
	}
	for (unsigned i = 0; i < lDenot->size(); ++i) {
		std::pair<int, int> p((*lDenot)[i], (*rDenot)[i]);
		if ((*joinMap)[p] != 0)
			this->nonEmptyDenot++;
		this->simpleDenotations.push_back((*joinMap)[p]);
	}
}

std::vector<int>* Join::GetInterpretation() {
	//this->UpdateInterpretation();
	return &(this->interpretation);
}

void Join::UpdateInterpretation() {
//	this->left->UpdateInterpretation();
//	this->right->UpdateInterpretation();
//	std::vector<int>::iterator first1 =
//			this->left->GetInterpretation()->begin();
//	std::vector<int>::iterator last1 = this->left->GetInterpretation()->end();
//	std::vector<int>::iterator first2 =
//			this->right->GetInterpretation()->begin();
//	std::vector<int>::iterator last2 = this->right->GetInterpretation()->end();
//
//	this->ClearInterpretation();
//
//	std::set_intersection(first1, last1, first2, last2,
//			std::back_inserter(this->interpretation));
	//std::sort(this->interpretation.begin(),this->interpretation.end());
}

Join::~Join() {
	// TODO Auto-generated destructor stub
}
}

