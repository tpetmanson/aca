# distutils: language = c++
# distutils: sources = aca/match.cpp aca/node.cpp aca/automaton.cpp
# -*- coding: utf-8 -*-
#from __future__ import unicode_literals, print_function, absolute_import

import six
from libcpp cimport bool
from libcpp.string cimport string
from libcpp.vector cimport vector
from libcpp.utility cimport pair
import unicodedata

cdef extern from "all.h" namespace "aca":
    cdef cppclass CppMatch:
        CppMatch() except +
        CppMatch(int, int, string) except +
        CppMatch(int, int, char*) except +
        void set_start(int)
        void set_end(int)
        void set_label(string)
        int get_start()
        int get_end()
        string get_label()
        int is_before(const CppMatch&)
        size_t size()

    cdef vector[CppMatch] cpp_remove_overlaps(vector[CppMatch]);

    cdef cppclass CppAutomaton:
        Automaton() except +
        void add(vector[string]&, string)
        void update_automaton()
        bool has_pattern(vector[string]&)
        bool has_prefix(vector[string]&)
        string get_value(vector[string]&)
        vector[CppMatch] get_matches(vector[string]&, bool)
        vector[pair[vector[string], string]] get_patterns_values()
        vector[pair[vector[string], string]] get_prefixes_values()

        # serialization related
        string serialize()
        void serialize_to (string) except +

        @staticmethod
        CppAutomaton* deserialize(string)

        @staticmethod
        CppAutomaton* deserialize_from(string) except +

        string str()


def normalize_unicode(text):
    return unicodedata.normalize('NFC', text)

def encode(txt):
    if isinstance(txt, six.string_types):
        return normalize_unicode(txt).encode('utf-8')
    return txt

def decode(txt):
    if isinstance(txt, six.binary_type):
        return txt.decode('utf-8')
    return txt

def encode_list(lst):
    return [encode(e) for e in lst]

def decode_list(lst):
    return [decode(e) for e in lst]


class Match:

    def __init__(self, start, end, label='Y'):
        self.__start = int(start)
        self.__end = int(end)
        assert self.__start < self.__end
        self.__label = str(label)
        self.__elems = None

    def __eq__(self, other):
        return self.start == other.start and self.end == other.end and self.label == other.label

    def __str__(self):
        return 'Match({},{},{},{})'.format(self.start, self.end, self.elems, self.label)

    def __repr__(self):
        return str(self)

    @property
    def start(self):
        return self.__start

    @property
    def end(self):
        return self.__end

    @property
    def label(self):
        return self.__label

    @property
    def elems(self):
        return self.__elems

    def set_elems(self, elems):
        self.__elems = elems


cdef vector[CppMatch] matches_to_cppmatches(matches):
    cdef vector[CppMatch] vec
    cdef CppMatch cppmatch;
    vec.reserve(len(matches))
    for match in matches:
        cppmatch.set_start(match.start)
        cppmatch.set_end(match.end)
        cppmatch.set_label(encode(match.label))
        vec.push_back(cppmatch)
    return vec

cdef cppmatches_to_matches(vector[CppMatch] cppmatches):
    result = [None]*cppmatches.size()
    for i in range(cppmatches.size()):
        result[i] = Match(cppmatches[i].get_start(), cppmatches[i].get_end(), decode(cppmatches[i].get_label()))
    return result

def remove_overlaps(matches):
    cdef vector[CppMatch] cppmatches;
    cdef vector[CppMatch] cppresult;
    cppmatches = matches_to_cppmatches(matches)
    cppresult = cpp_remove_overlaps(cppmatches)
    return cppmatches_to_matches(cppresult)


cdef class Automaton:
    """ Aho-Corasick keyword tree + automaton. """
    cdef CppAutomaton* cpp_automaton

    def __cinit__(self):
        self.cpp_automaton = new CppAutomaton()

    def __dealloc__(self):
        del self.cpp_automaton

    def load_from_file(self, fnm):
        cdef CppAutomaton* new_cpp_automaton = self.cpp_automaton.deserialize_from(encode(fnm))
        del self.cpp_automaton
        self.cpp_automaton = new_cpp_automaton

    def load_from_string(self, binstring):
        cdef CppAutomaton* new_cpp_automaton = self.cpp_automaton.deserialize(binstring)
        del self.cpp_automaton
        self.cpp_automaton = new_cpp_automaton

    def add(self, pattern, value='Y'):
        self.cpp_automaton.add(encode_list(pattern), encode(value))

    def add_all(self, patterns):
        for pattern in patterns:
            if isinstance(pattern, tuple):
                pattern, value = pattern
                self.add(pattern, value)
            else:
                self.add(pattern)

    def update_automaton(self):
        self.cpp_automaton.update_automaton()

    def has_pattern(self, pattern):
        return self.cpp_automaton.has_pattern(encode_list(pattern))

    def has_prefix(self, prefix):
        return self.cpp_automaton.has_prefix(encode_list(prefix))

    def get_matches(self, text, exclude_overlaps=True):
        matches = self.cpp_automaton.get_matches(encode_list(text), exclude_overlaps)
        results = cppmatches_to_matches(matches)
        for match in results:
            match.set_elems(text[match.start:match.end])
        return results

    def items(self):
        cdef vector[pair[vector[string], string]] vec = self.cpp_automaton.get_patterns_values()
        for idx in range(vec.size()):
            yield decode_list(vec[idx].first), decode(vec[idx].second)

    def prefixes(self):
        cdef vector[pair[vector[string], string]] vec = self.cpp_automaton.get_prefixes_values()
        for idx in range(vec.size()):
            yield decode_list(vec[idx].first), decode(vec[idx].second)

    '''def keys(self):
        for key, value in self.items():
            yield tuple(key)

    def values(self):
        for key, value in self.items():
            yield value

    def __iter__(self):
        return self.keys()'''

    def save_to_file(self, fnm):
        self.cpp_automaton.serialize_to(encode(fnm))

    def save_to_string(self):
        return self.cpp_automaton.serialize()

    def __getitem__(self, pattern):
        value = decode(self.cpp_automaton.get_value(encode_list(pattern)))
        if len(value) == 0:
            raise KeyError(pattern)
        return value

    def get(self, pattern, default=None):
        try:
            return self[pattern]
        except KeyError as exc:
            return default

    def __setitem__(self, pattern, value):
        self.add(pattern, value)

    def set(self, pattern, value=''):
        self.add(pattern, value)

    def __delitem__(self, pattern):
        value = self[pattern] # if value does not exist, raise an exception
        self.add(pattern, '')

    def __contains__(self, pattern):
        value = self.get(pattern, None)
        return value is not None

    def str(self):
        return decode(self.cpp_automaton.str())

