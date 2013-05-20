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
#include "State.hxx"
//#include <planning/Types.hxx>
//#include <planning/PDDL_Type.hxx>

namespace expression {
class Expression {
protected:
	int level;
	bool isRole;
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
	virtual int GetNonEmptyDenotationNum()=0;
	virtual std::vector<std::vector<int> > GetDenotationVec()= 0;
	virtual std::vector<std::vector<std::pair<int, int> > > GetDenotationRoleVec()= 0;

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
