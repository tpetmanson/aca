# Aho–Corasick automaton + keyword tree implementation for Python

This package is a C++ implementation of the Aho-Corasick automaton and wrapped in Python with the following features:

* dictionary matching with linear O(n) complexity 
* efficient String -> String dictionary
* serialization

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

## Example usage

### Example 1

Create a dictionary of medicines and find where they match in a text.

```python
# create a new AC automaton
from ac import Automaton
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

### Example 2

```python
# create a new AC automaton
from ac import Automaton
automaton = Automaton()

# instead of plain strings, you can also use lists of tokens
names = [
    ['Yuri', 'Artyukhin'],
    ['Tom', 'Anderson'],
    ['Tom', 'Anderson', 'Jr'],
]
automaton.add_all(names)

# if you are not using plain strings, make sure you tokenize the text as well
text = 'Tom Anderson Jr and Yuri Artyukhin work on my project'.split()

print ('matches that maximize the number of matched words')
for match in automaton.get_matches(text):
    print (match.start, match.end, match.elems)
```

Output:

```
matches that maximize the number of matched words
0 3 ['Tom', 'Anderson', 'Jr']
4 6 ['Yuri', 'Artyukhin']
```

Note that your dictionary contains both Tom Anderson and Tom Anderson Jr.
By default, the matcher removes the matches that overlap, but this feature
can be disabled.

```python
print ('all matches')
for match in automaton.get_matches(text, exclude_overlaps=False):
    print (match.start, match.end, match.elems)
```

Output:

```
0 2 ['Tom', 'Anderson']
0 3 ['Tom', 'Anderson', 'Jr']
4 6 ['Yuri', 'Artyukhin']
```



## Install

```
pip install ac
```



### Debugging

Define AC_DEBUG macro in ac.h header and recompile to see more debugging output.

### License

GPLv3