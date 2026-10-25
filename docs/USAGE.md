# General Wine / Proton usage

This guide covers the 32-bit build in this fork. Follow [installation and launcher
setup](../README.md#install-and-launch) first. Use the game's existing prefix and
set its executable folder as the working directory: the loader currently opens
`version.json` relative to that working directory.

## Choose a method

1. Start with `UseVirusekMethod: false` (or omit the key) for the standard method.
2. For SecuROM 7/8, try `UseVirusekMethod: true` if the standard method fails.
3. Use a game's actual drive label and required resources only when its disc checks
   require them. Do not copy `PRO EVOLUTION SOCCER 6` into another game's config.
4. Enable file-only diagnostics if neither method works.

The alternative method is intended for SecuROM 7/8. The upstream loader also has
older-generation paths, but they have not been retested across all games in this fork.
Some executables do not load a local `version.dll` automatically; the original
project documents a separate VersionInjector for such cases. The Linux build script
here builds only the DLL and does not build or install that injector.

## Configuration reference

JSON keys are case-sensitive. Use actual JSON booleans, no comments, and no trailing commas.
Backslashes in Windows paths must be doubled. Each game needs its own `version.json`.

| Key | Meaning / default |
| --- | --- |
| `UseVirusekMethod` | Alternative SecuROM 7/8 method; default `false`. |
| `logging` | Enable loader diagnostics; default `false`. |
| `logFile` | Log filename when logging is enabled, for example `version.log.txt`. |
| `logConsole` | Also use the interactive console; default `true` for upstream compatibility. Set `false` for file-only logging. |
| `pauseOnLog` | Allow diagnostic keyboard pauses; default `true`. Set `false` for unattended startup. |
| `logCreateFile` | Log file-open/attribute activity; default `false`. Useful temporarily, but can create large logs. |
| `CDROMDriveLetter` | Letter the standard method presents as an optical drive; default `L`. Use one letter, such as `D`. |
| `CDROMVolumeName` | Optional label returned by the ANSI volume-information hook; use the real label for that game. |
| `CPUCount` | Optional CPU affinity limit. Omit to leave it unchanged; the PES 6 example uses `1`. |
| `fileMappings` | Optional array of source/target path mappings; see below. |
| `DiscImage` | Optional read-only cooked ISO9660 image for the implemented SCSI reads; see below. |
| `GeometryCheckOneToZero` | Upstream geometry workaround for specific games; not a general recommendation. |
| `Override7C0` | Advanced upstream two-byte hexadecimal override, such as `0000`. Usually omit it; do not treat PES 6 experiments as universal values. |

For diagnostics, merge these keys into the existing configuration:

```json
{
  "logging": true,
  "logFile": "version.log.txt",
  "logConsole": false,
  "pauseOnLog": false,
  "logCreateFile": true
}
```

Check that the log updates on each launch. Disable verbose logging again after
troubleshooting, because games can repeatedly open their data files.

## Additional disc files

A loader cannot supply missing game assets. If a game reads files from its CD,
point it at the files from your own media, for example:

```json
{
  "CDROMDriveLetter": "L",
  "CDROMVolumeName": "YOUR_DISC_LABEL",
  "fileMappings": [
    {"source": "L:\\*", "target": ".\\disc\\*"}
  ],
  "logging": false,
  "pauseOnLog": false,
  "logConsole": false
}
```

That example maps `L:\data\intro.dat` to `disc/data/intro.dat` under the working
directory. Populate the directory with the required original files first.
Mappings affect the APIs hooked by the loader; they are not a replacement for
Wine drive mappings or a complete virtual filesystem.

## Optional disc image reader

`DiscImage` reads your own image directly for selected SCSI pass-through commands:

- READ(10): sectors from the image, including the original primary volume descriptor.
- READ CAPACITY(10): actual sector count and a 2048-byte block size.
- READ TOC: format 0, one session, one data track, LBA or MSF addresses.

Example JSON key:

```json
{"DiscImage": "Z:\\path\\to\\original-game.iso"}
```

The path must be visible inside the Wine/Proton process. A default Wine `Z:` mapping
usually exposes the host filesystem, but launcher sandboxes and custom mappings can
restrict it. Use a mapped drive that actually exists in your prefix.

The reader accepts **cooked 2048-byte-sector ISO9660** ISO/MDF files with the primary
volume descriptor at sector 16. It rejects raw 2352-byte-sector dumps, multi-track
layouts, UDF-only images, and unsupported TOC formats. A filename extension does
not establish the sector layout. MDS descriptors are not image data for this key.
An explicitly configured image that cannot be read returns an error for those
commands; it does not silently substitute another disc.

This is not full optical-drive emulation, does not implement every protection or
SCSI command, and does not replace missing disc data. An actual mounted image may
still be needed when a game reads ordinary files from its optical drive.
The successful PES 6 run used the alternative method and did not exercise this
reader, so do not assume every game requires `DiscImage`.

## Troubleshooting

| Symptom | Check |
| --- | --- |
| No fresh loader log | DLL next to the correct EXE, `version=n,b`, correct working directory and prefix, `logging: true`. |
| `Press any key...` | Set `pauseOnLog: false`; use `logConsole: false` for file-only logs. |
| Wrong disc | Method appropriate for that SecuROM version, real volume label, accessible original media/resources, new log. |
| Game hangs with alternative method | Retry the standard method in a separate launch; compatibility depends on the executable version. |
| `DiscImage` read failure | Wine-visible path, read permissions, 2048-byte ISO9660 layout, supported SCSI command. |
| Other mod stops loading | Existing `version.dll` collision or an overridden DLL entry was lost. Restore the backup. |

Preserve existing `WINEDLLOVERRIDES` entries. Do not change the entire game prefix or
replace its runner simply to install this loader. If reporting an issue, include
exact game edition/update, runner version, loader build, JSON, and a fresh log.

## Uninstall

Close the game. Restore the backed-up `version.dll`/`version.json`, or remove the
added files if none existed before. Remove only this loader's `version` override
and retain any unrelated overrides. Undo optional drive mappings separately.
