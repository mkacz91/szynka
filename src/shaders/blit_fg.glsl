uniform sampler2D texture;

varying vec2 frag_uv;

void main() {
    gl_FragColor = texture2D(texture, frag_uv);
}
