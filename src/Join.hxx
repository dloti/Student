/*
 * Join.hxx
 *
 *  Created on: Feb 27, 2013
 *      Author: dloti
 */

#ifndef JOIN_HXX_
#define JOIN_HXX_
#include "BinaryOperator.hxx"
#include "PreOps.hxx"
namespace expression {
class Join : public BinaryOperator {
public:
	Join(Expression* left, Expression* right, PreOps* preops);
	void UpdateDenotations();
	void UpdateSimpleDenotations();
	std::vector<int>* GetInterpretation();
	void UpdateInterpretation();
	virtual ~Join();
};
}

#endif /* JOIN_HXX_ */
