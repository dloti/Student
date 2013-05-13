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
	std::vector<std::string>* allObjects;
	std::vector<std::vector<int> > denotations;
	std::vector<std::vector<std::pair<int, int> > > denotationsRole;
public:
	Expression();
	virtual void print(std::ostream& s) const = 0;
	virtual void infix(std::ostream& s) const = 0;
	virtual void UpdateInterpretation() = 0;
	virtual void ClearInterpretation() = 0;
	virtual std::vector<int>* GetInterpretation() = 0;
	virtual std::vector<std::pair<int, int> >* GetRoleInterpretation() = 0;
	virtual std::vector<Expression*> GetChildren() = 0;

	inline bool IsRole() {
		return isRole;
	}
	inline void SetRole(bool isRole) {
		this->isRole = isRole;
	}
	inline bool SetLevel() {
		return level;
	}
	inline void GetLevel(int level) {
		this->level = level;
	}

	inline void SetObjects(std::vector<std::string>* objects) {
		this->allObjects = objects;
	}

	inline std::vector<std::string>* GetObjects() {
		return this->allObjects;
	}

	inline std::vector<std::vector<int> > GetDenotationVec() {
		return denotations;
	}

	inline std::vector<std::vector<std::pair<int, int> > > GetDenotationRoleVec() {
		return denotationsRole;
	}
	virtual ~Expression();
};
}
#endif /* COMPOUNDCONCEPT_HXX_ */
