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
#include "match.h"

#include <iostream>
#include <sstream>
#include <functional>
#include <algorithm>


BEGIN_NAMESPACE(aca)

CppMatch::CppMatch(const int start, const int end, const std::string& label) : start(start), end(end), label(label) { }
CppMatch::CppMatch(const int start, const int end, const char* label) : start(start), end(end), label(label) { }

bool CppMatch::operator==(const CppMatch& m) const {
    return start == m.start && end == m.end;
}

CppMatch* CppMatch::operator=(const CppMatch& m) {
    this->start = m.get_start();
    this->end = m.get_end();
    this->label = m.get_label();
    return this;
}

bool CppMatch::is_before(const CppMatch& m) const {
    return end <= m.start;
}

bool CppMatch::operator<(const CppMatch& m) const {
    if (start == m.start) {
        return end < m.end;
    }
    return start < m.start;
}

void CppMatch::set_start(const int start) {
    this->start = start;
}

void CppMatch::set_end(const int end) {
    this->end = end;
}

void CppMatch::set_label(const std::string& label) {
    this->label = label;
}

size_t CppMatch::size() const {
    return end - start;
}

std::string CppMatch::str() const {
    std::stringstream ss;
    ss << "CppMatch(" << start << ", " << end << ", " << label << ")";
    return ss.str();
}

MatchVector cpp_remove_overlaps(MatchVector matches) {
    if (matches.size() == 0) {
        return matches;
    }
    #ifdef ACA_DEBUG
        std::cout << "matches before removing overlaps:\n";
        for (int i=0 ; i<matches.size() ; ++i) {
            std::cout << matches[i].str() << " ";
        }
    #endif
    // compute the lengths
    IntVector lengths(matches.size(), 0);
    std::transform(matches.begin(), matches.end(), lengths.begin(), [](const CppMatch& m) {
        return m.size();
    });
    IntVector scores = lengths;
    IntVector prev(scores.size(), -1);
    int highscore = scores[0];
    int highpos = 0;
    for (size_t i=1 ; i<matches.size() ; ++i) {
        int bestscore = scores[i];
        int bestprev = -1;
        int j = static_cast<int>(i);
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
    #ifdef ACA_DEBUG
        std::cout << "matches:";
        for (int i=0 ; i<matches.size() ; ++i) {
            std::cout << " [" << i << "]=" << matches[i].str();
        }
        std::cout << '\n';

        std::cout << "scores:";
        for (int i=0 ; i<scores.size() ; ++i) {
            std::cout << " [" << i << "]=" << scores[i];
        }
        std::cout << '\n';

        std::cout << "prev:";
        for (int i=0 ; i<prev.size() ; ++i) {
            std::cout << " [" << i << "]=" << prev[i];
        }
        std::cout << '\n';

        std::cout << "highscore: " << highscore << "\n";
        std::cout << "highpos: " << highpos << "\n";
    #endif
    // back-track non-overlappng spans that we should keep
    IntVector keep;
    while (highpos != -1) {
        keep.push_back(highpos);
        highpos = prev[highpos];
    }
    #ifdef ACA_DEBUG
        std::cout << "keep:";
        for (int i=0 ; i<keep.size() ; ++i) {
            std::cout << " [" << i << "]=" << keep[i];
        }
    #endif
    // create new output array
    MatchVector result;
    result.reserve(keep.size());
    for (auto iter=keep.rbegin() ; iter != keep.rend() ; ++iter) {
        result.push_back(matches[*iter]);
    }
    return result;
}

END_NAMESPACE
