uniform vec4 rect;
uniform vec2 uv_adjust;

attribute vec2 uv;

varying vec2 frag_uv;

void main () {
    frag_uv = uv_adjust[0] + uv_adjust[1] * uv;
    gl_Position = vec4(mix(rect.xy, rect.zw, uv), 0.0, 1.0);
}
