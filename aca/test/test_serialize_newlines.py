# -*- coding: utf-8 -*-
"""
Example/test script that detects names from a longer text
"""
from __future__ import unicode_literals, print_function, absolute_import
import os
from tempfile import TemporaryDirectory
from aca import Automaton, Match

KEY = 'everything\non\nseparate\nlines'
VAL = 'YES'

def test_names():
    auto = Automaton()
    auto[KEY] = VAL
    auto.update_automaton()

    with TemporaryDirectory() as tmpdir:
        fnm = os.path.join(tmpdir, 'test.aca')
        auto.save_to_file(fnm)
        auto2 = Automaton()
        auto2.load_from_file(fnm)

    assert auto2[KEY] == VAL

