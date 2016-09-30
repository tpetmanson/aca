# -*- coding: utf-8 -*-
from __future__ import unicode_literals, print_function, absolute_import

# create a new AC automaton
from aca import Automaton
automaton = Automaton()

# instead of plain strings, you can also use lists of tokens
names = [
    (['Yuri', 'Artyukhin'], 'developer'),
    (['Tom', 'Anderson', 'Jr'], 'designer'),
]
automaton.add_all(names)

# you can add an item like this as well
automaton[['Tom', 'Anderson']] = 'manager'


# if you are not using plain strings, make sure you tokenize the text as well
text = 'Tom Anderson Jr and Yuri Artyukhin work on my project'.split()

print ('matches that maximize the number of matched words')
for match in automaton.get_matches(text):
    print (match.start, match.end, match.elems, match.label)


print ('all matches')
for match in automaton.get_matches(text, exclude_overlaps=False):
    print (match.start, match.end, match.elems, match.label)
