#version 450 core
out vec4 fcolor;
in vec2 tex_loc;
uniform sampler2D tex_image;

void main() {
    float v = texture(tex_image, tex_loc).x;
    fcolor = vec4((1 - v + 0.2), (1 - v), v, 1.0);
    //vec4(0.0, 0.0, 1.0, 1.0) + vec4(texture(tex_image, tex_loc).x, 0.0, 0.0, 0.0);
}
