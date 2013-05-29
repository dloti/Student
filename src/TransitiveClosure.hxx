/*
 * TransitiveClosure.hxx
 *
 *  Created on: Mar 12, 2013
 *      Author: dloti
 */

#ifndef TRANSITIVECLOSURE_HXX_
#define TRANSITIVECLOSURE_HXX_
#include "UnaryOperator.hxx"
namespace expression {
class TransitiveClosure : public UnaryOperator {
public:
	TransitiveClosure(Expression* child);
	std::vector<std::pair<int,int> >* GetRoleInterpretation();
	void UpdateDenotations();
	void UpdateSimpleDenotations(){};
	void UpdateInterpretation();
	virtual ~TransitiveClosure();
};
}

#endif /* TRANSITIVECLOSURE_HXX_ */
