$input v_normal, v_texcoord

/*
 * DON'T CHANGE DIRECTLY
 * This is a fragment shader header file generated by aderite for material 5
 * Generated at 2021-11-02 15.00.18
 */

// Libraries
#include "bgfx_shader.sh"
#include "shaderLib.sh"
#include "material_5.sh"

// Constants

// Functions
void main() {
	vec4 vPiHc7MT4X57sCqM6 = texture2D(u_5_Texture, v_texcoord);
	vec4 vsQs3NuOX3ECVw5k6 = u_5_Color + vPiHc7MT4X57sCqM6;
	gl_FragColor = vsQs3NuOX3ECVw5k6;
}
