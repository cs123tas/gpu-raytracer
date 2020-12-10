#version 410 core

in vec2 texCoord;

uniform sampler2D tex;
uniform vec2 dimensions;

out vec4 fragColor;

void main(){
    fragColor = vec4(0.0);

//    vec2 texelSize = 1.0/textureSize(tex, 0).xy;
//
//    const int supportWidth = 20;
//
//    fragColor = vec4(0.0);
//    float weights = 0.0;
//    for (int i = -supportWidth; i <= supportWidth; i++) {
//        float weight = (supportWidth + 1) - abs(i);
//        vec4 sampleColor = texture(tex, texCoord + i*texelSize.y);
//        fragColor += weight*sampleColor;
//        weights += weight;
//    }
//    fragColor /= weights;
    float x = ((texCoord.x/dimensions.x) + 1.f)/2.f;
    float y = ((texCoord.y/dimensions.y) + 1.f)/2.f;
    fragColor = texture(tex, vec2(x*dimensions.x, y(dimensions.y));
    //fragColor = vec4(1.f, 0.f, 0.f, 1.f);
}
