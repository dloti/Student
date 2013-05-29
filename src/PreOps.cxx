/*
 * PreOps.cxx
 *
 *  Created on: May 28, 2013
 *      Author: dloti
 */

#include "PreOps.hxx"

PreOps::PreOps(int obj_num) {
	this->obj_num = obj_num;
	this->subset_num = (int) pow(2., obj_num);
	MakeSubsets();
	Not();
	Join();
}

int PreOps::NextMask(int mask[]) {
	int i;
	for (i = 0; (i < obj_num) && mask[i]; ++i)
		mask[i] = 0;

	if (i < obj_num) {
		mask[i] = 1;
		return 1;
	}
	return 0;
}

void PreOps::printv(int mask[]) {
	int i;
	printf("{ ");
	for (i = 0; i < obj_num; ++i)
		if (mask[i])
			printf("%d ", i); /*i+1 is part of the subset*/
	printf("}\n");
}

void PreOps::AddSubset(int mask[]) {
	std::vector<int> tmpvec;
	for (int i = 0; i < obj_num; ++i)
		if (mask[i])
			tmpvec.push_back(i);
	subsets.push_back(tmpvec);
}

void PreOps::MakeSubsets() {
	int mask[subset_num];
	int i;
	for (i = 0; i < obj_num; ++i)
		mask[i] = 0;
	AddSubset(mask);
	while (NextMask(mask)) {
		AddSubset(mask);
	}
}

void PreOps::Not() {
	std::vector<std::vector<int> >::iterator it;
	for (unsigned i = 0; i < subsets.size(); ++i) {
		int k = 0;
		std::vector<int> tmpInterpretation;
		for (unsigned j = 0; j < obj_num; ++j) {
			if (k < subsets[i].size() && j == subsets[i][k]) {
				k++;
				continue;
			} else
				tmpInterpretation.push_back(j);
		}
		it = std::find(subsets.begin(), subsets.end(), tmpInterpretation);
		tmpInterpretation.clear();
		nots[i] = (int) (it - subsets.begin());
	}
}

void PreOps::Join() {
	std::vector<std::vector<int> >::iterator it;
	std::pair<int, int>* p;
	for (unsigned i = 0; i < subsets.size(); ++i) {
		std::vector<int>::iterator first1 = subsets[i].begin();
		std::vector<int>::iterator last1 = subsets[i].end();
		for (unsigned j = 0; j < subsets.size(); ++j) {
			std::vector<int> tmpInterpretation;
			std::vector<int>::iterator first2 = subsets[j].begin();
			std::vector<int>::iterator last2 = subsets[j].end();
			std::set_intersection(first1, last1, first2, last2, std::back_inserter(tmpInterpretation));
			p = new std::pair<int, int>(i, j);
			it = std::find(subsets.begin(), subsets.end(), tmpInterpretation);
			joins[(*p)] = (int) (it - subsets.begin());
			delete p;
		}
	}
}

void PreOps::TestPrint() {
	for (unsigned i = 0; i < subsets.size(); ++i) {
		for (unsigned j = 0; j < subsets[i].size(); ++j) {
			std::cout << subsets[i][j] << " ";
		}
		std::cout << std::endl;
	}
	std::map<std::pair<int, int>, int>::iterator itr;
	for (itr = joins.begin(); itr != joins.end(); ++itr) {
		std::cout << (itr->first).first << " " << (itr->first).second << "-" << itr->second << std::endl;
	}
}

PreOps::~PreOps() {
	// TODO Auto-generated destructor stub
}

