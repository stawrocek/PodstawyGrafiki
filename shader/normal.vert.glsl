#version 410 core
layout(location=0) in vec3 pos3D;
layout(location=1) in vec3 normal;

out vec4 normalView;

uniform mat4 vm;        // view * model
uniform mat4 projection;// projection matrix
uniform mat4 N;         // normal vectors transform

void main()
{
    vec4 pos = vm * vec4(pos3D, 1.0f);
    normalView = N * vec4(normal, 0.0); // not 1.0 since there is no translations on normal

    gl_Position = projection * pos;
}