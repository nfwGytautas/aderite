$input v_normal, v_texcoord

/*
 * DON'T CHANGE DIRECTLY
 * This is a fragment shader header file generated by aderite for material ExampleType
 * Generated at 2022-05-28 20.08.01
 */

// Libraries
#include "bgfx_shader.sh"
#include "shaderLib.sh"
#include "material_2.sh"

// Constants

// Functions
void main() {
	// ExampleType
	BgfxSampler2D ExampleType_Texture = mf_ExampleType_Texture;
	// MOFFT6fqCjso
	vec2 MOFFT6fqCjso_v_texcoord = v_texcoord;
	// 7XHvVXXYmk19
	vec4 7XHvVXXYmk19_Color = texture2D(ExampleType_Texture, MOFFT6fqCjso_v_texcoord);
	// hzebHuBSvCf8
	gl_FragColor = 7XHvVXXYmk19_Color;
}
