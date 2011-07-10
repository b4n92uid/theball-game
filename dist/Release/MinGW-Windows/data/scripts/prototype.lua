--[[

    Name: Prototype
    Author: b4n92uid

    Description:
    Prototype script for in-game integration

--]]

-- Global variable for handling time party
gametime = 180

include("fragai.lua")

-- AI process proc
function onAi(player)
    fragAiProcess(player, 8.0, 8.0, 512)
end

function removeimmunity(player)
    setNumber(player, "immunity", 0)
    setOpacity(player, 1.0)
end

function onPlayerRespawn(player)

    spawn = getElementsRand("spawnpoint[0-9]?")
    setPosition(player, getPosition(spawn))

    attachWeapon(player, "Blaster")
    attachWeapon(player, "Shotgun")

    attachPower(player, "BulletTime")
    attachPower(player, "Boost")
    attachPower(player, "GravityGun")
    
    setNumber(player, "immunity", 1)
    setOpacity(player, 0.5)
    
    setTimeout("removeimmunity", 3000, player)

end

-- Hook for player shooting
function onPlayerShoot(player, weaponName)
    return true
end

-- Hook for player power-use
function onPlayerPower(player, powerName)
    return true
end

-- Hook whene a player get dammage
function onPlayerDammage(player, shoter)

    if getNumber(player, "immunity") == 1 then
        return false
    end
    
    setNumber(shoter, "score", getNumber(shoter, "score") + 10)
    return true
end

-- Hook whene a player get killed
function onPlayerKilled(player, killer)
    setNumber(killer, "score", getNumber(killer, "score") + 100)
    return true
end

-- Hook whene a player jump
function onPlayerJump(player)
end

-- Hook whene a player move
function onPlayerMove(player, diriction)
end

-- Callback for Jumper behaviour
function onJumper(player, elem)

    setVelocity(player, {x=0.0, y=0.0, z=0.0})
    impulse(player, {x=0.0, y=1.0, z=0.0}, 64)
    playSound("jumper", elem)

    display("JUMPER !!!")

end

-- Callback for Teleporter behaviour
function onTeleporter(player, elem)

    setPosition(player, randomPosition())
    playSound("teleporter", elem)

end

-- Callback called each seconde (setInterval)
function onEachSecond(userplayer)

    gametime = gametime - 1

  if gametime < 10 then
    playSound("notime", userplayer)
  end

end

-- Callback called each frame
function onEachFrame(userplayer)

    leader = nil

    for i, player in pairs(playerList()) do

        if leader == nil then
            leader = player
        end

        if not leader == player and getNumber(player, "score") > getNumber(leader, "score") then
            leader = player
        end
    end

    statustext = "Game time: " .. os.date("%M:%S", gametime) .. "\n"
                .. "Leader: " .. getNickName(leader) .. " = " .. getNumber(leader, "score") .. "\n"
        .. "Score: " .. getNumber(userplayer, "score")

    status(statustext)

  if gametime <= 0 then
        gameover(leader, statustext)
    end

end

function onPlayerOutOfArena(player)

  spawn = getElementsRand("spawnpoint[0-9]?")
  stopMotion(player)
  setPosition(player, getPosition(spawn))

  return true

end

-- Create one bot player
-- createPlayers(1)

fragAiInit()

-- Load map sounds ressource
loadSound("jumper", "data/sfxart/jumper.wav")
loadSound("teleporter", "data/sfxart/teleport.wav")
loadSound("notime", "data/sfxart/notime.wav")

-- Register callback for dynamic element
registerCollid("jumper", "onJumper")
registerCollid("teleporter", "onTeleporter")

-- Register callback for player event
registerPlayerHook("ai", "onAi")
registerPlayerHook("shoot", "onPlayerShoot")
registerPlayerHook("power", "onPlayerPower")
registerPlayerHook("dammage", "onPlayerDammage")
registerPlayerHook("killed", "onPlayerKilled")
registerPlayerHook("respawn", "onPlayerRespawn")

-- Register callback for global event
registerGlobalHook("frame", "onEachFrame")
registerGlobalHook("out", "onPlayerOutOfArena")

-- Register a callback called each second
setInterval("onEachSecond", 1000)
