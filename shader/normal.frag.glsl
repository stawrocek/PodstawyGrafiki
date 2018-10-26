#version 410 core
layout(location=0) out vec4 mainColor;

in vec4 normalView;

void main()
{
    mainColor = vec4(normalView.xyz * 0.5 + 0.5, 1.0);
}