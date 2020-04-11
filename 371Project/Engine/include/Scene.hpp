#ifndef _SCENE_H
#define _SCENE_H

#include "Model.hpp"
#include "ShaderProgram.hpp"
#include "Primitive.hpp"

#include <vector>

class Primitive;
class Model;
class ShaderProgram;
class Scene
{   
protected:
    /* Scene Shader Program */
    ShaderProgram*      scene_shader_program;

    /* Models in the scene */
    std::vector<Model*> models = std::vector<Model*>();
    
    /* Scene Camera */
    bool         is_camera_flying = false;
    float        camera_aspect_ratio;               // Aspect Ratio
    float        camera_fov;                        // Field of View
    float        camera_near;                       // Near
    float        camera_far;                        // Far
    glm::vec3    camera_position;                   // Camera Position
    glm::vec3    camera_front;                      // Point Camera is looking at
    glm::vec3    camera_up;                         // Up Direction of camera
    float        camera_yaw;                        // Camera Yaw angle
    float        camera_pitch;                      // Camera Pitch angle
    float        camera_bound_x_neg = -1000*Unit;   // Camera negative X boundary
    float        camera_bound_x_pos = 1000*Unit;    // Camera positive X boundary
    float        camera_bound_z_neg = -1000*Unit;   // Camera negative Z boundary
    float        camera_bound_z_pos = 1000*Unit;    // Camera positive Z boundary



    /* Matrices */
    glm::mat4    view_matrix;
    glm::mat4    projection_matrix;

    /* World Scale */
    float Unit = 1.0f;

    /* Construct */
    Scene(void);

public:

    /* Toggle */
    void                ToggleSceneTextures(void);

    /* Scene Parameters */
    ShaderProgram*      GetSceneShaderProgram(void);
    std::vector<Model*> GetSceneModels(void);
    // glm::mat4       GetSceneLightViewMatrix(void);
    // glm::mat4       GetSceneLightProjectionMatrix(void);
    void                SetSceneShaderProgram(ShaderProgram* shader);

    /* Camera Parameters */
    glm::mat4       GetCameraViewMatrix(void);
    glm::mat4       GetCameraProjectionMatrix(void);
    glm::vec3       GetCameraPosition(void);
    void            SetCameraPerspective(float fov, float ar, float near, float far);
    void            SetCameraOrthographic(float a, float b, float c, float d, float near, float far);
    void            SetCameraView(glm::vec3 cameraPosition, glm::vec3 cameraFront, glm::vec3 cameraUp);
    void inline     SetCameraFlying(bool is_flying){ is_camera_flying = is_flying;}
    float           GetCameraYaw(void);
    float           GetCameraPitch(void);
    void            UpdateCameraView(void);

    /* Camera Controller */
    void            TranslateCameraRight(float distance);
    void            TranslateCameraLeft(float distance);
    void            TranslateCameraForward(float distance);
    void            TranslateCameraBackward(float distance);
    void            RotateCameraEuler(float yaw, float pitch);
    void            SetCameraBoundingBox(float neg_x, float pos_x, float neg_z, float pos_z);
    bool            CheckBoundingBoxCollision(glm::vec3 pos, glm::vec3 dir, float distance); 

    /* Create */
    Geometry*       primitive_plane;
    Geometry*       primitive_cube;
    Geometry*       primitive_sphere;
    Geometry*       primitive_line;
    Model*          CreateModelPrimitive(PrimitiveType type, Model* parent);
    
    /* Draw */
    void            DrawScene(void);
    void            DrawScene(ShaderProgram* shader);
    virtual void    Initialize(void) = 0;
    virtual void    Update(float dt) = 0;
    virtual void    InputController(GLFWwindow* window) = 0;
    virtual void    SetKeyCallback(GLFWwindow* window) = 0;
};
#endif /* _SCENE_H */