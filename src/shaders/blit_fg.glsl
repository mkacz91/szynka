uniform sampler2D texture;

varying vec2 frag_uv;

void main() {
    float f = texture2D(texture, frag_uv).r;
    if (f < 0.5)
    {
        gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);
    }
    else
    {
        gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0);
    }
    //gl_FragColor = texture2D(texture, frag_uv);
}
