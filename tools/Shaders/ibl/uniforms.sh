/*
 * Copyright 2016 Dario Manesku. All rights reserved.
 * License: https://github.com/bkaradzic/bgfx#license-bsd-2-clause
 */

uniform vec4 u_ibl_params[12];
#define u_mtx0          u_ibl_params[0]
#define u_mtx1          u_ibl_params[1]
#define u_mtx2          u_ibl_params[2]
#define u_mtx3          u_ibl_params[3]
#define u_glossiness    u_ibl_params[4].x
#define u_reflectivity  u_ibl_params[4].y
#define u_exposure      u_ibl_params[4].z
#define u_bgType        u_ibl_params[4].w
#define u_metalOrSpec   u_ibl_params[5].x
#define u_unused        u_ibl_params[5].yzw
#define u_doDiffuse     u_ibl_params[6].x
#define u_doSpecular    u_ibl_params[6].y
#define u_doDiffuseIbl  u_ibl_params[6].z
#define u_doSpecularIbl u_ibl_params[6].w
#define u_camPos        u_ibl_params[7].xyz
#define u_unused7       u_ibl_params[7].w
#define u_rgbDiff       u_ibl_params[8]
#define u_rgbSpec       u_ibl_params[9]
#define u_lightDir      u_ibl_params[10].xyz
#define u_unused10      u_ibl_params[10].w
#define u_lightCol      u_ibl_params[11].xyz
#define u_unused11      u_ibl_params[11].w
