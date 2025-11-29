# Build-Anleitung

## Schnellstart (Windows)

### Voraussetzungen
- Visual Studio 2019 oder 2022 (mit C++ Desktop-Entwicklung)
- CMake 3.10 oder höher
- Windows SDK (automatisch mit Visual Studio installiert)

### Build-Schritte

#### 1. Build-Verzeichnis erstellen (oder bereinigen)

```bash
# Falls bereits vorhanden, lösche das alte Build-Verzeichnis
rmdir /s /q build

# Erstelle neues Build-Verzeichnis
mkdir build
cd build
```

#### 2. CMake konfigurieren

**Für Visual Studio 2022 (64-bit Host, 32-bit Target):**
```bash
cmake .. -G "Visual Studio 17 2022" -A Win32
```

**Für Visual Studio 2019:**
```bash
cmake .. -G "Visual Studio 16 2019" -A Win32
```

**Für MinGW:**
```bash
cmake .. -G "MinGW Makefiles"
```

#### 3. Kompilieren

**Visual Studio:**
```bash
cmake --build . --config Release
```

**MinGW:**
```bash
mingw32-make
```

#### 4. DLL finden

Die kompilierte DLL befindet sich in:
```
build/bin/Release/SauerbratenESP.dll     (Visual Studio)
build/bin/SauerbratenESP.dll              (MinGW)
```

## Häufige Probleme

### Linker-Fehler "nicht aufgelöste Externe"

**Problem:**
```
Nicht aufgelöstes externes Symbol "__imp_glVertex2f"
Nicht aufgelöstes externes Symbol "main"
```

**Lösung:**
1. Lösche das Build-Verzeichnis komplett:
   ```bash
   rmdir /s /q build
   ```

2. Stelle sicher, dass du CMake 3.10+ verwendest:
   ```bash
   cmake --version
   ```

3. Baue neu auf (siehe Build-Schritte oben)

### "Cannot open opengl32.lib"

**Problem:**
Windows SDK nicht gefunden.

**Lösung:**
Installiere Visual Studio mit "Desktop-Entwicklung mit C++"-Workload.

### CMake findet Visual Studio nicht

**Lösung:**
Öffne "Developer Command Prompt for VS 2022" statt normale CMD.

### "wglUseFontBitmapsA" nicht gefunden

**Problem:**
gdi32.lib nicht gelinkt.

**Lösung:**
Die neueste Version von CMakeLists.txt enthält gdi32. Führe `git pull` aus.

## Alternative Build-Methode (Visual Studio IDE)

1. Öffne Visual Studio
2. Wähle "Ordner öffnen" → wähle das Repository-Verzeichnis
3. Visual Studio erkennt automatisch CMakeLists.txt
4. Wähle "Build" → "Alle erstellen"
5. DLL befindet sich in `out/build/x86-Release/bin/`

## Verifizieren des Builds

Nach erfolgreichem Build:

```bash
# Prüfe ob DLL existiert
dir build\bin\Release\SauerbratenESP.dll

# Zeige DLL-Info
dumpbin /HEADERS build\bin\Release\SauerbratenESP.dll
```

Die DLL sollte folgende Exports haben:
- `DllMain`

## Nächste Schritte

Nach erfolgreichem Build:
1. Kopiere `SauerbratenESP.dll` in ein geeignetes Verzeichnis
2. Kompiliere den Injector: `g++ injector_example.cpp -o injector.exe`
3. Führe den Injector aus (siehe README.md)

## Debug-Build (für Entwicklung)

```bash
cmake --build . --config Debug
```

Debug-DLL enthält Symbole für Debugging und befindet sich in:
```
build/bin/Debug/SauerbratenESP.dll
```

## Cross-Compilation (Fortgeschritten)

### Linux (Wine)
```bash
mkdir build && cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=../toolchain-mingw.cmake
make
```

Benötigt MinGW-w64-Cross-Compiler:
```bash
sudo apt-get install mingw-w64
```

## Build-Optionen

### Release vs Debug

**Release** (optimiert, klein):
```bash
cmake --build . --config Release
```

**Debug** (Symbole, langsamer):
```bash
cmake --build . --config Debug
```

### Verbose Output

Um zu sehen, welche Befehle ausgeführt werden:
```bash
cmake --build . --config Release --verbose
```

## Troubleshooting

### Vollständiger Clean-Build

Wenn nichts funktioniert:

```bash
# Alles löschen
rmdir /s /q build
git clean -fdx build/

# Neu starten
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022" -A Win32
cmake --build . --config Release
```

### Compiler-Warnungen deaktivieren

Bereits in CMakeLists.txt konfiguriert mit:
- `/W4` für MSVC
- `_CRT_SECURE_NO_WARNINGS` für CRT-Warnungen
- `-Wall -Wextra` für GCC/Clang

---

**Bei Problemen:** Erstelle ein Issue auf GitHub mit:
- CMake-Version (`cmake --version`)
- Visual Studio-Version
- Vollständiger Fehlerlog
- Betriebssystem-Version
