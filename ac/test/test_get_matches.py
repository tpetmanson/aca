# -*- coding: utf-8 -*-
from __future__ import unicode_literals, print_function, absolute_import

from ac import Automaton, Match


def test_automaton_with_words():
    auto = Automaton()
    for token in ['he', 'she', 'his', 'hers']:
        auto.add(token)

    expected_all_matches = [Match(1, 4, 'Y'), Match(2, 4, 'Y'), Match(2, 6, 'Y')]
    all_matches = auto.get_matches('ushers', exclude_overlaps=False)
    assert expected_all_matches == all_matches

    expected_nonoverlap_matches = [Match(2, 6, 'Y')]
    nonoverlap_matches = auto.get_matches('ushers', exclude_overlaps=True)
    assert expected_nonoverlap_matches == nonoverlap_matches


def test_with_updating():
    auto = Automaton()
    auto.add('hers')
    matches = auto.get_matches('ushers')
    assert len(matches) == 1
    auto.add('us')
    matches = auto.get_matches('ushers')
    assert len(matches) == 2
