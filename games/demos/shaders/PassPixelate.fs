out vec4 color;

uniform sampler2D tex;
uniform float xPixels = 100.f, yPixels = 100.f; // [1..] each

void main() {
    vec2 uv = vec2(floor(texcoord.s * xPixels) / xPixels, floor(texcoord.t * yPixels) / yPixels);
    color = texture2D(tex, uv);
}
