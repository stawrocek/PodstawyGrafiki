#version 410 core
layout(location=0) in vec2 pos2D;
// glGetAttribLocation(programId, "pos2D");

uniform mat4 transform;

void main()
{
    gl_Position = transform * vec4(pos2D, 0.0, 1.0);
}