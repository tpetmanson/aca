# -*- coding: utf-8 -*-
from __future__ import unicode_literals, print_function, absolute_import

# create a new AC automaton
from aca import Automaton
map = Automaton()

# use the automaton as a map
map['electrify'] = 'verb'
map['elegant'] = 'adjective'
map['acid'] = 'noun'
map['acidic'] = 'adjective'

# access it like a Python dictionary
print (map['acid'])

# using an invalid key raises a KeyError
#print (map['invalid key'])

# you can use get to provide a default value when key is missing
print (map.get('invalid key', 'default value'))

# NB! Implementation specific special case: empty strings
# denote "missing" values, so you can't use these
map['special'] = ''
#print (map['special'])

# you can delete items
del map['electrify']

# trying to delete a non-existent item raises KeyError
#del map['invalid key']

# iterate items like a dict
print ('items:')
for key, value in map.items():
    print ('{}: {}'.format(key, value))


# you can also iterate prefixes
print ('prefixes:')
for prefix, value in map.prefixes():
    print ('{}: {}'.format(prefix, value))
