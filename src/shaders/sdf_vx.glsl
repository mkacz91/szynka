uniform vec4 transform_n0;
uniform vec4 transform_n1;
uniform vec4 transform_r0;
uniform vec4 transform_r1;

attribute vec2 position;

varying vec4 frag_uvs0;
varying vec4 frag_uvs1;

vec2 apply_transform(vec4 transform, vec2 position) {
    // transform = [cos(phi), sin(phi), dx, dy]
    vec4 p = transform.xyyx * position.xxyy * vec4(1.0, 1.0, -1.0, 1.0);
    return p.xy + p.zw + transform.zw;
}

void main() {
    frag_uvs0 = vec4(
        apply_transform(transform_n0, position),
        apply_transform(transform_r0, position));
    frag_uvs1 = vec4(
        apply_transform(transform_n1, position),
        apply_transform(transform_r1, position));
    gl_Position = vec4(2.0 * position - 1.0, 0.0, 1.0);
}
