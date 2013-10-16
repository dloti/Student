#include <iostream>
#include <sstream>
#include <cstring>
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
vector<Expression*> hittingConcepts;
vector<int> setsTouched;
vector<Expression*> minHitSet;
vector<Expression*> candidateHitSet;
map<string, Expression*> features;
map<string, Expression*> candidateFeatures;
unsigned candidateWeight(0), minHitSetWeight(0);
vector<vector<int> > subsets;
double avHitWeight(0);

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
//vector<Rule> ruleSet;
PreOps* preops;
int denotationSize(0);
int runCount(1);

void print_min_hitset() {
	cout << endl;
	int weight = 0;
	for (unsigned i = 0; i < minHitSet.size(); ++i) {
		minHitSet[i]->infix(cout);
		weight += minHitSet[i]->GetWeight();
		cout << " hits " << minHitSet[i]->GetHits() << endl;
	}
	cout << "Total weight: " << weight << endl;
	cout << "Number of sets: " << conceptSets.size();
}

bool moreHits(Expression* x, Expression* y) {
	return (x->GetHits() / x->GetWeight()) > (y->GetHits() / y->GetWeight());
}
double uniform_deviate(int seed) {
	return seed * (1.0 / (RAND_MAX + 1.0));
}

int hit_sets(Expression* concept) {
	int num_hit = 0;
	vector<int> hitSetIndexes = concept->GetHitSetIndexes();
	for (unsigned j = 0; j < hitSetIndexes.size(); ++j) {
		if (!setsTouched[hitSetIndexes[j]])
			++num_hit;
		setsTouched[hitSetIndexes[j]] += 1;
	}
	return num_hit;
}

void find_min_hitset_greedy() {
	int remainingSets = conceptSets.size();
	sort(hittingConcepts.begin(), hittingConcepts.end(), moreHits);
	int conc = 0;
	while (remainingSets > 0) {
		if (conc >= hittingConcepts.size()) {
			cout << "ERR min hitting set not possible!!!!";
			return;
		}
		if (hittingConcepts[conc]->GetHits() == 0)
			continue;
		if (hittingConcepts[conc]->IsHitting())
			continue;
		hittingConcepts[conc]->SetIsHitting(true);
		minHitSet.push_back(hittingConcepts[conc]);
		minHitSetWeight += hittingConcepts[conc]->GetWeight();
		remainingSets -= hit_sets(hittingConcepts[conc]);
		conc++;
	}
}

void set_average_weight_hit() {
	avHitWeight = 0;
	for (int i = 0; i < hittingConcepts.size(); ++i)
		avHitWeight += hittingConcepts[i]->GetWeight();
	avHitWeight /= hittingConcepts.size();
	cout << "Hitting concepts:" << hittingConcepts.size() << " Average hit/weight: " << avHitWeight << endl;
}
//TODO
int boltzman() {
	int cnd;
}

vector<int> candidate_sgreedy() {
	int remainingSets = conceptSets.size();
	candidateWeight = 0;
	vector<int> st;
	vector<int> thrown_out;
	for (unsigned i = 0; i < conceptSets.size(); ++i)
		st.push_back(0);
	int cnd = -1;
	int cnt = -1;
	while (remainingSets > 0) {
		if (cnt == -1) {
			cnd = rand() % hittingConcepts.size();
			if ((hittingConcepts[cnd]->GetHits() / hittingConcepts[cnd]->GetWeight()) < avHitWeight)
				continue;
			if (find(minHitSet.begin(), minHitSet.end(), hittingConcepts[cnd]) != minHitSet.end())
				continue;
			if (hittingConcepts[cnd]->GetHits() == 0)
				continue;
			candidateHitSet.push_back(hittingConcepts[cnd]);
			candidateWeight += hittingConcepts[cnd]->GetWeight();
			vector<int> hitSetIndexes = hittingConcepts[cnd]->GetHitSetIndexes();
			for (unsigned j = 0; j < hitSetIndexes.size(); ++j) {
				if (!st[hitSetIndexes[j]])
					--remainingSets;
				st[hitSetIndexes[j]] += 1;
			}
			++cnt;
			cnd = -1;
		} else {
			++cnd;
			vector<int> hitSetIndexes = minHitSet[cnd]->GetHitSetIndexes();
			bool subsumed = true;
			for (unsigned j = 0; j < hitSetIndexes.size(); ++j) {
				if (!st[hitSetIndexes[j]]) {
					subsumed = false;
				}
			}
			if (subsumed) {
				thrown_out.push_back(cnd);
				continue;
			}

			candidateHitSet.push_back(minHitSet[cnd]);
			candidateWeight += minHitSet[cnd]->GetWeight();

			for (unsigned j = 0; j < hitSetIndexes.size(); ++j) {
				if (!st[hitSetIndexes[j]])
					--remainingSets;
				st[hitSetIndexes[j]] += 1;
				++cnt;
			}

			if (cnd == (minHitSet.size() - 1))
				break;
		}
	}
	if (remainingSets > 0)
		thrown_out.clear();
	return thrown_out;
}

void sgreedy() {
	find_min_hitset_greedy();
	int distance(0);
	cout << endl << " Stochastic greedy: " << minHitSet.size() << " w:" << minHitSetWeight << " ";
	for (int i = 0; i < 10000; ++i) {
		vector<int> thrown_out = candidate_sgreedy();
		if (thrown_out.size() == 0)
			continue;

		if ((candidateHitSet.size() <= minHitSet.size()) && (candidateWeight <= minHitSetWeight)) {
			for (int j = 0; j < thrown_out.size(); ++j) {
				minHitSet[j]->SetIsHitting(false);
			}

			minHitSet.clear();
			minHitSetWeight = candidateWeight;
			for (unsigned i = 0; i < candidateHitSet.size(); ++i) {
				candidateHitSet[i]->SetIsHitting(true);
				minHitSet.push_back(candidateHitSet[i]);
			}
			cout << minHitSet.size() << " w:" << minHitSetWeight << " ";
		}
		candidateHitSet.clear();
	}
}

void init_min_hitting_set() {
	int remainingSets = conceptSets.size();
	candidateWeight = 0;
	vector<int> st;
	vector<int> concept_idx;
	for (unsigned i = 0; i < conceptSets.size(); ++i)
		st.push_back(0);
	int rnd = 0;
	while (remainingSets > 0) {
		rnd = rand() % rootConcepts.size();
		if (find(concept_idx.begin(), concept_idx.end(), rnd) != concept_idx.end())
			continue;
		if (rootConcepts[rnd]->GetHits() == 0)
			continue;

		concept_idx.push_back(rnd);
		candidateHitSet.push_back(rootConcepts[rnd]);
		candidateWeight += rootConcepts[rnd]->GetWeight();
		vector<int> hitSetIndexes = rootConcepts[rnd]->GetHitSetIndexes();
		for (unsigned j = 0; j < hitSetIndexes.size(); ++j) {
			if (!st[hitSetIndexes[j]])
				--remainingSets;
			st[hitSetIndexes[j]] += 1;
		}
	}
}

void iterated() {
	int distance(0);
	cout << endl << " Random walks:";
	for (int i = 0; i < 100000; ++i) {
		init_min_hitting_set();
		if (i == 0) {
			for (unsigned j = 0; j < candidateHitSet.size(); ++j)
				minHitSet.push_back(candidateHitSet[j]);
			minHitSetWeight = candidateWeight;
		}
		distance = candidateHitSet.size() * candidateWeight - minHitSet.size() * minHitSetWeight;
		if (distance < 0) {
			minHitSet.clear();
			minHitSetWeight = candidateWeight;
			for (unsigned i = 0; i < candidateHitSet.size(); ++i)
				minHitSet.push_back(candidateHitSet[i]);
			cout << minHitSet.size() << " ";
		}
		candidateHitSet.clear();
	}
}

void annealGetNext() {
	int remainingSets = conceptSets.size();
	vector<int> st;
	vector<int> concept_idx;
	for (unsigned i = 0; i < conceptSets.size(); ++i)
		st.push_back(0);
	for (unsigned i = 0; i < conceptSets.size(); ++i)
		if (conceptSets[i].size() == 0)
			--remainingSets;

	int swap_num = rand() % (candidateHitSet.size() / 2);

	int rnd(0);
	for (int i = 0; i < swap_num; ++i) {
		rnd = rand() % candidateHitSet.size();
		candidateHitSet.erase(candidateHitSet.begin() + rnd);
		candidateHitSet[rnd]->SetIsHitting(false);
	}

	for (unsigned i = 0; i < candidateHitSet.size(); ++i) {
		vector<int> hitSetIndexes = candidateHitSet[i]->GetHitSetIndexes();
		for (unsigned j = 0; j < hitSetIndexes.size(); ++j) {
			if (!st[hitSetIndexes[j]])
				--remainingSets;
			st[hitSetIndexes[j]] += 1;
		}
	}

	while (remainingSets > 0) {
		rnd = rand() % rootConcepts.size();
		if (find(concept_idx.begin(), concept_idx.end(), rnd) != concept_idx.end())
			continue;

		concept_idx.push_back(rnd);
		candidateHitSet.push_back(rootConcepts[rnd]);
		vector<int> hitSetIndexes = rootConcepts[rnd]->GetHitSetIndexes();
		for (unsigned j = 0; j < hitSetIndexes.size(); ++j) {
			if (!st[hitSetIndexes[j]])
				--remainingSets;
			st[hitSetIndexes[j]] += 1;
		}
	}
}

void simulated_annealing() {
	double proba;
	double alpha = 0.999;
	double temperature = 100.0;
	double epsilon = 0.001;
	double delta;
	init_min_hitting_set();
	for (unsigned i = 0; i < candidateHitSet.size(); ++i)
		minHitSet.push_back(candidateHitSet[i]);
	int cost = minHitSet.size();

	while (temperature > epsilon) {
		annealGetNext();
		delta = candidateHitSet.size() - cost;
		if (delta < 0) {
			minHitSet.clear();
			for (unsigned i = 0; i < candidateHitSet.size(); ++i)
				minHitSet.push_back(candidateHitSet[i]);
			cost += delta;
		} else {
			proba = uniform_deviate(rand());
			double ex = exp(-delta / temperature);
			if (ex > proba) {
				minHitSet.clear();
				for (unsigned i = 0; i < candidateHitSet.size(); ++i)
					minHitSet.push_back(candidateHitSet[i]);
				cost += delta;
			} else {
				candidateHitSet.clear();
				for (unsigned i = 0; i < minHitSet.size(); ++i)
					candidateHitSet.push_back(minHitSet[i]);
			}
		}
		temperature *= alpha;
	}
}

void find_min_hitset() {
	int remainingSets = setsTouched.size();
	while (remainingSets > 0) {
		for (unsigned i = 0; i < setsTouched.size(); ++i) {
			if (!setsTouched[i]) {
				for (unsigned k = 0; k < conceptSets[i].size(); ++k) {
					conceptSets[i][k]->SetIsHitting(true);
					minHitSet.push_back(conceptSets[i][k]);
					vector<int> hitSetIndexes = conceptSets[i][k]->GetHitSetIndexes();

					for (unsigned j = 0; j < hitSetIndexes.size(); ++j) {
						if (!setsTouched[hitSetIndexes[j]])
							--remainingSets;
						setsTouched[hitSetIndexes[j]] = true;
					}
				}
			}
		}
	}
}
void get_all_states() {
	for (unsigned i = 0; i < instances.size(); ++i) {
		vector<State>* iStates = instances[i].GetStates();
		for (unsigned j = 0; j < iStates->size(); ++j) {
			allStates.push_back((*iStates)[j]);
		}
	}
}

void generate_concept_sets() {
	srand((unsigned int) time(NULL));
	get_all_states();
	string signature;
	for (unsigned i = 0; i < allStates.size() - 1; ++i) {
		for (unsigned j = i + 1; j < allStates.size(); ++j) {
			bool isEmpty = true;
			if (allStates[i].GetAction().compare(allStates[j].GetAction()) != 0) {
				conceptSets.push_back(vector<Expression*>());
				for (unsigned k = 0; k < rootConcepts.size(); ++k) {
					signature = rootConcepts[k]->GetSignature();
					if (signature[i] != signature[j]) {
						if (strcmp(typeid(rootConcepts[k]).name(), "Not") == 0) {
							cout << "Not out: ";
							rootConcepts[k]->infix(cout);
							cout << endl;
							continue;
						}
						conceptSets[conceptSets.size() - 1].push_back(rootConcepts[k]);
						isEmpty = false;
						rootConcepts[k]->IncHits();
						rootConcepts[k]->AddHit(conceptSets.size() - 1);
					}
				}
				if (isEmpty)
					cout << "Empty set: " << i << "," << j << " " << allStates[i].GetAction() << " "
							<< allStates[j].GetAction() << endl;
//				cout << "States: " << i << ", " << j << " : " << allStates[i].GetAction() << "-"
//						<< allStates[j].GetAction() << " size: " << conceptSets[conceptSets.size() - 1].size() << endl;
			}
		}
	}

	for (unsigned i = 0; i < rootConcepts.size(); ++i) {
		if (rootConcepts[i]->GetHits() > 0)
			hittingConcepts.push_back(rootConcepts[i]);
	}
	for (unsigned i = 0; i < conceptSets.size(); ++i)
		if (conceptSets[i].size() == 0) {
			cout << "ERR empty set" << endl;
			return;
		}

	for (unsigned i = 0; i < conceptSets.size(); ++i)
		setsTouched.push_back(0);
	set_average_weight_hit();
	cout << "Finding min hitting set" << endl;

	sgreedy();
//find_min_hitset_greedy();
//iterated();
//simulated_annealing();
	cout << "Minimum hitset with " << minHitSet.size() << " members inside." << endl;
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
		if (runCount > -1) {
			delete exp;
			return;
		}
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
		if (runCount > -1) {
			delete exp;
			return;
		}
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

			uo = new Not(*conceptIt, &allObjectsIdx, preops);
			uo->SetLevel(runCount);
			insert_candidate(uo, &candidates);
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
//
//	for (unsigned i = 0; i < rootConcepts.size(); ++i) {
//		rootConcepts[i]->infix(cout);
//		cout << "-" << rootConcepts[i]->GetNonEmptyDenotationNum() << "-" << ((Operator*) rootConcepts[i])->GetLevel()
//				<< endl;
//		rootConcepts[i]->SimplifyDenotations();
//
//		vector<int>* denotations = rootConcepts[i]->GetSimpleDenotationVec();
//		for (unsigned j = 0; j < denotations->size(); ++j) {
//			//for (unsigned k = 0; k < denotations[j].size(); ++k) {
//			preops->Print((*denotations)[j], allObjects);
//			cout << endl;
//			//}
//		}
//	}
//
//	for (unsigned i = 0; i < rootRoles.size(); ++i) {
//		rootRoles[i]->infix(cout);
//		cout << endl;
//		vector<vector<pair<int, int> > > denotations = rootRoles[i]->GetDenotationRoleVec();
//		for (unsigned j = 0; j < denotations.size(); ++j) {
//			for (unsigned k = 0; k < denotations[j].size(); ++k) {
//				cout << "(" << allObjects[denotations[j][k].first] << "," << allObjects[denotations[j][k].second]
//						<< ") ";
//			}
//			cout << endl;
//		}
//	}
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

//	cout << "\tRuleset" << endl;
//	for (unsigned i = 0; i < ruleSet.size(); ++i) {
//		cout << ruleSet[i];
//		cout << endl;
//	}
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

void make_features() {
	BinaryOperator *bo = NULL;
	map<string, Expression*>::iterator it = features.begin(), fnd;
	map<string, Expression*>::iterator cit = candidateFeatures.begin();
	map<string, Expression*> tmp;
	for (it = features.begin(); it != features.end(); ++it) {
		for (cit = candidateFeatures.begin(); cit != candidateFeatures.end(); ++cit) {
			bo = new Join((*it).second, (*cit).second, preops);
			string signature = bo->GetSignature();
			if (bo->GetNonEmptyDenotationNum() == 0) {
				delete bo;
				continue;
			}
			fnd = features.find(signature);
			if (fnd != features.end()) {
				//if ((bo->EqualSimpleDenotationVec((*fnd).second))) {
				delete bo;
				continue;
				//}
			}

			fnd = candidateFeatures.find(signature);
			if (fnd != candidateFeatures.end()) {
				//if ((bo->EqualSimpleDenotationVec((*fnd).second))) {
				delete bo;
				continue;
				//}
			}

			tmp[signature] = bo;
		}
	}

	candidateFeatures.clear();
	cout << "temp size " << tmp.size() << endl;
	for (fnd = tmp.begin(); fnd != tmp.end(); ++fnd)
		candidateFeatures[(*fnd).second->GetSignature()] = (*fnd).second;
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
	for (unsigned i = 0; i < minHitSet.size(); ++i) {
		minHitSet[i]->prefix(fout);
		fout << endl;
		fout << minHitSet[i]->GetSignature();
		fout << endl;
	}
	fout.close();
	fout.open("actions.txt");
	for (unsigned j = 0; j < instances.size(); ++j) {
		for (unsigned k = 0; k < instances[j].GetStates()->size(); ++k) {
			for (unsigned l = 0; l < actions.size(); ++l) {
				if (actions[l].compare(instances[j][k].GetAction()) == 0) {
					if (j == instances.size() - 1 && k == instances[j].GetStates()->size() - 1)
						fout << l;
					else
						fout << l << " ";
				}
			}
		}
	}
	fout.close();
}
//void make_policy() {
//	for (unsigned i = 0; i < minHitSet.size(); ++i) {
//		features[minHitSet[i]->GetSignature()] = minHitSet[i];
//		candidateFeatures[minHitSet[i]->GetSignature()] = minHitSet[i];
//	}
//	map<string, Expression*>::iterator cit = candidateFeatures.begin();
//	int numCovered = 0;
//	int layer = 0;
//	cout << "Total number of concepts: " << rootConcepts.size() << endl;
//	while (numCovered < denotationSize) {
//		if (candidateFeatures.size() == 0) {
//			cout << "Full coverage not possible, total number of features generated: " << features.size()
//					<< " covered: " << numCovered << " of " << denotationSize << endl;
//			return;
//		}
//
//		for (cit = candidateFeatures.begin(); cit != candidateFeatures.end(); ++cit) {
//			string cSignature = (*cit).second->GetSignature();
//			if ((*cit).second->GetNonEmptyDenotationNum() == 0) {
//				cout << "ERR: all empty feature ";
//				(*cit).second->infix(cout);
//				cout << endl;
//				continue;
//			}
//			bool mistake = false;
//			int correct = 0;
//
//			for (unsigned j = 0; j < actions.size(); ++j) {
//				correct = 0;
//				mistake = false;
//				vector<pair<int, int> > coVector;
//				for (unsigned k = 0; k < cSignature.length(); ++k) {
//
//					if (cSignature[k] != '0' && !((*aDenot)[j][k])) {
//						mistake = true;
//						break;
//					}
//					if (cSignature[k] != '0' && (*aDenot)[j][k] && (*aDenot)[j][k] != 2) {
//						++correct;
//						pair<int, int> p(j, k);
//						coVector.push_back(p);
//					}
//				}
//
//				if (!mistake && correct > 0) {
//					for (unsigned v = 0; v < coVector.size(); ++v) {
//						aDenot->SetCovered(coVector[v].first, coVector[v].second);
//					}
//					numCovered += correct;
//					Rule r((*cit).second, actions[j]);
//					r.SetCorrect(correct);
//					ruleSet.push_back(r);
//				}
//				coVector.clear();
//			}
//		}
//
//		if (layer != 0)
//			features.insert(candidateFeatures.begin(), candidateFeatures.end());
//		++layer;
//		make_features();
//	}
//}

//void write_policy() {
//	ofstream fout;
//	fout.open("policy.txt");
//	for (unsigned i = 0; i < primitiveConcepts.size(); ++i)
//		fout << primitiveConcepts[i] << " ";
//	fout << endl;
//	for (unsigned i = 0; i < primitiveRoles.size(); ++i)
//		fout << primitiveRoles[i] << " ";
//	fout << endl;
//	for (unsigned i = 0; i < ruleSet.size(); ++i)
//		ruleSet[i].SaveRule(fout);
//	fout << endl;
//
//}

bool testHitSet() {
	vector<int> hindex;
	for (int i = 0; i < minHitSet.size(); ++i) {
		vector<int> vec = minHitSet[i]->GetHitSetIndexes();
		for (int j = 0; j < vec.size(); ++j) {
			if (std::find(hindex.begin(), hindex.end(), vec[j]) == hindex.end())
				hindex.push_back(vec[j]);
		}
	}

	if (hindex.size() != conceptSets.size())
		return false;
	sort(hindex.begin(), hindex.end());
	for (int i = 0; i < hindex.size(); ++i) {
		if (hindex[i] != i) {
			return false;
		}
	}

	return true;
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
//	make_policy();
//	sort(ruleSet.begin(), ruleSet.end());
	printout();
	write_policy();
	tf = time_used();
	cout << endl << "Total time: ";
	report_interval(t0, tf, cout);
	cout << endl;

	if (testHitSet())
		cout << "HitSet fine" << endl;
	else
		cout << "ERR HITSET WRONG" << endl;
	cleanup();
	return 0;
}

