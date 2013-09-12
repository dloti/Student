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
	int level;
	std::vector<int> interpretation;
	std::vector<std::pair<int, int> > roleInterpretation;
public:
	Operator(char op);
	Operator(char op, int lvl);
	void ClearInterpretation();

	std::vector<int>* GetInterpretation();
	std::vector<std::pair<int, int> >* GetRoleInterpretation();

	inline bool EqualSimpleDenotationVec(Expression* exp) {
		if (exp->GetNonEmptyDenotationNum() != this->nonEmptyDenot)
			return false;

		this->SimplifyDenotations();
		exp->SimplifyDenotations();

		std::vector<int>* sdenot = exp->GetSimpleDenotationVec();

		for (unsigned i = 0; i < simpleDenotations.size(); ++i) {
			if (this->simpleDenotations[i] != (*sdenot)[i])
				return false;
		}
		return true;
	}

	bool EqualDenotationVec(Expression* exp);
	virtual void UpdateDenotations()=0;
	virtual void UpdateSimpleDenotations() = 0;
	inline int GetLevel() {
		return level;
	}

	inline void SetLevel(int lvl) {
		this->level = lvl;
	}
	virtual ~Operator();
};
}

#endif /* OPERATOR_HXX_ */
