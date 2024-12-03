#version 450 core
in vec2 fragment_texture_coord;
out vec4 color;
uniform sampler2D frame_texture;

void main() {
    float base_color = length(texture(frame_texture, fragment_texture_coord).xyz);
    color = vec4(base_color, base_color, base_color, 1.0);
}
