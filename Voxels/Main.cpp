#include <iostream>
#include "Graphics/GameWindow.h"
#include "ChunkData.h"
#include "Graphics/Renderer.h"
#include "Camera.h"
#include "Vector3.h"
#include "Input.h"
#include "Graphics/TextureReader.h"
#include <string>
#include "Time.h"
#include "Game/WorldGeneration/PerlinNoise.h"
#include "Game/World.h"
#include "Game/Entities/Player.h"

int main()
{
    PerlinNoise::Init();
    //gameCamera.m_transform.Position.z = -10;
    //gameCamera.m_transform.Position.y = 50;
    GameWindow window = GameWindow();
    Input::SetWindow(&window);
    window.Init(&World::ActiveWorld()->GetCamera());
    World::ActiveWorld()->AddEntity(new Player({0, 0, 0}));

    World::ActiveWorld()->LoadChunkRegionWithMesh({ 0, -1, 0 }, { 1, 2, 1 });
    //World::ActiveWorld()->LoadChunkRegionWithMesh({ 0, -1, 0 }, { 10, 10, 10 });
    //World::ActiveWorld()->LoadChunkWithMeshNow({ 0, 0, 0 });
    //World::ActiveWorld()->LoadChunk({ 0, 0, 1 });
    //World::ActiveWorld()->GetChunk({ 0, 0, 1 })->Blocks[0] = BlockType::Air;
    //World::ActiveWorld()->LoadChunkWithMesh({ 0, 0, 0 });

    /* Loop until the user closes the window */
    while (!window.ShouldClose())
    {
        Time::UpdateDeltaTime();

        if (Input::GetKeyPressed(GLFW_KEY_ESCAPE))
            Input::ToggleLockCurser();
        //Time::Timer someTimer("Something");
        // update stuff
        World::ActiveWorld()->UpdateEntities();

        // render stuff
        World::ActiveWorld()->UpdateQueuedMeshDataToGPU();
        window.m_renderer.Clear();
        window.m_renderer.PreRender();
        window.m_renderer.DrawWorld(*World::ActiveWorld());

        //std::string posText = "pos " + std::to_string(gameCamera.m_transform.Position.x) + " " + std::to_string(gameCamera.m_transform.Position.y) + " " + std::to_string(gameCamera.m_transform.Position.z);
        //window.m_renderer.DrawText(posText, { -1.0f, -.7f }, {1, 1, 1, 1}, 0.05f, .005f);
        //std::string fwdText = "fwd " + std::to_string(gameCamera.m_transform.Forward().x) + " " + std::to_string(gameCamera.m_transform.Forward().y) + " " + std::to_string(gameCamera.m_transform.Forward().z);
        //window.m_renderer.DrawText(fwdText, { -1.0f, -.8f }, { 1, 1, 1, 1 }, 0.05f, .005f);
        Input::Reset();
        window.SwapBuffer();
        //someTimer.Stop();
        //std::cout << "DeltaTime: " << Time::DeltaTime << std::endl;
    }
    glfwTerminate();
    return 0;
}
