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
#ifndef AC__NODE_H
#define AC__NODE_H

#include "aca.h"

BEGIN_NAMESPACE(aca)


class CppNode {
private:
    int node_id, depth;
    std::string value;
    std::map<std::string, NodePtr> outs;
    NodeVector matches;
public:
    CppNode(const int node_id, const int depth);
    CppNode(const int node_id, const int depth, const std::string& value);

    int get_id() const { return node_id; }
    int get_depth() const { return depth; }
    void set_value(const std::string& value) { this->value = value; }
    std::string get_value() const { return value; }
    NodePtr get_outnode(const std::string& key) const;
    void set_outnode(const std::string& key, const NodePtr value);
    void add_match(const NodePtr node);

    bool operator==(const CppNode& n) const;
    std::string str() const;

    friend class CppAutomaton;
};

END_NAMESPACE

#endif
