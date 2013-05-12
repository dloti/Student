/*
 * Relation.hxx
 *
 *  Created on: Feb 26, 2013
 *      Author: dloti
 */

#ifndef RELATION_HXX_
#define RELATION_HXX_
#include "Operand.hxx"
namespace expression {
class Relation : public Operand {
public:
	Relation();
	virtual ~Relation();
};
}

#endif /* RELATION_HXX_ */
