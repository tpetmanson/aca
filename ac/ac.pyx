# distutils: language = c++
# distutils: sources = ac/match.cpp ac/node.cpp ac/automaton.cpp
from libcpp.string cimport string
from libcpp.vector cimport vector
from collections import namedtuple

PyMatch = namedtuple('PyMatch', 'start', 'end', 'label')

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
        int has_pattern(vector[string]&)
        int has_prefix(vector[string]&)
        string get_value(vector[string]&, string)
        vector[Match] get_matches(vector[string]&, bool)


cdef class PyAutomaton:
    cdef Automaton cpp_automaton
    def __cinit__(self):
        self.cpp_automaton = Automaton()

    def add(self, pattern, value):
        self.cpp_automaton.add(pattern, value)

    def has_pattern(self, pattern):
        return bool(self.cpp_automaton.has_pattern(pattern))

    def has_prefix(self, prefix):
        return bool(self.cpp_automaton.has_prefix(prefix))

    def get_matches(self, text, exclude_overlaps=True):
        matches = self.cpp_automaton.get_matches(text, exclude_overlaps)
        result = [None]*matches.size()
        for i in range(matches.size()):
            result[i] = PyMatch(matches[i].get_start(), matches[i].get_end(), matches[i].get_label())
        return result

