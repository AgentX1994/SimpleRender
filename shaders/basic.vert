#version 110
uniform mat4 MVP;
attribute vec3 vPos;
attribute vec2 vUv;
attribute vec3 vNormal;
varying vec2 uv;
varying vec3 normal;

void main()
{
    gl_Position = MVP * vec4(vPos, 1.0);
    uv = vUv;
    normal = vNormal;
}
