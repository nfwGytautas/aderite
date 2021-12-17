$input a_position, a_normal, a_texcoord0
$output v_normal, v_texcoord

/*
 * DON'T CHANGE DIRECTLY
 * This is a vertex shader header file generated by aderite for material ColorShift
 * Generated at 2021-12-18 00.25.36
 */

#include "bgfx_shader.sh"
#include "shaderLib.sh"
#include "material_7.sh"

void main()
{
	gl_Position = mul(u_modelViewProj, vec4(a_position, 1.0));
	v_texcoord = a_texcoord0;
	v_normal = a_normal;
}
