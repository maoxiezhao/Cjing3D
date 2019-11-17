require("common.common")

FPSCamera = ClassDefine(FPSCamera);
function FPSCamera:ctor()
    self.m_is_mouse_move = false;
    self.m_rotate_x_delta = 0.0;
    self.m_rotate_y_delta = 0.0;
    self.m_move_delta = Vector:new(0, 0, 0);
    self.m_move_speed = 10.0;
	self.m_camera_transform = Transform:new();
    self.m_camera_inst = nil;
end 

function FPSCamera:reset(camera_pos)
    self.m_is_mouse_move = false;

    self.m_camera_transform:Clear();
    self.m_camera_transform:Translate(camera_pos);
    self.m_camera_transform:Update();
    
    self.m_camera_inst:Transform(self.m_camera_transform);
end 

function FPSCamera:setCamera(camera)
    self.m_camera_inst = camera;
end 

function FPSCamera:setMoveSpeed(speed)
    self.m_move_speed = speed;
end 

function FPSCamera:setMoveDelta(move_delta)
    self.m_move_delta = move_delta;
end 

function FPSCamera:notifyInput()
	local input_manager = global_context.m_input_manager;

    local move_delta = Vector:new(0, 0, 0); 
	if input_manager:IsKeyHold(KeyCode.W) == true then 
		move_delta:SetZ(move_delta:GetZ() + 1);
	end 
	if input_manager:IsKeyHold(KeyCode.A) == true then 
		move_delta:SetX(move_delta:GetX() - 1);
	end 
	if input_manager:IsKeyHold(KeyCode.S) == true then 
		move_delta:SetZ(move_delta:GetZ() - 1);
	end 
	if input_manager:IsKeyHold(KeyCode.D) == true then 
		move_delta:SetX(move_delta:GetX() + 1);
    end 
    
    self:setMoveDelta(move_delta);

    local orginMouse = nil;
    if self.m_is_mouse_move == false then 
    
    end 
end 

function FPSCamera:update(deltaTime)
    if self.m_camera_inst == nil then 
        return;
    end 

    self:notifyInput();

    local move_delta = self.m_move_delta;
    local speed = self.m_move_speed * deltaTime;
    move_delta = Vector.MultiplyWithNumber(move_delta, speed);

    local move_length = move_delta:Length();
    if move_length <= 0.00001 and 
       (math.abs(self.m_rotate_x_delta) + math.abs(self.m_rotate_y_delta)) <= 0 then 

        return;
    end 

    local move_vec = move_delta;

    self.m_camera_transform:Translate(move_vec);
    self.m_camera_transform:RotateRollPitchYaw(Vector:new(
        self.m_rotate_y_delta, self.m_rotate_x_delta, 0.0
    ));
    self.m_camera_transform:Update();
    self.m_camera_inst:Transform(self.m_camera_transform);
end 