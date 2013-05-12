/*
 * CompoundConcept.hxx
 *
 *  Created on: Feb 26, 2013
 *      Author: dloti
 */

#ifndef COMPOUNDCONCEPT_HXX_
#define COMPOUNDCONCEPT_HXX_
#include <string>
#include <iostream>
#include <vector>
namespace expression {
class CompoundConcept {
public:
	CompoundConcept();
	virtual void print(std::ostream& s) const = 0;
	virtual void infix(std::ostream& s) const = 0;
	virtual void UpdateInterpretation() = 0;
	virtual void ClearInterpretation() = 0;
	virtual std::vector<int>* GetInterpretation();
    virtual ~CompoundConcept();
};
}

#endif /* COMPOUNDCONCEPT_HXX_ */
