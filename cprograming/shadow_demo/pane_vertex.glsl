#version 450 core

layout(location = 0) in vec2 p;
layout(location = 1) in vec2 t;

out vec2 tex_loc;

void main() {
    gl_Position = vec4(vec2(p), 0.0, 1.0);
    tex_loc = t;
}
