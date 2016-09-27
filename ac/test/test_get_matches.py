from ac import PyAutomaton


def match(start, end, label=True):
    return Match(start, end, label)


def test_automaton_with_words():
    auto = Automaton()
    for token in ['he', 'she', 'his', 'hers']:
        auto.add(token)

    expected_all_matches = [match(1, 4), match(2, 4), match(2, 6)]
    all_matches = list(sorted(auto.iter_matches('ushers')))
    assert expected_all_matches == all_matches

    expected_nonoverlap_matches = [match(2, 6)]
    nonoverlap_matches = auto.get_nonoverlapping_matches('ushers')
    assert expected_nonoverlap_matches == nonoverlap_matches