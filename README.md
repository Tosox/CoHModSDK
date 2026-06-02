# CoHModSDK

## 📜 Description

CoHModSDK is a runtime SDK and loader for `Company of Heroes Relaunch`.

It is intended to make SDK-based mods easier to build and easier to install by
providing:

- a small modding SDK for native DLL mods
- a required shared runtime: `CoHModSDKRuntime.dll`
- a `CoHModSDKLoader.dll` mod loader
- a simple `mods/` loading workflow

## 📦 What Is Included

Releases are split into two parts:

- `CoHModSDK`
  - for game installation
  - contains `CoHModSDKShim.dll`, `CoHModSDKLoader.dll`, `CoHModSDKRuntime.dll`, and `CoHModSDKLoader.ini`
- `CoHModSDK-Dev`
  - for mod authors
  - contains `CoHModSDK.lib` and `CoHModSDK.hpp`

## 🔧 Installing

Installation uses a shim-based layout.

`CoHModSDKShim.dll` is the stable DLL named by `.module` files. It proxies the
game's required `GetDllInterface` and `GetDllVersion` exports to `WW2Mod.dll`,
resolves the active SDK folder, and loads that folder's `CoHModSDKLoader.dll`.

`CoHModSDKLoader.dll` is scoped SDK infrastructure. It should live beside the
matching `CoHModSDKRuntime.dll`, `CoHModSDKLoader.ini`, `mods`, `configs`, and
`logs` directories for one specific game mod.

### Vanilla Layout

When the game is launched without `-mod`, the shim uses `WW2\CoHModSDK`.

```text
Company of Heroes Relaunch\
  CoHModSDKShim.dll
  WW2\
    CoHModSDK\
      CoHModSDKLoader.dll
      CoHModSDKRuntime.dll
      CoHModSDKLoader.ini
      mods\
        YourSDKMod.dll
      configs\
      logs\
```

The vanilla `.module` file only needs:

```ini
[global]
DllName = CoHModSDKShim
```

### Modded Layout

When the game is launched with `-mod`, the shim treats the parameter value as
the active `.module` file name. For modded launches, `[global] ModSDKFolder` is
required.

```ini
[global]
DllName = CoHModSDKShim
ModSDKFolder = PhoenixMod\CoHModSDK
```

```text
Company of Heroes Relaunch\
  CoHModSDKShim.dll
  PhoenixMod.module
  PhoenixMod\
    CoHModSDK\
      CoHModSDKLoader.dll
      CoHModSDKRuntime.dll
      CoHModSDKLoader.ini
      mods\
        YourSDKMod.dll
      configs\
      logs\
```

`ModSDKFolder` is relative to the game directory. If it is missing during a
modded launch, startup fails with a clear error instead of falling back to a
different SDK folder.

### Loader Config

`CoHModSDKLoader.ini` is read from the resolved SDK folder. It lists one SDK mod
DLL per line:

```ini
# One DLL name per line
MyFirstSDKMod.dll
AnotherMod.dll
```

Those DLLs are loaded from:

```text
<resolved SDK folder>\mods\
```

Logs and configs remain scoped under the same SDK folder:

```text
<resolved SDK folder>\logs\
<resolved SDK folder>\configs\
```

## 📖 Documentation

Developer-focused documentation is in [`docs/development.md`](docs/development.md).

That includes:

- building the solution
- repository layout
- SDK exports and public API
- writing SDK mods
- loader setup for SDK-enabled mods
- current technical limitations

## 📄 License

See [LICENSE](LICENSE).

Important note:

- Independent mods using the project through its public interfaces are not required to use the repository license.
- Modified versions of this project itself are covered by the project license and its additional permissions.
