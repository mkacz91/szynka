uniform vec4 transform0;
uniform vec4 transform1;

attribute vec2 position;

varying vec4 frag_uvs;

vec2 apply_transform(vec4 transform, vec2 position) {
    // transform = [cos(phi), sin(phi), dx, dy]
    vec4 p = transform.xyyx * position.xxyy * vec4(1.0, 1.0, -1.0, 1.0);
    return p.xy + p.zw + transform.zw;
}

void main() {
    frag_uvs = vec4(
        apply_transform(transform0, position),
        apply_transform(transform1, position));
    gl_Position = vec4(2.0 * position - 1.0, 0.0, 1.0);
}
