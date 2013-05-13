#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <typeinfo>
#include <cmath>
#include <map>
#include <vector>
#include <algorithm>
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
vector<string> actions;
vector<Instance> instances;
vector<Rule> ruleSet;

void initialize_concepts() {
	for (unsigned i = 0; i < primitiveConcepts.size(); ++i) {
		ConceptNode* c = new ConceptNode(primitiveConcepts[i]);
		c->SetObjects(&allObjects);
		rootConcepts.push_back(c);
		c = new ConceptNode(primitiveConcepts[i]);
		c->SetObjects(&allObjects);
		c->IsGoal(true);
		rootConcepts.push_back(c);
	}

	for (unsigned i = 0; i < primitiveRoles.size(); ++i) {
		RoleNode* r = new RoleNode(primitiveRoles[i]);
		r->SetObjects(&allObjects);
		rootRoles.push_back(r);
		r = new RoleNode(primitiveRoles[i]);
		r->SetObjects(&allObjects);
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

void combine_concepts() {
	vector<Expression*>::iterator conceptIt, conceptIt1, roleIt, roleIt1;
	vector<Expression*> candidates;
	UnaryOperator* uo;
	BinaryOperator* bo;
	bool hasCandidates = true;
	while (hasCandidates) {
		for (conceptIt = rootConcepts.begin(); conceptIt < rootConcepts.end(); ++conceptIt) {
			uo = new Not(*conceptIt);
			candidates.push_back(uo);
			for (roleIt = rootRoles.begin(); roleIt < rootRoles.end(); ++roleIt) {
				bo = new ValueRestriction(*roleIt, *conceptIt);
				candidates.push_back(bo);
			}
		}

		for (unsigned i = 0; i < candidates.size(); ++i)
			rootConcepts.push_back(candidates[i]);
		candidates.clear();
		hasCandidates = false;
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
					//getline(iss, field, '\t');
					s.SetAction(line);
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
//		cout << primitiveConcepts[i] << " ";
//	cout << endl;
//	for (unsigned i = 0; i < primitiveRoles.size(); i++)
//		cout << primitiveRoles[i] << " ";
//	cout << endl;
//	for (unsigned i = 0; i < allObjects.size(); i++)
//		cout << allObjects[i] << " ";
//	cout << endl;
//	for (unsigned i = 0; i < actions.size(); i++)
//		cout << actions[i] << " ";
//	cout << endl;

	for (unsigned i = 0; i < rootConcepts.size(); ++i) {
		rootConcepts[i]->infix(cout);
		cout << endl;
		vector<vector<int> > denotations = rootConcepts[i]->GetDenotationVec();
		for (unsigned j = 0; j < denotations.size(); ++j) {
			for (unsigned k = 0; k < denotations[j].size(); ++k) {
				cout << allObjects[denotations[j][k]] << " ";
			}
			cout << endl;
		}
	}

	for (unsigned i = 0; i < rootRoles.size(); ++i) {
		rootRoles[i]->infix(cout);
		cout << endl;
		vector<vector<pair<int, int> > > denotations = rootRoles[i]->GetDenotationRoleVec();
		for (unsigned j = 0; j < denotations.size(); ++j) {
			for (unsigned k = 0; k < denotations[j].size(); ++k) {
				cout << "(" << allObjects[denotations[j][k].first] << "," << allObjects[denotations[j][k].second]
						<< ") ";
			}
			cout << endl;
		}
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
		((Operand*) rootConcepts[i])->UpdateDenotations(instances);
	for (unsigned i = 0; i < rootRoles.size(); ++i)
		((Operand*) rootRoles[i])->UpdateDenotations(instances);
}

void learn_concepts() {
	initialize();
	combine_roles();
	combine_concepts();
}

int main(int argc, char** argv) {
	get_input();
	initialize_concepts();
	learn_concepts();
	printout();
	cleanup();
	return 0;
}
