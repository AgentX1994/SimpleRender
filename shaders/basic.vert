#version 110
uniform mat4 MVP;
uniform mat4 inv_trans_model_view;
uniform mat4 model_view;
attribute vec3 vPos;
attribute vec2 vUv;
attribute vec3 vNormal;
varying vec2 uv;
varying vec3 normal;
varying vec3 world_position;

void main()
{
    gl_Position = MVP * vec4(vPos, 1.0);
    uv = vUv;
    normal = (inv_trans_model_view * vec4(vNormal, 0.0)).xyz;
    vec4 world_position_vec4 = model_view * vec4(vPos, 1.0);
    world_position = (world_position_vec4).xyz;
}
