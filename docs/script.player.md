
getNickName(Player:player)
    Renvois le nom du joueur

setHealth(Player:player, int:value)
    Spécifier la santé du joueur

upHealth(Player:player, int:value)
    Ajoute de la santé du joueur

getHealth(Player:player)
    Renvois de la santé au joueur

setEnergy(Player:player, value)
    Spécifier l'énergie du joueur

upEnergy(Player:player, value)
    Ajoute de l'energie au joueur

getEnergy(Player:player)
    Renvois l'energie du joueur

setAmmo(Player:player, int:value)
    Spécifier les munitions pour l'arme courante

upAmmo(Player:player, int:value)
    Ajoute des munitions pour l'arme courante

getAmmo(Player:player)
    Renvois les munition pour l'arme courante

attachPower(Player:player, String:name)
    _name_ : Bullettime, Boost, GravityGun

selectPower(Player:player, int:slot)

switchPower(Player:player, bool:next)

getSelectedPower(Player:player)

attachWeapon(Player:player, String:name)
    _name_ : Blaster, Shotgun, Finder, Fusion

selectWeapon(Player:player, int:slot)
    Séléctione une arme

getSelectedWeapon(Player:player)
    Renvois le slot de l'arme séléctioner

switchWeapon(Player:player, bool:next)
    Arme suivante

move(Player:player, Vector3:direction)
    Affecte une force physique sur le joueur ce qui permet de le déplacer

jump(Player:player)
    Fait sauter le joueur

shoot(Player:player, Vector3:target)
    Fait tirer le joueur sur le vecteur _target_

power(Player:player, Vector3:target)
    Utilise le povoire séléction sur le vecteur _target_

dammage(Player:player, int:value)
    Fait subbir des dommages au joueur

createPlayers(int:count)
    Crée plusieur _count_ joueur aléatoirement

createPlayer(String:name, String:models)
    Crée un joueur définie par nom et modèle

killPlayer(Player:player, bool:respawn)
    Tue un joueur
    _respawn_ spécifier si le joueur peut réaparaitre

killAllPlayers(Player:player)
    Tue tout les joueurs

isKilledPlayer(Player:player)
    Renvois _true_ si le joueur est mort

kickPlayer(Player:player)
    Supprime un joueur

kickAllPlayers(Player:player, boolean:kickuser)
