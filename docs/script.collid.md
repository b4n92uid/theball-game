
registerCollid(Element:elem1, Element:elem2, String:callback)
    Enregistre un _callback_ qui sera appeller lors de la collision
    entre _elem1_ et _elem2_

registerCollid(Element:elem1, String:pattern, String:callback)
    Enregistre un _callback_ qui sera appeller lors si _elem1_
    se retrouve les élément décrit par _pattern_

registerGlobalHook(String:type, String:callback)
    Enregistre un callback qui sera appeller lors du déroulement
    du type d'action spécifier en générale
    _type_ peut etre: frame, out

registerPlayerHook(String:type, String:callback)
    Enregistre un callback qui sera appeller lors du déroulement
    du type d'action spécifier sur le joueur

    _type_ peut etre:
        dammage(player, shoter) : bool
        killed(player, killer) : bool
        shoot(player, target) : bool
        power(player, target) : bool
        jump(player) : bool
        move(player, diriction) : bool
        respawn(player) : void
        ai(player) : void
