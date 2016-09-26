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
#include <iostream>
#include <sstream>
#include <functional>
#include <algorithm>
#include <deque>
#include "match.h"

BEGIN_NAMESPACE(ac)

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

END_NAMESPACE