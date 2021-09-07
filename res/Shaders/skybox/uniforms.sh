/*
 * Copyright 2016 Dario Manesku. All rights reserved.
 * License: https://github.com/bkaradzic/bgfx#license-bsd-2-clause
 */

uniform vec4 u_skybox[5];
#define u_mtx0          u_skybox[0]
#define u_mtx1          u_skybox[1]
#define u_mtx2          u_skybox[2]
#define u_mtx3          u_skybox[3]
#define u_exposure      u_skybox[4].x
#define u_bgType        u_skybox[4].y
