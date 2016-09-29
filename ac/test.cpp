/*
Aho-Corasick keyword tree + automaton implementation for Python.
Copyright (C) 2016 Funderbeam OÜ ( tpetmanson@gmail.com )

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/
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

