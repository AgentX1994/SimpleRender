#version 110
uniform mat4 MVP;
attribute vec3 vPos;

void main()
{
    gl_Position = MVP * vec4(vPos, 1.0);
}
