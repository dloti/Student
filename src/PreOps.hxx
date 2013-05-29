/*
 * PreOps.hxx
 *
 *  Created on: May 28, 2013
 *      Author: dloti
 */

#ifndef PREOPS_HXX_
#define PREOPS_HXX_
#include <cstdio>
#include <cmath>
#include <vector>
#include <map>
#include <iostream>
#include <algorithm>
class PreOps {
	int obj_num, subset_num;
	std::vector<std::vector<int> > subsets;
	std::map<int, int> nots;
	std::map<std::pair<int,int>, int> joins;
	void MakeSubsets();
	void printv(int mask[]);
	int NextMask(int mask[]);
	void AddSubset(int mask[]);
	void Not();
	void Join();

public:
	PreOps(int obj_num);
	inline std::vector<std::vector<int> >* GetSubsets(){
		return &subsets;
	}

	inline std::map<std::pair<int,int>, int>* GetJoinMap(){
		return &joins;
	}

	inline std::map<int, int>* GetNotMap(){
			return &nots;
		}

	void TestPrint();
	virtual ~PreOps();
};

#endif /* PREOPS_HXX_ */
