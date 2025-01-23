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

void moveCamera(Camera& cam)
{
    float speed = 50;
    if (Input::GetKey(GLFW_KEY_LEFT_SHIFT))
        speed *= 10;
    if (Input::GetKey(GLFW_KEY_TAB))
        speed /= 10;
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
    cam.m_transform.MoveForwardAbsoluteUp(movement * speed * .5f);
    // rotation
    Vector3 rotation;
    if (Input::GetKey(GLFW_KEY_RIGHT))
        rotation.y--;
    if (Input::GetKey(GLFW_KEY_LEFT))
        rotation.y++;
    if (Input::GetKey(GLFW_KEY_UP))
        rotation.x--;
    if (Input::GetKey(GLFW_KEY_DOWN))
        rotation.x++;
    cam.m_transform.Rotate(rotation * speed);
    //cam.m_transform.Rotate({ 0, 1, 0 });
}

int main()
{
    PerlinNoise::Init();
    //for (int i = 0; i < 32; i++)
    //{
    //    for (int j = 0; j < 32; j++)
    //    {
    //        std::cout << PerlinNoise::Noise(double(i), double(j)) << " ";
    //    }
    //    std::cout << std::endl;
    //}
    /*{
        Texture tex = TextureReader::ReadImageFromFile("res/textures/TextTextures.data");
        return 0;
    }*/
    /*{
        Matrix4 mat;

        mat.Rotate({180, 90, 270});

        std::cout << mat.values[0] << " " << mat.values[1] << " " << mat.values[2] << " " << mat.values[3] << "\n";
        std::cout << mat.values[4] << " " << mat.values[5] << " " << mat.values[6] << " " << mat.values[7] << "\n";
        std::cout << mat.values[8] << " " << mat.values[9] << " " << mat.values[10] << " " << mat.values[11] << "\n";
        std::cout << mat.values[12] << " " << mat.values[13] << " " << mat.values[14] << " " << mat.values[15] << std::endl;

        return 0;
    }*/
    Camera gameCamera;
    gameCamera.m_transform.Position.z = -10;
    GameWindow window = GameWindow();
    Input::SetWindow(window.Window());
    window.Init(&gameCamera);
    //World::ActiveWorld()->LoadChunk({ x, y, z });

    //for (int x = -1; x <= 3; x++)
    //{
    //    for (int y = -2; y <= 4; y++)
    //    {
    //        for (int z = -1; z <= 3; z++)
    //        {
    //            World::ActiveWorld()->LoadChunk({ x, y, z });
    //        }
    //    }
    //}
    //for (int x = -1; x <= 3; x++)
    //{
    //    for (int y = -2; y <= 4; y++)
    //    {
    //        for (int z = -1; z <= 3; z++)
    //        {
    //            if (x != -1 && x != 3 && y != -2 && y != 4 && z != -1 && z != 3)
    //                continue;
    //            ChunkData* a = World::ActiveWorld()->GetChunk({x, y, z});
    //            for (int x0 = 0; x0 < ChunkWidth; x0++)
    //                for (int y0 = 0; y0 < ChunkWidth; y0++)
    //                    for (int z0 = 0; z0 < ChunkWidth; z0++)
    //                        a->Blocks[xyzToChunkIndex(x0, y0, z0)] = BlockType::Air;
    //        }
    //    }
    //}

    World::ActiveWorld()->LoadChunkRegionWithMesh({ 0, -1, 0 }, { 3, 3, 3 });
    //World::ActiveWorld()->LoadChunkRegionWithMesh({ 0, -1, 0 }, { 10, 10, 10 });
    //World::ActiveWorld()->LoadChunkWithMeshNow({ 0, 0, 0 });
    //World::ActiveWorld()->LoadChunk({ 0, 0, 1 });
    //World::ActiveWorld()->GetChunk({ 0, 0, 1 })->Blocks[0] = BlockType::Air;
    //World::ActiveWorld()->LoadChunkWithMesh({ 0, 0, 0 });

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    
    //ChunkData chunkData({0, 0, 0});
    ////Time::Timer t;
    ////t.Start();
    //chunkData.CreateMesh();
    ////t.Stop();
    //ChunkData chunkData2({ 0, 1, 0 });
    //chunkData2.CreateMesh();

    /* Loop until the user closes the window */
    while (!window.ShouldClose())
    {
        //Time::Timer someTimer("Something");
        Time::UpdateDeltaTime();
        // update stuff
        moveCamera(gameCamera);
        //gameCamera.m_transform.Position.x+=.1f;
        
        // render stuff
        World::ActiveWorld()->UpdateQueuedMeshDataToGPU();
        window.m_renderer.Clear();
        window.m_renderer.PreRender();
        window.m_renderer.DrawWorld(*World::ActiveWorld());

        //gameCamera.ViewMatrix().Print();
        //window.m_renderer.DrawTriangle();
        //window.m_renderer.DrawChunk(chunkData);
        //window.m_renderer.DrawChunk(chunkData2);
        //std::string posText = "pos " + std::to_string(gameCamera.m_transform.Position.x) + " " + std::to_string(gameCamera.m_transform.Position.y) + " " + std::to_string(gameCamera.m_transform.Position.z);
        //window.m_renderer.DrawText(posText, { -1.0f, -.7f }, {1, 1, 1, 1}, 0.05f, .005f);
        //std::string fwdText = "fwd " + std::to_string(gameCamera.m_transform.Forward().x) + " " + std::to_string(gameCamera.m_transform.Forward().y) + " " + std::to_string(gameCamera.m_transform.Forward().z);
        //window.m_renderer.DrawText(fwdText, { -1.0f, -.8f }, { 1, 1, 1, 1 }, 0.05f, .005f);
        window.SwapBuffer();
        //someTimer.Stop();
        //std::cout << "DeltaTime: " << Time::DeltaTime << std::endl;
            
    }
    
    glfwTerminate();
    return 0;
}
