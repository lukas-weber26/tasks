#version 450 core
uniform vec3 view_loc;
in vec3 normal;
in vec3 frag_pos;
out vec4 fcolor;
vec3 light_direction = vec3(1.0, 1.0, -1.0);

void main() {
    float ambient = 0.1;

    vec3 view_dir = normalize(frag_pos - view_loc);
    float diff = 0.1 * max(dot(normal, light_direction), 0.0);

    vec3 reflect_dir = reflect(-light_direction, normal);
    float spec = 0.1 * pow(max(dot(view_dir, reflect_dir), 0.0), 4);

    fcolor = (spec + ambient + diff) * vec4(0.2, 0.7, 0.1, 1.0);
}
