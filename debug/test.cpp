#include <iostream>
#include "match.h"
#include "node.h"
#include "automaton.h"
using namespace aca;

int main() {
    StringVector strvec;
    strvec.push_back(std::string("sunlabob"));
    strvec.push_back(std::string("renewable"));
    strvec.push_back(std::string("energy"));
    strvec.push_back(std::string("receive"));
    CppAutomaton* au = CppAutomaton::deserialize_from("/Users/timo/projects/leech/appdata/persontypes.aca");
    MatchVector matches = au->get_matches(strvec);
    for (int i=0 ; i<matches.size() ; ++i) {
        std::cout << matches[i].str() << std::endl;
    }
    delete au;
}
