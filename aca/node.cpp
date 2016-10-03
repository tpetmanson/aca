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
#include <sstream>
#include <functional>
#include <set>

#include "node.h"

BEGIN_NAMESPACE(aca)


CppNode::CppNode(const int node_id, const int depth) : node_id(node_id), depth(depth), value("") { }

CppNode::CppNode(const int node_id, const int depth, const std::string& value) : node_id(node_id), depth(depth), value(value) { }

NodePtr CppNode::get_outnode(const std::string& key) const {
    auto iter = outs.find(key);
    if (iter != outs.end()) {
        return iter->second;
    }
    return NodePtr();
}

void CppNode::set_outnode(const std::string& key, const NodePtr value) {
    outs[key] = value;
}

void CppNode::add_match(const NodePtr node) {
    matches.push_back(node);
}

bool CppNode::operator==(const CppNode& n) const {
    return node_id == n.node_id;
}

END_NAMESPACE

BEGIN_NAMESPACE(std)
template <> struct hash<aca::CppNode> {
    size_t operator()(const aca::CppNode& node) const {
        return static_cast<size_t>(node.get_id());
    }
};
END_NAMESPACE

BEGIN_NAMESPACE(aca)

std::string CppNode::str() const {
    std::stringstream ss;
    std::string indent;
    for (int i=0 ; i<depth+1 ; ++i) indent += "  ";
    ss << indent << "VALUE: <" << value << ">\n";
    for (auto iter=outs.begin() ; iter != outs.end() ; ++iter) {
        ss << indent << iter->first << "\n";
        ss << iter->second->str();
    }
    return ss.str();
}


END_NAMESPACE
