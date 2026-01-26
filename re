local GAME_CONFIGS = {
    --Fish It
    [121864768012064] = { -- placeid
        name = "Fish It",
        script_ids = {
            normal = "b8028e5b4e26e741997b55c57bfd0445",
            premium = "7b1e582ad639f70e181b14087aeb6995",
        }
    },
    [7671049560] = { -- gameid
        name = "The Forge",
        script_ids = {
            normal = "fbfe7c35f21a354962c52cddb739a596",
            premium = "887166f553b6353c0b5443eab34d5358", 
        }
    },
    [7381705] = { -- creatorId
        name = "Fisch",
        script_ids = {
            normal = "e78c6c903a5e2e743a5bebfd88f864c4",
            premium = "6a543b585b21918489b0b3d5fe53bdb4",
        }
    },
}

-- Config UI
local Config = {
    DiscordLink = "https://discord.gg/thanhub",
    LogoAssetId = "rbxassetid://85779221265543",
    Title = "Than-Hub Key System",
    Description = "Welcome! Enter your key below.",
    GetKeyButtons = {
        {Name = "Work.ink", Link = "https://ads.luarmor.net/get_key?for=ThanHub_Workink-UUxSglryLPaR", Icon = ""},
        {Name = "Discord Link", Link = "https://discord.gg/cjqFSvX27J", Icon = ""},
        {Name = "Linkvertise", Link = "https://ads.luarmor.net/get_key?for=ThanHub_Linkvertise-nlOCoAciNNee", Icon = ""},
    }
}

local currentPlaceId = game.PlaceId
local MarketplaceService = game:GetService("MarketplaceService")
local creatorId = MarketplaceService:GetProductInfo(game.PlaceId).Creator.CreatorTargetId
local currentGameId = game.GameId
local gameConfig = GAME_CONFIGS[currentGameId] or GAME_CONFIGS[currentPlaceId] or GAME_CONFIGS[creatorId]

if not gameConfig then
    game:GetService("StarterGui"):SetCore("SendNotification", {
        Title = "Unsupported Game";
        Text = "PlaceId: " .. currentPlaceId .. " not supported.";
        Icon = "rbxassetid://85779221265543";
        Duration = 10;
    })
    warn("Game not supported. PlaceId:", currentPlaceId)
    return
end

local success, api = pcall(function()
    return loadstring(game:HttpGet("https://sdkapi-public.luarmor.net/library.lua"))()
end)

if not success then
    warn("Failed to load Luarmor library:", api)
    return
end

local function detectVersion()
    local version = getgenv().HoN_Version or rawget(_G, "Version") or _G.Version or "normal"

    if type(version) ~= "string" then
        version = rawget(_G, "Version") or _G.Version or "normal"
    end

    version = tostring(version):lower()

    if version ~= "premium" and version ~= "lite" then
        version = "normal"
    end

    warn("DETECTED VERSION:", version:upper())

    return version
end

local function loadScript()
    local version = getgenv().HoN_Version or detectVersion()
    local scriptId = gameConfig.script_ids[version]

    if not scriptId then
        warn("No script_id found for version:", version)
        return false, "Invalid version"
    end

    warn("LOADING VERSION:", version:upper())

    api.script_id = scriptId

    local currentKey = getgenv().script_key or _G.script_key
    if currentKey then
        pcall(function() script_key = currentKey end)
        pcall(function() _G.script_key = currentKey end)
    end

    local ok, err = pcall(function()
        api.load_script()
    end)

    if ok then
        warn("✅ Script loaded successfully!")
        return true
    else
        warn("Failed to load script:", err)
        return false, err
    end
end

local function checkKey(key)
    local function tryVersion(ver)
        local scriptId = gameConfig.script_ids[ver]
        if not scriptId then return false end
        api.script_id = scriptId
        local ok, result = pcall(function()
            return api.check_key(key)
        end)
        return ok and result and result.code == "KEY_VALID"
    end

    if tryVersion("premium") then
        return true, nil, "premium"
    elseif tryVersion("normal") then
        return true, nil, "normal"
    end

    return false, {code = "KEY_INCORRECT"}, nil
end

local keyFileName = "ThanHubSaveKey.txt" -- valid key name
local oldKeyFileName = "ThanHubKeySave" .. currentPlaceId .. ".txt" -- func ga kepake blm dihapus

local function checkSavedKey()
    local savedKey = nil

    pcall(function()
        if script_key and script_key ~= "" then
            savedKey = script_key
        end
    end)

    if not savedKey then
        savedKey = rawget(_G, "script_key") or _G.script_key
    end

    if not savedKey then
        savedKey = getgenv().script_key
    end

    if not savedKey and isfile and readfile then
        local success, content = pcall(function()
            return readfile(keyFileName)
        end)
        if success and content and content ~= "" then
            savedKey = content:gsub("%s+", "")
        end
    end

    if not savedKey and isfile and readfile then
        local success, content = pcall(function()
            return readfile(oldKeyFileName)
        end)
        if success and content and content ~= "" then
            savedKey = content:gsub("%s+", "")

            if writefile then
                pcall(function()
                    writefile(keyFileName, savedKey)
                end)
            end

            if delfile then
                pcall(function()
                    delfile(oldKeyFileName)
                end)
            end
        end
    end

    if savedKey then
        savedKey = savedKey:gsub("%s+", "")
    end

    if savedKey and savedKey ~= "" then
        local valid, result, detectedVersion = checkKey(savedKey)

        if valid then
            local version = detectedVersion or "normal"

            pcall(function() script_key = savedKey end)
            pcall(function() _G.script_key = savedKey end)
            pcall(function() getgenv().script_key = savedKey end)
            getgenv().HoN_Version = version

            if isfile and writefile then
                pcall(function()
                    writefile(keyFileName, savedKey)
                end)
            end

            local versionText = version == "premium" and "PREMIUM" or "NORMAL"

            game:GetService("StarterGui"):SetCore("SendNotification", {
                Title = "Key Valid!";
                Text = gameConfig.name .. " | " .. versionText;
                Icon = "rbxassetid://85779221265543";
                Duration = 3;
            })

            loadScript()
            return true
        else
            if isfile and delfile then
                pcall(function()
                    if isfile(keyFileName) then
                        delfile(keyFileName)
                    end
                    if isfile(oldKeyFileName) then
                        delfile(oldKeyFileName)
                    end
                end)
            end
        end
    end

    return false
end

-- version (normal, lite, premium)
if rawget(_G, "script_key") and not getgenv().script_key then
    getgenv().script_key = rawget(_G, "script_key")
elseif _G.script_key and not getgenv().script_key then
    getgenv().script_key = _G.script_key
end

if rawget(_G, "Version") and type(rawget(_G, "Version")) == "string" and not getgenv().HoN_Version then
    getgenv().HoN_Version = rawget(_G, "Version")
elseif _G.Version and type(_G.Version) == "string" and not getgenv().HoN_Version then
    getgenv().HoN_Version = _G.Version
end

warn("Final _G.Version:", _G.Version or "nil")

if checkSavedKey() then
    return
end

local TweenService = game:GetService("TweenService")
local UserInputService = game:GetService("UserInputService")
local Players = game:GetService("Players")
local LocalPlayer = Players.LocalPlayer

local function GetGui()
    local newGui = Instance.new("ScreenGui")
    newGui.ScreenInsets = Enum.ScreenInsets.None
    newGui.ResetOnSpawn = false
    newGui.ZIndexBehavior = Enum.ZIndexBehavior.Sibling
    newGui.DisplayOrder = 2147483647
    newGui.Parent = game:GetService("CoreGui")
    return newGui
end

local function Tween(instance, tweeninfo, propertytable)
    return TweenService:Create(instance, tweeninfo, propertytable)
end

local KeySystemUI = GetGui()

local Base = Instance.new("Frame")
Base.Name = "Base"
Base.AnchorPoint = Vector2.new(0.5, 0.5)
Base.BackgroundColor3 = Color3.fromRGB(0, 0, 0)
Base.BackgroundTransparency = 0.1
Base.BorderSizePixel = 0
Base.Position = UDim2.fromScale(0.5, 0.5)
Base.Size = UDim2.fromOffset(420, 360)
Base.Parent = KeySystemUI

local BaseUICorner = Instance.new("UICorner")
BaseUICorner.CornerRadius = UDim.new(0, 10)
BaseUICorner.Parent = Base

local BaseUIStroke = Instance.new("UIStroke")
BaseUIStroke.ApplyStrokeMode = Enum.ApplyStrokeMode.Border
BaseUIStroke.Color = Color3.fromRGB(255, 255, 255)
BaseUIStroke.Transparency = 0.9
BaseUIStroke.Parent = Base

-- Topbar
local Topbar = Instance.new("Frame")
Topbar.Name = "Topbar"
Topbar.BackgroundColor3 = Color3.fromRGB(255, 255, 255)
Topbar.BackgroundTransparency = 1
Topbar.Size = UDim2.new(1, 0, 0, 50)
Topbar.Parent = Base

local TopbarDivider = Instance.new("Frame")
TopbarDivider.AnchorPoint = Vector2.new(0, 1)
TopbarDivider.BackgroundColor3 = Color3.fromRGB(255, 255, 255)
TopbarDivider.BackgroundTransparency = 0.9
TopbarDivider.BorderSizePixel = 0
TopbarDivider.Position = UDim2.fromScale(0, 1)
TopbarDivider.Size = UDim2.new(1, 0, 0, 1)
TopbarDivider.Parent = Topbar

local TopbarPadding = Instance.new("UIPadding")
TopbarPadding.PaddingLeft = UDim.new(0, 20)
TopbarPadding.PaddingRight = UDim.new(0, 20)
TopbarPadding.Parent = Topbar

-- Logo
local Logo = Instance.new("ImageLabel")
Logo.Name = "Logo"
Logo.Image = Config.LogoAssetId
Logo.BackgroundTransparency = 1
Logo.AnchorPoint = Vector2.new(0, 0.5)
Logo.Position = UDim2.fromScale(0, 0.5)
Logo.Size = UDim2.fromOffset(32, 32)
Logo.Parent = Topbar

local LogoCorner = Instance.new("UICorner")
LogoCorner.CornerRadius = UDim.new(0, 6)
LogoCorner.Parent = Logo

-- Title
local Title = Instance.new("TextLabel")
Title.Name = "Title"
Title.FontFace = Font.new("rbxassetid://12187365364", Enum.FontWeight.SemiBold)
Title.Text = Config.Title
Title.TextColor3 = Color3.fromRGB(255, 255, 255)
Title.TextSize = 16
Title.TextTransparency = 0.1
Title.TextXAlignment = Enum.TextXAlignment.Center
Title.BackgroundTransparency = 1
Title.AnchorPoint = Vector2.new(0, 0.5)
Title.Position = UDim2.fromScale(0.04, 0.5)
Title.Size = UDim2.new(1, -80, 0, 20)
Title.Parent = Topbar

local TitlePadding = Instance.new("UIPadding")
TitlePadding.PaddingLeft = UDim.new(0, 45)
TitlePadding.Parent = Title

-- Close Button
local CloseBtn = Instance.new("TextButton")
CloseBtn.Name = "CloseBtn"
CloseBtn.Text = "X"
CloseBtn.FontFace = Font.new("rbxasset://fonts/families/SourceSansPro.json", Enum.FontWeight.Bold)
CloseBtn.TextColor3 = Color3.fromRGB(255, 255, 255)
CloseBtn.TextScaled = true
CloseBtn.BackgroundTransparency = 1
CloseBtn.AnchorPoint = Vector2.new(1, 0.5)
CloseBtn.Position = UDim2.new(1, 0, 0.5, 0)
CloseBtn.Size = UDim2.fromOffset(15, 15)
CloseBtn.Parent = Topbar

-- Content
local Content = Instance.new("Frame")
Content.Name = "Content"
Content.BackgroundTransparency = 1
Content.Position = UDim2.fromOffset(0, 50)
Content.Size = UDim2.new(1, 0, 1, -50)
Content.Parent = Base

local ContentPadding = Instance.new("UIPadding")
ContentPadding.PaddingBottom = UDim.new(0, 25)
ContentPadding.PaddingLeft = UDim.new(0, 25)
ContentPadding.PaddingRight = UDim.new(0, 25)
ContentPadding.PaddingTop = UDim.new(0, 20)
ContentPadding.Parent = Content

local ContentList = Instance.new("UIListLayout")
ContentList.Padding = UDim.new(0, 10)
ContentList.SortOrder = Enum.SortOrder.LayoutOrder
ContentList.Parent = Content

-- Game Info Label
local GameInfo = Instance.new("TextLabel")
GameInfo.Name = "GameInfo"
GameInfo.FontFace = Font.new("rbxassetid://12187365364", Enum.FontWeight.Medium)
GameInfo.Text = "Game: " .. gameConfig.name
GameInfo.TextColor3 = Color3.fromRGB(168, 85, 247)
GameInfo.TextSize = 12
GameInfo.TextTransparency = 0.3
GameInfo.TextXAlignment = Enum.TextXAlignment.Center
GameInfo.BackgroundTransparency = 1
GameInfo.Size = UDim2.new(1, 0, 0, 16)
GameInfo.LayoutOrder = -1
GameInfo.Parent = Content

-- Description (Status)
local Description = Instance.new("TextLabel")
Description.Name = "Description"
Description.FontFace = Font.new("rbxassetid://12187365364")
Description.Text = Config.Description
Description.TextColor3 = Color3.fromRGB(255, 255, 255)
Description.TextSize = 12
Description.TextTransparency = 0.5
Description.TextXAlignment = Enum.TextXAlignment.Center
Description.TextWrapped = true
Description.BackgroundTransparency = 1
Description.Size = UDim2.new(1, 0, 0, 16)
Description.Parent = Content

-- Key Input
local KeyInput = Instance.new("TextBox")
KeyInput.Name = "KeyInput"
KeyInput.FontFace = Font.new("rbxassetid://12187365364")
KeyInput.PlaceholderText = "Paste your key here..."
KeyInput.PlaceholderColor3 = Color3.fromRGB(255, 255, 255)
KeyInput.Text = ""
KeyInput.TextColor3 = Color3.fromRGB(255, 255, 255)
KeyInput.TextSize = 13
KeyInput.TextTransparency = 0.1
KeyInput.BackgroundColor3 = Color3.fromRGB(255, 255, 255)
KeyInput.BackgroundTransparency = 0.95
KeyInput.BorderSizePixel = 0
KeyInput.Size = UDim2.new(1, 0, 0, 38)
KeyInput.ClearTextOnFocus = false
KeyInput.LayoutOrder = 1
KeyInput.Parent = Content

local KeyInputCorner = Instance.new("UICorner")
KeyInputCorner.CornerRadius = UDim.new(0, 6)
KeyInputCorner.Parent = KeyInput

local KeyInputStroke = Instance.new("UIStroke")
KeyInputStroke.ApplyStrokeMode = Enum.ApplyStrokeMode.Border
KeyInputStroke.Color = Color3.fromRGB(255, 255, 255)
KeyInputStroke.Transparency = 0.9
KeyInputStroke.Parent = KeyInput

local KeyInputPadding = Instance.new("UIPadding")
KeyInputPadding.PaddingLeft = UDim.new(0, 12)
KeyInputPadding.PaddingRight = UDim.new(0, 12)
KeyInputPadding.Parent = KeyInput

-- Submit Button
local SubmitBtn = Instance.new("TextButton")
SubmitBtn.Name = "SubmitBtn"
SubmitBtn.Text = ""
SubmitBtn.FontFace = Font.new("rbxassetid://12187365364", Enum.FontWeight.Medium)
SubmitBtn.RichText = true
SubmitBtn.TextColor3 = Color3.fromRGB(255, 255, 255)
SubmitBtn.TextSize = 13
SubmitBtn.TextTransparency = 0.1
SubmitBtn.BackgroundColor3 = Color3.fromRGB(255, 255, 255)
SubmitBtn.BackgroundTransparency = 0.95
SubmitBtn.BorderSizePixel = 0
SubmitBtn.Size = UDim2.new(1, 0, 0, 38)
SubmitBtn.AutoButtonColor = false
SubmitBtn.LayoutOrder = 2
SubmitBtn.Parent = Content

local SubmitBtnCorner = Instance.new("UICorner")
SubmitBtnCorner.CornerRadius = UDim.new(0, 6)
SubmitBtnCorner.Parent = SubmitBtn

local SubmitBtnStroke = Instance.new("UIStroke")
SubmitBtnStroke.ApplyStrokeMode = Enum.ApplyStrokeMode.Border
SubmitBtnStroke.Color = Color3.fromRGB(255, 255, 255)
SubmitBtnStroke.Transparency = 0.9
SubmitBtnStroke.Parent = SubmitBtn

local SubmitBtnText = Instance.new("TextLabel")
SubmitBtnText.Name = "SubmitBtnText"
SubmitBtnText.Text = "Verify Key"
SubmitBtnText.FontFace = Font.new("rbxassetid://12187365364", Enum.FontWeight.Medium)
SubmitBtnText.RichText = true
SubmitBtnText.TextColor3 = Color3.fromRGB(255, 255, 255)
SubmitBtnText.TextSize = 13
SubmitBtnText.BackgroundTransparency = 1
SubmitBtnText.Size = UDim2.new(1, 0, 1, 0)
SubmitBtnText.Parent = SubmitBtn

local SubmitBtnGradient = Instance.new("UIGradient")
SubmitBtnGradient.Color = ColorSequence.new{
    ColorSequenceKeypoint.new(0, Color3.fromRGB(168, 85, 247)),
    ColorSequenceKeypoint.new(1, Color3.fromRGB(124, 58, 237))
}
SubmitBtnGradient.Rotation = 45
SubmitBtnGradient.Parent = SubmitBtnText

SubmitBtn.MouseEnter:Connect(function()
    Tween(SubmitBtn, TweenInfo.new(0.2, Enum.EasingStyle.Sine), {BackgroundTransparency = 0.9}):Play()
end)
SubmitBtn.MouseLeave:Connect(function()
    Tween(SubmitBtn, TweenInfo.new(0.2, Enum.EasingStyle.Sine), {BackgroundTransparency = 0.95}):Play()
end)

-- Divider 1
local Divider1 = Instance.new("Frame")
Divider1.Name = "Divider1"
Divider1.BackgroundColor3 = Color3.fromRGB(255, 255, 255)
Divider1.BackgroundTransparency = 0.9
Divider1.BorderSizePixel = 0
Divider1.Size = UDim2.new(1, 0, 0, 1)
Divider1.LayoutOrder = 2.5
Divider1.Parent = Content

-- Discord Button
local DiscordBtn = Instance.new("TextButton")
DiscordBtn.Name = "DiscordBtn"
DiscordBtn.Text = ""
DiscordBtn.BackgroundTransparency = 1
DiscordBtn.BorderSizePixel = 0
DiscordBtn.Size = UDim2.new(1, 0, 0, 20)
DiscordBtn.AutoButtonColor = false
DiscordBtn.LayoutOrder = 3
DiscordBtn.Parent = Content

local DiscordBtnText = Instance.new("TextLabel")
DiscordBtnText.Name = "DiscordBtnText"
DiscordBtnText.Text = "Need support? Join our Discord"
DiscordBtnText.FontFace = Font.new("rbxassetid://12187365364")
DiscordBtnText.RichText = true
DiscordBtnText.TextColor3 = Color3.fromRGB(255, 255, 255)
DiscordBtnText.TextSize = 12
DiscordBtnText.TextTransparency = 0.5
DiscordBtnText.BackgroundTransparency = 1
DiscordBtnText.Size = UDim2.new(1, 0, 1, 0)
DiscordBtnText.Parent = DiscordBtn

local DiscordTextGradient = Instance.new("UIGradient")
DiscordTextGradient.Color = ColorSequence.new{
    ColorSequenceKeypoint.new(0, Color3.fromRGB(255, 255, 255)),
    ColorSequenceKeypoint.new(0.6, Color3.fromRGB(255, 255, 255)),
    ColorSequenceKeypoint.new(0.61, Color3.fromRGB(168, 85, 247)),
    ColorSequenceKeypoint.new(1, Color3.fromRGB(124, 58, 237))
}
DiscordTextGradient.Parent = DiscordBtnText

-- Divider 2 with Text
local Divider2Container = Instance.new("Frame")
Divider2Container.Name = "Divider2Container"
Divider2Container.BackgroundTransparency = 1
Divider2Container.Size = UDim2.new(1, 0, 0, 20)
Divider2Container.LayoutOrder = 3.5
Divider2Container.Parent = Content

local Divider2Left = Instance.new("Frame")
Divider2Left.Name = "Divider2Left"
Divider2Left.BackgroundColor3 = Color3.fromRGB(255, 255, 255)
Divider2Left.BackgroundTransparency = 0.9
Divider2Left.BorderSizePixel = 0
Divider2Left.AnchorPoint = Vector2.new(0, 0.5)
Divider2Left.Position = UDim2.fromScale(0, 0.5)
Divider2Left.Size = UDim2.new(0.35, -5, 0, 1)
Divider2Left.Parent = Divider2Container

local Divider2Text = Instance.new("TextLabel")
Divider2Text.Name = "Divider2Text"
Divider2Text.Text = "Get Key"
Divider2Text.FontFace = Font.new("rbxassetid://12187365364", Enum.FontWeight.Medium)
Divider2Text.TextColor3 = Color3.fromRGB(255, 255, 255)
Divider2Text.TextSize = 12
Divider2Text.TextTransparency = 0.5
Divider2Text.BackgroundTransparency = 1
Divider2Text.AnchorPoint = Vector2.new(0.5, 0.5)
Divider2Text.Position = UDim2.fromScale(0.5, 0.5)
Divider2Text.Size = UDim2.fromOffset(60, 20)
Divider2Text.Parent = Divider2Container

local Divider2Right = Instance.new("Frame")
Divider2Right.Name = "Divider2Right"
Divider2Right.BackgroundColor3 = Color3.fromRGB(255, 255, 255)
Divider2Right.BackgroundTransparency = 0.9
Divider2Right.BorderSizePixel = 0
Divider2Right.AnchorPoint = Vector2.new(1, 0.5)
Divider2Right.Position = UDim2.fromScale(1, 0.5)
Divider2Right.Size = UDim2.new(0.35, -5, 0, 1)
Divider2Right.Parent = Divider2Container

-- Get Key Buttons Container
local GetKeyContainer = Instance.new("Frame")
GetKeyContainer.Name = "GetKeyContainer"
GetKeyContainer.BackgroundTransparency = 1
GetKeyContainer.Size = UDim2.new(1, 0, 0, 38)
GetKeyContainer.LayoutOrder = 4
GetKeyContainer.Parent = Content

local GetKeyList = Instance.new("UIListLayout")
GetKeyList.Padding = UDim.new(0, 8)
GetKeyList.FillDirection = Enum.FillDirection.Horizontal
GetKeyList.SortOrder = Enum.SortOrder.LayoutOrder
GetKeyList.Parent = GetKeyContainer

-- Create Get Key Buttons
for i, buttonData in ipairs(Config.GetKeyButtons) do
    local GetKeyBtn = Instance.new("TextButton")
    GetKeyBtn.Name = "GetKeyBtn" .. i
    GetKeyBtn.Text = ""
    GetKeyBtn.FontFace = Font.new("rbxassetid://12187365364", Enum.FontWeight.Medium)
    GetKeyBtn.TextColor3 = Color3.fromRGB(255, 255, 255)
    GetKeyBtn.TextSize = 12
    GetKeyBtn.TextTransparency = 0.1
    GetKeyBtn.BackgroundColor3 = Color3.fromRGB(255, 255, 255)
    GetKeyBtn.BackgroundTransparency = 0.95
    GetKeyBtn.BorderSizePixel = 0
    GetKeyBtn.Size = UDim2.new(0.32, 0, 1, 0)
    GetKeyBtn.AutoButtonColor = false
    GetKeyBtn.Parent = GetKeyContainer

    local GetKeyBtnCorner = Instance.new("UICorner")
    GetKeyBtnCorner.CornerRadius = UDim.new(0, 6)
    GetKeyBtnCorner.Parent = GetKeyBtn

    local GetKeyBtnStroke = Instance.new("UIStroke")
    GetKeyBtnStroke.ApplyStrokeMode = Enum.ApplyStrokeMode.Border
    GetKeyBtnStroke.Color = Color3.fromRGB(255, 255, 255)
    GetKeyBtnStroke.Transparency = 0.9
    GetKeyBtnStroke.Parent = GetKeyBtn

    local GetKeyBtnText = Instance.new("TextLabel")
    GetKeyBtnText.Name = "GetKeyBtnText"
    GetKeyBtnText.Text = buttonData.Icon .. " " .. buttonData.Name
    GetKeyBtnText.FontFace = Font.new("rbxassetid://12187365364", Enum.FontWeight.Medium)
    GetKeyBtnText.RichText = true
    GetKeyBtnText.TextColor3 = Color3.fromRGB(255, 255, 255)
    GetKeyBtnText.TextSize = 12
    GetKeyBtnText.BackgroundTransparency = 1
    GetKeyBtnText.Size = UDim2.new(1, 0, 1, 0)
    GetKeyBtnText.Parent = GetKeyBtn

    local GetKeyBtnGradient = Instance.new("UIGradient")
    GetKeyBtnGradient.Color = ColorSequence.new{
        ColorSequenceKeypoint.new(0, Color3.fromRGB(168, 85, 247)),
        ColorSequenceKeypoint.new(1, Color3.fromRGB(124, 58, 237))
    }
    GetKeyBtnGradient.Rotation = 45
    GetKeyBtnGradient.Parent = GetKeyBtnText

    GetKeyBtn.MouseEnter:Connect(function()
        Tween(GetKeyBtn, TweenInfo.new(0.2, Enum.EasingStyle.Sine), {BackgroundTransparency = 0.9}):Play()
    end)
    GetKeyBtn.MouseLeave:Connect(function()
        Tween(GetKeyBtn, TweenInfo.new(0.2, Enum.EasingStyle.Sine), {BackgroundTransparency = 0.95}):Play()
    end)

    GetKeyBtn.MouseButton1Click:Connect(function()
        setclipboard(buttonData.Link)
        Description.Text = buttonData.Name .. " link copied!"
        Description.TextColor3 = Color3.fromRGB(100, 255, 100)
        task.wait(2)
        Description.Text = Config.Description
        Description.TextColor3 = Color3.fromRGB(255, 255, 255)
    end)
end

local function UpdateStatus(msg, color)
    Description.Text = msg
    Description.TextColor3 = color
end

local function ButtonHover(btn, defaultTrans, hoverTrans)
    local btnText = btn:FindFirstChild("DiscordBtnText")
    if btnText then
        btn.MouseEnter:Connect(function()
            Tween(btnText, TweenInfo.new(0.2, Enum.EasingStyle.Sine), {TextTransparency = hoverTrans}):Play()
        end)
        btn.MouseLeave:Connect(function()
            Tween(btnText, TweenInfo.new(0.2, Enum.EasingStyle.Sine), {TextTransparency = defaultTrans}):Play()
        end)
    end
end

local isVerifying = false -- debounce

local function VerifyKey()
    if isVerifying then
        return
    end

    local key = KeyInput.Text:gsub("%s+", "")
    if key == "" then
        UpdateStatus("Please enter a key!", Color3.fromRGB(255, 100, 100))
        task.wait(2)
        UpdateStatus(Config.Description, Color3.fromRGB(255, 255, 255))
        return
    end

    isVerifying = true
    UpdateStatus("Checking key...", Color3.fromRGB(255, 200, 100))
    SubmitBtnText.Text = "Checking..."

    task.spawn(function()
        local valid, result, detectedVersion = checkKey(key)

        if valid then
            local version = detectedVersion or "normal"
            UpdateStatus("Key valid! Loading " .. version:upper() .. "...", Color3.fromRGB(100, 255, 100))

            getgenv().script_key = key
            getgenv().HoN_Version = version

            -- Save key ke file
            if writefile then
                pcall(function()
                    writefile(keyFileName, key)
                end)
            end

            local versionText = version == "premium" and "PREMIUM" or "NORMAL"

            game:GetService("StarterGui"):SetCore("SendNotification", {
                Title = Config.Title;
                Text = gameConfig.name .. " | " .. versionText .. " loaded!";
                Icon = "rbxassetid://85779221265543";
                Duration = 5;
            })

            KeySystemUI:Destroy()
            loadScript()
        else
            local statusMessages = {
                KEY_HWID_LOCKED = "Key locked to different device",
                KEY_INCORRECT = "Invalid key",
                KEY_EXPIRED = "Key expired",
                KEY_BANNED = "Key banned",
                KEY_INVALID = "Invalid key format",
                SCRIPT_ID_INCORRECT = "Script ID error",
                SCRIPT_ID_INVALID = "Invalid script ID",
                INVALID_EXECUTOR = "Unsupported executor",
                INVALID_VERSION = "Version not supported"
            }

            local msg = statusMessages[result.code] or ("Error: " .. tostring(result.message))
            UpdateStatus(msg, Color3.fromRGB(255, 100, 100))
            SubmitBtnText.Text = "Verify Key"
            isVerifying = false -- Reset flag
            task.wait(2)
            UpdateStatus(Config.Description, Color3.fromRGB(255, 255, 255))
        end
    end)
end

ButtonHover(DiscordBtn, 0.5, 0.3)

CloseBtn.MouseButton1Click:Connect(function()
    KeySystemUI:Destroy()
end)

DiscordBtn.MouseButton1Click:Connect(function()
    setclipboard(Config.DiscordLink)
    UpdateStatus("Discord link copied!", Color3.fromRGB(100, 255, 100))
    task.wait(2)
    UpdateStatus(Config.Description, Color3.fromRGB(255, 255, 255))
end)

SubmitBtn.MouseButton1Click:Connect(function()
    VerifyKey()
end)

KeyInput.FocusLost:Connect(function(enterPressed)
    if enterPressed then
        VerifyKey()
    end
end)

local dragging, dragInput, dragStart, startPos

Topbar.InputBegan:Connect(function(input)
    if input.UserInputType == Enum.UserInputType.MouseButton1 then
        dragging = true
        dragStart = input.Position
        startPos = Base.Position

        input.Changed:Connect(function()
            if input.UserInputState == Enum.UserInputState.End then
                dragging = false
            end
        end)
    end
end)

Topbar.InputChanged:Connect(function(input)
    if input.UserInputType == Enum.UserInputType.MouseMovement then
        dragInput = input
    end
end)

UserInputService.InputChanged:Connect(function(input)
    if input == dragInput and dragging then
        local delta = input.Position - dragStart
        Base.Position = UDim2.new(startPos.X.Scale, startPos.X.Offset + delta.X, startPos.Y.Scale, startPos.Y.Offset + delta.Y)
    end
end)

game:GetService("StarterGui"):SetCore("SendNotification", {
    Title = Config.Title;
    Text = "Key system loaded for " .. gameConfig.name;
    Icon = "rbxassetid://85779221265543";
    Duration = 3;
})
