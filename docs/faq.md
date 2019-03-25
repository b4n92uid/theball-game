# F.A.Q

## Quelle est la version minimal d’OpenGL pour lancer le jeu

Depuis la version 1.3, le jeu est compatible en minimum sur des implémentation d’OpenGL 1.4

## HardwareBuffer::HardwareBuffer; Buffer generation failed OpenGL error; énumérant non valide

Cette erreur est du au non-support des VBO’s (Vertex Buffer Object) sur votre carte graphique, les VBO’s sont une extension OpenGL qui permet d’optimiser le rendue, cette dernière est indispensable pour lancer le jeu, dans la plus part des cas une simple mise a jour des pilotes de votre carte graphique réglerai le problème.

## Quand je lance le jeu il me dit que la dll « libcc_s_dw2-1.dll » est manquante

Les dernier version du jeu ont normalement régler le probelemem, si tout fois ce dernier persiste, vous pourez télécharger la DLL en question ici http://www.down-dll.com/index.php?file-download=libgcc_s_dw2-1.dll, ensuit mettez la a coté de l’exécutable du jeu.

## Les effet après-traitement (P.P.Effects) ne sont pas activer ?

Les effet après-traitement requière le support des Shaders sur votre carte graphique, si le jeu ne les détecté pas les PPE sont automatiquement désactiver pour éviter un crash du programme.

## Le jeu ce quitte avec une erreur du au shader

Désactiver les effets après-traitement (P.P.Effects) dans le menu options sinon dans le fichier « video.xml »

    PPE Activer : <set name="useppe" value="1" />
    PPE Désactiver : <set name="useppe" value="0" />

