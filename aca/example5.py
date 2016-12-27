# Import the library and initiate the automaton
from aca import Automaton
automaton = Automaton()

# add the entities and build the automaton
automaton.add_all(['Funderbeam', 'Funderbeam Data', 'Funderbeam Markets'])
automaton.update_automaton()

# find matches
text = 'Funderbeam Data and Funderbeam Markets are two different products of Funderbeam'
for match in automaton.get_matches(text, exclude_overlaps=False):
    print (match.start, match.end, match.elems)

for match in automaton.get_matches(text, exclude_overlaps=True):
    print (match.start, match.end, match.elems)