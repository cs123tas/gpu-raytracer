#version 410 core

in vec2 texCoord;

uniform sampler2D tex;

out vec4 fragColor;

void main(){
    fragColor = vec4(1.0);

    vec2 texelSize = 1.0/textureSize(tex, 0).xy;

    const int supportWidth = 20;

    fragColor = vec4(0.0);
    float weights = 0.0;
    for (int i = -supportWidth; i <= supportWidth; i++) {
        float weight = (supportWidth + 1) - abs(i);
        vec4 sampleColor = texture(tex, texCoord + i*texelSize.y);
        fragColor += weight*sampleColor;
        weights += weight;
    }
    fragColor /= weights;
    //fragColor = vec4(1.f, 0.f, 0.f, 1.f);
}
