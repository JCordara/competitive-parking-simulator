--[[
READ-ONLY ATTRIBUTES:
    _chassisDimensionsX
    _chassisDimensionsY
    _chassisDimensionsZ
    _wheelRadius
]]

-- Chassis
mass = 3000
CMoffsetX = 0
CMoffsetY = -_chassisDimensionsY * 0.5 + 0.75
CMoffsetZ = 0.25

-- Engine
speed = 650
torque = 3000
engineMOI = 0.2
dampingFullThrottle = 0.1
dampingClutchEngaged = 5.0
dampingClutchDisengaged = 0.35

-- Wheels
turningRadius = 0.38 -- MAX 0.5
wheelMass = 20
wheelMOI = 25
handbrakeTorque = 3000
wheelDamping = 5
toeAngle = 0
