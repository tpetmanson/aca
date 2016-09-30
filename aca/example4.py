# -*- coding: utf-8 -*-
from __future__ import unicode_literals, print_function, absolute_import

# create a new AC automaton
from aca import Automaton

automaton = Automaton()
automaton['Estonia'] = 'Tallinn'
automaton['Germany'] = 'Berlin'
automaton['Finland'] = 'Helsinki'

# serialize to disk
automaton.save_to_file('myautomaton.bin')

# load from disk
automaton2 = Automaton()
automaton2.load_from_file('myautomaton.bin')

# save / load to binary string
automaton3 = Automaton()
automaton3.load_from_string(automaton.save_to_string())

print (automaton2['Estonia'])
print (automaton3['Germany'])
