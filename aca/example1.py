# -*- coding: utf-8 -*-
from __future__ import unicode_literals, print_function, absolute_import

# create a new AC automaton
from aca import Automaton
automaton = Automaton()

# add a dictionary of words to the automaton
painkillers = ['paracetamol', 'ibuprofen', 'hydrocloride']
automaton.add_all(painkillers)

# match the dictionary on a text
text = 'paracetamol and hydrocloride are a medications to relieve pain and fever. paracetamol is less efficient than ibuprofen'

for match in automaton.get_matches(text):
    print (match.start, match.end, match.elems)
