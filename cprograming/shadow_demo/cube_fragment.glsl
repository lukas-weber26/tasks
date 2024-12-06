#version 450 core
in vec3 f_norm;
in vec3 f_pos;
out vec4 f_color;
void main() {
    f_color = vec4(0.0, 0.8, 0.4, 1.0); //vec4(0.2, 0.8, 0.4, 1.0);
}
