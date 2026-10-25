# Pro Evolution Soccer 6 on Linux

## What was verified

PES 6 was confirmed running by the user on 2026-10-25 using the rebuilt 32-bit DLL,
CachyOS Proton (`proton-cachyos-slr`), and the officially updated game executable.
The user installed that update from their `PES6PATCH.mds/.mdf` media. We have not
established its numeric patch version or compatibility with every regional build.
The installed game executable was not edited or replaced.

## Install

1. Build the DLL following [README.md](README.md#build-on-linux).
2. Close PES 6 and back up any existing `version.dll` and `version.json` beside `PES6.exe`.
3. Copy `build/version.dll` into that folder.
4. Copy [the PES 6 example](Example%20Config%20Files/Pro%20Evolution%20Soccer%206/version.json)
   there as `version.json`.
5. In Heroic, select the existing PES 6 prefix and the main `PES6.exe`. Set the
   working directory to its folder and add `WINEDLLOVERRIDES=version=n,b`.
6. Launch PES 6 normally. Steam and plain Wine equivalents are in [README.md](README.md#install-and-launch).

The tested configuration uses `UseVirusekMethod: true`. Keep logging disabled for
normal play. `CPUCount: 1` is part of the tested example; it is not a requirement
for every other SecuROM game.

`exeFile` in historical examples is descriptive here: the launcher still chooses
the executable. Placing a name in the JSON does not configure Heroic's executable.

## Disc image (optional)

During troubleshooting, the user's original PES 6 MDF/MDS pair was mounted using
CDEmu. Its ISO9660 volume identifier was `PRO EVOLUTION SOCCER 6`, matching the
example configuration. The patch disc is separate from the original game disc.

Mounting and `DiscImage` were not independently proven necessary: the successful
alternative-method trace did not issue the SCSI commands handled by the image reader.
Try the supplied configuration first. If further disc access is needed, use your
own original media rather than assuming the patch disc can substitute for it.

If CDEmu is already installed, identify an unused device with `cdemu status`, then
load the MDS descriptor and mount the resulting optical device:

```bash
# Example only: use the device numbers reported on your machine.
cdemu load 0 '/path/to/Pro Evolution Soccer 6.mds'
cdemu device-mapping
udisksctl mount -b /dev/sr0
```

In `winecfg`, use the same game prefix, map `D:` to the mounted directory and set
the drive type to CD-ROM. The mounted folder must remain accessible to your launcher.
Do not replace an occupied CDEmu device. This setup is separate from building the DLL.

For a cooked 2048-byte-sector ISO9660 MDF, the optional image reader can instead
be configured with a Wine-visible path:

```json
{
  "UseVirusekMethod": true,
  "DiscImage": "Z:\\path\\to\\Pro Evolution Soccer 6.mdf",
  "logging": false,
  "logConsole": false,
  "pauseOnLog": false
}
```

Use that as an illustration of the extra key; retain the other settings in your
PES 6 configuration. The MDF contains the sectors; the MDS descriptor is used by
CDEmu, not by `DiscImage`. Raw 2352-byte images are not supported by this reader.
See [image-reader limits](docs/USAGE.md#optional-disc-image-reader).

## Diagnose a failure

Temporarily enable:

```json
{
  "logging": true,
  "logFile": "version.log.txt",
  "logConsole": false,
  "pauseOnLog": false
}
```

Merge those keys into the existing JSON. After launching, `version.log.txt`
should contain `Version.DLL Loaded!` and the alternative-method message.
If it does not update, check the DLL override, working directory, and whether you
selected the right executable/prefix. A stale log does not describe the latest launch.

With `pauseOnLog` left at its upstream default, debug logging can display
`Press any key...` and stop startup until input is supplied. The example disables it.

If the game still rejects the disc or hangs, record the executable version/hash,
runner version, exact error, and a new loader log. The confirmed result applies
to the tested installation; copying this JSON is not a guarantee for every edition.

## Revert

Close PES 6, restore the backed-up DLL and JSON, and restore the previous DLL
override in the launcher. If there was no previous loader, remove the two added
files and remove only the `version` entry from `WINEDLLOVERRIDES`. Preserve other overrides.
The loader installation does not require changing `PES6.exe` or the game data.
