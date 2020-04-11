#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNorm;
layout (location = 2) in vec2 aUV;

out vec3 fragment_position;
out vec3 normal;
out vec2 UV;


uniform mat4 projection_matrix = mat4(1.0f);
uniform mat4 view_matrix = mat4(1.0f);
uniform mat4 model_matrix = mat4(1.0f);

uniform float normal_scaler = 1.0f; // Primarily used to invert the normal by setting to -1.0f

void main()
{
    fragment_position = vec3(model_matrix * vec4(aPos, 1.0));
    normal = normal_scaler * transpose(inverse(mat3(model_matrix))) * aNorm;
    UV = aUV;


    gl_Position = projection_matrix * view_matrix * model_matrix * vec4(aPos, 1.0);
    //gl_PointSize = gl_Position.z; // For drawing points at varied size
}