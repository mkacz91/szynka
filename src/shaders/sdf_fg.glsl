uniform sampler2D coverage;
uniform vec4 mask0;
uniform vec4 mask1;

varying vec4 frag_uvs;

void main() {
    vec2 uv0 = frag_uvs.xy;
    vec2 uv1 = frag_uvs.zw;
    float c0 = dot(texture2D(coverage, uv0), mask0);
    float c1 = dot(texture2D(coverage, uv1), mask1);
    gl_FragColor = vec4(c0, c1, 0.0, 0.0);
}
