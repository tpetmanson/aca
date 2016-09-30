# -*- coding: utf-8 -*-
from __future__ import unicode_literals, print_function, absolute_import

import pytest
from aca import Automaton

def test_has_pattern():
    automaton = Automaton()
    automaton.add('himalaya')

    print (automaton)

    assert automaton.has_prefix('him')
    assert automaton.has_prefix('himalaya')
    assert not automaton.has_prefix('himalayas')
