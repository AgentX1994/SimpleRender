#version 110

uniform int mode;

void main()
{
    if (mode == 0) { // drawing surface
        gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
    } else { // drawing normals
        gl_FragColor = vec4(0.0, 0.0, 1.0, 1.0);
    }
}
