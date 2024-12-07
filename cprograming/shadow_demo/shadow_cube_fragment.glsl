#version 450 core
in vec3 f_norm;
in vec3 f_pos;
in vec4 f_lpos;
out vec4 f_color;
uniform sampler2D tex_image;

float ShadowCalculation(vec4 fragPosLightSpace)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(tex_image, projCoords.xy).r;
    //// get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    //// check whether current frag pos is in shadow
    float shadow = currentDepth > closestDepth ? 1.0 : 0.0;

    return shadow;
}

void main() {
    f_color = vec4((1 - ShadowCalculation(f_lpos)) * vec3(0.0, 0.8, 0.2), 1.0);
}
