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
#include "time.hxx"
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
#include "PreOps.hxx"
#include "ActionDenotations.hxx"

namespace mn {
using namespace std;
using namespace expression;

map<string, vector<Expression*> > candidateDenotMap;
map<string, vector<Expression*> > rootDenotMap;

vector<vector<Expression*> > conceptSets;
vector<bool> setsTouched;
vector<Expression*> minHitSet;

vector<Expression*> rootConcepts;
vector<Expression*> rootRoles;
vector<string> primitiveConcepts;
vector<string> primitiveRoles;
vector<string> allObjects;
vector<int> allObjectsIdx;
vector<string> actions;
ActionDenotations* aDenot;
vector<Instance> instances;
vector<State> allStates;
vector<Rule> ruleSet;
PreOps* preops;
int denotationSize(0);
int runCount(1);

void print_min_hitset() {
	cout << endl;
	int weight = 0;
	for (int i = 0; i < minHitSet.size(); ++i) {
		minHitSet[i]->infix(cout);
		weight += ((Operator*) minHitSet[i])->GetLevel();
		cout << " hits " << minHitSet[i]->GetHits() << endl;
	}
	cout << "Total weight: " << weight << endl;
	cout << "Number of sets: " << conceptSets.size();
}

bool moreHits(Expression* x, Expression* y) {
	return x->GetHits() > y->GetHits();
}

void find_min_hitset_greedy() {
	int remainingSets = conceptSets.size();
	for (int i = 0; i < conceptSets.size(); ++i)
		if (conceptSets[i].size() == 0)
			--remainingSets;
	sort(rootConcepts.begin(), rootConcepts.end(), moreHits);
	int conc = 0;
	while (remainingSets > 0) {
		if (conc >= rootConcepts.size()) {
			cout << " min hitting set not possible!!!!";
			return;
		}
		if (rootConcepts[conc]->IsHitting())
			continue;
		rootConcepts[conc]->SetIsHitting(true);
		minHitSet.push_back(rootConcepts[conc]);
		vector<int> hitSetIndexes = rootConcepts[conc]->GetHitSetIndexes();

		for (int j = 0; j < hitSetIndexes.size(); ++j) {
			if (!setsTouched[hitSetIndexes[j]])
				--remainingSets;
			setsTouched[hitSetIndexes[j]] = true;
		}
		conc++;
	}

	cout << "Minimum hitset found with " << minHitSet.size() << " members inside." << endl;

}

void find_min_hitset() {
	int remainingSets = setsTouched.size();
	while (remainingSets > 0) {
		for (int i = 0; i < setsTouched.size(); ++i) {
			if (!setsTouched[i]) {
				for (int k = 0; k < conceptSets[i].size(); ++k) {
					conceptSets[i][k]->SetIsHitting(true);
					minHitSet.push_back(conceptSets[i][k]);
					vector<int> hitSetIndexes = conceptSets[i][k]->GetHitSetIndexes();

					for (int j = 0; j < hitSetIndexes.size(); ++j) {
						if (!setsTouched[hitSetIndexes[j]])
							--remainingSets;
						setsTouched[hitSetIndexes[j]] = true;
					}
				}
			}
		}

		cout << "Minimum hitset found with" << minHitSet.size() << " members inside." << endl;

	}
}
void get_all_states() {
	for (int i = 0; i < instances.size(); ++i) {
		vector<State>* iStates = instances[i].GetStates();
		for (int j = 0; j < iStates->size(); ++j) {
			allStates.push_back((*iStates)[j]);
		}
	}
}

void generate_concept_sets() {
	get_all_states();

	cout << endl;

	for (int i = 0; i < allStates.size() - 1; ++i) {
		for (int j = i + 1; j < allStates.size(); ++j) {
			if (allStates[i].GetAction().compare(allStates[j].GetAction()) != 0) {
				conceptSets.push_back(vector<Expression*>());
				for (int k = 0; k < rootConcepts.size(); ++k) {
					if ((*rootConcepts[k]->GetSimpleDenotationVec())[i]
							!= (*rootConcepts[k]->GetSimpleDenotationVec())[j]) {
						conceptSets[conceptSets.size() - 1].push_back(rootConcepts[k]);
						rootConcepts[k]->IncHits();
						rootConcepts[k]->AddHit(conceptSets.size() - 1);
					}
				}
				cout << "States: " << i << ", " << j << " : " << allStates[i].GetAction() << "-"
						<< allStates[j].GetAction() << " size: " << conceptSets[conceptSets.size() - 1].size() << endl;
			}
		}
	}

	for (int i = 0; i < conceptSets.size(); ++i)
		setsTouched.push_back(false);

	find_min_hitset_greedy();
	print_min_hitset();
}

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

inline void insert_candidate(Operator* exp, vector<Expression*>* candidates) {
	if (exp->GetNonEmptyDenotationNum() == 0) {
		delete exp;
		return;
	}

	string signature = exp->GetSignature();
	map<string, vector<Expression*> >::iterator itr = rootDenotMap.find(signature);
	if (itr != rootDenotMap.end()) {
//		if (runCount > -1) {
//			delete exp;
//			return;
//		}
		for (unsigned i = 0; i < itr->second.size(); ++i) {
			if (exp->EqualSimpleDenotationVec(itr->second[i])) {
				delete exp;
				return;
			}
		}
		rootDenotMap[signature].push_back(exp);
	} else {
		vector<Expression*> tmp;
		tmp.push_back(exp);
		rootDenotMap[signature] = tmp;
	}

	map<string, vector<Expression*> >::iterator it = candidateDenotMap.find(signature);
	if (it != candidateDenotMap.end()) {
//		if (runCount > -1) {
//			delete exp;
//			return;
//		}
		for (unsigned i = 0; i < it->second.size(); ++i) {
			if (exp->EqualSimpleDenotationVec(it->second[i])) {
				delete exp;
				return;
			}
		}
		candidateDenotMap[signature].push_back(exp);
	} else {
		vector<Expression*> tmp;
		tmp.push_back(exp);
		candidateDenotMap[signature] = tmp;
	}

	exp->SimplifyDenotations();
	candidates->push_back(exp);
	if (candidates->size() % 1000 == 0)
		cout << "Candidates: " << candidates->size() << endl;
}

void combine_concepts() {
	vector<Expression*>::iterator conceptIt, conceptIt1, roleIt, roleIt1;
	vector<Expression*> candidates;
	vector<Expression*> nextLayer = rootConcepts;
	UnaryOperator* uo;
	BinaryOperator* bo;

	bool hasCandidates = true;
	denotationSize = rootConcepts[0]->GetDenotationVec().size();
	cout << "Denotation size: " << denotationSize << endl;

	for (unsigned i = 0; i < rootConcepts.size(); ++i) {
		rootConcepts[i]->SetPreops(preops);
		rootConcepts[i]->SimplifyDenotations();
		string signature = rootConcepts[i]->GetSignature();
		map<string, vector<Expression*> >::iterator itr = rootDenotMap.find(signature);
		if (itr != rootDenotMap.end()) {
			itr->second.push_back(rootConcepts[i]);
		} else {
			vector<Expression*> tmp;
			tmp.push_back(rootConcepts[i]);
			rootDenotMap[signature] = tmp;
		}
	}

	for (roleIt = rootRoles.begin(); roleIt < rootRoles.end(); ++roleIt) {
		for (roleIt1 = rootRoles.begin(); roleIt1 < rootRoles.end(); ++roleIt1) {
			if (roleIt == roleIt1)
				continue;
			bo = new Equality(*roleIt, *roleIt1, preops);
			bo->SetLevel(1);
			insert_candidate(bo, &candidates);
		}
	}

	while (hasCandidates) {
		cout << "Concepts: " << rootConcepts.size() << endl;
		int cnt = 1;
		for (conceptIt = nextLayer.begin(); conceptIt < nextLayer.end(); ++conceptIt) {
			uo = new Not(*conceptIt, &allObjectsIdx, preops);
			uo->SetLevel(runCount);
			insert_candidate(uo, &candidates);
			for (roleIt = rootRoles.begin(); roleIt < rootRoles.end(); ++roleIt) {
				bo = new ValueRestriction(*roleIt, *conceptIt, preops);
				bo->SetLevel(runCount);
				insert_candidate(bo, &candidates);
			}

//TODO Joins excluded
//			for (conceptIt1 = rootConcepts.begin(); conceptIt1 < rootConcepts.end(); ++conceptIt1) {
//				if (conceptIt == conceptIt1)
//					continue;
//				bo = new Join(*conceptIt, *conceptIt1, preops);
//				insert_candidate(bo, &candidates);
//
//			}
			if (++cnt % 10000 == 0)
				cout << "Evaluated: " << cnt << " concepts from current layer!" << endl;
		}

		rootConcepts.insert(rootConcepts.end(), candidates.begin(), candidates.end());
		if (candidates.size() == 0)
			hasCandidates = false;
		nextLayer = candidates;
		candidates.clear();
		candidateDenotMap.clear();
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
		cout << "-" << rootConcepts[i]->GetNonEmptyDenotationNum() << "-" << ((Operator*) rootConcepts[i])->GetLevel()
				<< endl;
//		vector<vector<int> > denotations = rootConcepts[i]->GetDenotationVec();
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
	for (unsigned i = 0; i < actions.size(); ++i) {
		cout << actions[i] << ": ";
		for (unsigned j = 0; j < (*aDenot)[i].size(); ++j) {
			if ((*aDenot)[i][j])
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
	int numCovered = 0;
	for (unsigned i = 0; i < minHitSet.size() && numCovered < denotationSize; ++i) {
		vector<int>* cDenot = minHitSet[i]->GetSimpleDenotationVec();
		if (minHitSet[i]->GetNonEmptyDenotationNum() == 0)
			continue;
		for (unsigned j = 0; j < actions.size(); ++j) {
			int correct = 0;
			bool mistake = false;
			//vector<pair<int, int> > coVector;

			for (unsigned k = 0; k < cDenot->size(); ++k) {
//				if ((*cDenot)[k] != 0 && !((*aDenot)[j][k])) {
//					mistake = true;
//					break;
//				}
				if ((*cDenot)[k] != 0 && (*aDenot)[j][k] && (*aDenot)[j][k] != 2) {
					correct++;
//					pair<int, int> p(j, k);
//					coVector.push_back(p);
				}
			}
			if (!mistake && correct > 0) {
//				for (int v = 0; v < coVector.size(); ++v)
//					aDenot->SetCovered(coVector[v].first, coVector[v].second);
				//numCovered += correct;
				Rule r(minHitSet[i], actions[j]);
				r.SetCorrect(correct);
				ruleSet.push_back(r);
			}
		}
	}
}

void write_policy() {
	ofstream fout;
	fout.open("policy.txt");
	for (unsigned i = 0; i < primitiveConcepts.size(); ++i)
		fout << primitiveConcepts[i] << " ";
	fout << endl;
	for (unsigned i = 0; i < primitiveRoles.size(); ++i)
		fout << primitiveRoles[i] << " ";
	fout << endl;
	for (unsigned i = 0; i < ruleSet.size(); ++i)
		ruleSet[i].SaveRule(fout);
	fout << endl;

}
}
using namespace mn;

int main(int argc, char** argv) {
	get_input();
	initialize_concepts();
	preops = new PreOps(allObjects.size());
	aDenot = new ActionDenotations(&instances, &actions);
	float t0, tf;
	t0 = time_used();
	learn_concepts();
	generate_concept_sets();
	make_policy();
	sort(ruleSet.begin(), ruleSet.end());
	printout();
	write_policy();
	tf = time_used();
	cout << endl << "Total time: ";
	report_interval(t0, tf, cout);
	cout << endl;
	cleanup();
	return 0;
}

