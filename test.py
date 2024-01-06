XLEN = 64


def int_from_bin(a: int, word_size=XLEN):
    # twos complement
    if a & (1 << (word_size - 1)):
        a = a - (1 << word_size)
    return a


def int_to_bin(a: int, word_size=XLEN):
    # twos complement
    if a < 0:
        a = (1 << word_size) + a
    return a


def se(value, bits=XLEN):
    sign_bit = 1 << (bits - 1)
    return (value & (sign_bit - 1)) - (value & sign_bit)

def sign_extend(value, curr, bits=XLEN):
    if curr >= bits:
        return value
    sign_bit = 1 << (curr - 1)
    return int_to_bin((value & (sign_bit - 1)) - (value & sign_bit))

print(bin(int_to_bin(se(int_from_bin(0b111111111110110110100,21)))))
print(bin(sign_extend(0b111111111110110110100,21)))
print(bin(0xffffffff))
print("/jksdf/sdkljs/".split("/"))

x = """Vor gut drei Jahren habe ich während des Lockdowns angefangen, mich mit Python zu beschäftigen, da mich Künstliche Intelligenz
sehr fasziniert hat. Deshalb habe ich mich mit Neuronalen Netzwerken in Pytorch befasst, vor allem in Bezug auf Bilderkennung,
Mit diesem Projekt war ich auch bei Jugend Forscht (Schüler experimentieren) erfolgreich (Landessieg Baden Württemberg).
Inzwischen habe ich mich auch mit vielen anderen Themen beschäftigt, wie zum Beispiel die Entwicklung von Webseiten mit HTML, CSS und JavaScript.
Außerdem habe ich ein bisschen Erfahrung mit Java und C# (Unity) gesammelt.
Letztes Jahr habe ich mir das Ziel gesetzt, Python Programme auf dem Arduino auszuführen. Dafür habe ich einen Transpiler geschrieben,
der ein Python Subset in C übersetzt, das auf dem Arduino ausgeführt werden kann. Dafür habe ich mich auch mit der Funktionsweise
von Compilern und Assemblern auseinandergesetzt. Das Projekt habe ich auch bei Jugend Forscht eingereicht und habe damit bei Jugend Forscht
auf Landesebene (Baden Württemberg) den dritten Platz im Bereich Mathematik/Informatik erreicht.
Danach hatte ich es satt, Windows zu benutzen und habe verschiedene Linux Distributionen ausprobiert.
In diesem Dezember habe ich auch am Advent of Code teilgenommen, bei dem es an jedem Tag im Advent eine, immer schwieriger werdende
Aufgabe zu lösen gibt. Die Aufgaben habe ich in Python gelöst, da ich mich mit dieser Sprache mitlerweile am besten auskenne.
Am Anfang der Weinachtsferien hatte ich erneut die Idee (genauer gesagt durch eine Wette), Python auf dem Arduino auszuführen.
Dafür habe ich mich grundlegend mit der Architektur von Prozessoren auseinandergesetzt und mir auch das RISC-V ISA genauer angeschaut.
Jetzt bin ich dabei, einen RISC-V Emulator für den Arduino zu schreiben, der den Python Interpreter ausführt und den sehr kleinen
Arbeitsspeicher (2 kb) des Arduinos durch externen Speicher erweitert."""

print(x.replace("\n", " "))