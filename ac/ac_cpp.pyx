# distutils: language = c++
# distutils: sources = ac/match.cpp ac/node.cpp ac/automaton.cpp
from libcpp cimport bool
from libcpp.string cimport string
from libcpp.vector cimport vector
import unicodedata

cdef extern from "all.h" namespace "ac":
    cdef cppclass CppMatch:
        CppMatch() except +
        CppMatch(int, int, string) except +
        CppMatch(int, int, char*) except +
        int get_start()
        int get_end()
        string get_label()
        int is_before(const CppMatch&)
        size_t size()

    cdef cppclass CppAutomaton:
        Automaton() except +
        void add(vector[string]&, string)
        bool has_pattern(vector[string]&)
        bool has_prefix(vector[string]&)
        string get_value(vector[string]&, string)
        vector[CppMatch] get_matches(vector[string]&, bool)
        string str()


def normalize_unicode(text):
    return unicodedata.normalize('NFC', text)

def encode(txt):
    if isinstance(txt, str):
        return normalize_unicode(txt).encode('utf-8')
    return txt

def decode(txt):
    if isinstance(txt, bytes):
        return txt.decode('utf-8')
    return txt

def encode_list(lst):
    return [encode(e) for e in lst]


class Match:

    def __init__(self, start, end, label):
        self.__start = int(start)
        self.__end = int(end)
        assert self.__start < self.__end
        self.__label = str(label)

    def __eq__(self, other):
        return self.start == other.start and self.end == other.end and self.label == other.label

    def __str__(self):
        return 'Match({},{},{})'.format(self.start, self.end, self.label)

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


cdef class Automaton:
    cdef CppAutomaton* cpp_automaton

    def __cinit__(self):
        self.cpp_automaton = new CppAutomaton()

    def __dealloc__(self):
        del self.cpp_automaton

    def add(self, pattern, value='Y'):
        self.cpp_automaton.add(encode_list(pattern), encode(value))

    def has_pattern(self, pattern):
        return self.cpp_automaton.has_pattern(encode_list(pattern))

    def has_prefix(self, prefix):
        return self.cpp_automaton.has_prefix(encode_list(prefix))

    def get_matches(self, text, exclude_overlaps=True):
        matches = self.cpp_automaton.get_matches(encode_list(text), exclude_overlaps)
        result = [None]*matches.size()
        for i in range(matches.size()):
            result[i] = Match(matches[i].get_start(), matches[i].get_end(), decode(matches[i].get_label()))
        return result

    def __str__(self):
        return decode(self.cpp_automaton.str())
