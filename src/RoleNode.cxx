/*
 * RoleNode.cxx
 *
 *  Created on: Feb 28, 2013
 *      Author: dloti
 */

#include "RoleNode.hxx"
namespace expression {
RoleNode::RoleNode() {
}

RoleNode::RoleNode(std::string predicate) {
	this->predicate = predicate;
}

void RoleNode::UpdateDenotations(std::vector<Instance> instances, std::vector<int>* allObjects) {
	for (unsigned i = 0; i < instances.size(); ++i) {
		std::vector<State>* states = instances[i].GetStates();
		for (unsigned j = 0; j < states->size(); ++j) {
			if (this->goal)
				this->denotationsRole.push_back(instances[i].GetGoal().GetRoleInterpretation(this->predicate));
			else
				this->denotationsRole.push_back((*states)[j].GetRoleInterpretation(this->predicate));
		}
	}
}

RoleNode::~RoleNode() {
	// TODO Auto-generated destructor stub
}

std::vector<int>* RoleNode::GetInterpretation() {
	return NULL;
}

std::vector<std::pair<int, int> >* RoleNode::GetRoleInterpretation() {
	return &(this->interpretation);
}

void RoleNode::UpdateInterpretation() {
	//TODO
}

void RoleNode::ClearInterpretation() {
	this->interpretation.clear();
}
}
