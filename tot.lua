
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

f._totemsByUUID={}

f._totalTotems=0
f._totalFavorited=0

f._totemChanged=mkSignal()
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
local h=IU("GetItemDataFromItemType","Totems",g)
if h and h.Data and h.Data.Name then return h.Data.Name end
local i=IU("GetTotemsData",g)
if i and i.Data and i.Data.Name then return i.Data.Name end
return tostring(g)
end

function a._isFavorited(f,g)
if not g then return false end
return g.Favorited==true
end

function a._isTotem(f,g)
if not g then return false end
local h=g.Id or g.id
local i=IU("GetItemDataFromItemType","Totems",h)
if i and i.Data then
local j=tostring(i.Data.Type or"")
if j:lower():find"totem"then
return true
end
end
local j=IU("GetTotemsData",h)
if j and j.Data then
local k=tostring(j.Data.Type or"")
if k:lower():find"totem"then
return true
end
end
return false
end

function a._createTotemData(f,g)
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
f._totemsByUUID={}
f._totalTotems=0
f._totalFavorited=0

local g=f:_get{"Inventory","Totems"}
if type(g)=="table"then
for h,i in ipairs(g)do
local j=f:_createTotemData(i)
local k=j.uuid

if k then
f._totemsByUUID[k]=j
f._totalTotems+=1

if j.favorited then
f._totalFavorited+=1
end
end
end
end
end

function a._addTotem(f,g)
if not f:_isTotem(g)then return end

local h=f:_createTotemData(g)
local i=h.uuid

if not i or f._totemsByUUID[i]then return end

f._totemsByUUID[i]=h
f._totalTotems+=1

if h.favorited then
f._totalFavorited+=1
end
end

function a._removeTotem(f,g)
local h=g.UUID or g.Uuid or g.uuid
if not h then return end

local i=f._totemsByUUID[h]
if not i then return end

f._totalTotems-=1

if i.favorited then
f._totalFavorited-=1
end

f._totemsByUUID[h]=nil
end

function a._updateFavorited(f,g,h)
local i=f._totemsByUUID[g]
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
if f:_isTotem(h)then
f:_addTotem(h)
f._totemChanged:Fire(f._totalTotems)
end
end))

table.insert(f._conns,f._data:OnArrayRemove({"Inventory","Items"},function(g,h)
if f:_isTotem(h)then
f:_removeTotem(h)
f._totemChanged:Fire(f._totalTotems)
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
if not i[k]and f._totemsByUUID[k]then
f:_removeTotem(l)
end
end

for k,l in pairs(i)do
if not j[k]and f:_isTotem(l)then
f:_addTotem(l)
end
end

for k,l in pairs(i)do
local m=j[k]
if m and f._totemsByUUID[k]then
local n=f:_isFavorited(l)
local o=f:_isFavorited(m)

if n~=o then
f:_updateFavorited(k,n)
end
end
end

if next(j)~=next(i)or#g~=#h then
f._totemChanged:Fire(f._totalTotems)
end
end))
end

function a.onReady(f,g)
if f._ready then task.defer(g);return{Disconnect=function()end}end
return f._readySig:Connect(g)
end

function a.onTotemChanged(f,g)
return f._totemChanged:Connect(g)
end

function a.onFavoritedChanged(f,g)
return f._favChanged:Connect(g)
end

function a.getAllTotems(f)
local g={}
for h,i in pairs(f._totemsByUUID)do
table.insert(g,i)
end
return g
end

function a.getFavoritedTotems(f)
local g={}
for h,i in pairs(f._totemsByUUID)do
if i.favorited then
table.insert(g,i)
end
end
return g
end

function a.getTotemsByName(f,g)
local h={}
for i,j in pairs(f._totemsByUUID)do
if j.name:lower():find(g:lower())then
table.insert(h,j)
end
end
return h
end

function a.getTotemByName(f,g)
for h,i in pairs(f._totemsByUUID)do
if i.name:lower()==g:lower()then
return i
end
end
return nil
end

function a.hasTotem(f,g)
return f:getTotemByName(g)~=nil
end

function a.getTotals(f)
return f._totalTotems,f._totalFavorited
end

function a.isFavoritedByUUID(f,g)
if not g then return false end
local h=f._totemsByUUID[g]
return h and h.favorited or false
end

function a.getTotemByUUID(f,g)
return f._totemsByUUID[g]
end

function a.dumpTotems(f,g)
g=tonumber(g)or 200
print(("-- TOTEMS (%d total, %d favorited) --"):format(
f._totalTotems,f._totalFavorited
))

local h=f:getAllTotems()
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
local h=f:getFavoritedTotems()
print(("-- FAVORITED TOTEMS (%d) --"):format(#h))

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
f._totemChanged:Destroy()
f._favChanged:Destroy()
f._readySig:Destroy()
if e==f then
e=nil
end
end

return a
