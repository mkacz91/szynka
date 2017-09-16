uniform vec4 transform;

attribute vec2 position;

vec2 apply_transform(vec4 transform, vec2 position) {
    // transform = [cos(phi), sin(phi), dx, dy]
    vec4 p = transform.xyyx * position.xxyy * vec4(1.0, 1.0, -1.0, 1.0);
    return p.xy + p.zw + transform.zw;
}

void main ()
{
    gl_Position = vec4(apply_transform(transform, position), 0.0, 1.0);
}
