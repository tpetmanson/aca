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
#ifndef AC__AC_H
#define AC__AC_H

#define AC_DEBUG    1
#define BEGIN_NAMESPACE(x)  namespace x {
#define END_NAMESPACE }

#include <string>
#include <vector>
#include <unordered_map>


BEGIN_NAMESPACE(ac)

// forward declare our classes
class Node;
class Match;
class Automaton;

// create some useful type definitions
typedef std::shared_ptr<Node> NodePtr;
typedef std::vector<Match> MatchVector;
typedef std::vector<std::string> StringVector;
typedef std::vector<int> IntVector;
typedef std::vector<NodePtr> NodeVector;

END_NAMESPACE


#endif