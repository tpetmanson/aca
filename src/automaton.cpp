/**
 * Aho-Corasick keyword tree + automaton implementation
 */
#include <string>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <set>
#include <functional>
#include <memory>

typedef std::vector<std::string> StringVector;

// forward declare Node
class Node;
typedef std::shared_ptr<Node> NodePtr;

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
// NODE
////////////////////////////////////////////////////////////////////////////////////////////////////////////


class Node {
private:
    int node_id, depth;
    std::string value;
    std::unordered_map<std::string, NodePtr> outs;
    std::vector<NodePtr> matches;
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
    std::vector<NodePtr> nodes;
    std::vector<int> fail_table;
    bool uptodate;
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

std::string Automaton::str() const {
    return root->str();
}

#include <iostream>

int main() {
    Automaton automaton;
    StringVector vec = {"token1", "token2", "token3"};
    StringVector vec2 = {"token1", "token4"};
    std::string val = "PER";

    automaton.add(vec, val);
    automaton.add(vec2, val);
    std::cout << automaton.str();
    std::cout << automaton.get_value(vec) << "\n";
    return 0;
}