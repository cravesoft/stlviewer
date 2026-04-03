#version 330 core

in vec3 a_position;
in vec3 a_normal;
uniform mat3 normalMatrix;
uniform mat4 modelViewMatrix;
uniform mat4 projectionMatrix;
out vec3 v_normal;
out vec3 v_position;

void main()
{
    v_normal = normalize(normalMatrix * a_normal);
    vec4 pos = modelViewMatrix * vec4(a_position, 1.0);
    v_position = pos.xyz;
    gl_Position = projectionMatrix * pos;
}
