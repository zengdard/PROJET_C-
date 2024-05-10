# Bienvenue sur Retalbank
## Configuration de l'Environnement de Développement C/C++ avec CMake.

Ce guide détaille les étapes nécessaires pour compiler le projet rentalbank.

### Installer CMake

1. Téléchargez CMake depuis le site officiel : [CMake](https://cmake.org/download/).
2. Lancez l'installateur et suivez les instructions. Assurez-vous de sélectionner l'option pour installer les en-têtes et bibliothèques pour le développement.

### Installer MariaDB

1. Téléchargement de l'installateur
     
          sudo apt update
          sudo apt install mariadb-server
          sudo mysql_secure_installation

3. Configuration post-installation
     Vous pouvez exécuter mysql_secure_installation à partir de la ligne de commande pour sécuriser votre installation de MariaDB. Vous devez changer le mot de passe root avec le mot de passe "toor", autoriser les connexions à ce compte et valider les changements.

## Installation de Qt 6

1. Ajouter le dépôt Qt:
Ajoutez le dépôt officiel Qt à votre système pour obtenir la dernière version de Qt 6 :

          sudo apt-get install qt6-base-dev
          sudo apt-get install qt6-tools-dev
          sudo apt-get install qt6-tools-dev-tools
          sudo apt-get install build-essential libgl1-mesa-dev


### Compiler le projet CMake
1. Ouvrir un terminal ou une invite de commandes.
2. Naviguer dans le répertoire du projet après avoir décompressé l'archive.
   
          cd retalbank
3. Créer un répertoire pour build.
   
          mkdir build
          cd build
4.Configurer le projet avec CMake.
     La configuration initiale de CMake analyse le CMakeLists.txt et prépare le projet à la compilation.
     
          cmake ..
5.Compiler le projet.
     Une fois la configuration terminée, utilisez make (ou un équivalent selon votre générateur) pour compiler le projet.
     
          cmake --build .
Maintenant vous pouvez éxécutez le projet en faisant :
          
          ./retalbank

