#version 410 core
layout(location=0) in vec3 pos3D;
layout(location=1) in vec3 normal;

struct PointLight {
    vec3 intensity; 
    vec3 position;  // world position
};
out vec4 normalView;
out vec3 lightDir;
out vec3 halfView;

uniform mat4 vm;        // view * model
uniform mat4 view;      // view/camera matrix
uniform mat4 projection;// projection matrix
uniform mat4 N;         // normal vectors transform
uniform PointLight light;

void main()
{
    vec4 pos = vm * vec4(pos3D, 1.0f);
    vec4 lightView = view * vec4(light.position, 1.0);
    vec3 viewer = normalize(-pos.xyz);

    normalView = N * vec4(normal, 0.0); // not 1.0 since there is no translations on normal
    lightDir = normalize(lightView.xyz - pos.xyz);
    halfView = normalize(viewer + lightDir);

    gl_Position = projection * pos;
}