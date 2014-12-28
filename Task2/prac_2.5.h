#include <vector>
#include <stack>
#include <map>
#include <set>
using namespace std;

enum ActionType {
    SHIFT, REDUCE, ACCEPT, REJECT, GOTO
};

struct Action {
    ActionType type;
    int number;
    Action() {
        type = REJECT;
        number = 0;
    }
    Action(ActionType newType, int newnumber) {
        type = newType;
        number = newnumber;
    }
    Action(ActionType newType) {
        type = newType;
        number = 0;
    }
};

struct State {
    int rule;
    int position;
    int lookahead;
    State(int Newrule, int Newposition, int Newlookahead) {
        rule = Newrule;
        position = Newposition;
        lookahead = Newlookahead;
    }
    bool operator<(const State &st) const {
        auto p1 = make_pair(make_pair(rule, position), lookahead);
        auto p2 = make_pair(make_pair(st.rule, st.position), st.lookahead);
        return p1 < p2;
    }
};

class LRA {
    string fileName;
    vector<vector<Action> > table;
    int numberRules;
    int numberSymbols;
    vector<int> leftRules;
    vector<vector<int> > rightRules;
    vector<vector<set<int> > > beginSuf;
    vector<vector<bool> > beginConditionSuf;
    map<string, int> symbols;
    set<int> terminal;
    int numberSets = 0;
    map<set<State>, int> stateSets;
    vector<set<int> > begin;
    vector<bool> flags;
    set<int> beginCondition;
    vector<set<State> > states;
public:
    void pop(stack<int> &st, int cnt);
    int get(const string &s);
    void read(string grammarFile);
    vector<vector<int> >  startGraph();
    void dfsStartGraph(int v, vector<vector<int> > graph);
    void dfsStart(int numberber, vector<vector<int> > graph, int letter);
    void startSuff();
    vector<State> getClojure(set<State> states);
    set<State> successor(set<State> v, int letter);
    int dfs(set<State> currStates);
    LRA(string grammarFile);
    bool run(string s);
};