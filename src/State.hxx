/*
* State.hxx
*
* Created on: May 10, 2013
* Author: dloti
*/

#ifndef STATE_HXX_
#define STATE_HXX_
#include <map>
#include <vector>
#include <string>
#include <iostream>
class State {
        int type, number;
        std::map<std::string, std::vector<int> > conceptInterpretation;
        std::map<std::string, std::vector<std::pair<int, int> > > roleInterpretation;
        std::string action;
        int significantObject;
        int action_num;
        bool covered;
public:
        State();
        virtual ~State();
        inline void AddConceptInterpretation(std::string concept, std::vector<int> interpretation) {
                conceptInterpretation[concept] = interpretation;
        }

        inline void AddRoleInterpretation(std::string role, std::vector<std::pair<int, int> > interpretation) {
                roleInterpretation[role] = interpretation;
        }

        inline std::vector<int> GetConceptInterpretation(std::string name) {
                return conceptInterpretation[name];
        }

        inline std::vector<std::pair<int, int> > GetRoleInterpretation(std::string name) {
                return roleInterpretation[name];
        }

        inline void SetNumState(int stateNum) {
                number = stateNum;
        }

        inline void SetGoal() {
                type = 2;
        }

        inline bool IsGoal() {
                return (type == 2);
        }

        inline void SetAction(std::string a, std::vector<std::string> actions) {
                action = a;
                for(int i=0;i<actions.size();++i){
                        if(actions[i].compare(a)==0){
                                action_num = i;
                                break;
                        }
                }
        }

        inline void SetSignificantObject(int object) {
                this->significantObject = object;
                }

        inline int GetSignificantObject() {
                        return this->significantObject;
                        }

        inline void Covered() {
                covered = true;
        }

        inline bool IsCovered() {
                return covered;
        }

        inline std::string GetAction() {
                return action;
        }

        inline int GetActionNum() {
                return action_num;
        }

        void Print(std::vector<std::string> allObjects);
};

#endif /* STATE_HXX_ */
