# ESP Stealth Loader

Advanced DLL injector with manual mapping for undetected injection.

## Features

### 🔒 Stealth Injection Methods

**1. Manual Mapping (Recommended)**
- Does NOT use `LoadLibrary` (harder to detect)
- Manually parses PE headers
- Resolves imports manually
- Fixes relocations in memory
- Cleans injection traces after load
- **Most stealthy option**

**2. LoadLibrary Injection**
- Traditional injection method
- Uses Windows `LoadLibraryA`
- Faster but easier to detect
- Good for testing

### 🛡️ Anti-Detection Features

- **No module list entry**: Manual mapping doesn't add DLL to module list
- **Trace cleaning**: Removes loader shellcode after injection
- **Memory protection**: Sets correct page protections
- **Import hiding**: Resolves imports without leaving traces
- **Administrator check**: Warns if not elevated

## Building

### Prerequisites
- Visual Studio 2019/2022
- CMake 3.10+
- Administrator privileges (for injection)

### Compile

```bash
cd loader
mkdir build
cd build

# Configure
cmake .. -G "Visual Studio 17 2022" -A Win32

# Build
cmake --build . --config Release
```

Output: `bin/SauerbratenLoader.exe`

## Usage

### Basic Usage

```bash
# Manual mapping (stealthiest)
SauerbratenLoader.exe sauerbraten.exe SauerbratenESP.dll

# With explicit manual mapping flag
SauerbratenLoader.exe sauerbraten.exe SauerbratenESP.dll --manual
```

### Standard LoadLibrary

```bash
SauerbratenLoader.exe sauerbraten.exe SauerbratenESP.dll --loadlib
```

### Wait for Process

```bash
# Wait for game to start, then inject
SauerbratenLoader.exe sauerbraten.exe SauerbratenESP.dll --wait
```

### Command Line Options

| Option | Description |
|--------|-------------|
| `-m, --manual` | Use manual mapping (default) |
| `-l, --loadlib` | Use LoadLibrary injection |
| `-w, --wait` | Wait for process if not running |
| `-h, --help` | Show help message |

## Examples

### Example 1: Stealth Injection
```bash
# Most stealthy - manual mapping
SauerbratenLoader.exe sauerbraten.exe SauerbratenESP.dll
```

### Example 2: Quick Testing
```bash
# Faster for testing, less stealthy
SauerbratenLoader.exe sauerbraten.exe SauerbratenESP.dll --loadlib
```

### Example 3: Auto-wait
```bash
# Start loader first, then launch game
SauerbratenLoader.exe sauerbraten.exe Sauerbraten ESP.dll --wait
```

## How Manual Mapping Works

1. **Parse PE File**
   - Read DLL from disk
   - Parse DOS and NT headers
   - Identify sections

2. **Allocate Memory**
   - Allocate RWX memory in target process
   - Write headers and sections

3. **Fix Relocations**
   - Calculate delta from preferred base
   - Apply relocation fixups

4. **Resolve Imports**
   - Load required DLLs in target
   - Resolve function addresses
   - Update IAT

5. **Set Protections**
   - Apply correct memory protections
   - Execute > Read > Write

6. **Call DllMain**
   - Execute entry point
   - Initialize DLL

7. **Clean Traces**
   - Free loader data
   - Remove shellcode

## Detection Resistance

### What Makes It Stealthy?

**Manual Mapping:**
- ✅ No `LoadLibrary` call logged
- ✅ DLL not in module list (`EnumProcessModules`)
- ✅ No LdrLoadDll hook triggered
- ✅ Loader traces cleaned after injection
- ✅ No file path in memory

**LoadLibrary:**
- ❌ Calls logged by anti-cheat
- ❌ Module appears in list
- ❌ Path stored in memory
- ✅ Simpler and faster

### What Can Still Detect It?

- **Memory scanning**: Signature scanning still works
- **Hook detection**: API hooks can be detected
- **Behavioral analysis**: Unusual memory patterns
- **Kernel-level AC**: Kernel drivers see everything

## Troubleshooting

### "Failed to open process"

**Problem:** Access denied

**Solution:**
1. Run as Administrator
2. Check if game has anti-debug protection
3. Disable anti-virus temporarily

### "Shellcode returned error code: 2"

**Problem:** Import resolution failed

**Solution:**
- DLL dependencies missing
- Wrong architecture (32-bit vs 64-bit)
- Corrupted DLL file

### "Manual mapping successful" but no ESP

**Problem:** DLL loaded but not working

**Solution:**
1. Check if offsets are correct for your game version
2. Verify OpenGL is being used (not DirectX)
3. Look for DLL console window

### Game Crashes

**Problem:** Incompatible DLL or injection

**Solutions:**
1. Try `--loadlib` method instead
2. Check DLL is 32-bit for 32-bit game
3. Verify DLL has no errors
4. Test DLL in single-player first

## Security Considerations

### Administrator Privileges

The loader requires admin rights because:
- Opening processes requires `PROCESS_ALL_ACCESS`
- Writing to process memory requires privileges
- Most games run with standard user rights

The manifest automatically requests elevation.

### Anti-Virus Warnings

Anti-virus may flag this as malware because:
- Process injection is used by malware
- Memory manipulation looks suspicious
- Manual mapping is advanced technique

**This is a false positive for legitimate testing.**

Add exception for:
- `SauerbratenLoader.exe`
- `SauerbratenESP.dll`

## Advanced Usage

### Debugging Injection

Enable verbose output by modifying code:
```cpp
// In loader.cpp, uncomment debug prints
std::cout << "[DEBUG] Writing section: " << pSectionHeader->Name << "\n";
```

### Custom Injection Timing

Add delay before injection:
```cpp
// In main.cpp after opening process
Sleep(5000); // Wait 5 seconds
success = Loader::ManualMapInject(hProcess, dllPath);
```

### Process Monitoring

Check if DLL is loaded:
```bash
# PowerShell
Get-Process sauerbraten | Select-Object Modules
```

Manual mapped DLLs won't appear in this list!

## Technical Details

### Shellcode Architecture

The loader uses position-independent shellcode:
- Runs entirely in target process
- No dependencies on loader process
- Self-contained import resolver
- Automatic cleanup

### Memory Layout

```
Target Process Memory:
├── DLL Image (PAGE_EXECUTE_READWRITE)
│   ├── Headers
│   ├── .text section (code)
│   ├── .data section (data)
│   └── .rdata section (imports)
├── Loader Data (PAGE_READWRITE) [freed after]
└── Shellcode (PAGE_EXECUTE_READ) [freed after]
```

### Relocation Process

Manual mapping handles ASLR:
1. Calculate delta from preferred base
2. Find all relocation entries
3. Apply fixups to affected addresses
4. Update memory to absolute addresses

## Comparison: Manual Map vs LoadLibrary

| Feature | Manual Map | LoadLibrary |
|---------|------------|-------------|
| Stealth | ⭐⭐⭐⭐⭐ | ⭐⭐ |
| Speed | ⭐⭐⭐ | ⭐⭐⭐⭐⭐ |
| Complexity | High | Low |
| Module List | Hidden | Visible |
| Debugging | Harder | Easier |
| Stability | Good | Excellent |

## Legal Notice

This tool is for **educational and anti-cheat testing purposes only**.

- ✅ Use for security research
- ✅ Use for testing your own anti-cheat
- ✅ Use in controlled environments
- ❌ DO NOT use on public servers
- ❌ DO NOT distribute for malicious use

## References

- [PE Format Documentation](https://docs.microsoft.com/en-us/windows/win32/debug/pe-format)
- [Manual Mapping Guide](https://www.unknowncheats.me/wiki/Manually_Mapping_a_DLL)
- [Process Injection Techniques](https://attack.mitre.org/techniques/T1055/)

---

**Author**: 0xTym
**Version**: 1.0.0
**License**: Educational Use Only
