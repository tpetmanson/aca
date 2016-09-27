/*
Aho-Corasick keyword tree + automaton implementation for Python.
Copyright (C) 2016 Timo Petmanson ( tpetmanson@gmail.com )

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
#include "automaton.h"
#include "match.h"
#include "node.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <deque>

BEGIN_NAMESPACE(ac)

CppAutomaton::CppAutomaton() : uptodate(false) {
    root = std::make_shared<CppNode>(0, -1);
    nodes.push_back(root);
}

void CppAutomaton::add(const StringVector& pattern, const std::string& value) {
    #ifdef AC_DEBUG
        std::cout << "adding pattern with value <" << value << "> where pattern is ";
        for (const std::string& s : pattern) {
            std::cout << s << " ";
        }
        std::cout << "\n";
    #endif
    NodePtr node = root;
    NodePtr outnode;
    const std::string* elem;
    for (size_t depth=0 ; depth<pattern.size() ; ++depth) {
        elem = &pattern[depth];
        alphabet.insert(*elem);
        outnode = node->get_outnode(*elem);
        if (outnode) {
            node = outnode;
        } else {
            NodePtr newnode = std::make_shared<CppNode>(nodes.size(), depth);
            node->set_outnode(*elem, newnode);
            nodes.push_back(newnode);
            node = newnode;
        }
    }
    node->set_value(value);
    node->add_match(node);
    uptodate = false;
}

NodePtr CppAutomaton::find_node(const StringVector& prefix) const {
    NodePtr node = root;
    NodePtr outnode;
    for (size_t idx=0 ; idx<prefix.size() ; ++idx) {
        outnode = node->get_outnode(prefix[idx]);
        if (outnode) {
            node = outnode;
        } else {
            return NodePtr();
        }
    }
    return node;
}

bool CppAutomaton::has_pattern(const StringVector& pattern) const {
    NodePtr node = find_node(pattern);
    return node && node->get_value() != "";
}

bool CppAutomaton::has_prefix(const StringVector& prefix) const {
    NodePtr node = find_node(prefix);
    return node != NULL;
}

std::string CppAutomaton::get_value(const StringVector& pattern) const {
    NodePtr node = find_node(pattern);
    return node ? node->get_value() : std::string("");
}

NodePtr CppAutomaton::goto_node(const int node_id, const std::string& elem) {
    NodePtr node = this->nodes[node_id];
    auto iter = node->outs.find(elem);
    if (iter != node->outs.end()) {
        return iter->second;
    } else if (node_id == 0) {
        return this->root;
    }
    return NodePtr();
}

void CppAutomaton::update_automaton() {
    #ifdef AC_DEBUG
        std::cout << "updating automaton\n";
    #endif
    IntVector fail_table(nodes.size(), 0);
    NodePtr root = this->root;
    std::deque<int> Q;
    for (auto iter = root->outs.begin() ; iter != root->outs.end() ; ++iter) {
        Q.push_back(iter->second->node_id);
    }
    while (Q.size() > 0) {
        int node_id = Q[0]; Q.pop_front();
        NodePtr node = nodes[node_id];
        #ifdef AC_DEBUG
            std::cout << "processing node " << node_id << " value " << node->value << "\n";
        #endif
        for (auto iter=node->outs.begin() ; iter != node->outs.end() ; ++iter) {
            auto dest_node = iter->second;
            #ifdef AC_DEBUG
                std::cout << "    dest node id " << dest_node->node_id << " with key " << iter->first << "\n";
            #endif
            Q.push_back(dest_node->node_id);
            int fail_node_id = fail_table[node_id];
            while (goto_node(fail_node_id, iter->first) == NULL) {
                fail_node_id = fail_table[fail_node_id];
            }
            fail_table[dest_node->node_id] = goto_node(fail_node_id, iter->first)->node_id;
            // copy the matches
            NodePtr fail_node = nodes[fail_table[dest_node->node_id]];
            dest_node->matches.reserve(dest_node->matches.size() + fail_node->matches.size());
            std::copy(fail_node->matches.begin(), fail_node->matches.end(), std::back_inserter(dest_node->matches));
            #ifdef AC_DEBUG
                std::cout << "    dest node has " << dest_node->matches.size() << " matches\n";
            #endif
        }
    }
    this->fail_table = fail_table;
    this->uptodate = true;
}


MatchVector CppAutomaton::get_matches(const StringVector& text, const bool exclude_overlaps) {
    MatchVector matches;
    if (!this->uptodate) {
        this->update_automaton();
    }
    int node_id = this->root->node_id;
    for (size_t idx=0 ; idx<text.size() ; ++idx) {
        while (goto_node(node_id, text[idx]) == NULL) {
            node_id = this->fail_table[node_id]; // follow fail
        }
        NodePtr node = goto_node(node_id, text[idx]);
        node_id = node->node_id;
        #ifdef AC_DEBUG
            std::cout << "matching pos " << idx << " " << text[idx] << " with node " << node->node_id << " value " << node->value << "\n";
        #endif
        if (node->value != "") {
            for (NodePtr resnode : node->matches) {
                matches.push_back(CppMatch(idx - resnode->depth, idx+1, resnode->value));
                #ifdef AC_DEBUG
                    std::cout << "  " << matches[matches.size()-1].str() << "\n";
                #endif
            }
        }
    }
    if (exclude_overlaps) {
        return cpp_remove_overlaps(matches);
    }
    return matches;
}

std::string CppAutomaton::str() const {
    return root->str();
}

KeyValueVector CppAutomaton::get_patterns_values() const {
    KeyValueVector vec;
    return vec;
}

KeyValueVector CppAutomaton::get_prefixes_values() const {
    KeyValueVector vec;
    return vec;
}


void serialize_to_stream(CppAutomaton& automaton, std::ostream& os) {
}

void deserialize_from_stream(CppAutomaton& automaton, std::istream& is) {
}

void CppAutomaton::serialize_to(const std::string filename) {
    std::ofstream fout(filename);
    serialize_to_stream(*this, fout);
    fout.close();
}

std::string CppAutomaton::serialize() {
    std::stringstream ss;
    serialize_to_stream(*this, ss);
    return ss.str();
}

void CppAutomaton::deserialize_from(const std::string filename) {
    std::ifstream fin(filename);
    deserialize_from_stream(*this, fin);
    fin.close();
}

void CppAutomaton::deserialize(const std::string serialized) {
    std::stringstream ss(serialized);
    deserialize_from_stream(*this, ss);
}

END_NAMESPACE
