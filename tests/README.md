# Disc-image API probe

`disc_image_probe.cpp` is a 32-bit Windows integration test for the rebuilt DLL.
It checks API behavior, buffer boundaries, and actual image contents. It is not
a game compatibility test.

Use an isolated Wine/Proton prefix, not a running game's prefix. Compile with the
same LLVM-MinGW compiler used for the DLL:

```bash
mkdir -p build/probe
"$LLVM_MINGW/bin/i686-w64-mingw32-clang++" -static -O1 \
  tests/disc_image_probe.cpp -o build/probe/disc_image_probe.exe
cp build/version.dll build/probe/version.dll
```

Create `build/probe/version.json`, replacing the path with your own cooked
2048-byte-sector ISO9660 image:

```json
{
  "DiscImage": "Z:\\path\\to\\game.iso",
  "CDROMDriveLetter": "D",
  "logging": true,
  "logFile": "probe.log",
  "logConsole": false,
  "pauseOnLog": false
}
```

Run from the probe directory, passing the same Windows-visible image path:

```bash
cd build/probe
WINEPREFIX='/absolute/path/to/isolated-test-prefix' \
  WINEDLLOVERRIDES='version=n,b' \
  wine ./disc_image_probe.exe 'Z:\path\to\game.iso'
```

A Wine runner capable of running 32-bit Windows executables is required. To use
Proton, invoke the probe through your existing UMU/Proton setup with an isolated
prefix and the same arguments/environment instead of the plain Wine command.
Do not point the test at a game's active prefix.

`probe-results.txt` should contain nine PASS lines and `failures=0`; the executable
returns zero on success. Checks cover DLL loading without keyboard prompts,
Kernel32 and KernelBase optical-device opens, image sector equality, capacity,
full and short TOC boundaries, out-of-range reads, and undersized buffers.
The probe reads the image without changing it. No game images are included.
