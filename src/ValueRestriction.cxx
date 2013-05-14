/*
 * ValueRestriction.cxx
 *
 *  Created on: Mar 1, 2013
 *      Author: dloti
 */

#include "ValueRestriction.hxx"
#include<vector>
namespace expression {
ValueRestriction::ValueRestriction(Expression* left, Expression* right) :
		BinaryOperator('.') {
	this->SetLeft(left);
	this->SetRight(right);
	this->UpdateDenotations();
}

void ValueRestriction::SetLeft(Expression* left) {
	this->left = left;
}
void ValueRestriction::SetRight(Expression* right) {
	this->right = right;
}
std::vector<int>* ValueRestriction::GetInterpretation() {
	//this->UpdateInterpretation();
	return &(this->interpretation);
}

void ValueRestriction::UpdateDenotations() {
	std::vector<std::vector<std::pair<int, int> > > rDenot = this->left->GetDenotationRoleVec();
	std::vector<std::vector<int> > cDenot = this->right->GetDenotationVec();
	if (cDenot.size() != rDenot.size()) {
		std::cout << "ERR Value restriction";
		return;
	}

	for (unsigned i = 0; i < rDenot.size(); ++i) {
		std::vector<int> tmpInterpretation;
		std::vector<int> riFirst;
		std::vector<std::pair<int, int> >::iterator pairIterator;
		std::vector<int>::iterator it, end;
		for (pairIterator = rDenot[i].begin(); pairIterator != rDenot[i].end(); ++pairIterator) {
			riFirst.push_back(pairIterator->second);
		}
		it = riFirst.begin();
		end = riFirst.end();
		std::vector<int>::iterator first2 = cDenot[i].begin();
		std::vector<int>::iterator last2 = cDenot[i].end();
		std::set_intersection(it, end, first2, last2, std::back_inserter(tmpInterpretation));
		if(tmpInterpretation.size()>0) this->nonEmptyDenot++;
		this->denotations.push_back(tmpInterpretation);
	}
}

void ValueRestriction::UpdateInterpretation() {
	this->left->UpdateInterpretation();
	this->right->UpdateInterpretation();
	std::vector<std::pair<int, int> >* ri = (this->left)->GetRoleInterpretation();
	//(dynamic_cast<RoleNode*>(this->left))->GetRoleInterpretation();
	std::vector<int> riFirst;
	std::vector<std::pair<int, int> >::iterator pairIterator;
	std::vector<int>::iterator it, end;
	for (pairIterator = ri->begin(); pairIterator != ri->end(); ++pairIterator) {
		riFirst.push_back(pairIterator->second);
	}
	it = riFirst.begin();
	end = riFirst.end();
	std::vector<int>::iterator first2 = this->right->GetInterpretation()->begin();
	std::vector<int>::iterator last2 = this->right->GetInterpretation()->end();

	this->ClearInterpretation();
	std::set_intersection(it, end, first2, last2, std::back_inserter(this->interpretation));
}

ValueRestriction::~ValueRestriction() {
	// TODO Auto-generated destructor stub
}
}

