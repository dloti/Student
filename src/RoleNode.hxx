/*
 * RoleNode.hxx
 *
 *  Created on: Feb 28, 2013
 *      Author: dloti
 */

#ifndef ROLENODE_HXX_
#define ROLENODE_HXX_
#include "Operand.hxx"
#include <vector>
namespace expression {
class RoleNode : public Operand {
protected:
	std::vector<std::pair<int,int> > interpretation;
public:
	RoleNode();
	RoleNode(std::string predicate);
	std::vector<int>* GetInterpretation();
	std::vector<std::pair<int,int> >* GetRoleInterpretation();
	void UpdateInterpretation();
	void UpdateDenotations(std::vector<Instance> instances, std::vector<int>* allObjects);
	void ClearInterpretation();
	virtual ~RoleNode();
};
}

#endif /* ROLENODE_HXX_ */
