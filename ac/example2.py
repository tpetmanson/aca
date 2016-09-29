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


print ('all matches')
for match in automaton.get_matches(text, exclude_overlaps=False):
    print (match.start, match.end, match.elems)
