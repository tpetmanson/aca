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
#include "automaton.h"
#include "match.h"
#include "node.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <deque>
#include <exception>
#include <set>


BEGIN_NAMESPACE(aca)

CppAutomaton::CppAutomaton() : uptodate(false) {
    root = std::make_shared<CppNode>(0, -1);
    nodes.push_back(root);
}

void CppAutomaton::add(const StringVector& pattern, const std::string& value) {
    #ifdef ACA_DEBUG
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
        //alphabet.insert(*elem);
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
    #ifdef ACA_DEBUG
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
        #ifdef ACA_DEBUG
            std::cout << "processing node " << node_id << " value " << node->value << "\n";
        #endif
        for (auto iter=node->outs.begin() ; iter != node->outs.end() ; ++iter) {
            auto dest_node = iter->second;
            #ifdef ACA_DEBUG
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
            #ifdef ACA_DEBUG
                std::cout << "    dest node has " << dest_node->matches.size() << " matches\n";
            #endif
        }
    }
    this->fail_table = fail_table;
    this->remove_duplicate_matches();
    this->uptodate = true;
}

void CppAutomaton::remove_duplicate_matches() {
    for (int i=0 ; i<nodes.size() ; ++i) {
        std::set<int> s;
        for (int j=0 ; j<nodes[i]->matches.size() ; ++j) {
            s.insert(nodes[i]->matches[j]->get_id());
        }
        nodes[i]->matches.clear();
        for (auto j=s.begin() ; j != s.end() ; ++j) {
            nodes[i]->matches.push_back(nodes[*j]);
        }
    }
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
        #ifdef ACA_DEBUG
            std::cout << "matching pos " << idx << " " << text[idx] << " with node " << node->node_id << " value " << node->value << std::endl;
        #endif
        if (node->value != "") {
            for (NodePtr resnode : node->matches) {
                const int start = idx - resnode->depth;
                const int end = idx + 1;
                if (start < end) {
                    #ifdef ACA_DEBUG
                        std::cout << "adding match " << start << " " << end << std::endl;
                    #endif
                    matches.push_back(CppMatch(start, end, resnode->get_value()));
                    #ifdef ACA_DEBUG
                        std::cout << "  " << matches[matches.size()-1].str() << std::endl;
                    #endif
                }
            }
        }
    }
    // sort the matches
    std::sort(matches.begin(), matches.end(), [](const CppMatch& a, const CppMatch& b) {
        if (a.get_start() == b.get_start()) {
            return a.get_end() < b.get_end();
        }
        return a.get_start() < b.get_start();
    });
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
    vec.reserve(this->nodes.size()); // ~approximate size
    StringVector strvec;
    this->__get_patterns_values(root, vec, strvec);
    return vec;
}

void CppAutomaton::__get_patterns_values(NodePtr node, KeyValueVector& vec, StringVector& strvec) const {
    if (strvec.size() > 0 && node->get_value().size() > 0) {
        vec.push_back(KeyValue(strvec, node->get_value()));
    }
    for (auto iter=node->outs.begin() ; iter != node->outs.end() ; ++iter) {
        strvec.push_back(iter->first);
        this->__get_patterns_values(iter->second, vec, strvec);
        strvec.pop_back();
    }
}

KeyValueVector CppAutomaton::get_prefixes_values() const {
    KeyValueVector vec;
    vec.reserve(this->nodes.size()*2); // ~approximate size
    StringVector strvec;
    this->__get_prefixes_values(root, vec, strvec);
    return vec;
}

void CppAutomaton::__get_prefixes_values(NodePtr node, KeyValueVector& vec, StringVector& strvec) const {
    vec.push_back(KeyValue(strvec, node->get_value()));
    for (auto iter=node->outs.begin() ; iter != node->outs.end() ; ++iter) {
        strvec.push_back(iter->first);
        this->__get_prefixes_values(iter->second, vec, strvec);
        strvec.pop_back();
    }
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SERIALIZATION
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const std::string AUTOMATON_MARKER = "A";
const std::string FAILTABLE_MARKER = "F";
const std::string NODE_MARKER = "N";
const std::string OUT_MARKER = "O";
const std::string MATCHES_MARKER = "M";

void CppAutomaton::serialize_to_stream(std::ostream& os) {
    if (!this->uptodate) {
        this->update_automaton();
    }
    // write generic information
    os << AUTOMATON_MARKER << " " << nodes.size() << " " << uptodate << "\n";
    // write fail table
    os << FAILTABLE_MARKER << " " << fail_table.size();
    for (int i=0 ; i<fail_table.size() ; ++i) {
        os << " " << fail_table[i];
    }
    os << "\n";
    // write individual nodes
    for (int i=0 ; i<nodes.size() ; ++i) {
        #ifdef ACA_DEBUG
            std::cout << "Writing node " << i << "\n";
        #endif
        NodePtr node = nodes[i];
        if (node->node_id != i) {
            std::cerr << "ERROR! node[" << i << "]->node_id = " << node->node_id << "\n";
        }
        os << NODE_MARKER << " " << node->node_id << " " << node->depth << " " << node->outs.size() << " ";
        os << node->value << '\0';
        os << OUT_MARKER << " ";
        for (auto j = node->outs.begin() ; j != node->outs.end() ; ++j) {
            os << j->first << '\0';
            os << j->second->node_id << " ";
        }
        os << MATCHES_MARKER << " " << node->matches.size();
        for (int k=0 ; k<node->matches.size() ; ++k) {
            os << " ";
            os << node->matches[k]->node_id;
        }
        os << " ";
    }
}


void CppAutomaton::serialize_to(const std::string filename) {
    std::ofstream fout(filename);
    serialize_to_stream(fout);
    fout.close();
}

std::string CppAutomaton::serialize() {
    std::stringstream ss;
    serialize_to_stream(ss);
    return ss.str();
}

CppAutomaton* CppAutomaton::deserialize_from_stream(std::istream& is) {
    CppAutomaton* cppauto = new CppAutomaton();

    std::string tmpstr;
    char tmpchr;
    long nnodes;

    is >> tmpstr >> nnodes >> cppauto->uptodate;
    if (tmpstr != AUTOMATON_MARKER) {
        std::string err = "ERROR! Automaton marker not found!";
        std::cerr << err;
        throw new std::runtime_error(err);
    }
    // read fail table
    is >> tmpstr >> nnodes;
    if (tmpstr != FAILTABLE_MARKER) {
        std::string err = "ERROR! Failtable marker not found!";
        std::cerr << err;
        throw new std::runtime_error(err);
    }
    cppauto->fail_table.resize(nnodes);
    for (int i=0 ; i<cppauto->fail_table.size() ; ++i) {
        is >> cppauto->fail_table[i];
    }
    #ifdef ACA_DEBUG
        std::cout << "Fail table size is " << cppauto->fail_table.size() << "\n";
        std::cout.flush();
    #endif

    // create nodes
    cppauto->nodes.clear();
    for (int i=0 ; i<nnodes ; ++i) {
        #ifdef ACA_DEBUG
            std::cout << "Creating node " << i << "\n"; std::cout.flush();
        #endif
        cppauto->nodes.push_back(std::make_shared<CppNode>(0, 0));
    }
    #ifdef ACA_DEBUG
        std::cout << "Nodes created!\n;"; std::cout.flush();
    #endif
    // prepare reading node
    for (int i=0 ; i<cppauto->nodes.size() ; ++i) {
        int outsize = 0;
        NodePtr node = cppauto->nodes[i];
        is >> tmpstr >> node->node_id >> node->depth >> outsize;
        if (tmpstr != NODE_MARKER) {
            std::stringstream ss;
            ss << "ERROR! Node marker not found! Found <" << tmpstr << "> instead at byte " << is.tellg();
            std::cerr << ss.str();
            throw new std::runtime_error(ss.str());
        }
        is.get(); // eat space char
        std::getline(is, node->value, '\0');
        #ifdef ACA_DEBUG
            std::cout << "Read node " << node->node_id << "\n"; std::cout.flush();
        #endif

        if (node->node_id != i) {
            std::stringstream ss;
            ss << "ERROR! Node id <" << node->node_id << "> for node <" << i << "> do not match!";
            std::cerr << ss.str();
            throw new std::runtime_error(ss.str());
        }

        // read outs
        is >> tmpstr;
        if (tmpstr != OUT_MARKER) {
            std::stringstream ss;
            ss << "ERROR! Out marker not found! Found <" << tmpstr << "> instead at byte " << is.tellg();
            std::cerr << ss.str();
            throw new std::runtime_error(ss.str());
        }
        node->outs.clear();
        for (int j=0 ; j<outsize ; ++j) {
            is.get(); // eat space char
            getline(is, tmpstr, '\0');
            int destnode;
            is >> destnode;
            node->outs[tmpstr] = cppauto->nodes[destnode];
        }
        // read matches
        is >> tmpstr;
        if (tmpstr != MATCHES_MARKER) {
            std::stringstream ss;
            ss << "ERROR! Match marker not found! Found <" << tmpstr << "> instead at byte " << is.tellg();
            std::cerr << ss.str();
            throw new std::runtime_error(ss.str());
        }
        int matchsize;
        is >> matchsize;
        for (int j=0 ; j<matchsize ; ++j) {
            int matchid;
            is >> matchid;
            node->matches.push_back(cppauto->nodes[matchid]);
        }
    }
    cppauto->root = cppauto->nodes[0];

    cppauto->remove_duplicate_matches();
    return cppauto;
}

CppAutomaton* CppAutomaton::deserialize_from(const std::string filename) {
    std::ifstream fin(filename);
    CppAutomaton* automaton = deserialize_from_stream(fin);
    fin.close();
    return automaton;
}

CppAutomaton* CppAutomaton::deserialize(const std::string serialized) {
    std::stringstream ss(serialized);
    return deserialize_from_stream(ss);
}

END_NAMESPACE
