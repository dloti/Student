#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <typeinfo>
#include <cmath>
#include <map>
#include <vector>
#include <algorithm>
#include <bitset>
#include <stdlib.h>
#include "Instance.hxx"
#include "Expression.hxx"
#include "ConceptNode.hxx"
#include "RoleNode.hxx"
#include "BinaryOperator.hxx"
#include "Join.hxx"
#include "Not.hxx"
#include "InverseRole.hxx"
#include "TransitiveClosure.hxx"
#include "Equality.hxx"
#include "ValueRestriction.hxx"
#include "Rule.hxx"

using namespace std;
using namespace expression;

vector<Expression*> rootConcepts;
vector<Expression*> rootRoles;
vector<string> primitiveConcepts;
vector<string> primitiveRoles;
vector<string> allObjects;
vector<int> allObjectsIdx;
vector<string> actions;
vector<vector<bool> > actionDenotations;
vector<Instance> instances;
vector<Rule> ruleSet;
int runCount = 0;

void initialize_concepts() {
	for (unsigned i = 0; i < allObjects.size(); ++i)
		allObjectsIdx.push_back(i);

	for (unsigned i = 0; i < primitiveConcepts.size(); ++i) {
		ConceptNode* c = new ConceptNode(primitiveConcepts[i]);
		rootConcepts.push_back(c);
		c = new ConceptNode(primitiveConcepts[i]);
		c->IsGoal(true);
		rootConcepts.push_back(c);
	}

	for (unsigned i = 0; i < primitiveRoles.size(); ++i) {
		RoleNode* r = new RoleNode(primitiveRoles[i]);
		rootRoles.push_back(r);
		r = new RoleNode(primitiveRoles[i]);
		r->IsGoal(true);
		rootRoles.push_back(r);
	}
}

void combine_roles(bool first = true) {
	vector<Expression*>::iterator roleIt;
	vector<Expression*> candidates;
	UnaryOperator* uo;
	if (first) {
		for (roleIt = rootRoles.begin(); roleIt < rootRoles.end(); ++roleIt) {
			uo = new InverseRole(*roleIt);
			candidates.push_back(uo);
			uo = new TransitiveClosure(*roleIt);
			candidates.push_back(uo);
		}
	}
	//TODO Composition
	for (unsigned i = 0; i < candidates.size(); ++i)
		rootRoles.push_back(candidates[i]);
}

void insert_candidate(Operator* exp, vector<Expression*>* candidates) {
	if (exp->GetNonEmptyDenotationNum() == 0) {
		delete exp;
		return;
	}

	for (unsigned i = 0; i < rootConcepts.size(); ++i) {
		if (exp->EqualDenotationVec(rootConcepts[i], runCount)) {
			delete exp;
			return;
		}
	}
	for (unsigned i = 0; i < candidates->size(); ++i) {
		if (exp->EqualDenotationVec((*candidates)[i], runCount)) {
			delete exp;
			return;
		}
	}
	candidates->push_back(exp);
	if (candidates->size() % 500 == 0)
		cout << "Candidates: " << candidates->size() << endl;
}

void combine_concepts() {
	vector<Expression*>::iterator conceptIt, conceptIt1, roleIt, roleIt1;
	vector<Expression*> candidates;
	//vector<Expression*> nextLayer = rootConcepts;
	UnaryOperator* uo;
	BinaryOperator* bo;
	bool hasCandidates = true;

	cout << "Denotation size: " << rootConcepts[0]->GetDenotationVec().size() << endl;
	while (hasCandidates) {
		cout << "Concepts: " << rootConcepts.size() << endl;
		for (conceptIt = rootConcepts.begin(); conceptIt < rootConcepts.end(); ++conceptIt) {
			uo = new Not(*conceptIt, &allObjectsIdx);
			insert_candidate(uo, &candidates);
			for (roleIt = rootRoles.begin(); roleIt < rootRoles.end(); ++roleIt) {
				bo = new ValueRestriction(*roleIt, *conceptIt);
				insert_candidate(bo, &candidates);

				for (roleIt1 = rootRoles.begin(); roleIt1 < rootRoles.end(); ++roleIt1) {
					if (roleIt == roleIt1)
						continue;
					bo = new Equality(*roleIt, *roleIt1);
					insert_candidate(bo, &candidates);
				}
			}
			for (conceptIt1 = rootConcepts.begin(); conceptIt1 < rootConcepts.end(); ++conceptIt1) {
				if (conceptIt == conceptIt1)
					continue;
				bo = new Join(*conceptIt, *conceptIt1);
				insert_candidate(bo, &candidates);
			}
		}

		for (unsigned i = 0; i < candidates.size(); ++i)
			rootConcepts.push_back(candidates[i]);
		//nextLayer = candidates;
		if (candidates.size() == 0)
			hasCandidates = false;
		candidates.clear();
		++runCount;
		cout << "Pass: " << runCount << endl;
	}
}

int get_obj_pos(string object) {
	int pos = std::find(allObjects.begin(), allObjects.end(), object) - allObjects.begin();
	if (pos >= allObjects.size()) {
		cout << "ERR " << object << endl;
		return -1;
	} else
		return pos;
}

void get_input() {
	string line;
	string field;
	ifstream fin("domain.txt");
	if (fin.is_open()) {
		int i = 0;
		while (!fin.eof() && getline(fin, line)) {
			istringstream iss(line);
			while (getline(iss, field, '\t')) {
				if (i == 0) {
					primitiveConcepts.push_back(field);
				} else if (i == 1) {
					primitiveRoles.push_back(field);
				} else if (i == 2) {
					allObjects.push_back(field);
				} else if (i == 3) {
					actions.push_back(field);
				}
			}
			i++;
		}
	}
	fin.close();
	fin.clear();
	fin.open("plans.txt");
	if (fin.is_open()) {
		while (!fin.eof() && getline(fin, line)) {
			Instance inst;
			istringstream iss(line);
			getline(iss, field, '\t');
			inst.SetNumPlan(atoi(field.c_str()));
			getline(iss, field, '\t');
			inst.SetNumActions(atoi(field.c_str()));

			for (int i = 0; i < inst.GetNumActions() + 1; ++i) {
				State s;
				vector<int> conceptInterpretation;
				vector<pair<int, int> > roleInterpretation;

				//for concepts
				for (unsigned j = 0; j < primitiveConcepts.size() - 1; ++j) {
					if (!fin.eof() && getline(fin, line)) {
						istringstream iss(line);
						getline(iss, field, '\t');
						string cname = field;
						getline(iss, field, '\t');
						istringstream interpretation(field);
						string object;
						while (getline(interpretation, object, ' ')) {
							int pos = get_obj_pos(object);
							if (pos > -1)
								conceptInterpretation.push_back(pos);
						}
						s.AddConceptInterpretation(cname, conceptInterpretation);
						conceptInterpretation.clear();
					}
				}
				//for roles
				for (unsigned j = 0; j < primitiveRoles.size(); ++j) {
					if ((!fin.eof() && getline(fin, line))) {
						istringstream iss(line);
						getline(iss, field, '\t');
						string rname = field;
						getline(iss, field, '\t');
						istringstream interpretation(field);
						string pair;
						while (getline(interpretation, pair, ' ')) {
							istringstream pairInterpretation(pair);
							string object;
							int pair_num = 0;
							std::pair<int, int> p;
							while (getline(pairInterpretation, object, ',')) {
								int pos = get_obj_pos(object);
								if (pos > -1) {
									if (pair_num == 0) {
										p.first = pos;
										++pair_num;
									} else {
										p.second = pos;
										pair_num = 0;
									}
								}
							}
							roleInterpretation.push_back(p);
						}
						//cout << endl;
						s.AddRoleInterpretation(rname, roleInterpretation);
						roleInterpretation.clear();
					}
				}

				if ((!fin.eof() && i < inst.GetNumActions() && getline(fin, line))) {
					istringstream iss(line);
					getline(iss, field, ' ');
					s.SetAction(field);
					s.SetNumState(i);
					inst.AddState(s);
				} else if (i == inst.GetNumActions()) {
					inst.SetGoal(s);
				}
			}
			instances.push_back(inst);
		}
	}
	fin.close();
}

void printout() {
//Input print
//	cout << "Number of instances: " << instances.size() << endl;
//	for (unsigned i = 0; i < instances.size(); ++i) {
//		cout << instances[i].GetNumPlan() << " " << instances[i].GetNumActions() << endl;
//		for (unsigned j = 0; j < instances[i].GetStates().size(); ++j) {
//			instances[i].GetStates()[j].Print(allObjects);
//		}
//		instances[i].GetGoal().Print(allObjects);
//	}
//	cout << endl;
//	for (unsigned i = 0; i < primitiveConcepts.size(); i++)
//		cout << primitiveConcepts[i] << "-";
//	cout << endl;
//	for (unsigned i = 0; i < primitiveRoles.size(); i++)
//		cout << primitiveRoles[i] << "-";
//	cout << endl;
//	for (unsigned i = 0; i < allObjects.size(); i++)
//		cout << allObjects[i] << "-";
//	cout << endl;
//	for (unsigned i = 0; i < actions.size(); i++)
//		cout << actions[i] << "-";
//	cout << endl;

	for (unsigned i = 0; i < rootConcepts.size(); ++i) {
		rootConcepts[i]->infix(cout);
		cout << "-" << rootConcepts[i]->GetNonEmptyDenotationNum() << endl;
		vector<vector<int> > denotations = rootConcepts[i]->GetDenotationVec();
//		for (unsigned j = 0; j < denotations.size(); ++j) {
//			for (unsigned k = 0; k < denotations[j].size(); ++k) {
//				cout << allObjects[denotations[j][k]] << " ";
//			}
//			cout << endl;
//		}
	}

	for (unsigned i = 0; i < rootRoles.size(); ++i) {
		rootRoles[i]->infix(cout);
		cout << endl;
//		vector<vector<pair<int, int> > > denotations = rootRoles[i]->GetDenotationRoleVec();
//		for (unsigned j = 0; j < denotations.size(); ++j) {
//			for (unsigned k = 0; k < denotations[j].size(); ++k) {
//				cout << "(" << allObjects[denotations[j][k].first] << "," << allObjects[denotations[j][k].second]
//						<< ") ";
//			}
//			cout << endl;
//		}
	}
	cout << "\tAction denotations" << endl;
	for (unsigned i = 0; i < actionDenotations.size(); ++i) {
		cout << actions[i] << ": ";
		for (unsigned j = 0; j < actionDenotations[i].size(); ++j) {
			if (actionDenotations[i][j])
				cout << '+';
			else
				cout << '-';
		}
		cout << endl;
	}

	cout << "\tRuleset" << endl;
	for (unsigned i = 0; i < ruleSet.size(); ++i) {
		cout << ruleSet[i];
		cout << endl;
	}
}

void cleanup() {

	for (unsigned i = 0; i < rootConcepts.size(); ++i) {
		delete rootConcepts[i];
	}

	for (unsigned i = 0; i < rootRoles.size(); ++i) {
		delete rootRoles[i];
	}

	rootConcepts.clear();
	rootRoles.clear();

}

void initialize() {
	for (unsigned i = 0; i < rootConcepts.size(); ++i)
		((ConceptNode*) rootConcepts[i])->UpdateDenotations(instances, &allObjectsIdx);
	for (unsigned i = 0; i < rootRoles.size(); ++i)
		((RoleNode*) rootRoles[i])->UpdateDenotations(instances, &allObjectsIdx);
}

void learn_concepts() {
	initialize();
	combine_roles();
	combine_concepts();
}

void make_policy() {
	for (unsigned i = 0; i < actions.size(); ++i) {
		vector<bool> tmpVec;
		for (unsigned j = 0; j < instances.size(); ++j) {
			for (unsigned k = 0; k < instances[j].GetStates().size(); ++k) {
				if (actions[i].compare(instances[j][k].GetAction()) == 0)
					tmpVec.push_back(true);
				else
					tmpVec.push_back(false);
			}
		}
		actionDenotations.push_back(tmpVec);
	}

	for (unsigned i = 0; i < rootConcepts.size(); ++i) {
		vector<vector<int> > cDenot = rootConcepts[i]->GetDenotationVec();
		if (rootConcepts[i]->GetNonEmptyDenotationNum() == 0)
			continue;
		int correct = 0;
		for (unsigned j = 0; j < actions.size(); ++j) {
			bool mistake = false;
			for (unsigned k = 0; k < cDenot.size(); ++k) {
				if (cDenot[k].size() > 0 && !(actionDenotations[j][k])) {
					mistake = true;
					break;
				}
				if(cDenot[k].size() > 0 && actionDenotations[j][k])
					correct++;
			}
			if (!mistake) {
				Rule r(rootConcepts[i], actions[j]);
				r.SetCorrect(correct);
				ruleSet.push_back(r);
			}
		}
	}
	sort(ruleSet.begin(),ruleSet.end());
}

void write_policy(){
	ofstream fout;
	fout.open("policy.txt");
	for(unsigned i=0;i<primitiveConcepts.size();++i)
		fout<<primitiveConcepts[i]<<" ";
	fout<<endl;
	for(unsigned i=0;i<primitiveRoles.size();++i)
		fout<<primitiveRoles[i]<<" ";
	fout<<endl;
	for(unsigned i=0;i<ruleSet.size();++i)
			ruleSet[i].SaveRule(fout);
	fout << endl;

}

int main(int argc, char** argv) {
	get_input();
	initialize_concepts();
	learn_concepts();
	make_policy();
	printout();
	write_policy();
	cleanup();
	return 0;
}
