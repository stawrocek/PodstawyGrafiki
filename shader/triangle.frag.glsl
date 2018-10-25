#version 410 core
layout(location=0) out vec4 outColor;
// glBindFragDataLocation(id, 0, "outColor");

void main()
{
    outColor = vec4(1.0, 1.0, 0.5, 1.0);
}