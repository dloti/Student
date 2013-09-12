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

void ConceptNode::UpdateDenotations(std::vector<Instance> instances, std::vector<int>* allObjects) {
	this->nonEmptyDenot = 0;
	if (this->predicate.compare("OBJECT") == 0) {
		std::vector<int> vec;
		for (unsigned k = 0; k < allObjects->size(); ++k)
			vec.push_back(k);
		for (unsigned i = 0; i < instances.size(); ++i) {
			std::vector<State>* states = instances[i].GetStates();
			for (unsigned j = 0; j < states->size(); ++j) {
				this->nonEmptyDenot++;
				this->denotations.push_back(vec);
			}
		}
		return;
	}

	for (unsigned i = 0; i < instances.size(); ++i) {
		std::vector<State>* states = instances[i].GetStates();
		for (unsigned j = 0; j < states->size(); ++j) {
			if (this->goal) {
				std::vector<int> vec = instances[i].GetGoal().GetConceptInterpretation(this->predicate);
				if(vec.size()>0) this->nonEmptyDenot++;
				sort(vec.begin(),vec.end());
				this->denotations.push_back(vec);
			} else {
				std::vector<int> vec = (*states)[j].GetConceptInterpretation(this->predicate);
				if(vec.size()>0) this->nonEmptyDenot++;
				sort(vec.begin(),vec.end());
				this->denotations.push_back(vec);
			}
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

