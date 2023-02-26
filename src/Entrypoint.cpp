// todo: move camera logic to class / cleanup
// todo: renderer / mesh abstraction and cleanup
// todo: quaternion rotation port?
// todo: packaging and resource finding better (so we can actually run the built executable outside of vs)
// todo: cmake building + true cross platform tests

#include <imgui.h>

#include "StringId.h"
#include "Math.h"
#include "Color.h"
#include "Shader.h"
#include "Texture.h"
#include "ImGuiHelper.h"
#include "Display.h"
#include "Debug.h"
#include "Input.h"
#include "Time.h"
#include "TransformComponent.h"
#include "Camera.h"
#include "Mesh.h"
#include "Primatives.h"

using namespace glm;

static Display display;
static ImGuiHelper imGuiHelper;
static Input input;
static Time time;

static float s_mouseSensitivity{ 0.05f };
static bool s_isMouseHidden{ true };
static bool s_isFullscreen{ false };
static Color s_clearColor{ ColorRgbDecimal(20, 20, 20) };
static float s_boxSpinSpeed{ 5 };
static float s_cameraSpeed{ 3 };
static float s_cameraAcceleration{ 5 };
static float s_cameraYaw{ -90 };
static float s_cameraPitch{ 0 };
static bool s_vSyncEnabled{ false };
static bool s_showDemoWindow{ false };
static bool s_quit{ false };
static bool s_wireframeOn{ false };
static bool s_cursorDisabled{ false };
static vec3 s_cubePositions[] {
        vec3(0.0f,  0.0f,  0.0f),
        vec3(2.0f,  5.0f, -15.0f),
        vec3(-1.5f, -2.2f, -2.5f),
        vec3(-3.8f, -2.0f, -12.3f),
        vec3(2.4f, -0.4f, -3.5f),
        vec3(-1.7f,  3.0f, -7.5f),
        vec3(1.3f, -2.0f, -2.5f),
        vec3(1.5f,  2.0f, -2.5f),
        vec3(1.5f,  0.2f, -1.5f),
        vec3(-1.3f,  1.0f, -1.5f)
};
static vec3 s_inputDirection{};
static vec3 s_velocity{};
static vec3 s_targetVelocity{};

void renderCubes(Mesh &mesh, mat4 &vp, Shader &shader, Texture &texture)
{
    mat4 model;

    for (unsigned int i = 0; i < 10; i++)
    {
        float angle = 20.0f * i;
        model = mat4(1.0f);
        model = translate(model, s_cubePositions[i]);
        model = rotate(model, (float)glfwGetTime() * radians(s_boxSpinSpeed), vec3(0.5f, 1.0f, 0.0f));
        model = rotate(model, radians(angle), vec3(1.0f, 0.3f, 0.5f));

        shader.use();
        shader.setMat4("transform", vp * model);
        texture.bindTo(GL_TEXTURE0);
        mesh.draw();
    }
}

void renderImGuiCamera(Camera *camera)
{
    auto angles = eulerAngles(camera->transform->rotation);
    auto position = camera->transform->position;

    ImGui::Begin("Camera");
    ImGui::Text("Rotation: %f, %f, %f", angles.x, angles.y, angles.z);
    ImGui::Text("Position: %f, %f, %f", position.x, position.y, position.z);
    ImGui::SliderFloat("FOV", &camera->fieldOfView, 5, 100);
    ImGui::DragFloat("Near Plane", &camera->nearPlane);
    ImGui::DragFloat("Far Plane", &camera->farPlane);
    ImGui::End();
}

void renderImGuiTools()
{
    ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_FirstUseEver);
    ImGui::Begin("Tools");

    if (ImGui::CollapsingHeader("Graphics"))
    {
        ImGui::ColorEdit3("Clear Color", &s_clearColor.r);

        if (ImGui::Checkbox("Wireframe On", &s_wireframeOn))
            glPolygonMode(GL_FRONT_AND_BACK, s_wireframeOn ? GL_LINE : GL_FILL);

        ImGui::DragFloat("Spin Speed", &s_boxSpinSpeed);

        if (ImGui::Checkbox("VSync", &s_vSyncEnabled))
            display.setVSync(s_vSyncEnabled);
    }
    if (ImGui::CollapsingHeader("Camera"))
    {
        ImGui::Text("Input direction: (%f, %f)", s_inputDirection.x, s_inputDirection.y);
        ImGui::Text("Speed: %f", glm::length(s_velocity));
        ImGui::Text("Target Speed: %f", glm::length(s_targetVelocity));
        ImGui::DragFloat("Camera Speed", &s_cameraSpeed);
        ImGui::DragFloat("Camera Acceleration", &s_cameraAcceleration);
        ImGui::DragFloat("Sensitivity", &s_mouseSensitivity);
    }

    static vec3 euler;
    
    ImGui::Checkbox("Show Demo Window", &s_showDemoWindow);

    if (s_showDemoWindow)
        ImGui::ShowDemoWindow();

    static float s_timeSinceGuiUpdate;
    static float s_deltaTime;
    static float s_updateRate{ 0.1f };

    if (s_timeSinceGuiUpdate > s_updateRate)
    {
        s_deltaTime = time.deltaTime();
        s_timeSinceGuiUpdate = 0;
    }

    s_timeSinceGuiUpdate += time.deltaTime();

    ImGui::DragFloat("Update Rate", &s_updateRate);
    ImGui::Text("Delta Time: %f ms", s_deltaTime * 1000);
    ImGui::Text("FPS: %.0f", 1 / s_deltaTime);
    
    ImGui::End();
}

void processInput()
{
    input.update();

    if (input.keyDown(GLFW_KEY_F11))
        display.setFullscreen(!display.fullscreen());

    if (input.keyDown(GLFW_KEY_ESCAPE))
        input.setCursorVisible(!input.cursorVisible());


    if (!input.cursorVisible())
    {
        vec2 mouseDelta = input.mouseDelta() * s_mouseSensitivity;
        s_cameraYaw += mouseDelta.x;
        s_cameraPitch -= mouseDelta.y;
        s_cameraPitch = Math::clamp(s_cameraPitch, -89, 89);
    }

    float inputX, inputY, inputZ;
    inputY = input.axis(GLFW_KEY_W, GLFW_KEY_S);
    inputX = input.axis(GLFW_KEY_A, GLFW_KEY_D);
    inputZ = input.axis(GLFW_KEY_SPACE, GLFW_KEY_LEFT_SHIFT);

    s_inputDirection = vec3(inputX, inputY, inputZ);

    if (s_inputDirection.x != 0 || s_inputDirection.y != 0 || s_inputDirection.z != 0)
        s_inputDirection = normalize(s_inputDirection);
}

int main(int argc, char* args[])
{
    display.initialize("OpenGL", 1300, 600, 144);
    imGuiHelper.initialize(display.window());
    input.initialize(display.window());

    Mesh mesh = Primatives::generateCube();

    TransformComponent cameraTransform{};
    Camera camera{ &cameraTransform, &display };

    // Setup game loop data

    Shader shader{ "./assets/Standard.vert", "./assets/Standard.frag" };
    Texture texture{ { "./assets/container.jpg" } };

    input.setCursorVisible(false);
    input.setRawInput(true);
    cameraTransform.position = { 5, 5, 5 };
    cameraTransform.rotation = quatLookAt(normalize(Constants::zero - cameraTransform.position), Constants::up);
    s_cameraPitch = eulerAngles(cameraTransform.rotation).y;
    s_cameraYaw = eulerAngles(cameraTransform.rotation).x;
    while (!display.shouldClose())
    {
        imGuiHelper.preUpdate();
        time.update();

        processInput();

        s_targetVelocity = cameraTransform.right() * s_inputDirection.x + cameraTransform.forward() * s_inputDirection.y + cameraTransform.up() * s_inputDirection.z;
        s_targetVelocity *= s_cameraSpeed;
        s_velocity = Math::lerp(s_velocity, s_targetVelocity, s_cameraAcceleration * time.deltaTime());

        if (!input.cursorVisible())
            cameraTransform.rotation = quat{ {-radians(s_cameraPitch), -radians(s_cameraYaw), 0} };

        cameraTransform.position += s_velocity * time.deltaTime();

        // Render scene

        glClearColor(s_clearColor.r, s_clearColor.g, s_clearColor.b, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        renderImGuiTools();
        renderImGuiCamera(&camera);

        mat4 view = camera.viewMatrix();
        mat4 projection = camera.projectionMatrix();
        mat4 vp = projection * view;

        Debug::drawRay(vec3{}, Constants::forward, vp, { 1, 0, 0 });
        Debug::drawRay(vec3{}, Constants::up, vp, { 0, 1, 0 });
        Debug::drawRay(vec3{}, Constants::right, vp, { 0, 0, 1 });

        renderCubes(mesh, vp, shader, texture);

        imGuiHelper.render();
        display.swapBuffers();
    }

    imGuiHelper.shutdown();
    display.shutdown();

    return 0;
}