/*
 * ConceptNode.cxx
 *
 *  Created on: Feb 14, 2013
 *      Author: dloti
 */

#include "ConceptNode.hxx"
#include <algorithm>
namespace expression {
ConceptNode::ConceptNode() {
	this->goal = false;
	this->predicate = "";
}

ConceptNode::ConceptNode(std::string predicate) {
	this->goal = false;
	this->predicate = predicate;
}

ConceptNode::~ConceptNode() {
}

void ConceptNode::UpdateInterpretation() {
}

void ConceptNode::UpdateDenotations(std::vector<Instance> instances) {
	if (this->predicate.compare("OBJECT") == 0) {
		std::vector<int> vec;
		for (unsigned k = 0; k < allObjects->size(); ++k)
			vec.push_back(k);

		for (unsigned i = 0; i < instances.size(); ++i) {
			std::vector<State> states = instances[i].GetStates();
			for (unsigned j = 0; j < states.size(); ++j) {
				this->denotations.push_back(vec);
			}
		}
		return;
	}

	for (unsigned i = 0; i < instances.size(); ++i) {
		std::vector<State> states = instances[i].GetStates();
		for (unsigned j = 0; j < states.size(); ++j) {
			if (this->goal)
				this->denotations.push_back(instances[i].GetGoal().GetConceptInterpretation(this->predicate));
			else
				this->denotations.push_back(states[j].GetConceptInterpretation(this->predicate));
		}
	}
}

void ConceptNode::ClearInterpretation() {
	this->interpretation.clear();
}

std::vector<std::pair<int, int> >* ConceptNode::GetRoleInterpretation() {
	return NULL;
}

std::vector<int>* ConceptNode::GetInterpretation() {
	return &(this->interpretation);
}
}

