/*
 * CompoundConcept.cxx
 *
 *  Created on: Feb 26, 2013
 *      Author: dloti
 */

#include "Expression.hxx"

namespace expression {
Expression::Expression() {
	this->isRole = false;
	hitSets = 0;
	isHitting = false;
}

Expression::~Expression() {
}
}
