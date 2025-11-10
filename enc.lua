
local a={}
a.__index=a

local b=game:GetService"ReplicatedStorage"
local c=require(b.Packages.Replion)
local d=require(b.Shared.ItemUtility)

local e

local function mkSignal()
local f=Instance.new"BindableEvent"
return{
Fire=function(g,...)f:Fire(...)end,
Connect=function(g,h)return f.Event:Connect(h)end,
Destroy=function(g)f:Destroy()end
}
end

function a.new()
local f=setmetatable({},a)
f._data=nil

f._stonesByUUID={}

f._totalStones=0
f._totalFavorited=0

f._stoneChanged=mkSignal()
f._favChanged=mkSignal()
f._readySig=mkSignal()
f._ready=false
f._conns={}

c.Client:AwaitReplion("Data",function(g)
f._data=g
f:_initialScan()
f:_subscribeEvents()
f._ready=true
f._readySig:Fire()
end)

return f
end

function a.getShared()
if not e then
e=a.new()
end
return e
end

function a._get(f,g)
local h,i=pcall(function()return f._data and f._data:Get(g)end)
return h and i or nil
end

local function IU(f,...)
local g=d and d[f]
if type(g)=="function"then
local h,i=pcall(g,d,...)
if h then return i end
end
return nil
end

function a._resolveName(f,g)
if not g then return"<?>"end
local h=IU("GetItemDataFromItemType","Enchant Stones",g)
if h and h.Data and h.Data.Name then return h.Data.Name end
local i=IU("GetItemData",g)
if i and i.Data and i.Data.Name then return i.Data.Name end
return tostring(g)
end

function a._isFavorited(f,g)
if not g then return false end
return g.Favorited==true
end

function a._isEnchantStone(f,g)
if not g then return false end
local h=g.Id or g.id
local i=IU("GetItemDataFromItemType","Enchant Stones",h)
if i and i.Data then
local j=tostring(i.Data.Type or"")
if j:lower():find"enchant"and j:lower():find"stone"then
return true
end
end
local j=IU("GetItemData",h)
if j and j.Data then
local k=tostring(j.Data.Type or"")
if k:lower():find"enchant"and k:lower():find"stone"then
return true
end
end
return false
end

function a._createStoneData(f,g)
local h=g.Metadata or{}
return{
entry=g,
id=g.Id or g.id,
uuid=g.UUID or g.Uuid or g.uuid,
metadata=h,
name=f:_resolveName(g.Id or g.id),
favorited=f:_isFavorited(g),
amount=g.Amount or 1
}
end

function a._initialScan(f)
f._stonesByUUID={}
f._totalStones=0
f._totalFavorited=0

local g=f:_get{"Inventory","Items"}
if type(g)=="table"then
for h,i in ipairs(g)do
if f:_isEnchantStone(i)then
local j=f:_createStoneData(i)
local k=j.uuid

if k then
f._stonesByUUID[k]=j
f._totalStones+=1

if j.favorited then
f._totalFavorited+=1
end
end
end
end
end
end

function a._addStone(f,g)
if not f:_isEnchantStone(g)then return end

local h=f:_createStoneData(g)
local i=h.uuid

if not i or f._stonesByUUID[i]then return end

f._stonesByUUID[i]=h
f._totalStones+=1

if h.favorited then
f._totalFavorited+=1
end
end

function a._removeStone(f,g)
local h=g.UUID or g.Uuid or g.uuid
if not h then return end

local i=f._stonesByUUID[h]
if not i then return end

f._totalStones-=1

if i.favorited then
f._totalFavorited-=1
end

f._stonesByUUID[h]=nil
end

function a._updateFavorited(f,g,h)
local i=f._stonesByUUID[g]
if not i then return end

local j=i.favorited
if j==h then return end

i.favorited=h

if h then
f._totalFavorited+=1
else
f._totalFavorited-=1
end

f._favChanged:Fire(f._totalFavorited)
end

function a._subscribeEvents(f)
for g,h in ipairs(f._conns)do pcall(function()h:Disconnect()end)end
table.clear(f._conns)

table.insert(f._conns,f._data:OnArrayInsert({"Inventory","Items"},function(g,h)
if f:_isEnchantStone(h)then
f:_addStone(h)
f._stoneChanged:Fire(f._totalStones)
end
end))

table.insert(f._conns,f._data:OnArrayRemove({"Inventory","Items"},function(g,h)
if f:_isEnchantStone(h)then
f:_removeStone(h)
f._stoneChanged:Fire(f._totalStones)
end
end))

table.insert(f._conns,f._data:OnChange({"Inventory","Items"},function(g,h)
if type(g)~="table"or type(h)~="table"then return end

local i={}
local j={}

for k,l in ipairs(g)do
local m=l.UUID or l.Uuid or l.uuid
if m then i[m]=l end
end

for k,l in ipairs(h)do
local m=l.UUID or l.Uuid or l.uuid
if m then j[m]=l end
end

for k,l in pairs(j)do
if not i[k]and f._stonesByUUID[k]then
f:_removeStone(l)
end
end

for k,l in pairs(i)do
if not j[k]and f:_isEnchantStone(l)then
f:_addStone(l)
end
end

for k,l in pairs(i)do
local m=j[k]
if m and f._stonesByUUID[k]then
local n=f:_isFavorited(l)
local o=f:_isFavorited(m)

if n~=o then
f:_updateFavorited(k,n)
end
end
end

if next(j)~=next(i)or#g~=#h then
f._stoneChanged:Fire(f._totalStones)
end
end))
end

function a.onReady(f,g)
if f._ready then task.defer(g);return{Disconnect=function()end}end
return f._readySig:Connect(g)
end

function a.onStoneChanged(f,g)
return f._stoneChanged:Connect(g)
end

function a.onFavoritedChanged(f,g)
return f._favChanged:Connect(g)
end

function a.getAllStones(f)
local g={}
for h,i in pairs(f._stonesByUUID)do
table.insert(g,i)
end
return g
end

function a.getFavoritedStones(f)
local g={}
for h,i in pairs(f._stonesByUUID)do
if i.favorited then
table.insert(g,i)
end
end
return g
end

function a.getStonesByName(f,g)
local h={}
for i,j in pairs(f._stonesByUUID)do
if j.name:lower():find(g:lower())then
table.insert(h,j)
end
end
return h
end

function a.getStoneByName(f,g)
for h,i in pairs(f._stonesByUUID)do
if i.name:lower()==g:lower()then
return i
end
end
return nil
end

function a.hasStone(f,g)
return f:getStoneByName(g)~=nil
end

function a.getTotals(f)
return f._totalStones,f._totalFavorited
end

function a.isFavoritedByUUID(f,g)
if not g then return false end
local h=f._stonesByUUID[g]
return h and h.favorited or false
end

function a.getStoneByUUID(f,g)
return f._stonesByUUID[g]
end

function a.dumpStones(f,g)
g=tonumber(g)or 200
print(("-- ENCHANT STONES (%d total, %d favorited) --"):format(
f._totalStones,f._totalFavorited
))

local h=f:getAllStones()
for i,j in ipairs(h)do
if i>g then
print(("... truncated at %d"):format(g))
break
end

local k=j.favorited and"★"or""
local l=j.amount>1 and("x"..j.amount)or""

print(i,j.name,j.uuid or"-",l,k)
end
end

function a.dumpFavorited(f,g)
g=tonumber(g)or 200
local h=f:getFavoritedStones()
print(("-- FAVORITED STONES (%d) --"):format(#h))

for i,j in ipairs(h)do
if i>g then
print(("... truncated at %d"):format(g))
break
end

local k=j.amount>1 and("x"..j.amount)or""

print(i,j.name,j.uuid or"-",k)
end
end

function a.destroy(f)
for g,h in ipairs(f._conns)do pcall(function()h:Disconnect()end)end
table.clear(f._conns)
f._stoneChanged:Destroy()
f._favChanged:Destroy()
f._readySig:Destroy()
if e==f then
e=nil
end
end

return a
