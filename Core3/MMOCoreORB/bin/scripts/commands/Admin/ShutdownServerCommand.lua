ShutdownServerCommand = {
    name = "shutdownserver",
    permissionLevel = 15, -- Admin permission level (adjust as needed)
    arguments = {
        { "delay", "integer", "The delay in minutes before shutdown." }
    }
}

function ShutdownServerCommand:execute(player, args)
    -- Check if the player is an admin
    if not player:hasPermission("admin") then
        player:sendSystemMessage("You do not have permission to use this command.")
        return
    end

    local delay = args.delay or 1 -- Default delay is 1 minute
    local delayInMilliseconds = delay * 60000 -- Convert minutes to milliseconds

    -- Broadcast the shutdown message to the server
    local message = "Server will shut down in " .. delay .. " minute(s)."
    ChatManager:getInstance():broadcastMessage(nil, message, nil)

    -- Schedule the shutdown
    createEvent(delayInMilliseconds, "ShutdownServerCommand", "shutdownServer", nil, "")
end

function ShutdownServerCommand:shutdownServer()
    -- Shut down the server
    local server = getServer()
    server:shutdown()
end

-- Register the command
registerCommand(ShutdownServerCommand.name, ShutdownServerCommand)