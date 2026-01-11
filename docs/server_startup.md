# Server Startup Process

This document describes the SEGS server startup sequence, focusing on filesystem initialization and game data mounting.

## Startup Sequence

```
main() [Servers/AuthServer/main.cpp]
  │
  ├─► registerEnvSingleton()
  │     └─► Creates StandaloneServiceLocator
  │           └─► BaseServiceLocator(native_fs, app_dir)
  │                 ├─► Creates RootFilesystem
  │                 ├─► Registers filesystem factories ("", "pigg")
  │                 ├─► Mounts "/" at "/" (priority -99)
  │                 └─► Mounts app_dir at "app:" (priority -50)
  │
  ├─► Settings::setSettingsPath(config_path)
  │     └─► Resolves virtual paths via resolveToNativePath()
  │
  ├─► initializeGameDataMounts(fs)
  │     ├─► Reads coh_install_dir from [GameData] section
  │     ├─► Mounts coh_install_dir at "coh_install:" (priority -99)
  │     ├─► Iterates coh_install:piggs/*.pigg
  │     │     └─► Mounts each PIGG at "coh_data:" (priority -99)
  │     └─► Mounts app:coh_data at "coh_data:" (priority 0) if exists
  │
  └─► CreateServers()
        └─► MapServer loads game data from "coh_data:"
```

## Virtual Filesystem

The server uses a virtual filesystem abstraction (`RootFilesystem`) that supports:

- **Mount points**: Map virtual paths to physical locations or archives
- **Priority-based resolution**: Higher priority mounts are searched first
- **Multiple backends**: Native filesystem, PIGG archives

### Path Format

Virtual paths use the format `mountpoint:relative/path` (no `/` after colon):

```cpp
"coh_data:bin/costume.bin"  // File inside mounted game data
"app:settings.cfg"          // File relative to application directory
"/absolute/path/file.txt"   // Native filesystem absolute path
```

### Mount Points

| Mount Point | Source | Priority | Purpose |
|-------------|--------|----------|---------|
| `/` | Native filesystem | -99 | Absolute path access |
| `app:` | Application directory | -50 | Portable app-relative paths |
| `coh_install:` | CoH installation | -99 | Access to game installation |
| `coh_data:` | PIGG archives | -99 | Game data from archives |
| `coh_data:` | Local `./coh_data/` | 0 | Local overrides (highest priority) |

### Resolution Order

When opening `coh_data:bin/tricks.bin`:

1. Find all mounts matching `coh_data:` prefix
2. Sort by priority (higher first): local override (0), then PIGGs (-99)
3. Try each mount until file is found
4. Return first successful match

This allows local files in `./coh_data/` to override PIGG contents.

## Configuration

### settings.cfg

```ini
[GameData]
coh_install_dir = /path/to/coh/installation
```

The `coh_install_dir` should point to the CoH game installation containing the `piggs/` directory.

## Native Path Resolution

Some components (ACE config parser, third-party libraries) require native filesystem paths. Use `resolveToNativePath()`:

```cpp
auto* fs = getServiceLocator()->getFS();
String native = fs->resolveToNativePath("app:settings.cfg");
// Returns "./settings.cfg" or "" if inside archive
```

Returns empty string if the path resolves to an archive (PIGG files cannot be accessed via native paths).

## Environment Portability

The `app:` mount point abstracts the application directory across environments:

| Environment | app: resolves to |
|-------------|------------------|
| Standalone server | `QDir::currentPath()` |
| Unreal Engine | `FPaths::ProjectDir()` |
| Tools | Tool-specific working directory |

This allows code to use `app:settings.cfg` instead of platform-specific paths.
