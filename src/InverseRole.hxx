/*
 * InverseRole.hxx
 *
 *  Created on: Mar 12, 2013
 *      Author: dloti
 */

#ifndef INVERSEROLE_HXX_
#define INVERSEROLE_HXX_
#include "UnaryOperator.hxx"
namespace expression {
class InverseRole : public UnaryOperator{
public:
	InverseRole(Expression* child);
	std::vector<std::pair<int,int> >* GetRoleInterpretation();
	void UpdateDenotations();
	void UpdateSimpleDenotations(){};
	void UpdateInterpretation();
	virtual ~InverseRole();
};
}

#endif /* INVERSEROLE_HXX_ */
