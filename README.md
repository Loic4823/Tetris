# 🧱 Projet Tetris C SDL2 - Liste Complète des Fonctionnalités

Ce projet est un clone complet de Tetris écrit en C avec la librairie SDL2. Il intègre des mécaniques modernes, un système de sauvegarde complet et une gestion avancée des paramètres.

## 🎮 Gameplay (Mécaniques de jeu)
* **Système Classique :** Grille standard de 10 colonnes sur 20 lignes.
* **7 Pièces (Tetrominoes) :** Gestion des formes I, O, T, S, Z, J, L avec couleurs distinctes.
* **Système de Rotation SRS Complet :** Implémentation du Super Rotation System officiel incluant les tables de "Wall Kicks". Cela permet aux pièces de tourner intelligemment dans les espaces restreints sans se bloquer.
* **Gestion des Entrées (DAS & ARR) :** Contrôles fluides et réactifs grâce au Delayed Auto-Shift (délai initial) et à l'Auto-Repeat Rate (vitesse de répétition), permettant des déplacements latéraux rapides et précis.
* **Générateur "7-Bag" :** Algorithme de distribution aléatoire équitable (mélange de sacs de 7 pièces) pour éviter les répétitions ou les "sécheresses".
* **Pièce Fantôme (Ghost) :** Affichage en transparence de la position d'atterrissage future pour aider à la visée.
* **Réserve (Hold) :** Possibilité de stocker une pièce avec la touche `C` pour l'échanger stratégiquement plus tard.
* **Prévisualisation (Next) :** Affichage de la prochaine pièce à venir.
* **Chute Rapide (Soft Drop) :** Accélération manuelle de la descente pour gagner du temps.
* **Chute Instantanée (Hard Drop) :** Pose immédiate de la pièce au sol.
* **Lock Delay Strict :** Une fois la pièce au sol, un délai de 0.5s est accordé avant le blocage définitif. Le chronomètre ne se réinitialise que si la pièce descend plus bas (méthode anti-infini).

## 📈 Progression & Score
* **Système de Score :** Points attribués selon le nombre de lignes détruites simultanément (1, 2, 3 ou "Tetris" de 4 lignes).
* **Niveaux :** Montée en niveau automatique toutes les 10 lignes détruites.
* **Vitesse Progressive :** La vitesse de chute augmente exponentiellement à chaque niveau.
* **High Scores :** Sauvegarde et affichage du Meilleur Score et du Meilleur nombre de Lignes.

## ⚙️ Paramètres & Options
* **Résolution Dynamique :** 3 modes d'affichage supportés :
    * Fenêtré (800x700)
    * Grand Fenêtré (1200x1050)
    * Plein Écran
* **Packs de Textures :** Changement de l'apparence des blocs en temps réel :
    * *Default* (Classique)
    * *Alt* (Alternatif)
    * *MC* (Style Minecraft)
* **Configuration des Touches :** Remapping complet du clavier pour toutes les actions (Gauche, Droite, Rotation, Hold...) avec support de deux touches par action.
* **Niveau de Départ :** Choix du niveau de difficulté initial (1 à 10).
* **Vitesse Auto :** Option pour activer/désactiver l'accélération automatique progressive.
* **Réinitialiser :** Bouton pour remettre tous les paramètres aux valeurs d'usine.

## 🎵 Audio
* **Playlist Musicale :** 10 pistes musicales intégrées (`tetris1.mp3` à `tetris11.mp3`) avec affichage du nom de l'ambiance (Electro, Orchestre, Japonais, Minecraft, etc.).
* **Mode Silencieux :** Option "Aucune" pour couper la musique tout en gardant les bruitages.
* **Effets Sonores (SFX) :** Son de validation (`clear.mp3`) joué en synchronisation avec l'animation de destruction de ligne.
* **Gestion du Volume :**
    * Contrôle global du volume (Musique + SFX liés).
    * Mode Muet (Mute) via touche raccourci ou menu.
    * *Sécurité :* Le jeu démarre toujours en mode "Muet" par défaut.

## 💾 Système & Sauvegarde
* **Persistance Complète :** Toutes les données sont sauvegardées dans un fichier binaire `save.dat`.
* **Sauvegarde des Paramètres :** Résolution, touches, choix de musique, textures, records sont conservés entre les sessions.
* **Reprise de Partie :** Si le jeu est quitté en cours de partie (ou via Échap), l'état exact de la grille est sauvegardé. Un bouton "Continuer" apparaît au prochain lancement.
* **Interface Graphique :** Menus navigables au clavier avec images de fond et décorations dynamiques (les pièces du menu changent selon le pack de texture choisi).


# 🚀 Tuto : Compiler et lancer le projet (Windows & Linux)

Voici la procédure pas à pas pour configurer, compiler et lancer le projet.

## 1. Préparation

À la racine de votre projet, créez un nouveau dossier nommé `build`.

## 2. Configuration du fichier CMakeLists.txt

Copiez le contenu ci-dessous dans votre fichier `CMakeLists.txt` selon votre système d'exploitation.

### 🐧 Pour Linux

```cmake
cmake_minimum_required(VERSION 3.16)

# Nom du projet
project(ProjetSDL2)

set(CMAKE_C_STANDARD 11)

# ==============================================================================
# CONFIGURATION STATIQUE & OPTIONS
# ==============================================================================

set(BUILD_SHARED_LIBS OFF CACHE BOOL "Force static libraries" FORCE)
set(SDL_SHARED OFF CACHE BOOL "Build shared SDL" FORCE)
set(SDL_STATIC ON CACHE BOOL "Build static SDL" FORCE)
set(SDL_TEST OFF CACHE BOOL "Disable SDL test" FORCE)

# --- CORRECTION DU PLANTAGE ARCH LINUX ---
# On désactive PipeWire car l'API a changé et fait planter la compilation de cette version de SDL
set(SDL_PIPEWIRE OFF CACHE BOOL "Disable PipeWire audio" FORCE)
set(SDL_PIPEWIRE_SHARED OFF CACHE BOOL "Disable PipeWire shared" FORCE)

# --- CONFIGURATION SDL_MIXER ---
set(SDL2MIXER_OPUS OFF CACHE BOOL "Disable Opus" FORCE)
set(SDL2MIXER_FLAC OFF CACHE BOOL "Disable FLAC" FORCE)
set(SDL2MIXER_MOD OFF CACHE BOOL "Disable MOD" FORCE)
set(SDL2MIXER_MIDI OFF CACHE BOOL "Disable MIDI" FORCE)
set(SDL2MIXER_WAVPACK OFF CACHE BOOL "Disable WavPack" FORCE)
set(SDL2MIXER_GME OFF CACHE BOOL "Disable GME" FORCE)

# MP3 via MiniMP3 (interne)
set(SDL2MIXER_MP3_MINIMP3 ON CACHE BOOL "Use internal MiniMP3" FORCE)
set(SDL2MIXER_VORBIS STB CACHE STRING "Use internal STB Vorbis" FORCE)

# ==============================================================================
# INCLUSION DES SOUS-PROJETS
# ==============================================================================

# 1. SDL2
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/external/SDL/CMakeLists.txt")
    add_subdirectory(external/SDL)
else()
    message(FATAL_ERROR "Dossier external/SDL manquant !")
endif()

# 2. SDL2_image
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/external/SDL_image/CMakeLists.txt")
    set(SDL2IMAGE_TIF OFF CACHE BOOL "" FORCE)
    set(SDL2IMAGE_WEBP OFF CACHE BOOL "" FORCE)
    set(SDL2IMAGE_JXL OFF CACHE BOOL "" FORCE)
    set(SDL2IMAGE_AVIF OFF CACHE BOOL "" FORCE)
    add_subdirectory(external/SDL_image)
endif()

# 3. SDL2_ttf
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/external/SDL_ttf/CMakeLists.txt")
    add_subdirectory(external/SDL_ttf)
endif()

# 4. SDL2_mixer
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/external/SDL_mixer/CMakeLists.txt")
    add_subdirectory(external/SDL_mixer)
endif()

# ==============================================================================
# EXÉCUTABLE
# ==============================================================================

file(GLOB SOURCES "src/*.c")
add_executable(ProjetSDL2 ${SOURCES})

target_include_directories(ProjetSDL2 PRIVATE 
    include
    external/SDL/include
    external/SDL_image
    external/SDL_ttf
    external/SDL_mixer/include
)

target_link_libraries(ProjetSDL2 PRIVATE 
    SDL2-static
    SDL2_image
    SDL2_ttf
    SDL2_mixer
)

# Configuration Linux (Libs système obligatoires)
if(UNIX AND NOT APPLE)
    target_link_libraries(ProjetSDL2 PRIVATE m dl pthread)
endif()

# Configuration Windows
if(WIN32)
    target_link_libraries(ProjetSDL2 PRIVATE mmdevapi version imm32 winmm setupapi)
    set_target_properties(ProjetSDL2 PROPERTIES WIN32_EXECUTABLE ON)
endif()

file(COPY ${CMAKE_SOURCE_DIR}/assets DESTINATION ${CMAKE_BINARY_DIR})
```
### 🪟 Pour Windows

```cmake
cmake_minimum_required(VERSION 3.16)

# ==============================================================================
# CORRECTIFS POUR WINDOWS & CMAKE RÉCENT
# ==============================================================================

# 1. Empêche l'erreur de certificat SSL lors du téléchargement
set(CMAKE_TLS_VERIFY 0)

# 2. Empêche l'erreur "Compatibility with CMake < 3.5" avec Freetype
set(CMAKE_POLICY_VERSION_MINIMUM "3.5")

project(ProjetSDL2)
set(CMAKE_C_STANDARD 11)

# ==============================================================================
# OPTIONS DE COMPILATION
# ==============================================================================

set(BUILD_SHARED_LIBS OFF CACHE BOOL "Force static libraries" FORCE)
set(SDL_SHARED OFF CACHE BOOL "Build shared SDL" FORCE)
set(SDL_STATIC ON CACHE BOOL "Build static SDL" FORCE)
set(SDL_TEST OFF CACHE BOOL "Disable SDL test" FORCE)

set(SDL_PIPEWIRE OFF CACHE BOOL "Disable PipeWire audio" FORCE)
set(SDL_PIPEWIRE_SHARED OFF CACHE BOOL "Disable PipeWire shared" FORCE)

# --- CONFIGURATION SDL_MIXER ---
set(SDL2MIXER_OPUS OFF CACHE BOOL "Disable Opus" FORCE)
set(SDL2MIXER_FLAC OFF CACHE BOOL "Disable FLAC" FORCE)
set(SDL2MIXER_MOD OFF CACHE BOOL "Disable MOD" FORCE)
set(SDL2MIXER_MIDI OFF CACHE BOOL "Disable MIDI" FORCE)
set(SDL2MIXER_WAVPACK OFF CACHE BOOL "Disable WavPack" FORCE)
set(SDL2MIXER_GME OFF CACHE BOOL "Disable GME" FORCE)
set(SDL2MIXER_MP3_MINIMP3 ON CACHE BOOL "Use internal MiniMP3" FORCE)
set(SDL2MIXER_VORBIS STB CACHE STRING "Use internal STB Vorbis" FORCE)

# ==============================================================================
# TÉLÉCHARGEMENT AUTOMATIQUE DE FREETYPE
# ==============================================================================
include(FetchContent)

message(STATUS "Téléchargement de Freetype (requis pour le texte)...")

set(FT_DISABLE_ZLIB ON CACHE BOOL "" FORCE)
set(FT_DISABLE_BZIP2 ON CACHE BOOL "" FORCE)
set(FT_DISABLE_PNG ON CACHE BOOL "" FORCE)
set(FT_DISABLE_BROTLI ON CACHE BOOL "" FORCE)
set(SKIP_INSTALL_ALL ON CACHE BOOL "" FORCE)

FetchContent_Declare(
    freetype
    URL "https://download.savannah.gnu.org/releases/freetype/freetype-2.13.2.tar.gz"
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE # Supprime l'avertissement jaune CMP0135
)
FetchContent_MakeAvailable(freetype)

if(NOT TARGET Freetype::Freetype)
    add_library(Freetype::Freetype ALIAS freetype)
endif()

set(FREETYPE_FOUND TRUE CACHE BOOL "" FORCE)
set(FREETYPE_LIBRARY freetype CACHE BOOL "" FORCE)
set(FREETYPE_INCLUDE_DIRS "${freetype_SOURCE_DIR}/include" CACHE STRING "" FORCE)

# ==============================================================================
# INCLUSION DES SOUS-PROJETS LOCAUX
# ==============================================================================

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/external/SDL/CMakeLists.txt")
    add_subdirectory(external/SDL)
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/external/SDL_image/CMakeLists.txt")
    set(SDL2IMAGE_TIF OFF CACHE BOOL "" FORCE)
    set(SDL2IMAGE_WEBP OFF CACHE BOOL "" FORCE)
    set(SDL2IMAGE_JXL OFF CACHE BOOL "" FORCE)
    set(SDL2IMAGE_AVIF OFF CACHE BOOL "" FORCE)
    add_subdirectory(external/SDL_image)
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/external/SDL_ttf/CMakeLists.txt")
    add_subdirectory(external/SDL_ttf)
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/external/SDL_mixer/CMakeLists.txt")
    add_subdirectory(external/SDL_mixer)
endif()

# ==============================================================================
# EXÉCUTABLE ET LIENS
# ==============================================================================

file(GLOB SOURCES "src/*.c")
add_executable(ProjetSDL2 ${SOURCES})

target_include_directories(ProjetSDL2 PRIVATE 
    include
    external/SDL/include
    external/SDL_image
    external/SDL_ttf
    external/SDL_mixer/include
    "${freetype_SOURCE_DIR}/include"
)

# --- CORRECTION LIENS WINDOWS ---
target_link_libraries(ProjetSDL2 PRIVATE 
    SDL2main
    SDL2-static
    SDL2_image
    SDL2_ttf
    SDL2_mixer
    freetype
)

if(UNIX AND NOT APPLE)
    target_link_libraries(ProjetSDL2 PRIVATE m dl pthread)
endif()

if(WIN32)
    target_link_libraries(ProjetSDL2 PRIVATE mmdevapi version imm32 winmm setupapi)
    set_target_properties(ProjetSDL2 PROPERTIES WIN32_EXECUTABLE ON)
endif()

file(COPY ${CMAKE_SOURCE_DIR}/assets DESTINATION ${CMAKE_BINARY_DIR})
```

### Pour mac

```cmake
cmake_minimum_required(VERSION 3.16)
project(ProjetSDL2 C)

set(CMAKE_C_STANDARD 11)

# ==============================================================================
# DÉTECTION DES LIBRAIRIES (VIA HOMEBREW)
# ==============================================================================
# Sur Mac, on utilise les paquets système, c'est beaucoup plus stable.

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(SDL2_mixer REQUIRED)

# ==============================================================================
# SOURCES
# ==============================================================================

file(GLOB SOURCES "src/*.c")

# ==============================================================================
# CRÉATION DU "APP BUNDLE" (.app)
# ==============================================================================
# MACOSX_BUNDLE dit à CMake de créer une vraie application Mac et pas juste un binaire.

add_executable(ProjetSDL2 MACOSX_BUNDLE ${SOURCES})

# On lie les librairies trouvées
target_link_libraries(ProjetSDL2 PRIVATE 
    SDL2::SDL2 
    SDL2_image::SDL2_image 
    SDL2_ttf::SDL2_ttf 
    SDL2_mixer::SDL2_mixer
)

# On inclut les headers (defs.h, etc.)
target_include_directories(ProjetSDL2 PRIVATE include)

# ==============================================================================
# GESTION DES ASSETS (COPIE DANS LE .APP)
# ==============================================================================
# C'est la partie "Mac Proof". On copie le dossier 'assets' DANS l'application.
# Chemin final : ProjetSDL2.app/Contents/Resources/assets

set(APPS_DIR "${CMAKE_CURRENT_BINARY_DIR}/ProjetSDL2.app/Contents/Resources")

add_custom_command(TARGET ProjetSDL2 POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${APPS_DIR}
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/assets ${APPS_DIR}/assets
    COMMENT "Copie du dossier assets dans le paquet de l'application..."
)

# Infos pour le système (Nom, Version...)
set_target_properties(ProjetSDL2 PROPERTIES
    MACOSX_BUNDLE_BUNDLE_NAME "TetrisSDL"
    MACOSX_BUNDLE_GUI_IDENTIFIER "com.monnom.tetris"
    MACOSX_BUNDLE_SHORT_VERSION_STRING "1.0"
)
```

### 3. Compilation et Lancement

Ouvrez votre terminal et placez-vous dans le dossier build que vous venez de créer :

```bash
cd build
```

Ensuite, lancez les commandes correspondantes à votre système :

### 🪟 Windows (MinGW64)

Mettez le dossier "mingw64" a la rascine de votre disque C:

[télécharger](https://www.mingw-w64.org/source/) mingw64

Compiler :

```bash
cmake -G "MinGW Makefiles" -DCMAKE_C_COMPILER="C:/mingw64/bin/gcc.exe" -DCMAKE_CXX_COMPILER="C:/mingw64/bin/g++.exe" -DCMAKE_MAKE_PROGRAM="C:/mingw64/bin/mingw32-make.exe" ..
cmake --build .
```

### Mac

A faire une fois pour installer "Homebrew" :

```bash
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
```

Compiler :

```bash
cmake ..
make
```

### 🐧 Linux

Compiler :

```bash
cmake ..
make
```

Une fois terminé, vous trouverez l'exécutable généré (ProjetSDL2 ou ProjetSDL2.exe) directement dans le dossier build.
