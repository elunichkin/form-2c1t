#include "prac_2.5.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>

LRA::LRA(string grammarFile) {
    fileName = grammarFile;
    for (size_t i = 0; i < 26; i++) {
        string s;
        char current = 'a' + i;
        s += current;
        symbols.insert(make_pair(s, i));
        terminal.insert(i);
    }
    symbols.insert(make_pair("$", 26));
    terminal.insert(26);
    numberSymbols = 27;
    read(fileName);
    vector<vector<int> > graph = startGraph();
    begin.resize(numberSymbols);
    flags.resize(numberSymbols, false);
    int numberberTerm = terminal.size();
    for (size_t i = 0; i < numberberTerm; i++) {
        dfsStart(i, graph, i);
    }
    for (size_t i = 0; i < numberRules; i++) {
        if (rightRules[i].size() == 0) {
            dfsStartGraph(leftRules[i], graph);
        }
    }
    startSuff();
    State startState(numberRules - 1, 0, get("$"));
    set<State> startSet;
    startSet.insert(startState);
    dfs(startSet);
}

void LRA::pop(stack<int> &st, int cnt) {
    for (int i = 0; i < cnt; i++)
        st.pop();
}

int LRA::get(const string &s) {
    if (symbols.find(s) != symbols.end())
        return symbols[s];
    else
        return symbols[s] = numberSymbols++;
}

void LRA::read(string grammarFile) {
    ifstream fin(grammarFile);
    fin >> numberRules;
    rightRules.resize(numberRules + 1);
    leftRules.resize(numberRules + 1);
    for (int i = 0; i < numberRules; i++) {
        string left, rem;
        fin >> left;
        int left_number = get(left);
        leftRules[i] = left_number;
        getline(fin, rem);
        stringstream sin(rem);
        string right;
        while (sin >> right) {
            int right_number = get(right);
            rightRules[i].push_back(right_number);
        }
    }
    leftRules[numberRules] = numberSymbols;
    rightRules[numberRules].push_back(leftRules[0]);
    numberRules++;
    numberSymbols++;
}

vector<vector<int> >  LRA::startGraph() {
    vector<vector<int> > res(numberSymbols);
    for (int i = 0; i < numberRules; i++) {
        if (rightRules[i].size() == 0)
            continue;
        int begin = rightRules[i][0];
        res[begin].push_back(leftRules[i]);
    }
    return res;
}

void LRA::dfsStartGraph(int v, vector<vector<int> > graph) {
    if (beginCondition.count(v) != 0)
        return;
    beginCondition.insert(v);
    for (size_t i = 0; i < graph[v].size(); i++) {
        int to = graph[v][i];
        dfsStartGraph(to, graph);
    }
}


void LRA::dfsStart(int numberber, vector<vector<int> > graph, int letter) {
    if (begin[numberber].count(letter) == 1)
        return;
    begin[numberber].insert(letter);
    for (size_t i = 0; i < graph[numberber].size(); i++) {
        int vert = graph[numberber][i];
        dfsStart(vert, graph, letter);
    }
}

void LRA::startSuff() {
    beginSuf.resize(numberRules);
    beginConditionSuf.resize(numberRules);
    for (size_t i = 0; i < numberRules; i++) {
        beginSuf[i].resize(rightRules[i].size());
        beginConditionSuf[i].resize(rightRules[i].size());
        for (int j = (int)rightRules[i].size() - 1; j >= 0; j--) {
            beginSuf[i][j] = begin[rightRules[i][j]];
            if (beginCondition.count(rightRules[i][j]) == 1 &&
                j != (int)rightRules[i].size() - 1) {
                beginSuf[i][j].insert(
                    beginSuf[i][j + 1].begin(), beginSuf[i][j + 1].end());
                if (beginConditionSuf[i][j + 1])
                    beginConditionSuf[i][j] = true;
            }
            else if (beginCondition.count(rightRules[i][j]) == 1) {
                beginConditionSuf[i][j] = true;
            }
        }
    }
}

vector<State> LRA::getClojure(set<State> states) {
    vector<State> result(states.begin(), states.end());
    set<State> flagsRes(result.begin(), result.end());
    for (size_t i = 0; i < result.size(); i++) {
        int rule = result[i].rule;
        int position = result[i].position;
        int lookahead = result[i].lookahead;
        if (position == (int)rightRules[rule].size())
            continue;
        int s = rightRules[rule][position];
        if (terminal.count(s) != 0)
            continue;
        set<int> currentBegin;
        if (position + 1 < (int)beginSuf[rule].size()) {
            currentBegin = beginSuf[rule][position + 1];
            if (beginConditionSuf[rule][position])
                currentBegin.insert(lookahead);
        }
        else
            currentBegin.insert(lookahead);
        for (size_t j = 0; j < numberRules; j++) {
            if (leftRules[j] == s) {
                for (auto it = currentBegin.begin(); it != currentBegin.end(); it++) {
                    State st(j, 0, *it);
                    if (flagsRes.count(st) == 0) {
                        flagsRes.insert(st);
                        result.push_back(st);
                    }
                }
            }
        }
    }
    return result;
}

set<State> LRA::successor(set<State> v, int letter) {
    set<State> result;
    for (auto it = v.begin(); it != v.end(); it++) {
        State state = *it;
        if (state.position == (int)rightRules[state.rule].size())
            continue;
        int next_letter = rightRules[state.rule][state.position];
        if (letter == next_letter) {
            result.insert(State(state.rule, state.position + 1, state.lookahead));
        }
    }
    return result;
}

int LRA::dfs(set<State> currStates) {
    vector<State> clojure = getClojure(currStates);
    set<State> clojureSet(clojure.begin(), clojure.end());
    if (stateSets.find(clojureSet) != stateSets.end())
        return stateSets[clojureSet];
    stateSets[clojureSet] = numberSets++;
    states.push_back(clojureSet);
    table.resize(numberSets);
    table[numberSets - 1].resize(numberSymbols);
    vector<int> current(numberSymbols, -1);
    for (size_t i = 0; i < numberSymbols; i++) {
        set<State> next = successor(clojureSet, i);
        if (next.empty())
            continue;
        current[i] = dfs(next);
    }
    int stateNumber = stateSets[clojureSet];
    for (size_t i = 0; i < numberSymbols; i++) {
        if (terminal.count(i) == 0) {
            if (current[i] != -1)
                table[stateNumber][i] = Action(GOTO, current[i]);
        }
        else {
            for (auto it = clojureSet.begin(); it != clojureSet.end(); it++) {
                State state = *it;
                if (state.position == (int)rightRules[state.rule].size()) {
                    if (i == (int)terminal.size() - 1 && state.rule == numberRules - 1) {
                        table[stateNumber][i] = Action(ACCEPT);
                    }
                    else if (state.lookahead == i)
                        table[stateNumber][i] = Action(REDUCE, state.rule);
                }
                else {
                    int letter = rightRules[state.rule][state.position];
                    if (letter == i && current[letter] != -1) {
                        table[stateNumber][i] = Action(SHIFT, current[letter]);
                    }
                }
            }

        }
    }

    return stateNumber;
}

bool LRA::run(string s) {
    stack<int> stack;
    stack.push(0);
    s += "$";
    for (size_t i = 0; i < s.length();) {
        int cur_state = stack.top();
        string currentSym = "";
        currentSym += s[i];
        int currentChar = symbols[currentSym];
        switch (table[cur_state][currentChar].type) {
        case SHIFT: {
                        i++;
                        stack.push(currentChar);
                        stack.push(table[cur_state][currentChar].number);
                        break;
        }

        case REDUCE: {
                         int number = table[cur_state][currentChar].number;
                         pop(stack, 2 * rightRules[number].size());
                         cur_state = stack.top();
                         stack.push(leftRules[number]);
                         stack.push(table[cur_state][leftRules[number]].number);
                         break;
        }

        case ACCEPT: {
                         return true;
        }

        case REJECT: {
                         return false;
        }

        }
    }
    return false;
}


int main(int argv, char **argc) {
    if (argv != 2) {
        cerr << "Wrong number of input files. Should be exactly one.\n";
        return -1;
    }

    LRA a(argc[1]);

    int m;
    cin >> m;
    while (m--) {
        string w;
        cin >> w;
        cout << (a.run(w) ? "YES\n" : "NO\n");
    }

    return 0;
}