/*
 * Not.hxx
 *
 *  Created on: Mar 1, 2013
 *      Author: dloti
 */

#ifndef NOT_HXX_
#define NOT_HXX_
#include "UnaryOperator.hxx"
namespace expression {
class Not :public UnaryOperator {
	std::vector<int>* allObjects;
public:
	Not(Expression* child, std::vector<int>* allObjects,  PreOps* preops);
	std::vector<int>* GetInterpretation();
	void UpdateDenotations();
	void UpdateSimpleDenotations();
	void UpdateInterpretation();
	virtual ~Not();
};
}

#endif /* NOT_HXX_ */
