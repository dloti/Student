/*
 * Operator.hxx
 *
 *  Created on: Feb 26, 2013
 *      Author: dloti
 */

#ifndef OPERATOR_HXX_
#define OPERATOR_HXX_
#include <typeinfo>
#include "Expression.hxx"
namespace expression {
class Operator: public Expression {
protected:
	char op;
	int nonEmptyDenot;
	std::vector<std::vector<int> > denotations;
	std::vector<std::vector<std::pair<int, int> > > denotationsRole;
	std::vector<int> interpretation;
	std::vector<std::pair<int, int> > roleInterpretation;
public:
	Operator(char op);
	void ClearInterpretation();
	std::vector<int>* GetInterpretation();
	std::vector<std::pair<int, int> >* GetRoleInterpretation();

	inline int GetNonEmptyDenotationNum() {
		return this->nonEmptyDenot;
	}

	inline std::vector<std::vector<int> > GetDenotationVec() {
		return denotations;
	}

	inline std::vector<std::vector<std::pair<int, int> > > GetDenotationRoleVec() {
		return denotationsRole;
	}

	bool EqualDenotationVec(Expression* exp, int runCount);
	virtual void UpdateDenotations()=0;
	virtual ~Operator();
};
}

#endif /* OPERATOR_HXX_ */
