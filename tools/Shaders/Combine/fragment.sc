$input v_texcoord0

#include <bgfx_shader.sh>

SAMPLER2DARRAY(s_texColor, 0);

void main()
{
   vec4 color = texture2DArray(s_texColor, vec3(v_texcoord0.xy, 0));
   color += texture2DArray(s_texColor, vec3(v_texcoord0.xy, 0));

   gl_FragColor = color;
}