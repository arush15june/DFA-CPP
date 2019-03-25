/*
    Deterministic Finite Automaton
        - arush15june 25/03/2019
*/
#include <iostream>
#include <array>
#include <vector>
#include <utility>
#include <string>
#include <sstream>
#include <fstream>
#include <algorithm> 
#include <cctype>
#include <locale>

#define MAXVERT 1000

struct StateDiagram {
    /*
        State diagram as an edge weight undirected graph

        @param vector< vector<pair<int, int>> > states: list of adjacency list
            pair.first => edge weight used as input director, -1 if none
                Edge Weight in ASCII represents string to select.
            pair.second => state no, -1 if first state
        @param vector<int> degree: outdegrees of all states
        @param int nvertices: number of vertices
        @param int nedges: number of edges
    */


    std::array<std::vector<std::pair<int, int>>, MAXVERT+1> states;
    std::array<int, MAXVERT+1> degree;
    int nvertices;
    int nedges;

    StateDiagram() {
        nvertices = 0;
        nedges = 0;
    }

    std::vector<std::pair<int, int> > getState(int index) {
        /*
            get the adjacency list vector for a certain state

            @param int index: state no
            @return std::vector<std::pair<int, int>> adjList: adjacency list of state `index`
        */

        return states[index];
    }

    void insertEdge(int state_no, int weight, int y) {
        /*
            Add edge to an adjacency list.

            @param int state_no: state to add to
            @param int weight: weight of edge
            @param int y: state no of edge
        */
        
        auto pair = std::make_pair(weight, y);
        auto& state = states[state_no];
        
        state.push_back(pair);
    }

    void printList() {
        for(auto i = 1; i <= nvertices; i++) {
            auto adjList = states[i];
            std::cout<<i<<": ";
            for (auto pair: adjList) {
                std::cout<<pair.first<<" "<<pair.second<<" ";
            }
            std::cout<<std::endl;
        }
    }
};

class DFA {
    /*
        Executes a DFA over input symbols.

        Elements of a DFA
        Q: Finite Set of states
            Represented via StateDiagram
        E: Input symbols
            Represented as a string (std::string)
        q: initial state
            represented as an integer
        f: final state
            represented as an integer

        @param StateDiagram state_diagram: state diagram; q
        @param int q: initial state
        @param int f: final state
    */
    StateDiagram state_diagram;
    int q;
    int f;

public:
    DFA() {};
    DFA(StateDiagram graph, int init_state, int final_state) : state_diagram {graph}, q{init_state}, f{final_state} {}

    bool execute(std::string input) {
        /*
        Execute the DFA over the input string.

        TODO: Templatize for any type of vector of input symbols

        Algorithm
            1) set current state to q i.e initial state of DFA.
            2) Iterate over the string.
                a) get the adjacency list for the current state.
                b) Iterate over the adjancency list
                    i) if the state's weight is same as the current symbols ASCII value
                        set the current state to the state's no and break the iteration.
            3) If the current state is same as the final state, set the final_state_reached flag true
            4) return final_state_reached

        @param std::string input: Input string to execute DFA on.
        @return bool final_state_reached: True if the final state was reached, else False.
        */

        bool final_state_reached = false;
        int current_state = q; 

        for(char symbol: input) {
            auto symbol_val = int(symbol);
            auto curr_adj_list = state_diagram.getState(current_state);

            for(auto &state: curr_adj_list) {
                if(state.first == symbol_val and state.second != current_state) {
                    current_state = state.second;
                    break;
                }
            }
        }


        if(current_state == f) final_state_reached = true;

        return final_state_reached;
    }

    void setInitialState(int init_state) {
        q = init_state;
    }
    
    int getInitState() {
        return q;
    }
    
    void setFinalState(int final_state) {
        f = final_state;
    }

    int getFinalstate() {
        return f;
    }

    void setStateDiagram(StateDiagram diag) {
        state_diagram = diag;
    }

    StateDiagram getStateDiagram() {
        return state_diagram;
    }
    
};

/* https://stackoverflow.com/questions/9435385/split-a-string-using-c11 */
std::vector<std::string> split(const std::string &s, char delim) {
    std::stringstream ss(s);
    std::string item;
    std::vector<std::string> elems;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
        // elems.push_back(std::move(item)); // if C++11 (based on comment from @mchiasson)
    }
    return elems;
}

/* https://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring */
// trim from start (in place)
static inline void ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
        return !std::isspace(ch);
    }));
}

// trim from end (in place)
static inline void rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

// trim from both ends (in place)
static inline void trim(std::string &s) {
    ltrim(s);
    rtrim(s);
}

// trim from start (copying)
static inline std::string ltrim_copy(std::string s) {
    ltrim(s);
    return s;
}

// trim from end (copying)
static inline std::string rtrim_copy(std::string s) {
    rtrim(s);
    return s;
}

// trim from both ends (copying)
static inline std::string trim_copy(std::string s) {
    trim(s);
    return s;
}

DFA build_dfa_from_file(std::string filename) {
    /*
    Build DFA and StateDiagram from file.

    File Structure:
        (Adjacency List's)

    First Line
        q -> initial state
    Second Line
        f -> final state
    Next Line(s):
        <vertex_no>: <weight> <vertex_no> | [<weight> <vertex_no> ... | ...] 
    ``` dfa.gph
    1               -> initial state
    2               -> final state
    1: 97 2 | 37 3    -> Adjacency List for Node 1
    2: 97 1 | 27 3
    3: 37 1 | 27 2
    ```

    @param std::string filename: filename to read from
    @return DFA dfa: returns a DFA.
    */

    StateDiagram state_diagram;
    DFA dfa;
    
    std::ifstream state_file(filename);
    std::string str;
    int counter = 0;
    int nvertices, nedges;

    while(std::getline(state_file, str)) {
        if(counter == 0) {
            dfa.setInitialState(std::stoi(str));
        }
        else if(counter == 1){
            dfa.setFinalState(std::stoi(str));
        }
        else {
            /*  Build State Diagram
                - Split at colon.
                - set current state
                - Split the part after colon at |
                - Iterate over split at |
                    - Trim whitespace
                    - Split pair at space
                    - Convert to integers
                        weight -> pair[0]
                        state_no_y -> pair[1]
                    - insert edge in state diagram.
            */
            auto split_at_colon = split(str, ':');
            auto state = std::stoi(split_at_colon[0]);

            nvertices += 1;
            
            auto adj_list_vector = split(split_at_colon[1], '|');
            auto degree = 0;
            for(auto &pair: adj_list_vector) {
                auto trimmed_pair = trim_copy(pair);
                auto split_pair = split(trimmed_pair, ' ');
                
                int weight = std::stoi(split_pair[0]);
                int state_no_y = std::stoi(split_pair[1]);
                
                state_diagram.insertEdge(state, weight, state_no_y);
                nedges += 1;
                degree += 1;
            }
            state_diagram.degree[state] = degree;
        }
        counter++;
    }
    state_diagram.nvertices = nvertices;
    state_diagram.nedges = nedges;

    dfa.setStateDiagram(state_diagram);

    return dfa;
}

int main(int argc, char** argv) {
    
    if(argc < 3) {
        std::cout<<"Invalid Input!"<<std::endl;
        std::cout<<"Usage: "<<std::endl;
        std::cout<<"./dfa <dfa_filename> <input_string>"<<std::endl;
        
        return 1;
    }

    std::string dfa_filename = std::string(argv[1]);
    std::string input_string = std::string(argv[2]);

    std::cout<<"Building DFA from "<<dfa_filename<<std::endl;
    auto dfa = build_dfa_from_file(dfa_filename);

    auto evaluate = dfa.execute(input_string);

    std::cout<<"Input: "<<input_string<<std::endl;
    if(evaluate) {
        std::cout<<"Evaluation: True"<<std::endl;
        return 0;
    }
    else {
        std::cout<<"Evaluation: False"<<std::endl;
        return 1;
    }
}