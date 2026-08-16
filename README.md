# Chokefish

Moteur d'échecs écrit en C, piloté par le protocole [UCI](https://www.chessprogramming.org/UCI), accompagné d'une application .NET (voir plus bas, section à compléter).

## Prérequis — Engine

Le moteur se compile en **C11** et ne dépend que de CMake (≥ 3.16) et d'un compilateur C.

### Linux / WSL

- `cmake` ≥ 3.16
- `gcc` ou `clang` (testé avec GCC 13)
- `make` (ou `ninja`)

### Windows natif

- [Visual Studio](https://visualstudio.microsoft.com/fr/) avec la charge de travail **« Développement Desktop en C++ »** (composant `Microsoft.VisualStudio.Component.VC.Tools.x86.x64`)
- CMake est fourni avec Visual Studio (`Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe`)
- PowerShell 7 pour les scripts de build (`.ps1`)
- WSL 2 avec une distribution Linux pour les scripts `.sh` (ou n'importe quel Linux)

## Build

Les scripts se trouvent à la racine de `engine/` (au même niveau que `CMakeLists.txt`).

Les binaires sont produits dans `build-<plateforme>/bin/<configuration>/` :
`engine/build-linux/bin/Release/Chokefish` et `engine/build-windows/bin/Release/Chokefish.exe`.

### Linux (dans `engine/`)

```sh
./linux_release.sh   # build Release dans build-linux/
./linux_debug.sh     # build Debug dans build-linux/
```

Équivalent manuel :

```sh
cmake -S . -B build-linux -DCMAKE_BUILD_TYPE=Release
cmake --build build-linux -j
```

### Windows (dans `engine/`)

```powershell
.\windows_release.ps1   # build Release dans build-windows/
.\windows_debug.ps1     # build Debug dans build-windows/
```

Le script localise automatiquement Visual Studio (via `vswhere`) et déduit le générateur CMake de la version installée.

Équivalent manuel :

```powershell
cmake -S . -B build-windows -G "Visual Studio 18 2026" -A x64
cmake --build build-windows --config Release
```

### Options CMake

| Option | Défaut | Description |
| --- | --- | --- |
| `CHOKEFISH_BUILD_TESTSUITE` | `ON` | Compile la suite de tests (`TestSuite`) |
| `CHOKEFISH_ENABLE_LTO` | `ON` | Link-Time Optimization en Release (`-flto` / `/GL`). Requise pour les performances du moteur : le code est compilé sans `inline` et repose sur le LTO pour l'inlining inter-TU |

## Exécution

Le moteur parle UCI sur stdin/stdout :

```
$ ./build-linux/bin/Release/Chokefish
uci
isready
readyok
position startpos
go movetime 1000
bestmove b2b3
quit
```

## Tests

La `TestSuite` vérifie la génération de coups par comptage de nœuds (perft) sur 37 positions de référence, jusqu'à des profondeurs 8 à 11.

```sh
# Linux / WSL
ctest --test-dir build-linux --output-on-failure
```

```powershell
# Windows
.\build-windows\bin\Release\TestSuite.exe
```

## Arborescence

```
engine/
├── CMakeLists.txt        # Build système (modules + applications)
├── linux_*.sh            # Scripts de build Linux
├── windows_*.ps1         # Scripts de build Windows
└── src/
    ├── host/             # Points d'entrée (Chokefish, TestSuite, Generator)
    └── modules/
        ├── core/         # Plateau, coups, magics bitboards, threading portable
        ├── serialization # FEN / notation de coups
        ├── game_tools    # Utilitaires de partie
        ├── ai/           # Recherche et évaluation
        ├── uci/          # Protocole UCI
        ├── generation/   # Générateur de magics bitboards
        └── test/         # Suite de tests (perft)
```

## Suite .NET

Interface graphique multi-plateformes (Windows, Linux, macOS) basée sur **Avalonia UI 12** (compiled bindings activés), servant à jouer contre le moteur et à organiser des matchs entre moteurs (test d'hypothèse SPRT).

### Prérequis

- [SDK .NET 10](https://dotnet.microsoft.com/download/dotnet/10.0) (ou supérieur)
- Le moteur compilé (`Chokefish` ou tout autre moteur UCI)

### Structure

```
suite/
├── AdrGaspard.ChokefishSuite.sln
├── AdrGaspard.ChokefishSuite.Core/        # Protocole UCI, données de jeu, communication inter-process
├── AdrGaspard.ChokefishSuite.MVVM/        # ViewModels et logique de match (CommunityToolkit.Mvvm)
└── AdrGaspard.ChokefishSuite.UI.Avalonia/ # Interface Avalonia (vues, convertisseurs, styles)
```

### Build et exécution

```sh
# dans suite/
dotnet build AdrGaspard.ChokefishSuite.sln -c Release
dotnet run --project AdrGaspard.ChokefishSuite.UI.Avalonia
```

### Utilisation

1. **Match vs humain** : sélectionner l'exécutable du moteur (natif Windows ou Linux), régler la position de départ (FEN), le temps de réflexion, puis jouer sur l'échiquier.
2. **SPRT** : opposer deux moteurs sur une liste de positions (`Resources/pos-fair.json` pour un exemple) ; le test séquentiel du rapport de vraisemblance détermine si le premier moteur est meilleur que le second de l'écart Elo configuré (risques α/β réglables).
3. L'historique des coups est navigable (premier/précédent/suivant/dernier), la perspective est inversable, et les informations de recherche du moteur (`info`) peuvent être affichées en temps réel.

