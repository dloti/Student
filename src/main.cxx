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

using namespace std;

vector<string> primitiveConcepts;
vector<string> primitiveRoles;
vector<string> allObjects;
vector<Instance> instances;

int get_obj_pos(string object) {
	int pos = std::find(allObjects.begin(), allObjects.end(), object)
			- allObjects.begin();
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

			for (int i = 0; i < inst.GetNumActions()+1; ++i) {
				State s;
				vector<int> conceptInterpretation;
				vector<pair<int, int> > roleInterpretation;

				//for concepts
				for (unsigned j = 0; j < primitiveConcepts.size()-1; ++j) {
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
						s.AddConceptInterpretation(cname,
								conceptInterpretation);
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

				if ((!fin.eof() && i < inst.GetNumActions()
						&& getline(fin, line))) {
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

void input_check() {
	cout << "Number of instances: " << instances.size() << endl;
	for (unsigned i = 0; i < instances.size(); ++i) {
		cout << instances[i].GetNumPlan() << " " << instances[i].GetNumActions()
				<< endl;
		for (unsigned j = 0; j < instances[i].GetStates().size(); ++j) {
			instances[i].GetStates()[j].Print(allObjects);
		}
		instances[i].GetGoal().Print(allObjects);
	}
	cout << endl;
	for (unsigned i = 0; i < primitiveConcepts.size(); i++)
		cout << primitiveConcepts[i] << " ";
	cout << endl;
	for (unsigned i = 0; i < primitiveRoles.size(); i++)
		cout << primitiveRoles[i] << " ";
	cout << endl;
	for (unsigned i = 0; i < allObjects.size(); i++)
		cout << allObjects[i] << " ";
	cout << endl;
}

int main(int argc, char** argv) {
	get_input();
	input_check();
	return 0;
}
