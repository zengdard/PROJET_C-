# Bienvenu sur Retal Bank
# Configuration de l'Environnement de Développement C/C++ avec CMake.

Ce guide détaille les étapes nécessaires pour configurer un environnement de développement en C/C++ avec CMake sur Visual Studio Code.

## Étapes d'Installation

### Installer CMake

1. Téléchargez CMake depuis le site officiel : [CMake](https://cmake.org/download/).
2. Lancez l'installateur et suivez les instructions. Assurez-vous de sélectionner l'option pour installer les en-têtes et bibliothèques pour le développement.

### Installer MSYS2

1. Téléchargez MSYS2 depuis [ici](https://www.msys2.org/).
2. Exécutez le fichier téléchargé et suivez les instructions d'installation.
3. Ouvrez la console MSYS2 et mettez à jour les paquets avec la commande : `pacman -S mingw-w64-ucrt-x86_64-gcc`.

### Configurer Visual Studio Code

1. Installez Visual Studio Code si ce n'est pas déjà fait.
2. Ouvrez Visual Studio Code et installez l'extension C/C++ en cherchant "C/C++" dans la Marketplace.

## Configuration du `tasks.json`

Pour configurer les chemins d'inclusion et de bibliothèque dans votre `tasks.json`, remplacez les chemins par ceux qui correspondent à votre installation Npcap et MSYS2.

### Ajustement des Chemins

Repérez où les bibliothèques Npcap sont stockées dans votre projet. Il devrait y avoir des dossiers `Include` et `Lib` (ou `Lib/x64` pour les architectures 64 bits) dans le dossier des bibliothèques de votre projet. Utilisez ces chemins pour mettre à jour les entrées dans `tasks.json`.

Voici un exemple de configuration pour `tasks.json` :

```json
{
    // ...
    "args": [
        "-g",
        "${file}",
        "-o",
        "${fileDirname}\\${fileBasenameNoExtension}.exe",
        "-I<PATCH_Include>", // Remplacez <PATCH_Include> par votre chemin d'accès réel
        "-L<PATCH_Lib>",     // Remplacez <PATCH_Lib> par votre chemin d'accès réel
        "-lwpcap"
    ],
    // ...
}
```

*Vous avez mon tasks.json perso dans le dossier `.vscode`, les libs sont dans `dev/libs/Npcap/` il y a `Lib` et `Include`.
    - Ajuster le `PATH`.
