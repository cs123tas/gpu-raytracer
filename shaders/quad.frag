#version 330 core

in vec2 texCoord;

uniform sampler2D tex;

out vec4 fragColor;

void main() {
//    fragColor = texture(tex, texCoord);

    fragColor = vec4(1.f, 0.f, 0.f, 1.f);

}

