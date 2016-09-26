import pytest
from ac import PyAutomaton

def test_has_pattern():
    automaton = PyAutomaton()
    automaton.add(list('he'))
    automaton.add(list('she'))
    automaton.add(list('us'))

    assert automaton.has_pattern(list('he'))
    assert automaton.has_pattern(list('she'))
    assert automaton.has_pattern(list('us'))
    assert not automaton.has_pattern(list('they'))
    assert not automaton.has_pattern(list('e'))
