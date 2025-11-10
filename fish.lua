
local a={}
a.__index=a

local b=game:GetService"ReplicatedStorage"
local c=require(b.Packages.Replion)
local d=require(b.Shared.ItemUtility)

local e
pcall(function()e=require(b.Shared.StringLibrary)end)

local f
pcall(function()f=require(b.Variants)end)

local g

local function mkSignal()
local h=Instance.new"BindableEvent"
return{
Fire=function(i,...)h:Fire(...)end,
Connect=function(i,j)return h.Event:Connect(j)end,
Destroy=function(i)h:Destroy()end
}
end

function a.new()
local h=setmetatable({},a)
h._data=nil

h._fishesByUUID={}

h._totalFish=0
h._totalFavorited=0
h._totalShiny=0
h._totalMutant=0


h._fishesByVariant={}

h._fishChanged=mkSignal()
h._favChanged=mkSignal()
h._readySig=mkSignal()
h._ready=false
h._conns={}

c.Client:AwaitReplion("Data",function(i)
h._data=i
h:_initialScan()
h:_subscribeEvents()
h._ready=true
h._readySig:Fire()
end)

return h
end

function a.getShared()
if not g then
g=a.new()
end
return g
end

function a._get(h,i)
local j,k=pcall(function()return h._data and h._data:Get(i)end)
return j and k or nil
end

local function IU(h,...)
local i=d and d[h]
if type(i)=="function"then
local j,k=pcall(i,d,...)
if j then return k end
end
return nil
end

function a._resolveName(h,i)
if not i then return"<?>"end
local j=IU("GetItemDataFromItemType","Fish",i)
if j and j.Data and j.Data.Name then return j.Data.Name end
local k=IU("GetItemData",i)
if k and k.Data and k.Data.Name then return k.Data.Name end
return tostring(i)
end

function a._resolveVariantName(h,i)
if not i then return nil end


local j=IU("GetVariantData",i)
if j and j.Data and j.Data.Name then
return j.Data.Name
end


if f then
for k,l in pairs(f)do
if l.Data and l.Data.Id==i then
return l.Data.Name or k
end
end
end

return tostring(i)
end

function a._fmtWeight(h,i)
if not i then return nil end
if e and e.AddWeight then
local j,k=pcall(function()return e:AddWeight(i)end)
if j and k then return k end
end
return tostring(i).."kg"
end

function a._isFavorited(h,i)
if not i then return false end
return i.Favorited==true
end

function a._isFish(h,i)
if not i then return false end
if i.Metadata and i.Metadata.Weight then return true end
local j=i.Id or i.id
local k=IU("GetItemData",j)
if k and k.Data and k.Data.Type=="Fish"then return true end
return false
end

function a._createFishData(h,i)
local j=i.Metadata or{}
local k=j.VariantId or j.Mutation

return{
entry=i,
id=i.Id or i.id,
uuid=i.UUID or i.Uuid or i.uuid,
metadata=j,
name=h:_resolveName(i.Id or i.id),
favorited=h:_isFavorited(i),
shiny=j.Shiny==true,
mutant=(k~=nil),
variantId=k,
variantName=h:_resolveVariantName(k)
}
end

function a._initialScan(h)
h._fishesByUUID={}
h._fishesByVariant={}
h._totalFish=0
h._totalFavorited=0
h._totalShiny=0
h._totalMutant=0

local i={"Items","Fish"}

for j,k in ipairs(i)do
local l=h:_get{"Inventory",k}
if type(l)=="table"then
for m,n in ipairs(l)do
if h:_isFish(n)then
local o=h:_createFishData(n)
local p=o.uuid

if p then
h._fishesByUUID[p]=o
h._totalFish+=1

if o.shiny then
h._totalShiny+=1
end

if o.mutant then
h._totalMutant+=1


if o.variantId then
if not h._fishesByVariant[o.variantId]then
h._fishesByVariant[o.variantId]={}
end
table.insert(h._fishesByVariant[o.variantId],o)
end
end

if o.favorited then
h._totalFavorited+=1
end
end
end
end
end
end
end

function a._addFish(h,i)
if not h:_isFish(i)then return end

local j=h:_createFishData(i)
local k=j.uuid

if not k or h._fishesByUUID[k]then return end

h._fishesByUUID[k]=j
h._totalFish+=1

if j.shiny then
h._totalShiny+=1
end

if j.mutant then
h._totalMutant+=1


if j.variantId then
if not h._fishesByVariant[j.variantId]then
h._fishesByVariant[j.variantId]={}
end
table.insert(h._fishesByVariant[j.variantId],j)
end
end

if j.favorited then
h._totalFavorited+=1
end
end

function a._removeFish(h,i)
local j=i.UUID or i.Uuid or i.uuid
if not j then return end

local k=h._fishesByUUID[j]
if not k then return end

h._totalFish-=1

if k.shiny then
h._totalShiny-=1
end

if k.mutant then
h._totalMutant-=1


if k.variantId and h._fishesByVariant[k.variantId]then
local l=h._fishesByVariant[k.variantId]
for m,n in ipairs(l)do
if n.uuid==j then
table.remove(l,m)
break
end
end

if#l==0 then
h._fishesByVariant[k.variantId]=nil
end
end
end

if k.favorited then
h._totalFavorited-=1
end

h._fishesByUUID[j]=nil
end

function a._updateFavorited(h,i,j)
local k=h._fishesByUUID[i]
if not k then return end

local l=k.favorited
if l==j then return end

k.favorited=j

if j then
h._totalFavorited+=1
else
h._totalFavorited-=1
end


h._favChanged:Fire(h._totalFavorited)
end

function a._subscribeEvents(h)
for i,j in ipairs(h._conns)do pcall(function()j:Disconnect()end)end
table.clear(h._conns)

local i={"Items","Fish"}


for j,k in ipairs(i)do
table.insert(h._conns,h._data:OnArrayInsert({"Inventory",k},function(l,m)
if h:_isFish(m)then
h:_addFish(m)
h._fishChanged:Fire(h._totalFish,h._totalShiny,h._totalMutant)
end
end))

table.insert(h._conns,h._data:OnArrayRemove({"Inventory",k},function(l,m)
if h:_isFish(m)then
h:_removeFish(m)
h._fishChanged:Fire(h._totalFish,h._totalShiny,h._totalMutant)
end
end))
end


for j,k in ipairs(i)do
table.insert(h._conns,h._data:OnChange({"Inventory",k},function(l,m)
if type(l)~="table"or type(m)~="table"then return end


local n={}
local o={}

for p,q in ipairs(l)do
local r=q.UUID or q.Uuid or q.uuid
if r then n[r]=q end
end

for p,q in ipairs(m)do
local r=q.UUID or q.Uuid or q.uuid
if r then o[r]=q end
end


for p,q in pairs(o)do
if not n[p]and h._fishesByUUID[p]then

h:_removeFish(q)
end
end


for p,q in pairs(n)do
if not o[p]and h:_isFish(q)then

h:_addFish(q)
end
end


for p,q in pairs(n)do
local r=o[p]
if r and h._fishesByUUID[p]then
local s=h:_isFavorited(q)
local t=h:_isFavorited(r)

if s~=t then
h:_updateFavorited(p,s)
end
end
end


if next(o)~=next(n)or#l~=#m then
h._fishChanged:Fire(h._totalFish,h._totalShiny,h._totalMutant)
end
end))
end
end

function a.onReady(h,i)
if h._ready then task.defer(i);return{Disconnect=function()end}end
return h._readySig:Connect(i)
end

function a.onFishChanged(h,i)
return h._fishChanged:Connect(i)
end

function a.onFavoritedChanged(h,i)
return h._favChanged:Connect(i)
end

function a.getAllFishes(h)
local i={}
for j,k in pairs(h._fishesByUUID)do
table.insert(i,k)
end
return i
end

function a.getFavoritedFishes(h)
local i={}
for j,k in pairs(h._fishesByUUID)do
if k.favorited then
table.insert(i,k)
end
end
return i
end

function a.getFishesByWeight(h,i,j)
local k={}
for l,m in pairs(h._fishesByUUID)do
local n=m.metadata.Weight
if n then
if(not i or n>=i)and(not j or n<=j)then
table.insert(k,m)
end
end
end
return k
end

function a.getShinyFishes(h)
local i={}
for j,k in pairs(h._fishesByUUID)do
if k.shiny then
table.insert(i,k)
end
end
return i
end

function a.getMutantFishes(h)
local i={}
for j,k in pairs(h._fishesByUUID)do
if k.mutant then
table.insert(i,k)
end
end
return i
end


function a.getFishesByVariant(h,i)
if not i then return{}end
return h._fishesByVariant[i]or{}
end


function a.getFishesByVariantName(h,i)
if not i then return{}end
local j=string.lower(i)
local k={}

for l,m in pairs(h._fishesByUUID)do
if m.variantName and string.lower(m.variantName)==j then
table.insert(k,m)
end
end

return k
end


function a.getAllVariants(h)
local i={}
for j,k in pairs(h._fishesByVariant)do
if#k>0 then
table.insert(i,{
id=j,
name=k[1].variantName or tostring(j),
count=#k
})
end
end
return i
end


function a.getVariantCount(h,i)
local j=h._fishesByVariant[i]
return j and#j or 0
end

function a.getTotals(h)
return h._totalFish,h._totalFavorited,h._totalShiny,h._totalMutant
end

function a.isFavoritedByUUID(h,i)
if not i then return false end
local j=h._fishesByUUID[i]
return j and j.favorited or false
end

function a.getFishByUUID(h,i)
return h._fishesByUUID[i]
end

function a.dumpFishes(h,i)
i=tonumber(i)or 200
print(("-- FISHES (%d total, %d favorited, %d shiny, %d mutant) --"):format(
h._totalFish,h._totalFavorited,h._totalShiny,h._totalMutant
))

local j=h:getAllFishes()
for k,l in ipairs(j)do
if k>i then
print(("... truncated at %d"):format(i))
break
end

local m=h:_fmtWeight(l.metadata.Weight)
local n=l.variantName or l.variantId or"-"
local o=l.shiny and"✦"or""
local p=l.favorited and"★"or""

print(k,l.name,l.uuid or"-",m or"-",n,o,p)
end
end

function a.dumpFavorited(h,i)
i=tonumber(i)or 200
local j=h:getFavoritedFishes()
print(("-- FAVORITED FISHES (%d) --"):format(#j))

for k,l in ipairs(j)do
if k>i then
print(("... truncated at %d"):format(i))
break
end

local m=h:_fmtWeight(l.metadata.Weight)
local n=l.variantName or l.variantId or"-"
local o=l.shiny and"✦"or""

print(k,l.name,l.uuid or"-",m or"-",n,o)
end
end


function a.dumpVariants(h)
local i=h:getAllVariants()
print(("-- VARIANTS IN INVENTORY (%d unique) --"):format(#i))


table.sort(i,function(j,k)return j.count>k.count end)

for j,k in ipairs(i)do
print(j,k.name,"x"..k.count,"[ID:"..tostring(k.id).."]")
end
end

function a.destroy(h)
for i,j in ipairs(h._conns)do pcall(function()j:Disconnect()end)end
table.clear(h._conns)
h._fishChanged:Destroy()
h._favChanged:Destroy()
h._readySig:Destroy()
if g==h then
g=nil
end
end

return a
