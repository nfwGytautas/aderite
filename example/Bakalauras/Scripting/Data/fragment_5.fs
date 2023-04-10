$input v_normal, v_texcoord

/*
 * DON'T CHANGE DIRECTLY
 * This is a fragment shader header file generated by aderite for material Material
 * Generated at 2022-05-28 19.36.30
 */

// Libraries
#include "bgfx_shader.sh"
#include "shaderLib.sh"
#include "material_5.sh"

// Constants

// Functions
void main() {
	// Material
	vec3 Material_Color = mf_Material_Color;
	BgfxSampler2D Material_Texture = mf_Material_Texture;
	// H9qs8ICb3s8A
	float H9qs8ICb3s8A_Value = 0.000000;
	// r8B8lZ6eQh1V
	vec4 r8B8lZ6eQh1V_Out = vec4(Material_Color, H9qs8ICb3s8A_Value);
	// QeunzNfzyzaa
	vec2 QeunzNfzyzaa_v_texcoord = v_texcoord;
	// yTxQxPEVVx3e
	vec4 yTxQxPEVVx3e_Color = texture2D(Material_Texture, QeunzNfzyzaa_v_texcoord);
	// Yn056mMqDKWB
	vec4 Yn056mMqDKWB_Sum = r8B8lZ6eQh1V_Out + yTxQxPEVVx3e_Color;
	// qL32RKHBgFA7
	gl_FragColor = Yn056mMqDKWB_Sum;
}