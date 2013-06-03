/*
 * CompoundConcept.hxx
 *
 *  Created on: Feb 26, 2013
 *      Author: dloti
 */

#ifndef COMPOUNDCONCEPT_HXX_
#define COMPOUNDCONCEPT_HXX_
#include <string>
#include <iostream>
#include <vector>
#include <algorithm>
#include "Instance.hxx"
#include "PreOps.hxx"
#include "State.hxx"
//#include <planning/Types.hxx>
//#include <planning/PDDL_Type.hxx>

namespace expression {
class Expression {
protected:
	PreOps* preops;
	int level;
	bool isRole;
	int nonEmptyDenot;
	std::vector<int> simpleDenotations;
	std::vector<std::vector<int> > denotations;
	std::vector<std::vector<std::pair<int, int> > > denotationsRole;
public:
	Expression();
	virtual void print(std::ostream& s) const = 0;
	virtual void infix(std::ostream& s) const = 0;
	virtual void prefix(std::ostream& s) const = 0;
	virtual void UpdateInterpretation() = 0;
	virtual void ClearInterpretation() = 0;
	virtual std::vector<int>* GetInterpretation() = 0;
	virtual std::vector<std::pair<int, int> >* GetRoleInterpretation() = 0;
	virtual std::vector<Expression*> GetChildren() = 0;
	inline void SimplifyDenotations() {
		if (simpleDenotations.size() > 0)
			return;
		std::vector<std::vector<int> >* subsets = preops->GetSubsets();
		std::vector<std::vector<int> >::iterator it;
		for (unsigned i = 0; i < this->denotations.size(); ++i) {
			it = std::find(subsets->begin(), subsets->end(), denotations[i]);
			if (it == subsets->end())
				std::cout << "ERR denotation simplifier" << std::endl;
			//		for (unsigned j = 0; j < denotations[i].size(); ++j)
			//			std::cout << denotations[i][j] << " ";
			//		std::cout << std::endl;
			simpleDenotations.push_back((it - subsets->begin()));
		}
	}

	inline void SetPreops(PreOps* preops) {
		this->preops = preops;
	}

	inline int GetNonEmptyDenotationNum() {
		return this->nonEmptyDenot;
	}

	inline std::vector<bool> GetSignature() {
		std::vector<bool> ret;
		if (simpleDenotations.size() == 0) {
			for (unsigned i = 0; i < denotations.size(); ++i)
				ret.push_back(denotations[i].size());
			return ret;
		}
		for (unsigned i = 0; i < simpleDenotations.size(); ++i)
			ret.push_back(simpleDenotations[i]);
		return ret;
	}

	inline std::vector<std::vector<int> > GetDenotationVec() {
		return denotations;
	}

	inline std::vector<int>* GetSimpleDenotationVec() {
		return &simpleDenotations;
	}

	inline std::vector<std::vector<std::pair<int, int> > > GetDenotationRoleVec() {
		return denotationsRole;
	}

	inline bool IsRole() {
		return isRole;
	}
	inline void SetRole(bool isRole) {
		this->isRole = isRole;
	}
	inline bool GetLevel() {
		return level;
	}
	inline void SetLevel(int level) {
		this->level = level;
	}
	virtual ~Expression();
};
}
#endif /* COMPOUNDCONCEPT_HXX_ */
