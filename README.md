# Aho–Corasick automaton + keyword tree implementation for Python

By Timo Petmanson @ Funderbeam ( https://funderbeam.com )

This package is a C++ implementation of the Aho-Corasick automaton and wrapped in Python with the following features:

* dictionary matching with linear O(n) complexity 
* efficient String -> String dictionary
* serialization
* functionality for removing overlaps while maximizing the number of matched tokens

Please refer to examples below for more details.


## The data structure

In computer science, the Aho–Corasick algorithm is a string searching algorithm invented by Alfred V. Aho and Margaret J. Corasick.
It is a kind of dictionary-matching algorithm that locates elements of a finite set of strings (the "dictionary") within an input text.
It matches all strings simultaneously.
The complexity of the algorithm is linear in the length of the strings plus the length of the searched text plus the number of output matches.
Note that because all matches are found, there can be a quadratic number of matches if every substring matches (e.g. dictionary = a, aa, aaa, aaaa and input string is aaaa).

Informally, the algorithm constructs a finite state machine that resembles a trie with additional links between the various internal nodes.
These extra internal links allow fast transitions between failed string matches (e.g. a search for cat in a trie that does not contain cat, but contains cart, and thus would fail at the node prefixed by ca), to other branches of the trie that share a common prefix (e.g., in the previous case, a branch for attribute might be the best lateral transition).
This allows the automaton to transition between string matches without the need for backtracking.

When the string dictionary is known in advance (e.g. a computer virus database), the construction of the automaton can be performed once off-line and the compiled automaton stored for later use.
In this case, its run time is linear in the length of the input plus the number of matched entries.

See https://en.wikipedia.org/wiki/Aho%E2%80%93Corasick_algorithm for more information.

See https://github.com/tpetmanson/aca/blob/master/docs/slides04.pdf to learn more about how AC automatons work.


## Example usage

### Example 1: basic use case

Create a dictionary of medicines and find where they match in a text.

```python
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

```

Output:

```
0 11 paracetamol
16 28 hydrocloride
74 85 paracetamol
109 118 ibuprofen
```

### Example 2: use case with tokenized keys and labels

```python
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
```

Output:

```
matches that maximize the number of matched words
0 3 ['Tom', 'Anderson', 'Jr'] designer
4 6 ['Yuri', 'Artyukhin'] developer
```

Note that your dictionary contains both Tom Anderson and Tom Anderson Jr.
By default, the matcher removes the matches that overlap, but this feature
can be disabled.

```python
print ('all matches')
for match in automaton.get_matches(text, exclude_overlaps=False):
    print (match.start, match.end, match.elems, match.label)
```

Output:

```
0 2 ['Tom', 'Anderson'] manager
0 3 ['Tom', 'Anderson', 'Jr'] designer
4 6 ['Yuri', 'Artyukhin'] developer
```

### Example 3: dictionary use case

You can use the automaton as a space-efficient dictionary.
However, there are some implementation specific constraints:
* keys can be only strings or string lists
* values must be non-empty strings (with length greater than 0)
* deleting keys won't free up memory, to do that you need to rebuild the Automaton
* items() will always yield a list of strings

```python
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
```

Output:

```
noun
```

---

```python
# Trying to access an non-existent key will raise KeyError
print (map['invalid key'])
```

Output:

```
KeyError: 'invalid key'
```

---

```python
# you can use get to provide a default value when key is missing
print (map.get('invalid key', 'default value'))
```

Output:
```
default value
```

---

```python
# NB! Implementation specific special case: empty strings
# denote "missing" values, so you can't use these
map['special'] = ''
print (map['special'])
```

Output:

```
KeyError: 'special'
```

---

```python
# you can delete items
del map['electrify']

# trying to delete a non-existent item raises KeyError
del map['invalid key']
```

Output:
```
KeyError: 'invalid key'
```

---

```python
# NB! Implementation specific special case: empty strings
# denote "missing" values, so you can't use these
map['special'] = ''
print (map['special'])
```

Output:

```
KeyError: 'special'
```

---

```python
# iterate items like a dict
# NB! Due to implementation specifics, this will always yield list of strings.
print ('items:')
for key, value in map.items():
    print ('{}: {}'.format(key, value))
```

Output:
```
items:
['a', 'c', 'i', 'd']: noun
['a', 'c', 'i', 'd', 'i', 'c']: adjective
['e', 'l', 'e', 'g', 'a', 'n', 't']: adjective
```

---

```python
# you can also iterate prefixes
print ('prefixes:')
for prefix, value in map.prefixes():
    print ('{}: {}'.format(prefix, value))
```

Output:

```
[]:
['a']:
['a', 'c']:
['a', 'c', 'i']:
['a', 'c', 'i', 'd']: noun
['a', 'c', 'i', 'd', 'i']:
['a', 'c', 'i', 'd', 'i', 'c']: adjective
['e']:
['e', 'l']:
['e', 'l', 'e']:
['e', 'l', 'e', 'c']:
['e', 'l', 'e', 'c', 't']:
['e', 'l', 'e', 'c', 't', 'r']:
['e', 'l', 'e', 'c', 't', 'r', 'i']:
['e', 'l', 'e', 'c', 't', 'r', 'i', 'f']:
['e', 'l', 'e', 'c', 't', 'r', 'i', 'f', 'y']:
['e', 'l', 'e', 'g']:
['e', 'l', 'e', 'g', 'a']:
['e', 'l', 'e', 'g', 'a', 'n']:
['e', 'l', 'e', 'g', 'a', 'n', 't']: adjective
['s']:
['s', 'p']:
['s', 'p', 'e']:
['s', 'p', 'e', 'c']:
['s', 'p', 'e', 'c', 'i']:
['s', 'p', 'e', 'c', 'i', 'a']:
['s', 'p', 'e', 'c', 'i', 'a', 'l']:
```

### Example 4: saving and loading

```python

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
```

Output:

```
Tallinn
Berlin
```

## Install

```
pip install wheel
pip install cython
pip install aca
```

### Development

For write / test cycles, use the following command to build the code in the project folder.
```
python setup.py build_ext --inplace
```

### Distributing the library

```
python setup.py build
python setup.py sdist bdist_wheel upload
```

### Debugging

Define ```ACA_DEBUG``` macro in ```aca.h``` header and recompile to see more debugging output.

### License

GPLv3