/*
 * Rule.cxx
 *
 *  Created on: Feb 1, 2013
 *      Author: dloti
 */

#include "Rule.hxx"

Rule::Rule(expression::Expression* concept, std::string action) {
	this->coverage = 0;
	this->correct = 0;
	this->examples = 1;
	this->action = action;
	this->concept = concept;
}

std::string Rule::GetAction() {
	return this->action;
}

void Rule::AddConcept(expression::Expression* concept) {
	this->concept = concept;
}

expression::Expression* Rule::GetConcept() {
	return this->concept;
}

bool operator ==(Rule& r1, Rule& r2) {
	return ((r1.concept != r2.concept)); //&& (r1.action->name() == r2.action->name()));
}

bool operator !=(Rule& r1, Rule& r2) {
	return !(r1 == r2);
}

std::ostream& operator <<(std::ostream& out, Rule& r) {
	r.concept->infix(out); out << ":" << r.action<<" Correct:"<<r.correct;
	return out;
}

bool Rule::operator <(const Rule& r2) const {
		return (this->correct > r2.correct);
}

void Rule::SaveRule(std::ostream &out){
	this->concept->prefix(out);
	out<<std::endl;
	out<<this->action;
	out<<std::endl;
}

//int Rule::GetCurrentCoverage() {
//	return this->concepts[0]->GetInterpretation()->size();
//}

Rule::~Rule() {
	// TODO Auto-generated destructor stub
}

