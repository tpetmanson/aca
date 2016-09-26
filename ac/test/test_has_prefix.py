import pytest
from ac import PyAutomaton

def test_has_pattern():
    automaton = PyAutomaton()
    automaton.add('himalaya')

    print (automaton)

    assert automaton.has_prefix('him')
    assert automaton.has_prefix('himalaya')
    assert not automaton.has_prefix('himalayas')
