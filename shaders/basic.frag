#version 110
varying vec2 uv;
varying vec3 normal;
varying vec3 world_position;

uniform vec3 light_position;

uniform float ambient_coefficient;
uniform float diffuse_coefficient;
uniform float specular_coefficient;

uniform float shininess;

uniform vec3 ambient_color;
uniform vec3 diffuse_color;
uniform vec3 specular_color;

void main()
{
    vec3 calc_normal;
    // If no normals, derive them from the derivative of the position
    if (normal == vec3(0,0,0)) {
        vec3 dFdxPos = dFdx(world_position);
        vec3 dFdyPos = dFdy(world_position);
        calc_normal = normalize( cross( dFdxPos, dFdyPos ) );
    } else {
        calc_normal = normalize(normal);
    }

    vec3 light_dir = normalize(light_position - world_position);

    float lambertian = max(dot(calc_normal, light_dir), 0.0);
    float specular = 0.0;
    if (lambertian > 0.0) {
        vec3 reflected_light_dir = reflect(-light_dir, calc_normal);
        vec3 viewer = normalize(-world_position);
        float specAngle = max(dot(reflected_light_dir, viewer), 0.0);
        specular = pow(specAngle, shininess);
    }

    gl_FragColor = vec4( ambient_coefficient * ambient_color +
            diffuse_coefficient * lambertian * diffuse_color +
            specular_coefficient * specular * specular_color, 1.0);
}
