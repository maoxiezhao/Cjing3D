require("common.common")

FPSCamera = ClassDefine(FPSCamera);
function FPSCamera:ctor()
    self.m_is_mouse_move = false;
    self.m_last_mouse_pos = nil;
    self.m_rotate_x_delta = 0.0;
    self.m_rotate_y_delta = 0.0;

    self.m_move_delta = Vector:new(0, 0, 0);

	self.m_camera_transform = Transform:new();
    self.m_camera_inst = nil;

    self.m_move_speed = 10.0;
    self.m_rotation_speed = 0.0005;
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

function FPSCamera:notifyInput()
	local input_manager = global_context.m_input_manager;

    local x, y, z = 0, 0, 0;
	if input_manager:IsKeyHold(KeyCode.W) == true then 
		z = z + 1;
	end 
	if input_manager:IsKeyHold(KeyCode.A) == true then 
		x = x - 1; 
	end 
	if input_manager:IsKeyHold(KeyCode.S) == true then 
		z = z - 1;
	end 
	if input_manager:IsKeyHold(KeyCode.D) == true then 
        x = x + 1; 
    end 
    self.m_move_delta:SetXYZ(x, y, z);

    ---------------------------------------------------------------------------------------
    if self.m_is_mouse_move == false then 
        self.m_last_mouse_pos = input_manager:GetMousePos();
    end 

    local current_mouse = input_manager:GetMousePos();
    if input_manager:IsKeyHold(KeyCode.Click_Right) == true then 
        self.m_is_mouse_move = true;
        self.m_rotate_x_delta = current_mouse:GetX() - self.m_last_mouse_pos:GetX();
        self.m_rotate_y_delta = current_mouse:GetY() - self.m_last_mouse_pos:GetY();

        self.m_rotate_x_delta = self.m_rotate_x_delta * (1.0 / 60.0);
        self.m_rotate_y_delta = self.m_rotate_y_delta * (1.0 / 60.0);
    else
        self.m_is_mouse_move = false;
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

    self.m_rotate_x_delta = self.m_rotate_x_delta * self.m_rotation_speed;
    self.m_rotate_y_delta = self.m_rotate_y_delta * self.m_rotation_speed;

    local move_length = move_delta:Length();
    if move_length <= 0.00001 and 
       (math.abs(self.m_rotate_x_delta) + math.abs(self.m_rotate_y_delta)) <= 0 then 

        return;
    end 

    local move_vec = move_delta;

    local cam_rotation = Matrix:new();
    cam_rotation:RotationQuaternion(self.m_camera_transform:GetRotationLocal());
    move_vec = Vector.TransformNormal(move_vec, cam_rotation);

    self.m_camera_transform:Translate(move_vec);
    self.m_camera_transform:RotateRollPitchYaw(Vector:new(
        self.m_rotate_y_delta, self.m_rotate_x_delta, 0.0
    ));
    self.m_camera_transform:Update();
    self.m_camera_inst:Transform(self.m_camera_transform);
end 