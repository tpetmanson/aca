from ac import Automaton

names = [('janek', 'nice'), ('jaan', 'nice'), ('jaagup', 'ugly'), ('jaanus', 'nice'), ('janis', 'nice')]

def test_items():
    auto = Automaton()
    auto.add_all(names)
    ens, evs = zip(*sorted(names))
    ns, vs = zip(*list(auto.items()))
    ns = [''.join(n) for n in ns]
    assert list(ens) == list(ns)
    assert list(evs) == list(vs)


test_items()
