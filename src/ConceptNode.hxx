/*
 * ConceptNode.hxx
 *
 *  Created on: Feb 14, 2013
 *      Author: dloti
 */

#ifndef CONCEPTNODE_HXX_
#define CONCEPTNODE_HXX_

#include "Operand.hxx"
namespace expression {
class ConceptNode : public Operand {
private:
	std::vector<int> interpretation;

public:
	ConceptNode();
	ConceptNode(std::string predicate);
	std::vector<std::pair<int,int> >* GetRoleInterpretation();
	virtual ~ConceptNode();
	void UpdateInterpretation();
	void UpdateDenotations(std::vector<Instance> instances, std::vector<int>* allObjects);
	void ClearInterpretation();
	std::vector<int>* GetInterpretation();
	//friend std::ostream& operator<< (std::ostream &out, ConceptNode &cNode);
//	void PrintConcept();
};
}

#endif /* CONCEPTNODE_HXX_ */
