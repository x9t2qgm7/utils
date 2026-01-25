local api = loadstring(game:HttpGet("https://sdkapi-public.luarmor.net/library.lua"))()

  local PREMIUM_ID = "2c588f38b123e5e9947610738acbab2d"
  local FREEMIUM_ID = "ae6d8a7c30031f5a06d0c7693d4128a6"

  local key = getgenv().script_key or _G.script_key or script_key

  if not key then
      warn("No key!")
      return
  end

  local function checkTier(scriptId)
      api.script_id = scriptId
      local ok, result = pcall(function()
          return api.check_key(key)
      end)
      return ok and result and result.code == "KEY_VALID"
  end

  local tier = nil

  if checkTier(PREMIUM_ID) then
      tier = "premium"
  elseif checkTier(FREEMIUM_ID) then
      tier = "freemium"
  end

  if tier then
      print("Tier:", tier:upper())
      api.load_script()
  else
      warn("Key invalid!")
  end
