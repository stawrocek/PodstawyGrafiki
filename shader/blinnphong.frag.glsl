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

uniform float Ia;
uniform vec3 ka, kd, ks;
uniform float phongExp;

void main()
{
    vec3 n = normalize(normalView.xyz);
    vec3 h = normalize(halfView);
    vec3 l = normalize(lightDir);

    vec3 surfaceColor =
          Ia * ka
        + light.intensity * kd * max(0.0, dot(n, l))
        + light.intensity * ks * pow(dot(n, h), phongExp);
    mainColor = vec4(surfaceColor, 1.0);
}