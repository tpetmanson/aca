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
#ifndef AC__AUTOMATON_H
#define AC__AUTOMATON_H

#include "aca.h"
#include <set>

BEGIN_NAMESPACE(aca)

class CppAutomaton {
private:
    NodePtr root;
    //std::set<std::string> alphabet;
    NodeVector nodes;
    IntVector fail_table;
    bool uptodate;
protected:
    NodePtr goto_node(const int node_id, const std::string& elem);
public:
    CppAutomaton();

    // add a new pattern (key) and associate it with a value
    void add(const StringVector& pattern, const std::string& value);

    // given a prefix pattern, find the node that represents it
    NodePtr find_node(const StringVector& prefix) const;

    // check if automaton contains the full pattern.
    bool has_pattern(const StringVector& pattern) const;

    // check if automaton contains the prefix.
    bool has_prefix(const StringVector& prefix) const;

    // rebuild the automaton
    void update_automaton();

    void remove_duplicate_matches();

    MatchVector get_matches(const StringVector& text, bool exclude_overlaps=true);

    // get the value of specified key.
    std::string get_value(const StringVector& pattern) const;

    // get all the patterns and their representive values in the automaton
    KeyValueVector get_patterns_values() const;
    void __get_patterns_values(NodePtr node, KeyValueVector& vec, StringVector& strvec) const;

    // get all the patterns+prefixes and their representive values in the automaton
    KeyValueVector get_prefixes_values() const;
    void __get_prefixes_values(NodePtr node, KeyValueVector& vec, StringVector& strvec) const;

    // serialization
    void serialize_to(const std::string filename);
    std::string serialize();
    void serialize_to_stream(std::ostream& os);

    // deserialize automaton from a file
    static CppAutomaton* deserialize_from_stream(std::istream& is);
    static CppAutomaton* deserialize_from(const std::string filename);
    static CppAutomaton* deserialize(const std::string serialized);

    // print the structure of the automaton
    std::string str() const;
};

END_NAMESPACE

#endif
