/**
 * Aho-Corasick keyword tree + automaton implementation
 */
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <deque>
#include <unordered_map>
#include <set>
#include <functional>
#include <algorithm>
#include <memory>

#define AC_DEBUG    1

class Node;
typedef std::shared_ptr<Node> NodePtr;
class Match;
typedef std::vector<Match> MatchVector;
typedef std::vector<std::string> StringVector;
typedef std::vector<int> IntVector;
typedef std::vector<NodePtr> NodeVector;


////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Match
////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Match {
private:
    int start, end;
    std::string label;
public:
    Match(const int start, const int end, const std::string& label);
    Match(const int start, const int end, const char* label);

    int get_start() const { return start; }
    int get_end() const { return end; }

    bool is_before(const Match& m) const;
    bool operator==(const Match& m) const;
    bool operator<(const Match& m) const;
    size_t size() const;
    std::string str() const;
};

Match::Match(const int start, const int end, const std::string& label) : start(start), end(end), label(label) { }
Match::Match(const int start, const int end, const char* label) : start(start), end(end), label(label) { }

bool Match::operator==(const Match& m) const {
    return start == m.start && end == m.end;
}

bool Match::is_before(const Match& m) const {
    return end <= m.start;
}

bool Match::operator<(const Match& m) const {
    if (start == m.start) {
        return end < m.end;
    }
    return start < m.start;
}

size_t Match::size() const {
    return end - start;
}

std::string Match::str() const {
    std::stringstream ss;
    ss << "Match(" << start << ", " << end << ", " << label << ")";
    return ss.str();
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////
// REMOVE OVERLAPS
////////////////////////////////////////////////////////////////////////////////////////////////////////////

MatchVector remove_overlaps(MatchVector matches) {
    if (matches.size() == 0) {
        return matches;
    }
    // sort the matches
    std::sort(matches.begin(), matches.end(), [](const Match& a, const Match& b) {
        if (a.get_start() == b.get_start()) {
            return a.get_end() < b.get_end();
        }
        return a.get_start() < b.get_start();
    });
    // compute the lengths
    IntVector lengths(matches.size(), 0);
    std::transform(matches.begin(), matches.end(), lengths.begin(), [](const Match& m) {
        return m.size();
    });
    IntVector scores = lengths;
    IntVector prev(scores.size(), -1);
    int highscore = -1;
    int highpos = -1;
    for (int i=1 ; i<matches.size() ; ++i) {
        int bestscore = -1;
        int bestprev = -1;
        int j = i;
        while (j >= 0) {
            // if spans do not overlap
            if (matches[j].is_before(matches[i])) {
                int l = scores[j] + lengths[i];
                if (l >= bestscore) {
                   bestscore = l;
                   bestprev = j;
                } else {
                    // in case of overlapping matches
                    l = scores[j] - lengths[j] + lengths[i];
                    if (l >= bestscore) {
                        bestscore = l;
                        bestprev = prev[j];
                    }
                }
            }
            j -= 1;
        }
        scores[i] = bestscore;
        prev[i] = bestprev;
        if (bestscore >= highscore) {
            highscore = bestscore;
            highpos = i;
        }
    }
    // back-track non-overlappng spans that we should keep
    IntVector keep;
    while (highpos != -1) {
        keep.push_back(highpos);
        highpos = prev[highpos];
    }
    // create new output array
    MatchVector result;
    result.reserve(keep.size());
    for (auto iter=keep.rbegin() ; iter != keep.rend() ; ++iter) {
        result.push_back(matches[*iter]);
    }
    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// NODE
////////////////////////////////////////////////////////////////////////////////////////////////////////////
class Automaton;

class Node {
private:
    int node_id, depth;
    std::string value;
    std::unordered_map<std::string, NodePtr> outs;
    NodeVector matches;
public:
    Node(const int node_id, const int depth);
    Node(const int node_id, const int depth, const std::string& value);

    int get_id() const { return node_id; }
    int get_depth() const { return depth; }
    void set_value(const std::string& value) { this->value = value; }
    std::string get_value() const { return value; }
    NodePtr get_outnode(const std::string& key) const;
    void set_outnode(const std::string& key, const NodePtr value);
    void add_match(const NodePtr node);

    bool operator==(const Node& n) const;

    std::string str() const;

    friend class Automaton;
};

Node::Node(const int node_id, const int depth) : node_id(node_id), depth(depth), value("") { }

Node::Node(const int node_id, const int depth, const std::string& value) : node_id(node_id), depth(depth), value(value) { }

NodePtr Node::get_outnode(const std::string& key) const {
    auto iter = outs.find(key);
    if (iter != outs.end()) {
        return iter->second;
    }
    return NodePtr();
}

void Node::set_outnode(const std::string& key, const NodePtr value) {
    outs[key] = value;
}

void Node::add_match(const NodePtr node) {
    matches.push_back(node);
}

bool Node::operator==(const Node& n) const {
    return node_id == n.node_id;
}

namespace std {
    template <> struct hash<Node> {
        size_t operator()(const Node& node) const {
            return static_cast<size_t>(node.get_id());
        }
    };
}

std::string Node::str() const {
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

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// AUTOMATON
////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Aho-Corasick keyword tree + automaton implementation.
 */
class Automaton {
private:
    NodePtr root;
    std::set<std::string> alphabet;
    NodeVector nodes;
    IntVector fail_table;
    bool uptodate;
protected:
    NodePtr goto_node(const int node_id, const std::string& elem);
public:
    Automaton();

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

    MatchVector get_matches(const StringVector& text, bool exclude_overlaps=true);

    // get the value of specified key.
    std::string get_value(const StringVector& pattern) const;

    std::string str() const;
};


Automaton::Automaton() : uptodate(false) {
    root = std::make_shared<Node>(0, -1);
    nodes.push_back(root);
}

void Automaton::add(const StringVector& pattern, const std::string& value) {
    NodePtr node = root;
    NodePtr outnode;
    const std::string* elem;
    for (int depth=0 ; depth<pattern.size() ; ++depth) {
        elem = &pattern[depth];
        alphabet.insert(*elem);
        outnode = node->get_outnode(*elem);
        if (outnode) {
            node = outnode;
        } else {
            NodePtr newnode = std::make_shared<Node>(nodes.size(), depth);
            node->set_outnode(*elem, newnode);
            nodes.push_back(newnode);
            node = newnode;
        }
    }
    node->set_value(value);
    node->add_match(node);
    uptodate = false;
}

NodePtr Automaton::find_node(const StringVector& prefix) const {
    NodePtr node = root;
    NodePtr outnode;
    for (int idx=0 ; idx<prefix.size() ; ++idx) {
        outnode = node->get_outnode(prefix[idx]);
        if (outnode) {
            node = outnode;
        } else {
            return NodePtr();
        }
    }
    return node;
}

bool Automaton::has_pattern(const StringVector& pattern) const {
    NodePtr node = find_node(pattern);
    return node && node->get_value() != "";
}

bool Automaton::has_prefix(const StringVector& prefix) const {
    NodePtr node = find_node(prefix);
    return !node;
}

std::string Automaton::get_value(const StringVector& pattern) const {
    NodePtr node = find_node(pattern);
    return node ? node->get_value() : std::string("");
}

NodePtr Automaton::goto_node(const int node_id, const std::string& elem) {
    NodePtr node = this->nodes[node_id];
    auto iter = node->outs.find(elem);
    if (iter != node->outs.end()) {
        return iter->second;
    } else if (node_id == 0) {
        return this->root;
    }
    return std::shared_ptr<Node>(NULL);
}

void Automaton::update_automaton() {
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
        }
    }
    this->fail_table = fail_table;
    this->uptodate = true;
}

MatchVector Automaton::get_matches(const StringVector& text, const bool exclude_overlaps) {
    MatchVector matches;
    if (!this->uptodate) {
        this->update_automaton();
    }
    int node_id = this->root->node_id;
    for (int idx=0 ; idx<text.size() ; ++idx) {
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
                matches.push_back(Match(idx - resnode->depth, idx+1, resnode->value));
            }
        }
    }
    if (exclude_overlaps) {
        return remove_overlaps(matches);
    }
    return matches;
}

std::string Automaton::str() const {
    return root->str();
}


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
