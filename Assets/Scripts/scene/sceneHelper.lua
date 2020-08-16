require("common.common")

SceneHelper = ClassDefinition(SceneHelper);
function SceneHelper.setEntityScale(entity, scale)
    if (entity == nil or entity == INVALID_ENTITY) then 
        return;
    end 

    local scene = Scene.GetMainScene();
    local transform = scene:GetTransform(entity);
    if (transform == nil) then 
        return;
    end 

    transform:Scale(scale);
end 

function SceneHelper.setEntityPosition(entity, pos)
    if (entity == nil or entity == INVALID_ENTITY) then 
        return;
    end 

    local scene = Scene.GetMainScene();
    local transform = scene:GetTransform(entity);
    if (transform == nil) then 
        return;
    end 

    transform:Translate(pos);
end 

function SceneHelper.setEntityRotation(entity, rotation)
    if (entity == nil or entity == INVALID_ENTITY) then 
        return;
    end 

    local scene = Scene.GetMainScene();
    local transform = scene:GetTransform(entity);
    if (transform == nil) then 
        return;
    end 

    transform:RotateRollPitchYaw(rotation);
end 

function SceneHelper.createDirectionLight(scene, energy, rotation)
    if scene == nil then return end

    local entity = scene:CreateEntity();
    local light = scene:CreateLight(entity);
    light:SetLightType(LightType_Directional);
    light:SetEnergy(energy);
    light:SetShadowBias(0.0005);

	local transform = scene:CreateTransform(entity);
    transform:RotateRollPitchYaw(rotation);

    return entity;
end 