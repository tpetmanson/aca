#include <iostream>
#include "match.h"
#include "node.h"
#include "automaton.h"
using namespace ac;


int main() {
    Automaton automaton;
    StringVector vec1 = {"h", "e"};
    StringVector vec2 = {"s", "h", "e"};
    StringVector vec3 = {"h", "e", "r"};
    StringVector vec4 = {"u", "s"};
    StringVector text = {"u", "s", "h", "e", "r"};
    std::string val = "PER";

    automaton.add(vec1, val);
    automaton.add(vec2, val);
    automaton.add(vec3, val);
    automaton.add(vec4, val);

    automaton.update_automaton();

    auto matches = automaton.get_matches(text);
    for (auto match : matches) {
        std::cout << match.str() << "\n";
    }
    return 0;
}

