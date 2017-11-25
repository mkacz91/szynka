const float HALF_SQRT_2 = 0.70710678118;
const float HALF_SQRT_2_MINUS_1 = 0.20710678118;

uniform sampler2D coverage;
uniform sampler2D map;
uniform vec2 map_uv_adjust;
uniform vec4 mask_n0;
uniform vec4 mask_n1;
uniform vec4 mask_r0;
uniform vec4 mask_r1;

varying vec4 frag_uvs0;
varying vec4 frag_uvs1;

vec4 get_map(float n, float r)
{
    return texture2D(map, vec2(map_uv_adjust[0] + map_uv_adjust[1] * vec2(n, r)));
}

void main() {
    float c_n0 = dot(texture2D(coverage, frag_uvs0.xy), mask_n0);
    float c_n1 = dot(texture2D(coverage, frag_uvs1.xy), mask_n1);
    float c_r0 = dot(texture2D(coverage, frag_uvs0.zw), mask_r0);
    float c_r1 = dot(texture2D(coverage, frag_uvs1.zw), mask_r1);

    float d = 0.0;
    if (c_r0 == 0.0 || c_r0 == 1.0)
    {

        float df1 = get_map(c_n1, c_r1).g;
        float dl = (1.0 - df1) * (-1.0) + df1 * HALF_SQRT_2_MINUS_1;
        if (c_r0 == 0.0)
        {
            d = -0.5 + dl;
        }
        else
        {
            d = 0.5 - dl;
        }
        //gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
        //return;
    }
    else
    {
        float df0 = get_map(c_n0, c_r0).r;
        d = (1.0 - df0) * (-HALF_SQRT_2) + df0 * HALF_SQRT_2;
        //gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
        //return;
    }

    d = (d + 3.0) / 6.0;
    gl_FragColor = vec4(c_n0, d, d, 1.0);
}
