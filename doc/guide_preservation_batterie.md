# Manuel d'optimisation de la batterie et utilisation écran éteint

Ce guide explique en détail comment utiliser scrcpy sur PC tout en **gardant l'écran physique de votre smartphone complètement éteint**, afin d'éliminer la surchauffe et de préserver la durée de vie de la batterie.

---

## 1. Pourquoi utiliser scrcpy avec l'écran du smartphone éteint ?

L'utilisation prolongée d'un smartphone branché en USB présente deux risques majeurs pour la batterie :
1. **La surchauffe thermique** : La charge de la batterie combinée à l'alimentation de l'écran OLED/LCD et au processeur graphique fait monter la température au-dessus de 40°C, ce qui accélère la dégradation chimique des cellules Lithium-Ion.
2. **Le marquage de l'écran (burn-in)** : Laisser un écran OLED allumé sur une image fixe use prématurément les sous-pixels.

Avec scrcpy, le flux vidéo est capturé directement depuis la mémoire vidéo du système Android. **L'écran physique du téléphone n'a pas besoin d'être allumé** pour que vous puissiez voir et contrôler l'appareil depuis votre PC.

---

## 2. La commande optimale (Protection batterie & surchauffe)

Dans le terminal ou dans un script de lancement :

```cmd
scrcpy -S -w --temperature --auto-reconnect
```

### Explication des options :
- **`-S` (`--turn-screen-off`)** : Éteint instantanément l'écran physique du téléphone dès le démarrage. L'écran devient totalement noir, mais la fenêtre sur votre PC reste fluide et réactive.
- **`-w` (`--stay-awake`)** : Empêche Android d'entrer en veille profonde lorsqu'il est branché. Le téléphone reste opérationnel sans jamais se verrouiller pendant que vous l'utilisez.
- **`--temperature`** : Affiche la température mesurée de la batterie en temps réel directement dans la barre de titre de la fenêtre (ex: `Pixel 8 [33.4°C]`).
- **`--auto-reconnect`** : Reconnecte automatiquement la session si le câble USB est heurté ou temporairement débranché.

---

## 3. Raccourcis clavier en direct (sans redémarrer scrcpy)

Pendant que scrcpy tourne, vous pouvez contrôler l'écran physique et la température avec les touches de votre clavier :

| Raccourci | Action |
|---|---|
| <kbd>Alt</kbd> + <kbd>o</kbd> | **Éteindre** l'écran physique du téléphone (le miroir PC reste actif). |
| <kbd>Alt</kbd> + <kbd>Maj</kbd> + <kbd>o</kbd> | **Rallumer** l'écran physique du téléphone. |
| <kbd>Alt</kbd> + <kbd>t</kbd> | **Afficher / masquer** la température dans la barre de titre. |
| <kbd>F10</kbd> | **Mode Navigation & Réseaux Sociaux** (TikTok, Reels, Stories au clavier). |
| <kbd>Échap</kbd> ou <kbd>Alt</kbd> + <kbd>Échap</kbd> | Bouton Retour Android (BACK). |
| <kbd>Alt</kbd> + <kbd>p</kbd> | Bouton Power physique du téléphone. |

---

## 4. Réglages avancés pour réduire l'échauffement au minimum

Si vous utilisez scrcpy toute la journée pour de la bureautique ou des messages, vous pouvez alléger considérablement le travail du processeur du téléphone :

### 4.1. Limiter la résolution et la fréquence d'images
```cmd
scrcpy -S -w -m 1280 --max-fps=60 -b 4M --temperature
```
- `-m 1280` : Réduit la taille maximale à 1280 pixels au lieu de la résolution native 2K/4K de votre téléphone (invisible sur un écran d'ordinateur, mais réduit la charge du GPU par deux).
- `--max-fps=60` : Bloque à 60 images par seconde (ou `--max-fps=30` pour de la simple messagerie).
- `-b 4M` : Débit vidéo de 4 Mbps suffisant pour une image nette sans saturer l'encodeur vidéo.

### 4.2. Verrouillage automatique à la fermeture
Ajoutez l'option `--power-off-on-close` :
```cmd
scrcpy -S -w --temperature --power-off-on-close
```
Dès que vous fermez la fenêtre scrcpy sur votre ordinateur, le smartphone se verrouille et passe immédiatement en veille.

---

## 5. Scripts Windows prêts à l'emploi (`.bat`)

Pour lancer ces configurations d'un simple double-clic :

### Script 1 : Écran éteint & Préservation batterie (`ecran_eteint.bat`)
Créez un fichier texte nommé `ecran_eteint.bat` dans votre dossier scrcpy contenant :

```bat
@echo off
title Scrcpy - Mode Economie Batterie
scrcpy -S -w -m 1280 --max-fps=60 -b 4M --temperature --power-off-on-close --auto-reconnect
pause
```

### Script 2 : Raccourci sans aucune fenêtre de terminal (`ecran_eteint_silencieux.vbs`)
Créez un fichier `ecran_eteint_silencieux.vbs` contenant :

```vbs
Set WshShell = CreateObject("WScript.Shell")
WshShell.Run "scrcpy.exe -S -w -m 1280 --max-fps=60 -b 4M --temperature --power-off-on-close --auto-reconnect", 0, False
```

Double-cliquez dessus pour ouvrir scrcpy instantanément et discrètement.

---

## 6. Conseils d'alimentation électrique

1. **Prise USB 2.0 recommandée pour la journée** :
   Brancher le téléphone sur un port USB 2.0 standard (fournissant 500 mA) maintient la charge stable sans déclencher la charge rapide haute température.
2. **Désactiver la charge rapide dans Android** :
   Dans *Paramètres* > *Batterie* > *Charge rapide*, désactiver l'option pendant les sessions de travail prolongées sur PC permet de maintenir la température sous les 35°C.
