#version 410 core
in vec2 texCoord;
uniform sampler2D tex;
out vec4 fragColor;

void main() {
    // sample tex at texCoord (uv)

    fragColor = texture(tex, texCoord);

    // TODO: remove debugging line - makes screen red if works
    // fragColor = vec4(1.f, 0.f, 0.f, 1.f);

}

