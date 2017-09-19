uniform sampler2D coverage;
uniform vec4 mask_n0;
uniform vec4 mask_n1;
uniform vec4 mask_r0;
uniform vec4 mask_r1;

varying vec4 frag_uvs0;
varying vec4 frag_uvs1;

void main() {
    float c_n0 = dot(texture2D(coverage, frag_uvs0.xy), mask_n0);
    float c_n1 = dot(texture2D(coverage, frag_uvs1.xy), mask_n1);
    float c_r0 = dot(texture2D(coverage, frag_uvs0.zw), mask_r0);
    float c_r1 = dot(texture2D(coverage, frag_uvs1.zw), mask_r1);
    gl_FragColor = vec4(c_n0, c_n1, c_r0, c_r1);
}
