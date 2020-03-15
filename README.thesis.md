# Installationsanweisungen
 1. Folgende Abhängigkeiten müssen installiert werden:
    - cmake 3.16.5
    - make 4.3
    - g++ 9.2.1
 2. Zuerst muss SEAL kompiliert werden. Dazu muss ins Verzeichnis `./native/src` gewechselt werden und `cmake .` gefolgt von `make` aufgerufen werden.
 3. Danach können die einzelnen Implementationen, welche in der Thesis verwendet werden erstellt werden. Dazu muss ins Verzeichnis `./native/thesis` gewechselt werden und `cmake .` gefolgt von `make` aufgerufen werden.
 4. Im Ordner `./native/bin` können die einzelnen Programme gefunden werden.
 5. Die einzelnen Programme haben eine Argumentbeschreibung. Diese wird ausgegeben, wenn das Programm aufgerufen wird, ohne ein Argument anzugeben.