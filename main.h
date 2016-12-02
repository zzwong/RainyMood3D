//
//  main.h
//  CSE167-Final
//
//  Created by Aaron Wong on 11/26/16.
//  Copyright © 2016 wongsauce. All rights reserved.
//
#ifndef _MAIN_H_
#define _MAIN_H_

#define GLFW_INCLUDE_GLEXT
#ifdef __APPLE__
#define GLFW_INCLUDE_GLCOREARB
#else
#include <GL/glew.h>
#endif
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <stdio.h>
#include "Window.h"

#endif