loadSound(String:id, String:path)
    Charge un son spécifier par _path_ et identifier par _id_

playSound(String:id, Vector3:pos)
    Joue le son identifier par _id_ à la position _pos_

stopSound(String:id)

volSound(String:id, float:vol)

isPlaySound(String:id)

loadMusic(id, path)
    Charge la music spécifier par _path_ et identifier par _id_
    loadMusic est utilise pour charger des fichier audio long durée
    qui seront jouer en streaming

playMusic(id)
    Joue la music identifier par _id_

pauseMusic(id)
    Met en pause la music identifier par _id_

stopMusic(id)
    Stop la music identifier par _id_
