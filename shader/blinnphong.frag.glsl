#version 410 core
layout(location=0) out vec4 mainColor;

in vec4 normalView;
in vec3 lightDir;
in vec3 halfView;

struct PointLight {
    vec3 intensity; 
    vec3 position;  // world position
};
uniform PointLight light;

void main()
{
    mainColor = vec4(normalView.xyz * 0.5 + 0.5, 1.0);
}