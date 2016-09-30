# -*- coding: utf-8 -*-
from __future__ import unicode_literals, print_function, absolute_import

from aca import Automaton
import sys

names = [('janek', 'nice'), ('jaan', 'nice'), ('jaagup', 'ugly'), ('jaanus', 'nice'), ('janis', 'nice')]


def test_items():
    auto = Automaton()
    auto.add_all(names)
    ens, evs = zip(*sorted(names))
    ns, vs = zip(*list(auto.items()))
    ns = [''.join(n) for n in ns]
    assert list(ens) == list(ns)
    assert list(evs) == list(vs)


def test_prefixes():
    auto = Automaton()
    auto.add_all(['jaanus', 'janek', 'janis'])
    prefixes, values = zip(*auto.prefixes())
    prefixes = [''.join(prefix) for prefix in prefixes]
    assert prefixes == ['', 'j', 'ja', 'jaa', 'jaan', 'jaanu', 'jaanus', 'jan', 'jane', 'janek', 'jani', 'janis']

