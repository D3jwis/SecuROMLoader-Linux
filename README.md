# SecuROMLoader-Linux

A fork of [nckstwrt/SecuROMLoader](https://github.com/nckstwrt/SecuROMLoader)
with Wine/Proton compatibility fixes, a Linux build script, and a tested PES 6 configuration.

The output is a **32-bit Windows `version.dll` loaded by Wine/Proton**. It is not a
native Linux executable. Use it with games you own. The upstream GPL-3.0 license
and third-party credits are retained.

## Compatibility

| Target | Status in this fork |
| --- | --- |
| Pro Evolution Soccer 6, with the user's official update from PES6PATCH media | Confirmed running on Linux with CachyOS Proton on 2026-10-25 |
| Other 32-bit SecuROM games | Configuration guidance provided; not retested with this fork |
| Windows | Not tested after these changes |
| 64-bit games | Not supported by the supplied Linux build script |

Upstream targets several SecuROM generations, including versions 3–8. See the
[original upstream compatibility list and credits](docs/UPSTREAM.md); that list
is historical upstream evidence, not a list of games verified with this fork.
Different regional releases and official patches can behave differently.

## Build on Linux

You need Bash and an **LLVM-MinGW toolchain with the i686 Clang compiler**.
Download and extract a Linux-hosted toolchain from the
[official LLVM-MinGW releases](https://github.com/mstorsjo/llvm-mingw/releases).
This fork was built with the UCRT toolchain release `20261020`.
A regular Linux `clang++` without the Windows headers/libraries is insufficient.

```bash
cd /path/to/SecuROMLoader-Linux
LLVM_MINGW=/absolute/path/to/extracted/llvm-mingw ./build-linux.sh
```

The result is `build/version.dll`. Alternatively, put
`i686-w64-mingw32-clang++` on `PATH` and run `./build-linux.sh`.
The build script does not install anything into a game or download a toolchain.

## Install and launch

1. Close the game. Back up any existing `version.dll` and `version.json` in its folder.
2. Copy `build/version.dll` next to the game's main `.exe`. If the game already uses
   a different `version.dll` mod, resolve that conflict before replacing it.
3. Put `version.json` in the same folder. Start with the configuration for your game.
4. Set `WINEDLLOVERRIDES=version=n,b` in the game's launcher.
5. Set the working directory to the folder containing the executable and `version.json`.
   Launch the original game executable normally.

`n,b` tells Wine to try the local Windows DLL before its built-in implementation.
If an override string already exists, preserve its other entries, for example
`dinput8=n,b;version=n,b`. See the [Wine manual](https://man.archlinux.org/man/wine.1.en#WINEDLLOVERRIDES).

### Heroic

Open the game's settings and add an environment variable:

| Name | Value |
| --- | --- |
| `WINEDLLOVERRIDES` | `version=n,b` |

Select the existing prefix where the game is installed, use the game's main EXE,
and launch it from Heroic. Menu labels can vary by Heroic version.

### Steam / Proton

Add this to the game's launch options:

```text
WINEDLLOVERRIDES="version=n,b" %command%
```

For a non-Steam shortcut, set **Start In** to the executable's directory and enable
Proton in the shortcut's compatibility settings.

### Plain Wine

Use the same Wine runner and prefix that the game was installed with:

```bash
cd '/path/to/game'
WINEPREFIX='/path/to/existing-prefix' WINEDLLOVERRIDES='version=n,b' wine './Game.exe'
```

## PES 6

Copy the supplied [PES 6 configuration](Example%20Config%20Files/Pro%20Evolution%20Soccer%206/version.json)
next to `PES6.exe`, alongside the rebuilt `version.dll`.

The essential tested settings are:

```json
{
  "UseVirusekMethod": true,
  "CDROMDriveLetter": "D",
  "CDROMVolumeName": "PRO EVOLUTION SOCCER 6",
  "CPUCount": 1,
  "logging": false,
  "logConsole": false,
  "pauseOnLog": false
}
```

Keep `WINEDLLOVERRIDES=version=n,b` enabled. The game executable does not need to
be edited or replaced. The standard loader method still gave a wrong-disc message
in our tests; use the alternative method above for this tested PES 6 setup.

See [the full PES 6 guide](PES6-WINE.md) for installation, optional disc mapping,
diagnostics, and the exact limits of the successful test.

## Other SecuROM games

Start with the standard loader method:

```json
{
  "UseVirusekMethod": false,
  "logging": false,
  "pauseOnLog": false,
  "logConsole": false
}
```

For a SecuROM 7/8 game that fails with that method, try `UseVirusekMethod: true`.
This alternative is not intended for SecuROM 3/4/5. Change one setting at a time;
PES 6's drive label and CPU limit are not universal settings for other games.

See [the general usage and configuration guide](docs/USAGE.md), including
file mappings, real disc images, troubleshooting, and rollback.
The [example configurations](Example%20Config%20Files) are starting points, not
additional games verified with this fork.

## Changes from upstream

- Apply the same file mapping and disc-device handling through Kernel32 and KernelBase.
- Prevent recursive logging when console output calls hooked Windows APIs.
- Allow file-only diagnostics (`logConsole`) and disable keyboard pauses (`pauseOnLog`).
- Bound a table-of-contents copy that previously read beyond its 20-byte source.
- Add opt-in read-only SCSI responses from a user's cooked ISO9660 disc image.
- Add Clang/SDK portability fixes and a Linux build script for the 32-bit DLL.

The successful PES 6 test followed several changes together. Their individual
necessity has not been isolated. In particular, the successful alternative-method
trace did not issue image-reader SCSI commands, so `DiscImage` is not established
as necessary for that successful launch.

## Validation

- The Linux build script completed successfully.
- PES 6 reached the game and was confirmed working by the user.
- Nine isolated Wine/Proton API checks passed, covering both file-open paths,
  image sector contents, capacity, TOC boundaries, and invalid reads.

See [test instructions](tests/README.md). The probe tests loader APIs and memory
boundaries; it does not establish compatibility with additional games.

## Credits and license

Original loader: [nckstwrt](https://github.com/nckstwrt/SecuROMLoader).
The Wine/Proton changes in this fork were developed and tested with assistance
from OpenAI Codex. Existing upstream and third-party authorship is retained.

Thanks to ELF_7719116, virusek, BitCrackers/version-proxy, and TsudaKageyu/MinHook.
See [upstream credits](docs/UPSTREAM.md#credits), [LICENSE](LICENSE), and the
[MinHook license](src/minhook/LICENSE.txt).
