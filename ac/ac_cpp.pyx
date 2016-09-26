# distutils: language = c++
# distutils: sources = ac/match.cpp ac/node.cpp ac/automaton.cpp
from libcpp cimport bool
from libcpp.string cimport string
from libcpp.vector cimport vector
import unicodedata

cdef extern from "all.h" namespace "ac":
    cdef cppclass Match:
        Match(int, int, string) except +
        Match(int, int, char*) except +
        int get_start()
        int get_end()
        string get_label()
        int is_before(const Match&)
        size_t size()

    cdef cppclass Automaton:
        Automaton() except +
        void add(vector[string]&, string)
        bool has_pattern(vector[string]&)
        bool has_prefix(vector[string]&)
        string get_value(vector[string]&, string)
        vector[Match] get_matches(vector[string]&, bool)
        string str()


def normalize_unicode(text):
    return unicodedata.normalize('NFC', text)

def encode(txt):
    if isinstance(txt, str):
        return txt.encode('utf-8')
    return txt

def decode(txt):
    if isinstance(txt, bytes):
        return txt.decode('utf-8')
    return txt

def encode_list(lst):
    return [encode]

cdef class PyAutomaton:
    cdef Automaton* cpp_automaton

    def __cinit__(self):
        self.cpp_automaton = new Automaton()

    def __dealloc__(self):
        del self.cpp_automaton

    def add(self, pattern, value):
        self.cpp_automaton.add(pattern, value)

    def has_pattern(self, pattern):
        return self.cpp_automaton.has_pattern(pattern)

    def has_prefix(self, prefix):
        return self.cpp_automaton.has_prefix(prefix)

    def get_matches(self, text, exclude_overlaps=True):
        matches = self.cpp_automaton.get_matches(text, exclude_overlaps)
        result = [None]*matches.size()
        for i in range(matches.size()):
            result[i] = dict(start=matches[i].get_start(), end=matches[i].get_end(), label=matches[i].get_label())
        return result

    def __str__(self):
        return decode(self.cpp_automaton.str())
