#version 110
varying vec2 uv;
varying vec3 normal;

void main()
{
    if (length(normal) > 0.01) {
        gl_FragColor = vec4(normal, 1.0);
    } else {
        gl_FragColor = vec4(1.0);
    }
}
