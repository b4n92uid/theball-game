rayCast(Vector3:start, Vector3:end)
    Effectue un lancé de rayon, tout element ce trouvent
    entre start et end sera renvoyer dans une table

isViewed(Element:elem1, Element:elem2)
    Renvois _true_ si le deuxieme élément est visible a partir du premier élément

randomPosition()
    Renvois une position aléatroire sur le sol de la carte

randomPosition(Vector3:pos, Vector3:radius)
    Renvois une position aléatroire sur le sol de la carte
    Le calcule est relative a pos (position) et radius (rayon)
    Si aucune position ne correspand nil et renvoyer

nearestPlayer(Player:player[, int:count = 1])
    Calcule le nombres de joueurs _count_
    les plus proches de _player_ dans une table

farestPlayer(Player:player[, int:count = 1])
    Calcule le nombres de joueurs _count_
    les plus eloigner de _player_ dans une table

diriction(elem1, elem2)

length(vec1, vec2)

normalize(vec)
