#version 450 core
layout(location = 0) in vec2 pos;
layout(location = 1) in vec2 texture_coord;
out vec2 fragment_texture_coord;

void main() {
    gl_Position = vec4(pos, 0.0, 1.0);
    fragment_texture_coord = texture_coord;
}
