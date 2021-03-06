$input v_normal, v_texcoord

/*
 * DON'T CHANGE DIRECTLY
 * This is a fragment shader header file generated by aderite for material ExampleType
 * Generated at 2022-05-28 20.10.30
 */

// Libraries
#include "bgfx_shader.sh"
#include "shaderLib.sh"
#include "material_3.sh"

// Constants

// Functions
void main() {
	// ExampleType
	BgfxSampler2D ExampleType_Texture = mf_ExampleType_Texture;
	// aAimd6pTmQVy
	vec2 aAimd6pTmQVy_v_texcoord = v_texcoord;
	// aH9Dk50pNqXp
	vec4 aH9Dk50pNqXp_Color = texture2D(ExampleType_Texture, aAimd6pTmQVy_v_texcoord);
	// aI3Ycp1PjmGI
	gl_FragColor = aH9Dk50pNqXp_Color;
}
