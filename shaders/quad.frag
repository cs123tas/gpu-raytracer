#version 430 core
in vec2 texCoord;

uniform sampler2D tex;

out vec4 fragColor;

void main() {
    // sample tex at texCoord (uv)
//    fragColor = texture(tex, texCoord);

    // debugging line - makes screen red if works
    fragColor = vec4(1.f, 0.f, 0.f, 1.f);

}

