#pragma once
#include "Entity.h"
#include "../../Settings.h"
#include "../../Input.h"
#include "../World.h"

class Player : public Entity
{
private:
    float m_speed = 30;
    float m_SprintSpeed = m_speed * 3;
public:
	Player(Vector3 position) : Entity(position, false)
	{
	}
    void Move()
    {
        float speed = m_speed;
        if (Input::GetKey(GLFW_KEY_LEFT_SHIFT))
            speed = m_SprintSpeed;
        Vector3 movement;
        if (Input::GetKey(GLFW_KEY_W))
            movement.z++;
        if (Input::GetKey(GLFW_KEY_S))
            movement.z--;
        if (Input::GetKey(GLFW_KEY_D))
            movement.x++;
        if (Input::GetKey(GLFW_KEY_A))
            movement.x--;
        if (Input::GetKey(GLFW_KEY_SPACE))
            movement.y++;
        if (Input::GetKey(GLFW_KEY_LEFT_CONTROL))
            movement.y--;
        m_transform.MoveForwardAbsoluteUp(movement * speed);
        // rotation
        Vector3 rotation;
        Vector2 mouseMovement = Input::MousePositionDelta();
        rotation.y = -mouseMovement.x;
        rotation.x = mouseMovement.y;
        m_transform.Rotate(rotation * Settings::m_rotationSpeed);
    }
    // m1 and m2
    void Interact()
    {
        if (Input::GetMousePressed(GLFW_MOUSE_BUTTON_1))
        {
            RaycastHit rayHit = World::ActiveWorld()->Raycast(World::ActiveWorld()->GetCamera().m_transform.Position, World::ActiveWorld()->GetCamera().m_transform.Forward(), 100);
            if (rayHit.hit)
            {
                World::ActiveWorld()->RemoveBlock(rayHit.pos);
            }
        }
        if (Input::GetMousePressed(GLFW_MOUSE_BUTTON_2))
        {
            RaycastHit rayHit = World::ActiveWorld()->Raycast(World::ActiveWorld()->GetCamera().m_transform.Position, World::ActiveWorld()->GetCamera().m_transform.Forward(), 100);
            if (rayHit.hit)
            {
                World::ActiveWorld()->AddBlock(rayHit.NextToPosition(), BlockType::Sand);
            }
        }
    }
    void SetCameraPosition()
    {
        Camera& cam = World::ActiveWorld()->GetCamera();
        cam.m_transform.Position.x = m_transform.Position.x;
        cam.m_transform.Position.y = m_transform.Position.y;
        cam.m_transform.Position.z = m_transform.Position.z;
        cam.m_transform.SetRotation(m_transform.Rotation());
    }
	void Update()
	{
        Interact();
        Move();
        SetCameraPosition();
	}
};