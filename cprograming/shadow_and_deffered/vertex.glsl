#version 450 core
layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec3 normal_position;
out vec3 normal;
out vec3 frag_pos;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;

void main() {
    gl_Position = projection * view * model * vec4(vertex_position, 1.0);
    normal = normal_position;
    frag_pos = gl_Position.xyz;
}
