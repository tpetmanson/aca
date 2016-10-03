# -*- coding: utf-8 -*-
from __future__ import unicode_literals, print_function, absolute_import
from aca.aca_cpp import Match, remove_overlaps


def matches(positions):
    return [Match(start, end) for start, end in positions]

def test_empty_list():
    assert [] == remove_overlaps([])


def test_single_span():
    assert [Match(1, 2)] == remove_overlaps([Match(1, 2)])


def test_nonoverlapping_spans():
    test_input = matches([(1, 2), (2, 3), (4, 5)])
    expected = test_input.copy()
    assert expected == remove_overlaps(test_input)

def test_two_spans_overlap():
    test_input = matches([(1, 2), (1, 3)])
    expected = matches([(1, 3)])
    assert expected == remove_overlaps(test_input)


def test_three_spans_overlap():
    test_input = matches([(1, 3), (2, 5), (4, 5)])
    expected = matches([(1, 3), (4, 5)])
    assert expected == remove_overlaps(test_input)


def test_two_overlapping_clusters():
    test_input = matches([(1, 3), (1, 2), (5, 7), (5, 6)])
    expected = matches([(1, 3), (5, 7)])
    assert expected == remove_overlaps(test_input)


def test_one_large_segment():
    test_input = matches([(1, 2), (2, 3), (4, 5), (4, 8), (5, 6), (6, 9), (7, 9), (2, 7), (1, 10)])
    expected = matches([(1, 10)])
    assert expected == remove_overlaps(test_input)
