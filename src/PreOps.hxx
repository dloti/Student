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
class PreOps {
	int obj_num, subset_num;
	std::vector<std::vector<int> > subsets;
	std::map<int, int> nots;
	std::map<std::pair<int,int>, int> joins;

	void printv(int mask[]);
	int NextMask(int mask[]);
	void AddSubset(int mask[]);
	void Not();
	void Join();
public:
	PreOps(int obj_num);
	void MakeSubsets();
	virtual ~PreOps();
};

#endif /* PREOPS_HXX_ */
