--[[

    Name: FragAI
    Author: b4n92uid

    Description:
    Frag mode AI behavior

	* player
		the bot player
		
	* pirimeter
		pirimeter radius to strike
		
	* mintoshot
		minimum distance to start shooting targer
	
--]]


function fragAiInit()

	-- Init score of all players
	for i, player in pairs(playerList()) do
		setNumber(player, "score", 0)
	end

end

function fragAiProcess(player, pirimeter, mintoshot)
	
	playerPos = getPosition(player)
    
    target = getNumber(player, "target")
    strike = getVector(player, "strike")

	-- Choose a the nearest target
    if target == nil or isKilledPlayer(target) then
        target = nearestPlayer(player)
		
		if target == nil then return end
        
        if getNumber(target, "immunity") == 1 then return end

		setNumber(player, "target", target)
        strike = nil
    end
	

    targetPos = getPosition(target)
	
	-- Check strike position
	strikeClock = getNumber(player, "strikeClock")

    if strikeClock == nil or os.clock() - strikeClock > 8 then
        strike = nil
        setNumber(player, "strikeClock", os.clock())
    end
	
    -- Set strike position
    if strike == nil then
        strike = randomPosition(targetPos, pirimeter)
		setVector(player, "strike", strike)
    else
        distance = length(playerPos, strike)
		
		if distance < 1.0 then
            strike = randomPosition(targetPos, pirimeter)
            setVector(player, "strike", strike)
        end
    end
	
	-- Move
	
	strike = diriction(playerPos, strike)
	strike = normalize(strike)

    strike.x = strike.x
    strike.y = 0
    strike.z = strike.z
    
    move(player, strike)

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