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
#include "Policy.hxx"
#include "NegativeExample.hxx"

namespace mn {
using namespace std;
using namespace expression;

map<string, vector<Expression*> > candidateDenotMap;
map<string, vector<Expression*> > rootDenotMap;

vector<vector<Expression*> > minHittingSets;
map<int, vector<NegativeExample> > negativeExamples;
map<int, vector<string> > negativeSignatures;

vector<vector<Expression*> > conceptSets;
vector<Expression*> hittingConcepts;
vector<int> setsTouched;
vector<Expression*> minHitSet;
vector<Expression*> candidateHitSet;
map<string, Expression*> features;
vector<Expression*> multipleHitsetFeatures;
map<string, Expression*> candidateFeatures;
unsigned candidateWeight(0), minHitSetWeight(0);
vector<vector<int> > subsets;
double avHitWeight(0);
vector<int> significantObjects;
map<string, int> significantObjectIdx;

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

void testHitSet() {
	vector<int> hindex;
	for (int i = 0; i < minHitSet.size(); ++i) {
		vector<int> vec = minHitSet[i]->GetHitSetIndexes();
		for (int j = 0; j < vec.size(); ++j) {
			if (std::find(hindex.begin(), hindex.end(), vec[j]) == hindex.end())
				hindex.push_back(vec[j]);
		}
	}

	if (hindex.size() != conceptSets.size()) {
		cout << "ERR HitSet wrong" << endl;
		;
		exit(-1);
	}
	sort(hindex.begin(), hindex.end());
	for (int i = 0; i < hindex.size(); ++i) {
		if (hindex[i] != i) {
			cout << "ERR HitSet wrong" << endl;
			;
			exit(-1);
		}
	}
	cout << "HitSet correct" << endl;
}

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

	vector<Expression*> tmp;
	for (unsigned i = 0; i < minHitSet.size(); ++i) {
		vector<int> hitSetIndexes = minHitSet[i]->GetHitSetIndexes();
		bool subsumed = true;

		for (unsigned j = 0; j < hitSetIndexes.size(); ++j) {
			if (setsTouched[hitSetIndexes[j]] < 2) {
				subsumed = false;
			}
		}
		if (!subsumed)
			tmp.push_back(minHitSet[i]);
		else {
			hittingConcepts[i]->SetIsHitting(false);
			minHitSetWeight -= hittingConcepts[i]->GetWeight();
			for (unsigned j = 0; j < hitSetIndexes.size(); ++j) {
				setsTouched[hitSetIndexes[j]]--;
			}
		}
	}

	minHitSet.clear();
	minHitSet = tmp;
}

void set_average_weight_hit() {
	avHitWeight = 0;
	for (unsigned i = 0; i < hittingConcepts.size(); ++i)
		avHitWeight += hittingConcepts[i]->GetWeight();
	avHitWeight /= hittingConcepts.size();
	cout << "Hitting concepts:" << hittingConcepts.size() << " Average hit/weight: " << avHitWeight << endl;
}

vector<int> candidate_sgreedy(double temperature) {
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
			double hitWeight = (hittingConcepts[cnd]->GetHits() / hittingConcepts[cnd]->GetWeight());
			double dist = hitWeight - avHitWeight;
			if (dist < 0) {
				double proba = rand();
				double ex = exp(dist / temperature);
				if (proba < ex) {
					continue;
				}
			}

			if (find(minHitSet.begin(), minHitSet.end(), hittingConcepts[cnd]) != minHitSet.end())
				continue;

			if (hittingConcepts[cnd]->GetHits() == 0) {
				cout << "ERR hitting concepts zero" << endl;
				continue;
			}

			candidateHitSet.push_back(hittingConcepts[cnd]);
			candidateWeight += hittingConcepts[cnd]->GetWeight();
			vector<int> hitSetIndexes = hittingConcepts[cnd]->GetHitSetIndexes();
			for (unsigned j = 0; j < hitSetIndexes.size(); ++j) {
				if (!st[hitSetIndexes[j]])
					--remainingSets;
				++st[hitSetIndexes[j]];
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
	double alpha = 0.999;
	double temperature = 50000.0;
	double epsilon = 0.001;
	unsigned steps = 0;
	find_min_hitset_greedy();
	cout << endl << " Stochastic greedy: " << minHitSet.size() << " w:" << minHitSetWeight << " ";
	while (temperature > epsilon) {
		vector<int> thrown_out = candidate_sgreedy(temperature);
		if (thrown_out.size() == 0) {
			++steps;
			temperature *= alpha;
			continue;
		}

		if ((candidateHitSet.size() <= minHitSet.size()) && (candidateWeight <= minHitSetWeight)) {
			for (unsigned j = 0; j < thrown_out.size(); ++j) {
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
		temperature *= alpha;
//		if (steps % 100 == 0)
//			cout << temperature << endl;
		++steps;
	}
	cout << "Sgreedy finished in " << steps << " steps" << endl;
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

string denote_for_obj(vector<Expression*> filteredExpressions, int example_num, int obj) {
	string ret;
	for (unsigned i = 0; i < filteredExpressions.size(); ++i)
		ret += filteredExpressions[i]->GetSignificantObjectSign(example_num, obj);
	return ret;
}

void set_bits_in_hitset(int pos) {
	for (unsigned i = 0; i < minHitSet.size(); ++i) {
		if (minHitSet[i]->GetSignature()[pos] != '1') {
			minHitSet[i] = new Not(minHitSet[i], &allObjectsIdx, preops);
		}
	}
}
void generate_concept_sets() {
	srand((unsigned int) time(NULL));

	string signature;
	vector<Expression*> filteredExpressions;
	for (unsigned i = 0; i < rootConcepts.size(); ++i) {
		if (dynamic_cast<Not*>(rootConcepts[i]))
			continue;
		filteredExpressions.push_back(rootConcepts[i]);
	}

	map<int, vector<NegativeExample> >::iterator itr;
	for (itr = negativeExamples.begin(); itr != negativeExamples.end(); ++itr) {
		for (unsigned i = 0; i < itr->second.size(); ++i) {
			string str = denote_for_obj(filteredExpressions, itr->first, itr->second[i].GetObj());
			negativeSignatures[itr->first].push_back(
					denote_for_obj(filteredExpressions, itr->first, itr->second[i].GetObj()));
		}
	}

	for (unsigned i = 0; i < allStates.size(); ++i) {
		for (unsigned j = 0; j < allStates.size(); ++j) {
			if (j == i)
				continue;

			bool isEmpty = true;
			if (allStates[i].GetAction().compare(allStates[j].GetAction()) != 0) {
				conceptSets.push_back(vector<Expression*>());
				for (unsigned k = 0; k < filteredExpressions.size(); ++k) {
					signature = filteredExpressions[k]->GetSignificantObjectSignature(&significantObjects);
					if (signature[i] != signature[j]) {

						if (signature[i] != '1' && signature[i] != '0') {
							cout << "ERR signature wrong: ";
							filteredExpressions[k]->infix(cout);
							cout << endl;
							cout << i << ":" << allStates.size() << endl;

						}
						conceptSets[conceptSets.size() - 1].push_back(filteredExpressions[k]);
						isEmpty = false;
						filteredExpressions[k]->IncHits();
						filteredExpressions[k]->AddHit(conceptSets.size() - 1);
					}
				}
				if (isEmpty) {
					cout << "Empty set: " << i << "," << j << " ";
					allStates[i].Print(allObjects);
					cout << endl << allStates[i].GetAction() << " " << allObjects[allStates[i].GetSignificantObject()]
							<< endl;
					allStates[j].Print(allObjects);
					cout << endl << allStates[j].GetAction() << " " << allObjects[allStates[j].GetSignificantObject()]
							<< endl;
					exit(-1);
				}
//				cout << "States: " << i << ", " << j << " : " << allStates[i].GetAction() << "-"
//				<< allStates[j].GetAction() << " size: " << conceptSets[conceptSets.size() - 1].size() << endl;
			}
		}

		string current;
		for (unsigned k = 0; k < filteredExpressions.size(); ++k) {
			signature = filteredExpressions[k]->GetSignificantObjectSignature(&significantObjects);
			current += signature[i];
		}

		if (negativeExamples.find(i) != negativeExamples.end()) {
			for (unsigned j = 0; j < negativeExamples[i].size(); ++j) {

				signature = negativeSignatures[i][j];
				conceptSets.push_back(vector<Expression*>());
				bool isEmpty = true;
				for (unsigned k = 0; k < filteredExpressions.size(); ++k) {
					if (current[k] != signature[k]) {
						isEmpty = false;
						if (signature[k] != '1' && signature[k] != '0') {
							cout << "ERR signature wrong: ";
							filteredExpressions[k]->infix(cout);
							cout << endl;
							cout << i << ":" << allStates.size() << endl;

						}
						conceptSets[conceptSets.size() - 1].push_back(filteredExpressions[k]);
						filteredExpressions[k]->IncHits();
						filteredExpressions[k]->AddHit(conceptSets.size() - 1);
					}
				}
				if (isEmpty) {
					cout << "Empty set: " << i << "," << j << " " << endl;
					cout << negativeExamples[i][j].GetAction() << " " << allObjects[negativeExamples[i][j].GetObj()]
							<< endl;
					exit(-1);
				}
			}
		}

		for (unsigned k = 0; k < filteredExpressions.size(); ++k) {
			if (filteredExpressions[k]->GetHits() > 0)
				hittingConcepts.push_back(filteredExpressions[k]);
		}

		for (unsigned k = 0; k < conceptSets.size(); ++k)
			if (conceptSets[k].size() == 0) {
				cout << "ERR empty set" << endl;
				return;
			}

		for (unsigned k = 0; k < conceptSets.size(); ++k)
			setsTouched.push_back(0);
		set_average_weight_hit();
		cout << "Finding min hitting set" << endl;

		sgreedy();

		cout << "Minimum hitset with " << minHitSet.size() << " members inside." << endl;
		testHitSet();
		set_bits_in_hitset(i);
		minHittingSets.push_back(minHitSet);

		print_min_hitset();
		conceptSets.clear();
		minHitSet.clear();
		candidateHitSet.clear();
		setsTouched.clear();
		hittingConcepts.clear();
		for (unsigned k = 0; k < filteredExpressions.size(); ++k) {
			filteredExpressions[k]->ClearHits();
		}
	}
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

	while (hasCandidates && runCount < 10) {
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

bool valid_signature(string signature) {
	for (int i = 0; i < signature.size(); ++i)
		if (signature[i] != '0' && signature[i] != '1')
			return false;
	return true;
}

void negate_concepts() {
	vector<Expression*>::iterator it;
	vector<Expression*> candidates;
	UnaryOperator* uo;
	for (it = rootConcepts.begin(); it != rootConcepts.end(); ++it) {
//		if ((*it)->GetNonEmptyDenotationNum() == 0)
//			continue;
		if (dynamic_cast<Not*>(*it))
			continue;
		uo = new Not(*it, &allObjectsIdx, preops);
		int level = 1;
		if (!dynamic_cast<Operand*>(*it))
			level = ((Operator*) (*it))->GetLevel();
		uo->SetLevel(level + 1);
		candidates.push_back(uo);
		if (!valid_signature(uo->GetSignificantObjectSignature(&significantObjects))) {
			cout << "ERR signature wrong ";
			uo->infix(cout);
			cout << endl;
		}
	}
	rootConcepts.insert(rootConcepts.end(), candidates.begin(), candidates.end());
}

void value_restriction() {
	vector<Expression*>::iterator it, roleIt;
	vector<Expression*> candidates;
	BinaryOperator* bo;
	for (it = rootConcepts.begin(); it != rootConcepts.end(); ++it) {
		for (roleIt = rootRoles.begin(); roleIt != rootRoles.end(); ++roleIt) {
//			if ((*it)->GetNonEmptyDenotationNum() == 0)
//				continue;
			bo = new ValueRestriction(*roleIt, *it, preops);
			int level = 1;
			if (!dynamic_cast<Operand*>(*it))
				level = ((Operator*) (*it))->GetLevel();
			bo->SetLevel(level + 1);
			bo->SimplifyDenotations();
			candidates.push_back(bo);
			if (!valid_signature(bo->GetSignificantObjectSignature(&significantObjects))) {
				cout << "ERR signature wrong ";
				bo->infix(cout);
				cout << endl;
			}
		}
	}
	rootConcepts.insert(rootConcepts.end(), candidates.begin(), candidates.end());
}

void syntax_concepts() {
	vector<Expression*>::iterator conceptIt, conceptIt1, roleIt, roleIt1;
	vector<Expression*> candidates;
	UnaryOperator* uo;
	BinaryOperator* bo;

	//Preops init
	for (unsigned i = 0; i < rootConcepts.size(); ++i) {
		rootConcepts[i]->SetPreops(preops);
		rootConcepts[i]->SimplifyDenotations();
		if (!valid_signature(rootConcepts[i]->GetSignificantObjectSignature(&significantObjects))) {
			cout << "ERR signature wrong ";
			rootConcepts[i]->infix(cout);
			cout << endl;
		}
	}
	cout << "Size: " << rootConcepts.size() << endl;

	//Combine roles
	for (roleIt = rootRoles.begin(); roleIt < rootRoles.end(); ++roleIt) {
		for (roleIt1 = rootRoles.begin(); roleIt1 < rootRoles.end(); ++roleIt1) {
			if (roleIt == roleIt1)
				continue;
			bo = new Equality(*roleIt, *roleIt1, preops);
			bo->SetLevel(2);
			bo->SimplifyDenotations();
			candidates.push_back(bo);
			if (!valid_signature(bo->GetSignificantObjectSignature(&significantObjects))) {
				cout << "ERR signature wrong ";
				bo->infix(cout);
				cout << endl;
			}
		}
	}
	rootConcepts.insert(rootConcepts.end(), candidates.begin(), candidates.end());
	candidates.clear();
	cout << "Size after equality: " << rootConcepts.size() << endl;
	negate_concepts();
	cout << "Size after negation: " << rootConcepts.size() << endl;
	value_restriction();
	cout << "Size after val. restriction: " << rootConcepts.size() << endl;
	negate_concepts();
	cout << "Size after negation: " << rootConcepts.size() << endl;
	value_restriction();
	cout << "Size after val. restriction: " << rootConcepts.size() << endl;
	cout << "Total concepts generated: " << rootConcepts.size() << endl;
}

int get_obj_pos(string object) {
	int pos = std::find(allObjects.begin(), allObjects.end(), object) - allObjects.begin();
	if (pos >= allObjects.size()) {
		cout << "ERR " << object << endl;
		return -1;
	} else
		return pos;
}

int resolve_object(string object) {
	for (int i = 0; i < allObjects.size(); ++i)
		if (allObjects[i].compare(object) == 0)
			return i;
	cout << "ERR: Object not found in allOBjects array";
	return -1;
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
				} else if (i > 3) {
					string tmp;
					int action_num = -1;
					getline(fin, tmp);
					std::istringstream(tmp) >> action_num;
					significantObjectIdx[line] = action_num;
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
					s.SetAction(field, actions);
					s.SetNumState(i);

					cout << "Objects:";
					int objectIdx = significantObjectIdx[field];
					s.SetSignificantObject(-1);
					int sig_counter = 0;

					while (getline(iss, field, ' ') && objectIdx >= sig_counter) {
						cout << " " << resolve_object(field) << " " << allObjects[resolve_object(field)];
						s.SetSignificantObject(resolve_object(field));
						++sig_counter;
					}
					cout << endl;
					inst.AddState(s);
				} else if (i == inst.GetNumActions()) {
					inst.SetGoal(s);
				}
			}
			instances.push_back(inst);
		}
	}
	fin.close();

	for (unsigned i = 0; i < instances.size(); ++i) {
		vector<int> v = instances[i].GetSignificantObjects();
		significantObjects.insert(significantObjects.end(), v.begin(), v.end());
	}

	get_all_states();
	fin.open("non_applicable.txt");
	if (fin.is_open()) {
		int cnt = 0;
		while (!fin.eof() && getline(fin, line)) {
			if (line.compare("") == 0) {
				++cnt;
				continue;
			}
			if (line.compare("-NO-OP-") == 0)
				continue;

			istringstream iss(line);
			getline(iss, field, ' ');
			string action = field;
			int sigObj = significantObjectIdx[action];
			for (unsigned j = 0; j <= sigObj; ++j) {
				getline(iss, field, ' ');
			}
			string object = field;
			int objectIdx = resolve_object(object);
			if (allStates[cnt].GetSignificantObject() == objectIdx)
				continue;
			int actionIdx = -1;
			for (unsigned k = 0; k < actions.size(); ++k) {
				if (actions[k].compare(action) == 0) {
					actionIdx = k;
					break;
				}
			}
			const NegativeExample nex(action, actionIdx, objectIdx);
			vector<NegativeExample> neg = negativeExamples[cnt];

			if (neg.empty() || std::find(neg.begin(), neg.end(), nex) == neg.end())
				negativeExamples[cnt].push_back(nex);
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
//	cout << "\tAction denotations" << endl;
//	for (unsigned i = 0; i < actions.size(); ++i) {
//		cout << actions[i] << ": ";
//		for (unsigned j = 0; j < (*aDenot)[i].size(); ++j) {
//			if ((*aDenot)[i][j])
//				cout << '+';
//			else
//				cout << '-';
//		}
//		cout << endl;
//	}

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
	for (unsigned i = 0; i < rootConcepts.size(); ++i) {
		((ConceptNode*) rootConcepts[i])->UpdateDenotations(instances, &allObjectsIdx);
	}
	for (unsigned i = 0; i < rootRoles.size(); ++i) {
		((RoleNode*) rootRoles[i])->UpdateDenotations(instances, &allObjectsIdx);
	}
}

void learn_concepts() {
	initialize();
	combine_roles();
	//combine_concepts();
	syntax_concepts();
}

Expression* get_expression_from_hitset(int pos) {
	Expression* ex = minHittingSets[pos][0];
	for (unsigned i = 1; i < minHittingSets[pos].size(); ++i) {
		ex = new Join(ex, minHittingSets[pos][i], preops);
	}
	ex->SimplifyDenotations();
	return ex;
}

void make_rules() {
	for (unsigned i = 0; i < minHittingSets.size(); ++i) {
		Expression* ex = get_expression_from_hitset(i);
		Rule r(ex, allStates[i].GetAction());
		ruleSet.push_back(r);
	}
	vector<int> correctly_covered;
	for (unsigned i = 0; i < ruleSet.size(); ++i) {
		string current_signature = ruleSet[i].GetConcept()->GetSignature();
		for (unsigned j = 0; j < allStates.size(); ++j) {
			if (find(correctly_covered.begin(), correctly_covered.end(), j) != correctly_covered.end())
				continue;
			if (current_signature[j] == '1') {

				ruleSet[i].IncCoverage();
				if (ruleSet[i].GetAction().compare(allStates[j].GetAction()) == 0) {
					if (ruleSet[i].GetConcept()->GetSignificantObjectSign(j, allStates[j].GetSignificantObject()).compare(
							"1") == 0) {
						cout << i << " ";
						correctly_covered.push_back(j);
						ruleSet[i].IncCorrect();
					}
				}
			}

		}
	}

	if (correctly_covered.size() != allStates.size()) {
		cout << endl << "Correctly covered:" << correctly_covered.size() << " " << allStates.size() << endl;
	}

	sort(ruleSet.begin(), ruleSet.end());
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
	make_rules();
	for (unsigned i = 0; i < ruleSet.size(); ++i) {
		if (ruleSet[i].GetCorrect() > 0)
			ruleSet[i].SaveRule(fout);
	}
	fout << endl;
}

void make_features_multiple_hitset() {
	for (unsigned i = 0; i < minHittingSets.size(); ++i) {
		for (unsigned j = 0; j < minHittingSets[i].size(); ++j) {
			if (find(multipleHitsetFeatures.begin(), multipleHitsetFeatures.end(), minHittingSets[i][j])
					== multipleHitsetFeatures.end())
				multipleHitsetFeatures.push_back(minHittingSets[i][j]);
		}
	}

	cout << endl << " --Expressions -- " << endl;
	cout << "Number of features: " << multipleHitsetFeatures.size() << endl;
	for (unsigned i = 0; i < multipleHitsetFeatures.size(); ++i) {
		multipleHitsetFeatures[i]->infix(cout);
		cout << endl;
	}
}

void debug_output() {
	ofstream fout;
	fout.open("debug.txt");
	for (unsigned i = 0; i < rootConcepts.size(); ++i) {
		rootConcepts[i]->infix(fout);
		fout << " hits:" << rootConcepts[i]->GetHits() << " weight:" << rootConcepts[i]->GetWeight() << " ratio:"
				<< rootConcepts[i]->GetHits() / rootConcepts[i]->GetWeight();
		fout << endl;
		fout << rootConcepts[i]->GetSignificantObjectSignature(&significantObjects) << endl;
	}
	fout.flush();
	fout.close();
}
}
using namespace mn;

int main(int argc, char** argv) {
	get_input();
	printout();
	initialize_concepts();
	preops = new PreOps(allObjects.size());
	aDenot = new ActionDenotations(&instances, &actions);
	float t0, tf;
	t0 = time_used();
	learn_concepts();
	generate_concept_sets();
	make_features_multiple_hitset();
//	make_policy();
//	sort(ruleSet.begin(), ruleSet.end());

	write_policy();
	debug_output();
	tf = time_used();
	cout << endl << "Total time: ";
	report_interval(t0, tf, cout);
	cout << endl;
	cleanup();
	return 0;
}

