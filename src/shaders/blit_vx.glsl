uniform vec4 rect;

attribute vec2 uv;

varying vec2 frag_uv;

void main () {
    frag_uv = uv;
    gl_Position = vec4(mix(rect.xy, rect.zw, uv), 0.0, 1.0);
}
