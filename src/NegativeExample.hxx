/*
 * NegativeExample.hxx
 *
 *  Created on: Nov 20, 2013
 *      Author: dloti
 */

#ifndef NEGATIVEEXAMPLE_HXX_
#define NEGATIVEEXAMPLE_HXX_
#include<string>

class NegativeExample {
	std::string action;
	int action_idx;
	int obj;
public:
	NegativeExample(std::string action, int action_idx, int obj);
	inline  std::string GetAction() const {
		return action;
	}
	inline  int GetActionIdx() const{
		return action_idx;
	}
	inline  int GetObj() const {
		return obj;
	}
	inline bool operator==(const NegativeExample &other) const {
		if (this->action.compare(other.GetAction()) == 0 && this->action_idx == other.GetActionIdx()
				&& this->obj == other.GetObj())
			return true;
		return false;
	}

	inline bool operator!=(const NegativeExample &other) const {
		return !(*this == other);
	}
	virtual ~NegativeExample();
};

#endif /* NEGATIVEEXAMPLE_HXX_ */
