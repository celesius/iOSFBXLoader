//
//  Math.h
//  OpenGLES_Class_02_Demo_01
//
//  Created by cxjwin on 12-8-25.
//  Copyright (c) 2012年 caixuejun. All rights reserved.
//

#ifndef OpenGLES_Class_02_Demo_01_CSMath_h
#define OpenGLES_Class_02_Demo_01_CSMath_h

typedef GLfloat Matrix3D[16];

static inline void Matrix3DSetIdentity(Matrix3D matrix)
{
    matrix[0] = matrix[5] =  matrix[10] = matrix[15] = 1.0;
    matrix[1] = matrix[2] = matrix[3] = matrix[4] = 0.0;
    matrix[6] = matrix[7] = matrix[8] = matrix[9] = 0.0;
    matrix[11] = matrix[12] = matrix[13] = matrix[14] = 0.0;
}

// 四元素矩阵的乘法,加入了对iPhone向量运算器的应用，所以在设备上乘法可以进行更快的计算
/*
 These define the vectorized version of the
 matrix multiply function and are based on the Matrix4Mul method from
 the vfp-math-library. This code has been modified, but is still subject to
 the original license terms and ownership as follow:
 
 VFP math library for the iPhone / iPod touch
 
 Copyright (c) 2007-2008 Wolfgang Engel and Matthias Grundmann
 http://code.google.com/p/vfpmathlibrary/
 
 This software is provided 'as-is', without any express or implied warranty.
 In no event will the authors be held liable for any damages arising
 from the use of this software.
 Permission is granted to anyone to use this software for any purpose,
 including commercial applications, and to alter it and redistribute it freely,
 subject to the following restrictions:
 
 1. The origin of this software must not be misrepresented; you must
 not claim that you wrote the original software. If you use this
 software in a product, an acknowledgment in the product documentation
 would be appreciated but is not required.
 
 2. Altered source versions must be plainly marked as such, and must
 not be misrepresented as being the original software.
 
 3. This notice may not be removed or altered from any source distribution.
 */
#if TARGET_OS_IPHONE && !TARGET_IPHONE_SIMULATOR
#define VFP_CLOBBER_S0_S31 "s0", "s1", "s2", "s3", "s4", "s5", "s6", "s7", "s8",  \
"s9", "s10", "s11", "s12", "s13", "s14", "s15", "s16",  \
"s17", "s18", "s19", "s20", "s21", "s22", "s23", "s24",  \
"s25", "s26", "s27", "s28", "s29", "s30", "s31"
#define VFP_VECTOR_LENGTH(VEC_LENGTH) "fmrx    r0, fpscr                    \n\t" \
"bic     r0, r0, #0x00370000               \n\t" \
"orr     r0, r0, #0x000" #VEC_LENGTH "0000 \n\t" \
"fmxr    fpscr, r0                         \n\t"
#define VFP_VECTOR_LENGTH_ZERO "fmrx    r0, fpscr            \n\t" \
"bic     r0, r0, #0x00370000  \n\t" \
"fmxr    fpscr, r0            \n\t"
#endif
static inline void Matrix3DMultiply(Matrix3D m1, Matrix3D m2, Matrix3D result)
{
#if TARGET_OS_IPHONE && !TARGET_IPHONE_SIMULATOR
    __asm__ __volatile__ ( VFP_VECTOR_LENGTH(3)
                          
                          // Interleaving loads and adds/muls for faster calculation.
                          // Let A:=src_ptr_1, B:=src_ptr_2, then
                          // function computes A*B as (B^T * A^T)^T.
                          
                          // Load the whole matrix into memory.
                          "fldmias  %2, {s8-s23}    \n\t"
                          // Load first column to scalar bank.
                          "fldmias  %1!, {s0-s3}    \n\t"
                          // First column times matrix.
                          "fmuls s24, s8, s0        \n\t"
                          "fmacs s24, s12, s1       \n\t"
                          
                          // Load second column to scalar bank.
                          "fldmias %1!,  {s4-s7}    \n\t"
                          
                          "fmacs s24, s16, s2       \n\t"
                          "fmacs s24, s20, s3       \n\t"
                          // Save first column.
                          "fstmias  %0!, {s24-s27}  \n\t"
                          
                          // Second column times matrix.
                          "fmuls s28, s8, s4        \n\t"
                          "fmacs s28, s12, s5       \n\t"
                          
                          // Load third column to scalar bank.
                          "fldmias  %1!, {s0-s3}    \n\t"
                          
                          "fmacs s28, s16, s6       \n\t"
                          "fmacs s28, s20, s7       \n\t"
                          // Save second column.
                          "fstmias  %0!, {s28-s31}  \n\t"
                          
                          // Third column times matrix.
                          "fmuls s24, s8, s0        \n\t"
                          "fmacs s24, s12, s1       \n\t"
                          
                          // Load fourth column to scalar bank.
                          "fldmias %1,  {s4-s7}    \n\t"
                          
                          "fmacs s24, s16, s2       \n\t"
                          "fmacs s24, s20, s3       \n\t"
                          // Save third column.
                          "fstmias  %0!, {s24-s27}  \n\t"
                          
                          // Fourth column times matrix.
                          "fmuls s28, s8, s4        \n\t"
                          "fmacs s28, s12, s5       \n\t"
                          "fmacs s28, s16, s6       \n\t"
                          "fmacs s28, s20, s7       \n\t"
                          // Save fourth column.
                          "fstmias  %0!, {s28-s31}  \n\t"
                          
                          VFP_VECTOR_LENGTH_ZERO
                          : "=r" (result), "=r" (m2)
                          : "r" (m1), "0" (result), "1" (m2)
                          : "r0", "cc", "memory", VFP_CLOBBER_S0_S31
                          );
#else
    result[0] = m1[0] * m2[0] + m1[4] * m2[1] + m1[8] * m2[2] + m1[12] * m2[3];
    result[1] = m1[1] * m2[0] + m1[5] * m2[1] + m1[9] * m2[2] + m1[13] * m2[3];
    result[2] = m1[2] * m2[0] + m1[6] * m2[1] + m1[10] * m2[2] + m1[14] * m2[3];
    result[3] = m1[3] * m2[0] + m1[7] * m2[1] + m1[11] * m2[2] + m1[15] * m2[3];
    
    result[4] = m1[0] * m2[4] + m1[4] * m2[5] + m1[8] * m2[6] + m1[12] * m2[7];
    result[5] = m1[1] * m2[4] + m1[5] * m2[5] + m1[9] * m2[6] + m1[13] * m2[7];
    result[6] = m1[2] * m2[4] + m1[6] * m2[5] + m1[10] * m2[6] + m1[14] * m2[7];
    result[7] = m1[3] * m2[4] + m1[7] * m2[5] + m1[11] * m2[6] + m1[15] * m2[7];
    
    result[8] = m1[0] * m2[8] + m1[4] * m2[9] + m1[8] * m2[10] + m1[12] * m2[11];
    result[9] = m1[1] * m2[8] + m1[5] * m2[9] + m1[9] * m2[10] + m1[13] * m2[11];
    result[10] = m1[2] * m2[8] + m1[6] * m2[9] + m1[10] * m2[10] + m1[14] * m2[11];
    result[11] = m1[3] * m2[8] + m1[7] * m2[9] + m1[11] * m2[10] + m1[15] * m2[11];
    
    result[12] = m1[0] * m2[12] + m1[4] * m2[13] + m1[8] * m2[14] + m1[12] * m2[15];
    result[13] = m1[1] * m2[12] + m1[5] * m2[13] + m1[9] * m2[14] + m1[13] * m2[15];
    result[14] = m1[2] * m2[12] + m1[6] * m2[13] + m1[10] * m2[14] + m1[14] * m2[15];
    result[15] = m1[3] * m2[12] + m1[7] * m2[13] + m1[11] * m2[14] + m1[15] * m2[15];
#endif
}

// 这个函数替代glTranslatef()，用于图像移动
static inline void Matrix3DSetTranslation(Matrix3D matrix, GLfloat xTranslate,
                                          GLfloat yTranslate, GLfloat zTranslate)
{
    matrix[0] = matrix[5] =  matrix[10] = matrix[15] = 1.0;
    matrix[1] = matrix[2] = matrix[3] = matrix[4] = 0.0;
    matrix[6] = matrix[7] = matrix[8] = matrix[9] = 0.0;
    matrix[11] = 0.0;
    matrix[12] = xTranslate;
    matrix[13] = yTranslate;
    matrix[14] = zTranslate;
}

// 这个函数替代glScalef()，用于图像放大缩小
static inline void Matrix3DSetScaling(Matrix3D matrix, GLfloat xScale,
                                      GLfloat yScale, GLfloat zScale)
{
    matrix[1] = matrix[2] = matrix[3] = matrix[4] = 0.0;
    matrix[6] = matrix[7] = matrix[8] = matrix[9] = 0.0;
    matrix[11] = matrix[12] = matrix[13] = matrix[14] = 0.0;
    matrix[0] = xScale;
    matrix[5] = yScale;
    matrix[10] = zScale;
    matrix[15] = 1.0;
}

//-----------------------------------------------------------------------------------
// 绕x轴旋转
static inline void Matrix3DSetXRotationUsingRadians(Matrix3D matrix, GLfloat degrees)
{
    matrix[0] = matrix[15] = 1.0;
    matrix[1] = matrix[2] = matrix[3] = matrix[4] = 0.0;
    matrix[7] = matrix[8] = 0.0;
    matrix[11] = matrix[12] = matrix[13] = matrix[14] = 0.0;
    
    matrix[5] = cosf(degrees);
    matrix[6] = -sinf(degrees);
    matrix[9] = -matrix[6];
    matrix[10] = matrix[5];
}
static inline void Matrix3DSetXRotationUsingDegrees(Matrix3D matrix, GLfloat degrees)
{
    Matrix3DSetXRotationUsingRadians(matrix, degrees * M_PI / 180.0);
}
//-----------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------
// 绕y轴旋转
static inline void Matrix3DSetYRotationUsingRadians(Matrix3D matrix, GLfloat degrees)
{
    matrix[0] = cosf(degrees);
    matrix[2] = sinf(degrees);
    matrix[8] = -matrix[2];
    matrix[10] = matrix[0];
    matrix[1] = matrix[3] = matrix[4] = matrix[6] = matrix[7] = 0.0;
    matrix[9] = matrix[11] = matrix[13] = matrix[12] = matrix[14] = 0.0;
    matrix[5] = matrix[15] = 1.0;
}
static inline void Matrix3DSetYRotationUsingDegrees(Matrix3D matrix, GLfloat degrees)
{
    Matrix3DSetYRotationUsingRadians(matrix, degrees * M_PI / 180.0);
}
//-----------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------
// 绕z轴旋转
static inline void Matrix3DSetZRotationUsingRadians(Matrix3D matrix, GLfloat degrees)
{
    matrix[0] = cosf(degrees);
    matrix[1] = sinf(degrees);
    matrix[4] = -matrix[1];
    matrix[5] = matrix[0];
    matrix[2] = matrix[3] = matrix[6] = matrix[7] = matrix[8] = 0.0;
    matrix[9] = matrix[11] = matrix[12] = matrix[13] = matrix[14] = 0.0;
    matrix[10] = matrix[15] = 1.0;
}
static inline void Matrix3DSetZRotationUsingDegrees(Matrix3D matrix, GLfloat degrees)
{
    Matrix3DSetZRotationUsingRadians(matrix, degrees * M_PI / 180.0);
}
//-----------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------
// 这个函数替代glRotatef()，用于图像旋转
static inline void Matrix3DSetRotationByRadians(Matrix3D matrix, GLfloat angle,
                                                GLfloat x, GLfloat y, GLfloat z)
{
    GLfloat mag = sqrtf((x*x) + (y*y) + (z*z));
    if (mag == 0.0)
    {
        x = 1.0;
        y = 0.0;
        z = 0.0;
    }
    else if (mag != 1.0)
    {
        x /= mag;
        y /= mag;
        z /= mag;
    }
    
    GLfloat c = cosf(angle);
    GLfloat s = sinf(angle);
    matrix[3] = matrix[7] = matrix[11] = matrix[12] = matrix[13] = matrix[14] = 0.0;
    matrix[15] = 1.0;
    
    matrix[0] = (x*x)*(1-c) + c;
    matrix[1] = (y*x)*(1-c) + (z*s);
    matrix[2] = (x*z)*(1-c) - (y*s);
    matrix[4] = (x*y)*(1-c)-(z*s);
    matrix[5] = (y*y)*(1-c)+c;
    matrix[6] = (y*z)*(1-c)+(x*s);
    matrix[8] = (x*z)*(1-c)+(y*s);
    matrix[9] = (y*z)*(1-c)-(x*s);
    matrix[10] = (z*z)*(1-c)+c;
    
}
static inline void Matrix3DSetRotationByDegrees(Matrix3D matrix, GLfloat angle,
                                                GLfloat x, GLfloat y, GLfloat z)
{
    Matrix3DSetRotationByRadians(matrix, angle * M_PI / 180.0, x, y, z);
}
//-----------------------------------------------------------------------------------

#endif
