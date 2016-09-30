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
#ifndef AC__MATCH_H
#define AC__MATCH_H

#include "aca.h"

BEGIN_NAMESPACE(aca)

class CppMatch {
private:
    int start, end;
    std::string label;
public:
    CppMatch() : start(0), end(0), label("") { };
    CppMatch(const int start, const int end, const std::string& label);
    CppMatch(const int start, const int end, const char* label);

    void set_start(const int start);
    void set_end(const int end);
    void set_label(const std::string& label);
    int get_start() const { return start; }
    int get_end() const { return end; }
    std::string get_label() const { return label; }

    bool is_before(const CppMatch& m) const;
    CppMatch* operator=(const CppMatch& m);
    bool operator==(const CppMatch& m) const;
    bool operator<(const CppMatch& m) const;
    size_t size() const;
    std::string str() const;
};


MatchVector cpp_remove_overlaps(MatchVector matches);


END_NAMESPACE

#endif