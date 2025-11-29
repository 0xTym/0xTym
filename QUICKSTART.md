# Quick Start Guide

Schnellste Methode, um das Sauerbraten ESP zu kompilieren und zu verwenden.

## ⚡ 5-Minuten-Setup

### Schritt 1: Projekt klonen

```bash
git clone https://github.com/0xTym/sauerbraten-esp.git
cd sauerbraten-esp
```

### Schritt 2: Alles bauen

```bash
# Build-Verzeichnis erstellen
mkdir build
cd build

# CMake konfigurieren (Visual Studio 2022)
cmake .. -G "Visual Studio 17 2022" -A Win32

# Alles kompilieren (DLL + Loader)
cmake --build . --config Release
```

### Schritt 3: Dateien finden

Nach dem Build findest du:
- **DLL**: `build/bin/SauerbratenESP.dll`
- **Loader**: `build/bin/SauerbratenLoader.exe`

### Schritt 4: Game starten & Injizieren

```bash
# 1. Starte Cube 2: Sauerbraten

# 2. Führe den Loader aus (als Administrator!)
cd build/bin
SauerbratenLoader.exe sauerbraten.exe SauerbratenESP.dll
```

### Schritt 5: Fertig! 🎉

Eine Konsole erscheint im Spiel. Nutze diese Tasten:

| Taste | Funktion |
|-------|----------|
| **INSERT** | ESP An/Aus |
| **HOME** | Boxen An/Aus |
| **END** | Snaplines An/Aus |
| **DELETE** | Cheat entladen |

## 🔧 Troubleshooting

### "cmake ist kein bekannter Befehl"

**Lösung:** Installiere CMake von https://cmake.org/download/

### "Visual Studio nicht gefunden"

**Lösung:** Installiere Visual Studio 2022 mit "Desktop-Entwicklung mit C++"

Oder nutze diese alternative:
```bash
# MinGW verwenden
cmake .. -G "MinGW Makefiles"
mingw32-make
```

### "Zugriff verweigert" beim Injizieren

**Lösung:** Rechtsklick auf `SauerbratenLoader.exe` → "Als Administrator ausführen"

### ESP wird nicht angezeigt

**Mögliche Ursachen:**

1. **Falsche Offsets** - Deine Spielversion ist anders
   - Lösung: Siehe [OFFSETS.md](OFFSETS.md) für andere Versionen

2. **Game nutzt DirectX statt OpenGL**
   - Lösung: ESP funktioniert nur mit OpenGL-Renderer

3. **DLL nicht korrekt geladen**
   - Lösung: Schaue nach der Debug-Konsole im Spiel

### Game crashed nach Injection

**Lösung:**
1. Nutze den alternativen Injection-Modus:
   ```bash
   SauerbratenLoader.exe sauerbraten.exe SauerbratenESP.dll --loadlib
   ```

2. Stelle sicher, dass das Game 32-bit ist (nicht 64-bit)

## 📋 Build-Modi im Detail

### Release Build (Empfohlen)
```bash
cmake --build . --config Release
```
- Optimiert
- Kleinere Dateigröße
- Schneller

### Debug Build (Für Entwicklung)
```bash
cmake --build . --config Debug
```
- Debugging-Symbole
- Keine Optimierungen
- Einfacher zu debuggen

## 🎯 Erweiterte Nutzung

### Automatisch warten auf Spielstart

```bash
# Loader startet und wartet, bis du das Game öffnest
SauerbratenLoader.exe sauerbraten.exe SauerbratenESP.dll --wait
```

### Verschiedene Injection-Modi

```bash
# Stealth (Manual Mapping) - Standard
SauerbratenLoader.exe sauerbraten.exe SauerbratenESP.dll --manual

# Schnell (LoadLibrary) - weniger stealth
SauerbratenLoader.exe sauerbraten.exe SauerbratenESP.dll --loadlib
```

## 📁 Projekt-Struktur

```
sauerbraten-esp/
├── src/                    # ESP Quellcode
│   ├── main.cpp           # DLL Entry Point
│   ├── esp.cpp/h          # ESP Rendering
│   ├── hooks.cpp/h        # OpenGL Hooks
│   └── rendering.cpp/h    # Text Rendering
├── loader/                 # Stealth Loader
│   ├── main.cpp           # Loader Entry Point
│   ├── loader.cpp/h       # Injection Logic
│   └── README.md          # Loader Dokumentation
├── build/                  # Build-Ausgabe (wird erstellt)
│   └── bin/
│       ├── SauerbratenESP.dll
│       └── SauerbratenLoader.exe
├── CMakeLists.txt         # Haupt-Build-Config
├── README.md              # Haupt-Dokumentation
├── BUILD.md               # Detaillierte Build-Anleitung
├── OFFSETS.md             # Memory-Offsets
└── QUICKSTART.md          # Diese Datei
```

## 🔍 Verifizierung

### Prüfen ob DLL existiert
```bash
dir build\bin\Release\SauerbratenESP.dll
```

### Prüfen ob Loader existiert
```bash
dir build\bin\SauerbratenLoader.exe
```

### Test im Spiel

1. Starte Sauerbraten
2. Starte einen Bot-Match (für Testing)
3. Injiziere die DLL
4. Drücke INSERT
5. Du solltest ESP-Boxen um Bots sehen

## ⚠️ Wichtige Hinweise

### Nur für Testing!

- ✅ Private Server
- ✅ Offline mit Bots
- ✅ Eigene Anti-Cheat Tests
- ❌ NICHT auf öffentlichen Servern!

### Administrator-Rechte

Der Loader braucht Admin-Rechte weil:
- Process Memory Injection erfordert Privilegien
- WriteProcessMemory benötigt Zugriff
- CreateRemoteThread benötigt Berechtigungen

### Anti-Virus Warnungen

Möglicherweise warnt dein AV vor dem Loader:
- **Grund:** Injections-Techniken werden von Malware genutzt
- **Sicher?** Ja, der Code ist open-source
- **Lösung:** Füge Ausnahme hinzu für:
  - `SauerbratenLoader.exe`
  - `SauerbratenESP.dll`

## 🚀 Nächste Schritte

### Nach erfolgreichem Test

1. **Passe Offsets an** (falls nötig)
   - Siehe [OFFSETS.md](OFFSETS.md)
   - Bearbeite `src/entities.h`

2. **Konfiguriere ESP**
   - Bearbeite `src/esp.h`
   - Ändere Farben, Features, etc.

3. **Rebuild**
   ```bash
   cd build
   cmake --build . --config Release
   ```

### Für Entwickler

Siehe:
- [BUILD.md](BUILD.md) - Detaillierte Build-Optionen
- [loader/README.md](loader/README.md) - Loader-Dokumentation
- [OFFSETS.md](OFFSETS.md) - Offset-Findung

## 💡 Tipps

### Schnellerer Workflow

1. Erstelle Batch-Datei `inject.bat`:
```batch
@echo off
cd build\bin
SauerbratenLoader.exe sauerbraten.exe SauerbratenESP.dll --wait
pause
```

2. Rechtsklick → "Als Administrator ausführen"
3. Starte danach das Game

### Testing mit Bots

1. Starte Sauerbraten
2. Multiplayer → Create Server
3. Add Bots (Taste `/addbot 5`)
4. Injiziere DLL
5. Teste ESP Funktionalität

### Clean Build

Wenn etwas nicht funktioniert:
```bash
# Alles löschen
rmdir /s /q build

# Neu starten
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022" -A Win32
cmake --build . --config Release
```

## 📞 Hilfe

Bei Problemen:

1. Lies [BUILD.md](BUILD.md) für Build-Probleme
2. Lies [loader/README.md](loader/README.md) für Injection-Probleme
3. Prüfe [OFFSETS.md](OFFSETS.md) für Game-Version-Issues
4. Erstelle Issue auf GitHub mit:
   - Windows-Version
   - Visual Studio-Version
   - CMake-Version
   - Vollständiger Fehler-Log

---

**Viel Erfolg beim Testen! 🎮**

Bei Fragen: vraquz@icloud.com
