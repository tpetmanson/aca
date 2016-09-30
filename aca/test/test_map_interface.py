# -*- coding: utf-8 -*-
from __future__ import unicode_literals, print_function, absolute_import

from aca import Automaton, Match


def test_map_interface():
    auto = Automaton()
    auto['us'] = 'USA'
    auto['her'] ='EUROPE'
    assert auto['us'] == 'USA'
    assert auto['her'] == 'EUROPE'
    matches = auto.get_matches('usher')
    assert len(matches) == 2
    assert matches[0].label == 'USA'
    assert matches[1].label == 'EUROPE'

    assert 'us' in auto
    assert 'his' not in auto
    #assert list(auto.keys()) == ['her', 'us']
    #assert list(auto.values()) == ['EUROPE', 'USA']
    #assert list(k for k in auto) == list(auto.keys())
