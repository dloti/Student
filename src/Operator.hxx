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
class Operator : public Expression {
protected:
	char op;
	std::vector<int> interpretation;
	std::vector<std::pair<int,int> > roleInterpretation;
public:
	Operator(char op);
	void ClearInterpretation();
	std::vector<int>* GetInterpretation();
	std::vector<std::pair<int,int> >* GetRoleInterpretation();
	bool EqualDenotationVec(Expression* exp);
	virtual void UpdateDenotations()=0;
	virtual ~Operator();
};
}

#endif /* OPERATOR_HXX_ */
