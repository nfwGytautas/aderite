$input a_position, a_normal, a_texcoord0, i_data0, i_data1, i_data2, i_data3
$output v_normal, v_texcoord

/*
 * DON'T CHANGE DIRECTLY
 * This is a vertex shader header file generated by aderite for material ColorShift
 * Generated at 2021-12-18 12.05.56
 */

#include "bgfx_shader.sh"
#include "shaderLib.sh"
#include "material_7.sh"

void main()
{
	mat4 model = mtxFromCols(i_data0, i_data1, i_data2, i_data3);
	vec4 worldPos = mul(model, vec4(a_position, 1.0));
	gl_Position = mul(u_viewProj, worldPos);
	v_texcoord = a_texcoord0;
	v_normal = a_normal;
}
