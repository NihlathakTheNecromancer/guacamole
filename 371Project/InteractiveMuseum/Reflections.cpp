
#include <iostream>

#include "../Engine/include/Scene.hpp"
#include <algorithm>

#define GLEW_STATIC 1   // This allows linking with Static Library on Windows, without DLL
#include <GL/glew.h>    // Include GLEW - OpenGL Extension Wrangler

#include <GLFW/glfw3.h> // GLFW provides a cross-platform interface for creating a graphical context,
// initializing OpenGL and binding inputs

#include <glm/glm.hpp>  // GLM is an optimized math library with syntax to similar to OpenGL Shading Language
#include <glm/gtc/matrix_transform.hpp> // include this to create transformation matrices
#include <glm/gtc/type_ptr.hpp>

using namespace glm;
using namespace std;

class PongRoomScene : public Scene
{
    struct DepthCubemap {
        unsigned int FBO;
        unsigned int texture;
        ShaderProgram* shader;
    };

public:
    /* Scene Globals */
    /* ------------- */
    float window_width, window_height;
    float shadow_width, shadow_height;
    float room_width = 30.0f * Unit;
    float room_height = 15.0f * Unit;
    float room_bound_neg_x = (-room_width  - 0.5f) * Unit;
    float room_bound_pos_x = (room_width - 0.5f) * Unit;
    float room_bound_neg_z = (-room_width - 0.5f) * Unit;
    float room_bound_pos_z = (room_width - 0.5f) * Unit;

    // Shadows
    DepthCubemap mDepthCubemap;
    glm::mat4 spotlightPerspective;
    glm::vec3 lightPosition; // Since we have one light source at a time, we only need one light position
    int spotlightCount;
    float lightNearPlane, lightFarPlane;

    /* Interaction */
    glm::vec3 ballInitialPosition;
    float ballRadius = 3.0f * Unit;
    float teeHeight = 1.0f * Unit;
    float ballSpeed;
    glm::vec3 ballMoveDir;
    float rebound = 0.0f;
    bool checkObstacles = false;

    /* Models */
    Model* mGround;
    Model* mAxisX, * mAxisY, * mAxisZ;
    Model* mFloor, * mCeiling, * mLeftWall, * mRightWall, * mFarWall, * mNearWall;
    Model* mPongBall;
    Model* mBlock1, *mBlock2, *mBlock3, *mBlock4, *mBlock5, *mBlock6;
    std::vector<Model*> obstacles;

    // Debug
    Primitive prim = Primitive();
    Geometry* debugCube = prim.Cube();
    Geometry* debugPlane = prim.Plane();
    float debugAngle = 0.0f;
    glm::mat4 rotation, nearCube, middleCube, farCube, debugFloor;

    /* Loading Shaders
    --------------------------------*/
    //ShaderProgram* scene_shader = new ShaderProgram("./res/shaders/artifact.fs", "./res/shaders/artifact.vs");
    ShaderProgram* point_light_shader = new ShaderProgram("./res/shaders/pointLight.fs", "./res/shaders/pointLight.vs");
    ShaderProgram* depth_cubemap_shader = new ShaderProgram("./res/shaders/depthCubemap.fs", "./res/shaders/depthCubemap.vs", "./res/shaders/depthCubemap.gs");

    /* Loading Textures
    --------------------------------*/
    Texture* ceiling_texture = new Texture("./res/textures/white-painted-ceiling-texture.jpg");
    Texture* artifact_arm_texture = new Texture("./res/textures/bark.jpg");

    /* Constructor */
    PongRoomScene(float windowWidth, float windowHeight)
    {
        window_width = windowWidth;
        window_height = windowHeight;
        shadow_width = std::max(window_width, window_height);
        shadow_height = shadow_width;
    }

    /* This initializes the scene */
    void inline Initialize(void)
    {
        SetSceneShaderProgram(point_light_shader);
        SetCameraView(glm::vec3(0.0f, 2 * Unit, 25 * Unit), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        SetCameraPerspective(70.0f, window_width / window_height, 0.001 * Unit, 200 * Unit);
        SetCameraBoundingBox(room_bound_neg_x, room_bound_pos_x, room_bound_neg_z, room_bound_pos_z);

        ballInitialPosition = glm::vec3(0.0f, ballRadius + Unit, 0.0f);
        ballSpeed = 4.0f;
        ballMoveDir = glm::vec3(0.0f);
        lightPosition = ballInitialPosition;

        /* Prepare for shadow depth map */
        {
            prepareDepthCubemap();
            // To make an omnidirectional shadow map we will create 6 spotlights
            spotlightCount = 6;
            // Each spotlight will face along one of the cardinal axes (postive and negative)
            lightNearPlane = ballRadius; // Near plane to minimum possible
            lightFarPlane = 25.0f;//sqrtf(room_width * room_width + room_width * room_width + room_height * room_height);
            spotlightPerspective = glm::perspective(glm::radians(90.0f), shadow_width / shadow_height, lightNearPlane, lightFarPlane);

            /* Uniforms that only need to be set once */
            // Set the far plane value for the depth shader
            glUseProgram(mDepthCubemap.shader->id);
            glUniform1i(glGetUniformLocation(mDepthCubemap.shader->id, "spotlight_count"), spotlightCount); // For geometry shader
            glUniform1f(glGetUniformLocation(mDepthCubemap.shader->id, "far_plane"), lightFarPlane); // For vertex shader

            // Clear the textures inside the the point light shader, set far_plane while we're at it
            glUseProgram(point_light_shader->id);
            glUniform1i(glGetUniformLocation(point_light_shader->id, "u_texture"), 0);
            glUniform1i(glGetUniformLocation(point_light_shader->id, "shadow_map"), 1);
            glUniform1f(glGetUniformLocation(point_light_shader->id, "far_plane"), lightFarPlane);
        }

        /* Modelling */
        {
            // Room surfaces
            {
                // Floor Model
                mFloor = CreateModelPrimitive(PLANE, NULL);
                mFloor->ScaleModel(glm::vec3(room_width, 1.0f, room_width));
                mFloor->TranslateModel(glm::vec3(0.0f, -0.01f, 0.0f));
                mFloor->SetModelFragmentColour(glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));

                // Ceiling Model
                mCeiling = CreateModelPrimitive(PLANE, NULL);
                mCeiling->ScaleModel(glm::vec3(room_width, 1.0f, room_width));
                mCeiling->RotateModel(glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
                mCeiling->TranslateModel(glm::vec3(0.0f, room_width - 0.01f, 0.0f));
                mCeiling->SetModelTexture(ceiling_texture);

                // Left Wall Model
                mLeftWall = CreateModelPrimitive(PLANE, NULL);
                mLeftWall->ScaleModel(glm::vec3(room_width, 1.0f, room_height));
                mLeftWall->RotateModel(glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
                mLeftWall->RotateModel(glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
                mLeftWall->TranslateModel(glm::vec3(-room_width, room_height - 0.01f, 0.0f));
                mLeftWall->SetModelFragmentColour(glm::vec4(0.4f, 0.4f, 0.4f, 1.0f));

                // Right Wall Model
                mRightWall = CreateModelPrimitive(PLANE, NULL);
                mRightWall->ScaleModel(glm::vec3(room_width, 1.0f, room_height));
                mRightWall->RotateModel(glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
                mRightWall->RotateModel(glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
                mRightWall->TranslateModel(glm::vec3(room_width, room_height - 0.01f, 0.0f));
                mRightWall->SetModelFragmentColour(glm::vec4(0.4f, 0.4f, 0.4f, 1.0f));

                // Far Wall Model
                mFarWall = CreateModelPrimitive(PLANE, NULL);
                mFarWall->ScaleModel(glm::vec3(room_width, 1.0f, room_height));
                mFarWall->RotateModel(glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
                mFarWall->TranslateModel(glm::vec3(0.0f, room_height - 0.01f, -room_width));
                mFarWall->SetModelFragmentColour(glm::vec4(0.4f, 0.4f, 0.4f, 1.0f));

                // Near Wall Model
                mNearWall = CreateModelPrimitive(PLANE, NULL);
                mNearWall->ScaleModel(glm::vec3(room_width, 1.0f, room_height));
                mNearWall->RotateModel(glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
                mNearWall->TranslateModel(glm::vec3(0.0f, room_height - 0.01f, room_width));
                mNearWall->SetModelFragmentColour(glm::vec4(0.4f, 0.4f, 0.4f, 1.0f));
            }

            // Axis Models
            {
            /*mAxisX = CreateModelPrimitive(LINE, NULL);
            mAxisX->SetModelRenderMode(GL_LINES);
            mAxisX->ScaleModel(glm::vec3(1.0f));
            mAxisX->SetModelFragmentColour(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));

            mAxisY = CreateModelPrimitive(LINE, NULL);
            mAxisY->SetModelRenderMode(GL_LINES);
            mAxisY->ScaleModel(glm::vec3(1.0f));
            mAxisY->RotateModel(glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
            mAxisY->SetModelFragmentColour(glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));

            mAxisZ = CreateModelPrimitive(LINE, NULL);
            mAxisZ->SetModelRenderMode(GL_LINES);
            mAxisZ->ScaleModel(glm::vec3(1.0f));
            mAxisZ->RotateModel(glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            mAxisZ->SetModelFragmentColour(glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));*/
            }

            // The ball // TODO would probably be best to draw this independently of DrawScene() calls
            mPongBall = CreateModelPrimitive(SPHERE, NULL);
            mPongBall->ScaleCollisionBoundaries(glm::vec3(ballRadius * 2));
            mPongBall->ScaleModel(glm::vec3(ballRadius * 2));
            mPongBall->TranslateModel(ballInitialPosition);
            mPongBall->SetModelFragmentColour(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
        
            // Blocks
            {
                // A block
                mBlock1 = CreateModelPrimitive(CUBE, NULL);
                mBlock1->ScaleCollisionBoundaries(glm::vec3(5.0f));
                mBlock1->ScaleModel(glm::vec3(5.0f));
                mBlock1->TranslateModel(glm::vec3(10.0f, 0.0f, -5.0f));
                mBlock1->SetModelFragmentColour(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
                //mBlock1->SetModelTexture(ceiling_texture);
                obstacles.push_back(mBlock1);

                // A block
                mBlock2 = CreateModelPrimitive(CUBE, NULL);
                mBlock2->ScaleCollisionBoundaries(glm::vec3(4.0f));
                mBlock2->ScaleModel(glm::vec3(4.0f));
                mBlock2->TranslateModel(glm::vec3(-10.0f, 10.0f, 10.0f));
                mBlock2->SetModelFragmentColour(glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));
                obstacles.push_back(mBlock2);

                // A block
                mBlock3 = CreateModelPrimitive(CUBE, NULL);
                mBlock3->ScaleCollisionBoundaries(glm::vec3(1.0f));
                mBlock3->ScaleModel(glm::vec3(1.0f));
                mBlock3->TranslateModel(glm::vec3(-5.0f, 15.0f, -25.0f));
                mBlock3->SetModelFragmentColour(glm::vec4(1.0f, 0.0f, 1.0f, 1.0f));
                obstacles.push_back(mBlock3);

                // A block
                mBlock4 = CreateModelPrimitive(CUBE, NULL);
                mBlock4->ScaleCollisionBoundaries(glm::vec3(2.0f));
                mBlock4->ScaleModel(glm::vec3(2.0f));
                mBlock4->TranslateModel(glm::vec3(25.0f, 7.5f, 5.0f));
                mBlock4->SetModelFragmentColour(glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
                obstacles.push_back(mBlock4);

                // A block
                mBlock5 = CreateModelPrimitive(CUBE, NULL);
                mBlock5->ScaleCollisionBoundaries(glm::vec3(3.0f));
                mBlock5->ScaleModel(glm::vec3(3.0f));
                mBlock5->TranslateModel(glm::vec3(-15.0f, 4.0f, -2.0f));
                mBlock5->SetModelFragmentColour(glm::vec4(0.0f, 1.0f, 1.0f, 1.0f));
                obstacles.push_back(mBlock5);

                // A block
                mBlock6 = CreateModelPrimitive(CUBE, NULL);
                mBlock6->ScaleCollisionBoundaries(glm::vec3(2.5f));
                mBlock6->ScaleModel(glm::vec3(2.5f));
                mBlock6->TranslateModel(glm::vec3(0.0f, room_height * 2 - 2.52f, 0.0f));
                mBlock6->SetModelFragmentColour(glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
                obstacles.push_back(mBlock6);
            }
        }
    }

    /* This is called once per frame */
    void inline Update(float dt)
    {
        // Move the ball some
        mPongBall->TranslateModel(ballMoveDir * dt * ballSpeed);
        glm::vec3 ballPosition = mPongBall->GetModelPosition();

        // Collision
        {
            rebound += 1.0f;

            if (rebound >= 10.0f)
            {
                float leftWall = mLeftWall->GetModelPosition().x;
                float rightWall = mRightWall->GetModelPosition().x;
                float ceiling = mCeiling->GetModelPosition().y;
                float floor = mFloor->GetModelPosition().y;
                float farWall = mFarWall->GetModelPosition().z;
                float nearWall = mNearWall->GetModelPosition().z;

                if (ballPosition.x - leftWall <= ballRadius ||
                    rightWall - ballPosition.x <= ballRadius) {
                    ballMoveDir.x *= -1;
                    rebound = 0.0f;
                }
                if (ballPosition.y - floor <= ballRadius ||
                    ceiling - ballPosition.y <= ballRadius) {
                    ballMoveDir.y *= -1;
                    rebound = 0.0f;
                }
                if (ballPosition.z - farWall <= ballRadius ||
                    nearWall - ballPosition.z <= ballRadius)
                {
                    std::cout << "ball radius " << ballRadius << std::endl;
                    std::cout << "ball pos " << ballPosition.x << "\t" << ballPosition.y << "\t" << ballPosition.z << "\t" << std::endl;
                    //std::cout << "left" << ballPosition.x - leftWall << std::endl;
                    //std::cout << "right" << rightWall - ballPosition.x << std::endl;
                    //std::cout << "floor" << ballPosition.y - floor << std::endl;
                    //std::cout << "ceiling" << ceiling - ballPosition.y << std::endl;
                    //std::cout << "far" << ballPosition.z - farWall << std::endl;
                    //std::cout << "near" << nearWall - ballPosition.z << std::endl;

                    ballMoveDir.z *= -1;
                    rebound = 0.0f;
                }

                for (Model* m : obstacles) {
                    if (checkObstacles) {
                        if (SphereRectCollision(mPongBall, m)) {
                                rebound = 0.0f;
                                //ballMoveDir = glm::vec3(0);
                                ballMoveDir = -ballMoveDir;
                            std::cout << "the cube" << std::endl;
                        }
                    }

                }
            }
        }

        // Shadows
        {
            lightPosition = ballPosition;
            std::vector<glm::mat4> spotlightViewProjections;
            spotlightViewProjections.push_back(spotlightPerspective *
                glm::lookAt(lightPosition, lightPosition + glm::vec3( 1.0,  0.0,  0.0), glm::vec3(0.0, -1.0,  0.0))); // Right
            spotlightViewProjections.push_back(spotlightPerspective *
                glm::lookAt(lightPosition, lightPosition + glm::vec3(-1.0,  0.0,  0.0), glm::vec3(0.0, -1.0,  0.0))); // Left
            spotlightViewProjections.push_back(spotlightPerspective *
                glm::lookAt(lightPosition, lightPosition + glm::vec3( 0.0,  1.0,  0.0), glm::vec3(0.0,  0.0,  1.0))); // Up
            spotlightViewProjections.push_back(spotlightPerspective *
                glm::lookAt(lightPosition, lightPosition + glm::vec3( 0.0, -1.0,  0.0), glm::vec3(0.0,  0.0, -1.0))); // Down
            spotlightViewProjections.push_back(spotlightPerspective *
                glm::lookAt(lightPosition, lightPosition + glm::vec3( 0.0,  0.0,  1.0), glm::vec3(0.0, -1.0,  0.0))); // Backwards (Near)
            spotlightViewProjections.push_back(spotlightPerspective *
                glm::lookAt(lightPosition, lightPosition + glm::vec3( 0.0,  0.0, -1.0), glm::vec3(0.0, -1.0,  0.0))); // Forward (Far)

            glUseProgram(mDepthCubemap.shader->id);
            glUniform3fv(glGetUniformLocation(mDepthCubemap.shader->id, "light_position"), 1, &lightPosition[0]);

            unsigned int i = 0;
            for (mat4 &viewProjection: spotlightViewProjections) {
                std::string uniformName = "view_projections[" + std::to_string(i++) + "]";
                glUniformMatrix4fv(glGetUniformLocation(mDepthCubemap.shader->id, uniformName.c_str()), 1, GL_FALSE, &viewProjection[0][0]);
            }

            glViewport(0, 0, (unsigned int)shadow_width, (unsigned int)shadow_height);
            glBindFramebuffer(GL_FRAMEBUFFER, mDepthCubemap.FBO); // Use our depth framebuffer
            glClear(GL_DEPTH_BUFFER_BIT);

            glDisable(GL_CULL_FACE); // Disable face culling to reduce Peter Panning
            DrawScene(mDepthCubemap.shader); // Draw the scene from the light source's perspective
            glEnable(GL_CULL_FACE);

            glBindFramebuffer(GL_FRAMEBUFFER, 0); // Back to the default framebuffer
            glViewport(0, 0, window_width, window_height);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        }

        glLineWidth(5.0f); // To make the axes more visible
        BindPongRoomUniforms();
        DrawScene(point_light_shader); // Draw the scene using the main shader
        glLineWidth(1.0f);
    }

    void shadowDebug(ShaderProgram* shader, float dt) {
        {
            debugAngle += dt * 20.0f;
            if (debugAngle > 360.0f) {
                debugAngle = 0.0f;
            }

            rotation = rotate(mat4(1.0f), radians(debugAngle), vec3(0.0f, 1.0f, 0.0f));
            nearCube = translate(rotation, glm::vec3(1.5f, 0.0f, -5.0f))
                * scale(mat4(1.0f), glm::vec3(3.0f));
            middleCube = translate(rotation, glm::vec3(0.0f, 0.0f, -9.0f))
                * scale(mat4(1.0f), glm::vec3(3.0f));
            farCube = translate(rotation, glm::vec3(-1.5f, 0.0f, -13.0f))
                * scale(mat4(1.0f), glm::vec3(3.0f));
            debugFloor = translate(mat4(1.0f), glm::vec3(0.0f, -5.01f, 0.0f))
                * scale(mat4(1.0f), glm::vec3(room_width, 0.0f, room_width));

            // Let's try manually drawing stuff.
            ShaderProgram* currentShader = shader;

            glUseProgram(currentShader->id);
            glUniform3fv(glGetUniformLocation(currentShader->id, "fragment_colour"), 1, glm::value_ptr(glm::vec3(1.0, 0.0, 1.0)));
            //glUniformMatrix4fv(glGetUniformLocation(currentShader->id, "fragment_colour"), 1, GL_FALSE, glm::value_ptr(glm::vec4(1.0, 1.0, 1.0, 1.0)));
            glUniformMatrix4fv(glGetUniformLocation(currentShader->id, "view_matrix"), 1, GL_FALSE, &GetCameraViewMatrix()[0][0]);
            glUniformMatrix4fv(glGetUniformLocation(currentShader->id, "projection_matrix"), 1, GL_FALSE, &GetCameraProjectionMatrix()[0][0]);

            glBindVertexArray(debugCube->vao);
            glUniformMatrix4fv(glGetUniformLocation(currentShader->id, "model_matrix"), 1, GL_FALSE, &nearCube[0][0]);
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);
            glUniformMatrix4fv(glGetUniformLocation(currentShader->id, "model_matrix"), 1, GL_FALSE, &middleCube[0][0]);
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);
            glUniformMatrix4fv(glGetUniformLocation(currentShader->id, "model_matrix"), 1, GL_FALSE, &farCube[0][0]);
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);

            glUniform3fv(glGetUniformLocation(currentShader->id, "fragment_colour"), 1, glm::value_ptr(glm::vec3(1.0, 1.0, 0.0)));
            glBindVertexArray(debugPlane->vao);
            glUniformMatrix4fv(glGetUniformLocation(currentShader->id, "model_matrix"), 1, GL_FALSE, &debugFloor[0][0]);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
        }

        // Shader debugging
        {
            //    ShaderProgram* debugShader = nullptr;
            //    //debugShader = mDepthCubemap.shader;
            //    debugShader = point_light_shader;

            //    float float_return = 0.0f;
            //    std::cout << float_return << std::endl;
            //    GLint uniform_location = glGetUniformLocation(debugShader->id, "far_plane");
            //    glGetUniformfv(debugShader->id, uniform_location, &float_return);
            //    std::cout << float_return << std::endl;

            //    glm::vec3 vec3_return(0.0f);
            //    std::cout << vec3_return.x << " " << vec3_return.y << " " << vec3_return.z << std::endl;
            //    uniform_location = glGetUniformLocation(debugShader->id, "fragment_colour");
            //    glGetUniformfv(debugShader->id, uniform_location, &vec3_return[0]);
            //    std::cout << vec3_return.x << " " << vec3_return.y << " " << vec3_return.z << std::endl;
        }
    }

    void SetKeyCallback(GLFWwindow* window)
    {
        glfwSetWindowUserPointer(window, (void*)this);
        glfwSetKeyCallback(window, key_callback);
    }

    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        PongRoomScene* roomInstance = (PongRoomScene*)glfwGetWindowUserPointer(window);

        if (key == GLFW_KEY_RIGHT_BRACKET && action == GLFW_RELEASE)
        {
            float newBallRadius = roomInstance->ballRadius * 2.0f;
            roomInstance->ballRadius = newBallRadius;

            roomInstance->mPongBall->model_matrix = glm::inverse(roomInstance->mPongBall->model_matrix);
            /*glm::vec3 pos = roomInstance->mPongBall->GetModelPosition();
            roomInstance->mPongBall->TranslateModel(-pos);
            roomInstance->mPongBall->ScaleModel(glm::vec3(newBallRadius));
            roomInstance->mPongBall->TranslateModel(pos);*/
        }
        if (key == GLFW_KEY_LEFT_BRACKET && action == GLFW_RELEASE)
        {
            float newBallRadius = roomInstance->ballRadius * 0.5f;
            roomInstance->ballRadius = newBallRadius;
            //roomInstance->mPongBall->model_matrix = glm::mat4(1.0f);
            glm::vec3 pos = roomInstance->mPongBall->GetModelPosition();
            roomInstance->mPongBall->TranslateModel(-pos);
            roomInstance->mPongBall->ScaleModel(glm::vec3(newBallRadius));
            roomInstance->mPongBall->TranslateModel(pos);
        }
    }

    /* Define Key Bindings */
    void inline InputController(GLFWwindow* window)
    {
        if (glfwGetKey(window, GLFW_KEY_EQUAL) == GLFW_PRESS)
            ballSpeed += 1.0f;

        if (glfwGetKey(window, GLFW_KEY_MINUS) == GLFW_PRESS)
            ballSpeed -= 1.0f;

        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
            ballMoveDir = glm::normalize(camera_front);
            checkObstacles = true;
        }
    }

    bool SphereRectCollision(Model* sphere, Model* rect)
    {
        float sphereXDistance = abs(sphere->position->x - rect->position->x);
        float sphereYDistance = abs(sphere->position->y - rect->position->y);
        float sphereZDistance = abs(sphere->position->z - rect->position->z);
        if (sphereXDistance >= (rect->dimensions.x + sphere->dimensions.x)) { return false; }
        if (sphereYDistance >= (rect->dimensions.y + sphere->dimensions.x)) { return false; }
        if (sphereZDistance >= (rect->dimensions.z + sphere->dimensions.x)) { return false; }

        if (sphereXDistance < (rect->dimensions.x)) { return true; }
        if (sphereYDistance < (rect->dimensions.y)) { return true; }
        if (sphereZDistance < (rect->dimensions.z)) { return true; }

        float cornerDistance_sq = ((sphereXDistance - rect->dimensions.x) * (sphereXDistance - rect->dimensions.x)) +
            ((sphereYDistance - rect->dimensions.y) * (sphereYDistance - rect->dimensions.y) +
            ((sphereYDistance - rect->dimensions.z) * (sphereYDistance - rect->dimensions.z)));

        return (cornerDistance_sq < (sphere->dimensions.x * sphere->dimensions.x) );
    }
    
    void prepareDepthCubemap() {
        /* Configure the depth cubemap texture */
        /* ---------------- */
        // Generate a texture and keep a reference to it in mDepthCubemap.texture
        glGenTextures(1, &mDepthCubemap.texture);
        // Bind the texture so we can configure it
        glBindTexture(GL_TEXTURE_CUBE_MAP, mDepthCubemap.texture);
        // Configure the currently bound texture
        for (int i = 0; i < 6; ++i) { // 0 = +x, 1 = -x, 2 = +y, 3 = -y, 4 = +z, 5 = -z
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0, GL_DEPTH_COMPONENT,
                shadow_width, shadow_height,
                0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        /* Configure the depth cubemap framebuffer object */
        /* ---------------- */
        // Generate a framebuffer and keep a reference to it in mDepthCubemap.FBO
        glGenFramebuffers(1, &mDepthCubemap.FBO);
        // Bind the framebuffer so we can configure it
        glBindFramebuffer(GL_FRAMEBUFFER, mDepthCubemap.FBO);
        // Configure the currently bound framebuffer
        glDrawBuffer(GL_NONE); // Do not write to the color buffer 
        glReadBuffer(GL_NONE); // Do not try to read color data from this buffer
        // Attach the texture to the Framebuffer Object as the depth buffer
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, mDepthCubemap.texture, 0);
        // Release the framebuffer so future framebuffer actions do not inadvertently affect it
        // Do this by going back to the default framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        /* Assign the shader we'll use to generate the depth map */
        /* ---------------- */
        mDepthCubemap.shader = depth_cubemap_shader;
    }

    void BindPongRoomUniforms() {
        glUseProgram(point_light_shader->id);
        glUniform3fv(glGetUniformLocation(point_light_shader->id, "light_position"), 1, &(lightPosition[0]));

        glActiveTexture(GL_TEXTURE1); // Bind the depth map texture so that the main shader can use it to place shadows
        glBindTexture(GL_TEXTURE_CUBE_MAP, mDepthCubemap.texture);

        glUseProgram(0);
    }
};