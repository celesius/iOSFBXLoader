//
//  GLView.h
//  OpenGLES_Class_01_Demo_01
//
//  Created by cai xuejun on 12-8-23.
//  Copyright (c) 2012年 caixuejun. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <QuartzCore/QuartzCore.h>
#import <OpenGLES/EAGL.h>
#import <OpenGLES/ES2/gl.h>
#import <OpenGLES/ES2/glext.h>
#import <GLKit/GLKit.h>
#import "FBXLoader.hpp"

@interface CLVDrawableVBO : NSObject

@property (nonatomic, assign) GLuint vertexBuffer;
@property (nonatomic, assign) GLuint triangleIndexBuffer;
@property (nonatomic, assign) GLuint normalBuffer;
@property (nonatomic, assign) GLuint uv0Buffer;

@property (nonatomic, assign) GLuint vertexSize;
@property (nonatomic, assign) GLuint triangleIndexSize;
@property (nonatomic, assign) GLuint normalSize;
@property (nonatomic, assign) GLuint uv0Size;

@end

@interface GLView : UIView
{
    CAEAGLLayer *_eaglLayer;// OpenGL层，这是用来渲染显示等等的
    EAGLContext *_context;// 环境上下文
    
    GLuint viewRenderbuffer;// 渲染缓冲区
    GLuint viewFramebuffer;// 帧缓冲区
    GLuint depthRenderbuffer;
    //GLuint depthRenderbuffer;// 深度缓冲区，启用这个缓冲是让图像具有3D的效果 2.0已经不需要这个了，2.0要自己写fov。
    
    GLKMatrix4 _projectionMatrix;
    GLKMatrix4 _modelViewMatrix;
    GLuint _programHandle;
    
    GLuint _positionSlot;
    GLuint _projectionSlot;
    GLuint _modelViewSlot;
    GLuint _normalMatrixSlot;
    GLuint _lightPositionSlot;
    
    GLint _normalSlot;
    GLint _ambientSlot;
    GLint _diffuseSlot;
    GLint _specularSlot;
    GLint _shininessSlot;
    
    
}
- (id)initWithFrame:(CGRect)frame andFBXLoader:(FBXLoader *)fbx;
- (void) updateDisplay;

@property (nonatomic, assign) FBXLoader *fbxLoader;
@property (nonatomic, assign) GLfloat modelZ;
@property (nonatomic, assign) GLfloat modelScale;
@property (nonatomic, assign) GLfloat modelRotate;

@property (nonatomic, assign) GLKVector3 lightPosition;
@property (nonatomic, assign) GLKVector4 ambient;
@property (nonatomic, assign) GLKVector4 diffuse;
@property (nonatomic, assign) GLKVector4 specular;
@property (nonatomic, assign) GLfloat    shininess;




@end
