# -*- coding: utf-8 -*-
from __future__ import unicode_literals, print_function, absolute_import

"""
Example/test script that detects names from a longer text
"""
from aca import Automaton, Match

NAMES = '''
Viktor Mikhaylovich Afanasyev
Vladimir Aksyonov
Aleksandr Pavlovich Aleksandrov
Ivan Anikeyev s
Anatoly Artsebarsky
Yuri Artyukhin 
Oleg Atkov
Toktar Aubakirov
Sergei Avdeyev
Nikolai Budarin
Vladimir Dezhurov
Aleksandr Balandin
Yuri Baturin
Pavel Belyayev 
Georgi Beregovoi 
Anatoly Berezovoy 
Valentin Bondarenko 
Andrei Borisenko
Nikolai Budarin
Valery Bykovsky
Vladimir N. Dezhurov
Georgiy Dobrovolskiy 
Lev Dyomin 
Vladimir Dzhanibekov
Konstantin Feoktistov 
Valentin Filatyev 
Anatoly Filipchenko
Yuri Gagarin 
Yuri Gidzenko
Yuri Glazkov 
Viktor Gorbatko
Georgi Grechko
Aleksei Gubarev
Aleksandr Kaleri
Sergei Krikalev
Aleksandr Ivanchenkov
Anatoli Ivanishin
Aleksandr Kaleri
Yevgeny Khrunov 
Leonid Kizim 
Pyotr Klimuk
Vladimir Komarov 
Yelena V. KondakovaSymbol venus.svg
Dmitri Kondratyev
Oleg Kononenko
Mikhail Korniyenko
Valery Korzun
Oleg Kotov
Vladimir Kovalyonok
Konstantin Kozeyev
Sergei Krikalev
Valeri Kubasov
Aleksei Leonov
Aleksandr Laveykin
Vasili Lazarev 
Aleksandr Lazutkin
Valentin Lebedev
Aleksei Leonov
Anatoli Levchenko 
Yuri Lonchakov
Vladimir Lyakhov
Yuri Lonchakov
Oleg Makarov 
Yuri Malenchenko
Yury Malyshev 
Gennadi Manakov
Musa Manarov
Alexander Misurkin
Boris Morukov 
Talgat Musabayev
Gennady Padalka
Grigori Nelyubov 
Andriyan Nikolayev 
Oleg Novitski
Yuri Onufrienko
Aleksey Ovchinin
Gennady Padalka
Viktor Patsayev 
Aleksandr Poleshchuk
Valeri Polyakov
Leonid Popov
Pavel Popovich 
Sergei Revin
Roman Romanenko
Yuri Romanenko
Valery Rozhdestvensky
Nikolai Rukavishnikov 
Sergei Ryazanski
Valery Ryumin
Leonid Popov
Svetlana Savitskaya
Aleksandr Serebrov
Aleksandr Samokutyayev
Gennadi Sarafanov 
Viktor Savinykh
Svetlana SavitskayaSymbol venus.svg
Aleksandr Serebrov 
Yelena SerovaSymbol venus.svg
Vitali Sevastyanov 
Yuri Shargin
Salizhan Sharipov
Vladimir Shatalov
Anton Shkaplerov
Georgi Shonin 
Oleg Skripochka
Aleksandr Skvortsov
Anatoly Solovyev
Vladimir Solovyov
Gennadi Strekalov 
Maksim Surayev
Valentina Tereshkova
Yevgeni Tarelkin
Valentina TereshkovaSymbol venus.svg, First woman in space.
Gherman Titov 
Vladimir Titov
Valeri Tokarev
Sergei Treshchov
Vasili Tsibliyev
Mikhail Tyurin
Yuri Usachov
Vladimir Vasyutin 
Aleksandr Viktorenko
Pavel Vinogradov
Igor Volk
Alexander Volkov
Sergei Aleksandrovich Volkov
Vladislav Volkov 
Boris Volynov
Fyodor Yurchikhin
Sergei Vozovikov 
Boris Yegorov 
Aleksei Yeliseyev
Fyodor Yurchikhin
Dmitri Zaikin 
Sergei Zalyotin
Vitali Zholobov
Vyacheslav Zudov
Marcos Pontes
Guy Laliberté
Chris Hadfield
Robert Thirsk
Joseph M. Acaba
Michael R. Barratt
Ken Bowersox
Daniel C. Burbank
Leroy Chiao
Timothy Creamer
Michael Fincke
Michael Foale
Michael E. Fossum
Ronald J. Garan, Jr.
Richard Garriott
Michael S. Hopkins
Scott Kelly
Kjell N. Lindgren
Michael Lopez-Alegria
Ed Lu
Thomas Marshburn
Richard Mastracchio
William S. McArthur
Karen Nyberg
Gregory Olsen
Donald Pettit
John L. Phillips
William Shepherd
Charles Simonyi
Norman Thagard
Dennis Tito
Terry W. Virts
Shannon WalkerSymbol
Douglas H. Wheelock
Peggy WhitsonSymbol
Jeffrey Williams
Sunita WilliamsSymbol
Barry E. Wilmore
Gregory R. Wiseman
Toyohiro Akiyama
Akihiko Hoshide
Soichi Noguchi
Satoshi Furukawa
Koichi Wakata
Kimiya Yui
'''

TEXT = ' who was a very good friend of '.join(NAMES.strip().splitlines())
NAMES = [name.split() for name in NAMES.strip().splitlines()]


def test_names():
    auto = Automaton(NAMES)
    auto.add_all(NAMES)
    print (auto)

    matches = set(' '.join(match.elems) for match in auto.get_matches(TEXT.split()))
    names = set(' '.join(name) for name in NAMES)

    assert names == matches

