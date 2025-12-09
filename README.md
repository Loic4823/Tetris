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
    URL [https://download.savannah.gnu.org/releases/freetype/freetype-2.13.2.tar.gz](https://download.savannah.gnu.org/releases/freetype/freetype-2.13.2.tar.gz)
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

### 3. Compilation et Lancement

Ouvrez votre terminal et placez-vous dans le dossier build que vous venez de créer :

```bash
cd build
```

Ensuite, lancez les commandes correspondantes à votre système :

### 🪟 Windows (MinGW64)

```bash
cmake -G "MinGW Makefiles" -DCMAKE_C_COMPILER="C:/mingw64/bin/gcc.exe" -DCMAKE_CXX_COMPILER="C:/mingw64/bin/g++.exe" -DCMAKE_MAKE_PROGRAM="C:/mingw64/bin/mingw32-make.exe" ..
cmake --build .
```

### 🐧 Linux

```bash
cmake ..
make
```

Une fois terminé, vous trouverez l'exécutable généré (ProjetSDL2 ou ProjetSDL2.exe) directement dans le dossier build.
