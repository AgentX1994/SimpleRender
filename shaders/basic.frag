#version 110
varying vec2 uv;
varying vec3 normal;

void main()
{
    gl_FragColor = vec4(normal, 1.0);
}
