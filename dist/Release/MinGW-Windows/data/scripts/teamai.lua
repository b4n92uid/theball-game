--[[

    Name: FragAI
    Author: b4n92uid

    Description:
    Team mode AI behavior

	* player
		the bot player
		
	* pirimeter
		pirimeter radius to strike
		
	* mintoshot
		minimum distance to start shooting targer
		
	* movespeed
	    the bot player speed
	
--]]


function teamAiInit()

	-- Init score of all players
	for i, player in pairs(playerList()) do
		setElemDataN(player, "score", 0)
		setElemDataN(player, "team", i%2)
	end

end

function teamAiProcess(player, pirimeter, mintoshot, movespeed)
	
	playerPos = getPosition(player)
    
    target = getElemDataN(player, "target")
    strike = getElemDataV(player, "strike")

	-- Choose a the nearest target
    if target == nil or isKilledPlayer(target) then
        target = nearestPlayer(player)
		
		if target == nil then return end

		setElemDataN(player, "target", target)
        strike = nil
    end
	

    targetPos = getPosition(target)
	
	-- Check strike position
	strikeClock = getElemDataN(player, "strikeClock")

    if strikeClock == nil or os.clock() - strikeClock > 8 then
        strike = nil
        setElemDataN(player, "strikeClock", os.clock())
    end
	
    -- Set strike position
    if strike == nil then
        strike = randomPosition(targetPos, pirimeter)
		setElemDataV(player, "strike", strike)
    else
        distance = length(playerPos, strike)
		
		if distance < 1.0 then
            strike = randomPosition(targetPos, pirimeter)
            setElemDataV(player, "strike", strike)
        end
    end
	
	-- Move
	
	strike = diriction(playerPos, strike)
	strike = normalize(strike)

    strike.x = strike.x * movespeed
    strike.y = 0
    strike.z = strike.z * movespeed
    
    setForce(player, strike)

	-- Check if ammo is empty
    ammo = getAmmo(player)

    if ammo == 0 then
        switchWeapon(player)
    end

    -- Shoot by brust
    if length(playerPos, targetPos) < mintoshot then
        shoot(player, targetPos)
    end
	
end