#version 450 core
layout(location = 0) in vec3 v_pos;
layout(location = 1) in vec3 v_norm;
uniform mat4 projection_view;
uniform mat4 model;
out vec3 f_norm;
out vec3 f_pos;
void main() {
    gl_Position = projection_view * model * vec4(v_pos, 1.0);
    f_norm = vec3(projection_view * model * vec4(v_norm, 1.0));
    f_pos = vec3(projection_view * model * vec4(v_pos, 1.0));
}
