# create a new AC automaton
from ac import Automaton
automaton = Automaton()

# add a dictionary of words to the automaton
painkillers = ['paracetamol', 'ibuprofen', 'hydrocloride']
automaton.add_all(painkillers)

# serialize to disk
automaton.serialize_to('myautomaton.bin')

# load from disk
automaton.load_from_file()