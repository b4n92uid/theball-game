--[[

    Name: map01
    Author: theBall Team !

    Description:
    ...

--]]

lastspawn = "spawn01"

function welcome()

	display("Welcome !!!")

end

function onStartGame(player)

    whiteFlash(1, 0.01)
	
	setTimeout("welcome", 2000)

end

function onPlayerRespawn(player)

    spawn = getElement(lastspawn)
    setPosition(player, getPosition(spawn))

    attachWeapon(player, "Blaster")
    attachWeapon(player, "Shotgun")

    attachPower(player, "BulletTime")
    attachPower(player, "Boost")
    attachPower(player, "GravityGun")

end

function onEachFrame(userplayer)

end

function onPlayerOutOfArena(player)

  stopMotion(player)

  spawn = getElement(lastspawn)
  setPosition(player, getPosition(spawn))

  return true

end

function onZone1Complete(player, elem)

    display("Zone 1 complete")
    status("Zone completed 1/3")

    lastspawn = "spawn02"

    whiteFlash(1, 0.05)

    spawn = getElement(lastspawn)
    setPosition(player, getPosition(spawn))

end

function onZone2Complete(player, elem)

    display("Zone 2 complete")
    status("Zone completed 2/3")
    
    lastspawn = "spawn03"

    whiteFlash(1, 0.05)

    spawn = getElement(lastspawn)
    setPosition(player, getPosition(spawn))

end

function onZone3Complete(player, elem)

    display("Zone 3 complete")
    status("Zone completed 3/3")
    
    lastspawn = "spawn03"

    whiteFlash(1, 0.05)

    gameover(player, "Tutorial Part 1/2 complete")

end

registerCollid("teleporter01", "onZone1Complete")
registerCollid("teleporter02", "onZone2Complete")
registerCollid("teleporter03", "onZone3Complete")

-- Register callback for player event
registerPlayerHook("respawn", "onPlayerRespawn")

-- Register callback for global event
registerGlobalHook("start", "onStartGame")
registerGlobalHook("frame", "onEachFrame")
registerGlobalHook("out", "onPlayerOutOfArena")

glowSettings(0.25, 0.9, 1)
glowEnable(true)
