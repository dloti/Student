/*
 * Equality.hxx
 *
 *  Created on: Mar 11, 2013
 *      Author: dloti
 */

#ifndef EQUALITY_HXX_
#define EQUALITY_HXX_
#include "BinaryOperator.hxx"
namespace expression {
class Equality : public BinaryOperator {
public:
	Equality(Expression* left, Expression* right,  PreOps* preops);
	void UpdateDenotations();
	void UpdateSimpleDenotations();
	std::vector<int>* GetInterpretation();
	void UpdateInterpretation();
	virtual ~Equality();
};
}

#endif /* EQUALITY_HXX_ */
