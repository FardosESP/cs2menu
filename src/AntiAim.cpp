#include "AntiAim.h"
#include "Memory.h"
#include <cmath>
#include <random>

// Normalize angle to -180 to 180
static float NormalizeAngle(float angle)
{
    while (angle > 180.0f) angle -= 360.0f;
    while (angle < -180.0f) angle += 360.0f;
    return angle;
}

void AntiAim::Apply(C_CSPlayerPawn* localPlayer, Vector3& viewAngles)
{
    if (!enabled || !localPlayer)
        return;
    
    // Check if player is alive and on ground
    int health = localPlayer->GetHealth();
    if (health <= 0)
        return;
    
    bool onGround = localPlayer->IsOnGround();
    if (!onGround)
        return; // Don't apply AA in air
        
        // Apply pitch
        switch (pitchType)
        {
            case PitchType::Up:
                viewAngles.x = -89.0f;
                break;
            case PitchType::Down:
                viewAngles.x = 89.0f;
                break;
            case PitchType::Zero:
                viewAngles.x = 0.0f;
                break;
            case PitchType::Fake:
                viewAngles.x = 89.0f; // Send down, show up
                break;
            case PitchType::FakeUp:
                viewAngles.x = -89.0f;
                break;
            case PitchType::FakeDown:
                viewAngles.x = 89.0f;
                break;
            default:
                break;
        }
        
        // Apply yaw
        float baseYaw = viewAngles.y;
        
        switch (yawType)
        {
            case YawType::Backward:
                viewAngles.y = NormalizeAngle(baseYaw + 180.0f + yawOffset);
                break;
                
            case YawType::Spin:
            {
                currentYaw += spinSpeed;
                if (currentYaw > 360.0f) currentYaw -= 360.0f;
                viewAngles.y = NormalizeAngle(currentYaw + yawOffset);
                break;
            }
            
            case YawType::Jitter:
            {
                static bool jitterSwitch = false;
                jitterSwitch = !jitterSwitch;
                float jitter = jitterSwitch ? jitterRange : -jitterRange;
                viewAngles.y = NormalizeAngle(baseYaw + 180.0f + jitter + yawOffset);
                break;
            }
            
            case YawType::Sideways:
            {
                static bool sidewaysSwitch = false;
                sidewaysSwitch = !sidewaysSwitch;
                float side = sidewaysSwitch ? 90.0f : -90.0f;
                viewAngles.y = NormalizeAngle(baseYaw + side + yawOffset);
                break;
            }
            
            case YawType::Random:
            {
                static std::random_device rd;
                static std::mt19937 gen(rd());
                static std::uniform_real_distribution<float> dis(-180.0f, 180.0f);
                viewAngles.y = NormalizeAngle(dis(gen) + yawOffset);
                break;
            }
            
            case YawType::FakeBackward:
                viewAngles.y = NormalizeAngle(baseYaw + 180.0f + yawOffset);
                break;
                
            case YawType::FakeSpin:
            {
                currentYaw += spinSpeed * 2.0f;
                if (currentYaw > 360.0f) currentYaw -= 360.0f;
                viewAngles.y = NormalizeAngle(currentYaw + yawOffset);
                break;
            }
            
            default:
                break;
        }
        
        // Clamp angles
        viewAngles.x = std::clamp(viewAngles.x, -89.0f, 89.0f);
        viewAngles.y = NormalizeAngle(viewAngles.y);
        viewAngles.z = 0.0f; // Roll should always be 0
}

void AntiAim::FakeLag(int ticksToChoke)
{
    if (!fakeLagEnabled)
        return;
    
    // TODO: Implement packet choking
    // This requires hooking network functions
    // For now, just track the ticks
    chokedTicks = ticksToChoke;
}

void AntiAim::EdgeAA(C_CSPlayerPawn* localPlayer, Vector3& viewAngles)
{
    // TODO: Implement edge detection
    // Trace rays to find walls and adjust yaw accordingly
}

void AntiAim::Freestanding(C_CSPlayerPawn* localPlayer, Vector3& viewAngles)
{
    // TODO: Implement freestanding
    // Analyze enemy positions and adjust yaw to minimize exposure
}

void AntiAim::ManualAA(Vector3& viewAngles, int direction)
{
    float baseYaw = viewAngles.y;
    
    switch (direction)
    {
        case 0: // Back
            viewAngles.y = NormalizeAngle(baseYaw + 180.0f);
            break;
        case 1: // Left
            viewAngles.y = NormalizeAngle(baseYaw + 90.0f);
            break;
        case 2: // Right
            viewAngles.y = NormalizeAngle(baseYaw - 90.0f);
            break;
    }
}
