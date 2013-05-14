/*
 * CompoundConcept.cxx
 *
 *  Created on: Feb 26, 2013
 *      Author: dloti
 */

#include "Expression.hxx"

namespace expression {
Expression::Expression() {
	this->level = 0;
	this->isRole = false;
	this->nonEmptyDenot = 0;
}

Expression::~Expression() {
}
}
