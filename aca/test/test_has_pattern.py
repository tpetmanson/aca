# -*- coding: utf-8 -*-
from __future__ import unicode_literals, print_function, absolute_import

import pytest
from aca import Automaton

def test_has_pattern():
    automaton = Automaton()
    automaton.add('he')
    automaton.add('she')
    automaton.add('us')

    assert automaton.has_pattern('he')
    assert automaton.has_pattern('she')
    assert automaton.has_pattern('us')
    assert not automaton.has_pattern('they')
    assert not automaton.has_pattern('e')
    assert not automaton.has_pattern('use')
