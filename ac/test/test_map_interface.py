from ac import Automaton, Match


def test_map_interface():
    auto = Automaton()
    auto['us'] = 'USA'
    auto['her'] ='EUROPE'
    assert auto['us'] == 'USA'
    assert auto['her'] == 'EUROPE'
    matches = auto.get_matches('usher')
    assert len(matches) == 2
    assert matches[0].label == 'USA'
    assert matches[1].label == 'EUROPE'
