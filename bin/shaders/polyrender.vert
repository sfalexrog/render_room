#version 430

layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_normal;

layout (location = 0) uniform mat4 model;
layout (location = 1) uniform mat4 view;
layout (location = 2) uniform mat4 mvp;
layout (location = 6) uniform vec3 LightPosition_worldspace;

out vec3 Position_worldspace;
out vec3 Normal_cameraspace;
out vec3 EyeDirection_cameraspace;
out vec3 LightDirection_cameraspace;

void main()
{
    gl_Position = mvp * vec4(in_position, 1.0);
    Normal_cameraspace = (view * model * vec4(in_normal, 1.0)).xyz;
    Position_worldspace = (model * vec4(in_position, 1.0)).xyz;
    vec3 Position_cameraspace = (view * model * vec4(in_position, 1.0)).xyz;
    EyeDirection_cameraspace = vec3(0.0, 0.0, 0.0) - Position_cameraspace;
    vec3 LightPosition_cameraspace = ( view * vec4(LightPosition_worldspace,1)).xyz;
    LightDirection_cameraspace = LightPosition_cameraspace + EyeDirection_cameraspace;
    Normal_cameraspace = (view * model * vec4(in_normal, 0.0)).xyz;
}
