// todo:  renderer 
// todo: make boxes use transform, move camera logic to class / cleanup in general objectify game entities
// todo: configuration and better asset finding
// todo: cmake building + true cross platform tests
// todo: better dependency management than vcpkg
// todo: scene saving + loading, aka serialization
// todo: actual model importing, assimp

#include <imgui.h>
#include <ImGuizmo.h>

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
static mat4 s_cubePositions[] {
        glm::translate(glm::identity<mat4>(), vec3(0.0f,  0.0f,  0.0f)),
        glm::translate(glm::identity<mat4>(), vec3(2.0f,  5.0f, -15.0f)),
        glm::translate(glm::identity<mat4>(), vec3(-1.5f, -2.2f, -2.5f)),
        glm::translate(glm::identity<mat4>(), vec3(-3.8f, -2.0f, -12.3f)),
        glm::translate(glm::identity<mat4>(), vec3(2.4f, -0.4f, -3.5f)),
        glm::translate(glm::identity<mat4>(), vec3(-1.7f,  3.0f, -7.5f)),
        glm::translate(glm::identity<mat4>(), vec3(1.3f, -2.0f, -2.5f)),
        glm::translate(glm::identity<mat4>(), vec3(1.5f,  2.0f, -2.5f)),
        glm::translate(glm::identity<mat4>(), vec3(1.5f,  0.2f, -1.5f)),
        glm::translate(glm::identity<mat4>(), vec3(-1.3f,  1.0f, -1.5f))
};
static vec3 s_inputDirection{};
static vec3 s_velocity{};
static vec3 s_targetVelocity{};

void EditTransform(float* cameraView, float* cameraProjection, float* matrix, bool editTransformDecomposition)
{
    static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::TRANSLATE);
    static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::LOCAL);
    static bool useSnap = false;
    static float snap[3] = { 1.f, 1.f, 1.f };
    static float bounds[] = { -0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f };
    static float boundsSnap[] = { 0.1f, 0.1f, 0.1f };
    static bool boundSizing = false;
    static bool boundSizingSnap = false;

    float identityMatrix[16] = { 0.0 };

    {
        const float *pSource = (const float*)glm::value_ptr(glm::identity<mat4>());
        for (int i = 0; i < 16; ++i)
            identityMatrix[i] = pSource[i];
    }

    if (editTransformDecomposition)
    {
        if (ImGui::IsKeyPressed(ImGuiKey_T))
            mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
        if (ImGui::IsKeyPressed(ImGuiKey_E))
            mCurrentGizmoOperation = ImGuizmo::ROTATE;
        if (ImGui::IsKeyPressed(ImGuiKey_R)) // r Key
            mCurrentGizmoOperation = ImGuizmo::SCALE;
        if (ImGui::RadioButton("Translate", mCurrentGizmoOperation == ImGuizmo::TRANSLATE))
            mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
        ImGui::SameLine();
        if (ImGui::RadioButton("Rotate", mCurrentGizmoOperation == ImGuizmo::ROTATE))
            mCurrentGizmoOperation = ImGuizmo::ROTATE;
        ImGui::SameLine();
        if (ImGui::RadioButton("Scale", mCurrentGizmoOperation == ImGuizmo::SCALE))
            mCurrentGizmoOperation = ImGuizmo::SCALE;
        float matrixTranslation[3], matrixRotation[3], matrixScale[3];
        ImGuizmo::DecomposeMatrixToComponents(matrix, matrixTranslation, matrixRotation, matrixScale);
        ImGui::InputFloat3("Tr", matrixTranslation);
        ImGui::InputFloat3("Rt", matrixRotation);
        ImGui::InputFloat3("Sc", matrixScale);
        ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, matrix);

        if (mCurrentGizmoOperation != ImGuizmo::SCALE)
        {
            if (ImGui::RadioButton("Local", mCurrentGizmoMode == ImGuizmo::LOCAL))
                mCurrentGizmoMode = ImGuizmo::LOCAL;
            ImGui::SameLine();
            if (ImGui::RadioButton("World", mCurrentGizmoMode == ImGuizmo::WORLD))
                mCurrentGizmoMode = ImGuizmo::WORLD;
        }
        if (ImGui::IsKeyPressed(ImGuiKey_S))
            useSnap = !useSnap;
        ImGui::Checkbox("##UseSnap", &useSnap);
        ImGui::SameLine();

        switch (mCurrentGizmoOperation)
        {
        case ImGuizmo::TRANSLATE:
            ImGui::InputFloat3("Snap", &snap[0]);
            break;
        case ImGuizmo::ROTATE:
            ImGui::InputFloat("Angle Snap", &snap[0]);
            break;
        case ImGuizmo::SCALE:
            ImGui::InputFloat("Scale Snap", &snap[0]);
            break;
        }
        ImGui::Checkbox("Bound Sizing", &boundSizing);
        if (boundSizing)
        {
            ImGui::PushID(3);
            ImGui::Checkbox("##BoundSizing", &boundSizingSnap);
            ImGui::SameLine();
            ImGui::InputFloat3("Snap", boundsSnap);
            ImGui::PopID();
        }
    }

    ImGuiIO& io = ImGui::GetIO();
    float viewManipulateRight = io.DisplaySize.x;
    float viewManipulateTop = 0;
    static ImGuiWindowFlags gizmoWindowFlags = 0;
    
    ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);

    //ImGuizmo::DrawGrid(cameraView, cameraProjection, identityMatrix, 100.f);
    //ImGuizmo::DrawCubes(cameraView, cameraProjection, matrix, 1);
    ImGuizmo::Manipulate(cameraView, cameraProjection, mCurrentGizmoOperation, mCurrentGizmoMode, matrix, NULL, useSnap ? &snap[0] : NULL, boundSizing ? bounds : NULL, boundSizingSnap ? boundsSnap : NULL);

    //ImGuizmo::ViewManipulate(cameraView, camDistance, ImVec2(viewManipulateRight - 128, viewManipulateTop), ImVec2(128, 128), 0x10101010);
}

void renderCubes(Mesh &mesh, mat4 view, mat4 projection, Shader &shader, Texture &texture)
{
    bool dirty = false;
    ImGuiIO &io = ImGui::GetIO();
    ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);

    for (unsigned int i = 0; i < 2; i++)
    {
        float viewArr[16] = { 0.0 };
        float projArr[16] = { 0.0 };

        {
            const float *pSource = (const float*)glm::value_ptr(view);
            for (int i = 0; i < 16; ++i)
                viewArr[i] = pSource[i];
        }

        {
            const float *pSource = (const float*)glm::value_ptr(projection);
            for (int i = 0; i < 16; ++i)
                projArr[i] = pSource[i];
        }

        float angle = s_boxSpinSpeed * time.deltaTime();
        mat4 model = s_cubePositions[i];
        model = rotate(model, radians(angle), vec3(0.5f, 1.0f, 0.0f));
        model = rotate(model, radians(angle), vec3(1.0f, 0.3f, 0.5f));
        
        float gizmoArr[16] = { 0.0 };

        {
            const float *pSource = (const float*)glm::value_ptr(model);
            for (int i = 0; i < 16; ++i)
                gizmoArr[i] = pSource[i];
        }

        ImGuizmo::SetID(i);
        //ImGuizmo::Manipulate(viewArr, projArr, ImGuizmo::TRANSLATE, ImGuizmo::WORLD, gizmoArr, 0, 0);
        EditTransform(viewArr, projArr, gizmoArr, true);
        model = glm::make_mat4(gizmoArr);

        mat4 mvp = projection * view * model;

        shader.use();
        shader.setMat4("transform", mvp);
        texture.bindTo(GL_TEXTURE0);
        mesh.draw();

        s_cubePositions[i] = model;
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

        renderCubes(mesh, view, projection, shader, texture);

        imGuiHelper.render();
        display.swapBuffers();
    }

    imGuiHelper.shutdown();
    display.shutdown();

    return 0;
}