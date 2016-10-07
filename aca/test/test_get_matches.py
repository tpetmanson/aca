# -*- coding: utf-8 -*-
from __future__ import unicode_literals, print_function, absolute_import

from aca import Automaton, Match


def test_automaton_with_words():
    auto = Automaton()
    for token in ['he', 'she', 'his', 'hers']:
        auto.add(token)

    expected_all_matches = [Match(1, 4, 'Y'), Match(2, 4, 'Y'), Match(2, 6, 'Y')]
    all_matches = auto.get_matches('ushers', exclude_overlaps=False)
    print (all_matches)
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


def test_with_words():
    auto = Automaton()
    auto.add(['funderbeam'])
    auto.add(['mattermark'])
    auto.add(['500', 'startups'])

    txt = 'funderbeam and mattermark along with 500 startups'.split()
    expected = [Match(0, 1, 'Y'), Match(2, 3, 'Y'), Match(5, 7, 'Y')]
    actual = auto.get_matches(txt)
    assert expected == actual


def test_lemmas():
    auto = Automaton()
    auto.add(['sunlabob'], 'CO')
    auto.add(['renewable'], 'CO')
    lemmas = ['sunlabob', 'renewable', 'energy', 'receive', '$', '2.1', 'million', 'investment']
    print (auto.str())
    matches = auto.get_matches(lemmas)
    assert len(matches) == 2


test_lemmas()
