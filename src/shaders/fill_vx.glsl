uniform mat3 transform;

attribute vec2 position;

void main ()
{
    gl_Position = vec4((transform * vec3(position, 1.0)).xy, 0.0, 1.0);
}
