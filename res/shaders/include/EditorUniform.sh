/*
 * This is the shader header file where aderite editor uniforms are stored in
 */

uniform vec4 u_editor[2];
#define u_wfColor      u_editor[0].xyz
#define u_wfOpacity    u_editor[0].w
#define u_wfThickness  u_editor[1].x
