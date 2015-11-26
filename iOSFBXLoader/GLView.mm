//
//  GLView.m
//  OpenGLES_Class_01_Demo_01
//
//  Created by cai xuejun on 12-8-23.
//  Copyright (c) 2012年 caixuejun. All rights reserved.
//

#import "GLView.h"
#import <UIKit/UIKit.h>
#import <Foundation/Foundation.h>
#import <Availability.h>
#import "GLESUtils.h"
#import "ksMatrix.h"
//#import "CSMath.h"

// 自定义宏，角度到弧度的转换
#define DEGREES_TO_RADIANS(__ANGLE__) ((__ANGLE__) / 180.0 * M_PI)
#define BUFFER_OFFSET(i) ((char *)NULL + (i))
// 这里定义了一个结构体，用于储存顶点位置坐标（x,y,z）和颜色值（RGBA）
typedef struct{
    CGFloat Position[3];
    CGFloat Color[4];
}Vertex;

// 这是一个三角型
const Vertex Vertices[] = {
    {{0, 0.577, 0}, {1, 0, 0, 1}},
    {{0.5, 0, 0}, {0, 0, 1, 1}},
    {{-0.5, 0, 0}, {0, 1, 0, 1}},
};

// 这里存储了顶点的链接顺序，就想画几何图形一样，不同的顺序会勾勒出不同的形状
const GLubyte Indices[]= {
    0, 1, 2,
};

@implementation CLVDrawableVBO



@end



@interface GLView()

@property (nonatomic, assign) GLfloat autoR;
@property (nonatomic, strong) CLVDrawableVBO *renderVBO;

@end

@implementation GLView

// 这个方法不能少，此时的层是OpenGLES层，不再是原来的层
+ (Class)layerClass
{
    return [CAEAGLLayer class];
}

- (id)initWithFrame:(CGRect)frame andFBXLoader:(FBXLoader *)fbx
{
    self = [super initWithFrame:frame];
    if (self) {
        // Initialization code
        // 这里注意调用顺序，还是有一定的逻辑性的，当然可以灵活应用，这么安排只是一个更好的逻辑连贯性
        [self setupLayer];
        [self setupContext];
        [self setupProgram];
        [self setupProjection];
        [self setupLights];
        _fbxLoader = fbx;
        _modelViewMatrix = GLKMatrix4Identity;
        _modelViewMatrix =  GLKMatrix4MakeTranslation(0, 0, -2.0);  //GLKMatrix4MakeRotation(rot, 1.0, 0, 0);//(rr, rot, 1.0, 0.0, 0.0);
        //[self setupDepthBuffer];
        //[self setupRenderBuffer];
        //[self setupFrameBuffer];
        //[self setupVBOs];
        //[self setupDisplayLink];
    }
    return self;
}

- (void)setupLayer
{
    _eaglLayer = (CAEAGLLayer*) self.layer;
    
    // CALayer 默认是透明的，必须将它设为不透明才能让其可见
    _eaglLayer.opaque = YES;
    
    // 设置描绘属性，在这里设置不维持渲染内容以及颜色格式为 RGBA8
    _eaglLayer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:
                                    [NSNumber numberWithBool:NO], kEAGLDrawablePropertyRetainedBacking, kEAGLColorFormatRGBA8, kEAGLDrawablePropertyColorFormat, nil];
}

- (void)setupContext
{
    // 我们这本Demo是用的1.x版本
    _context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
    // 容错处理
    if (!_context || ![EAGLContext setCurrentContext:_context]) {
        //[self release];
        return;
    }
}

// 设置渲染缓冲区


// 设置深度缓冲区,其实你仅仅是绘制2D图形的话，这个缓冲区可以忽略




- (void)setupProgram
{
    // Load shaders
    NSString * vertexShaderPath = [[NSBundle mainBundle] pathForResource:@"VertexShader"
                                                                  ofType:@"glsl"];
    NSString * fragmentShaderPath = [[NSBundle mainBundle] pathForResource:@"FragmentShader"
                                                                    ofType:@"glsl"];
    // Create program, attach shaders, compile and link program
    _programHandle = [GLESUtils loadProgram:vertexShaderPath
                 withFragmentShaderFilepath:fragmentShaderPath];
    if (_programHandle == 0) {
        NSLog(@" >> Error: Failed to setup program.");
        return;
    }
    glUseProgram(_programHandle);
    
    [self getSlotsFromProgram];
}

- (void)getSlotsFromProgram {
    // Get attribute slot from program
    _projectionSlot     = glGetUniformLocation(_programHandle, "projection");
    _modelViewSlot      = glGetUniformLocation(_programHandle, "modelView");
    _normalMatrixSlot   = glGetUniformLocation(_programHandle, "normalMatrix");
    _lightPositionSlot  = glGetUniformLocation(_programHandle, "vLightPosition");
    _ambientSlot        = glGetUniformLocation(_programHandle, "vAmbientMaterial");
    _specularSlot       = glGetUniformLocation(_programHandle, "vSpecularMaterial");
    _shininessSlot      = glGetUniformLocation(_programHandle, "shininess");
    _positionSlot       = glGetAttribLocation(_programHandle, "vPosition");
    _normalSlot         = glGetAttribLocation(_programHandle, "vNormal");
    _diffuseSlot        = glGetAttribLocation(_programHandle, "vDiffuseMaterial");
}

-(void)setupProjection
{
    // Generate a perspective matrix with a 60 degree FOV
    float aspect = self.frame.size.width / self.frame.size.height;
    _projectionMatrix = GLKMatrix4Identity;
    _projectionMatrix = GLKMatrix4MakePerspective(80.0*M_PI/180, aspect, 0.5f, 200.0f);
    // Load projection matrix
    //glUniformMatrix4fv(_projectionSlot, 1, GL_FALSE, (GLfloat*)&_projectionMatrix.m[0][0]);
    glUniformMatrix4fv(_projectionSlot, 1,GL_FALSE, &_projectionMatrix.m[0]);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    
}

- (void)setupLights
{
    // Initialize various state.
    //
    glEnableVertexAttribArray(_positionSlot);
    glEnableVertexAttribArray(_normalSlot);
    
    // Set up some default material parameters.
    //
    _lightPosition.x = 100.0;
    _lightPosition.y = 100.0;
    _lightPosition.z = 100.0;
    
    _ambient.r = 0.3;
    _ambient.g = 0.3;
    _ambient.b = 0.3;
    _specular.r = 0.1;
    _specular.g = 0.1;
    _specular.b = 0.1;
    
    _diffuse.r = 0.0;
    _diffuse.g = 0.9;
    _diffuse.b = 0.0;
    
    _shininess = 0.1;
}

- (void)updateLights
{
    glUniform3f(_lightPositionSlot, _lightPosition.x, _lightPosition.y, _lightPosition.z);
    glUniform4f(_ambientSlot, _ambient.r, _ambient.g, _ambient.b, _ambient.a);
    glUniform4f(_specularSlot, _specular.r, _specular.g, _specular.b, _specular.a);
    glVertexAttrib4f(_diffuseSlot, _diffuse.r, _diffuse.g, _diffuse.b, _diffuse.a);
    glUniform1f(_shininessSlot, _shininess);
}

- (void)setupRenderBuffer
{
    glGenRenderbuffers(1, &viewRenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, viewRenderbuffer);
    [_context renderbufferStorage:GL_RENDERBUFFER fromDrawable:_eaglLayer];
}
- (void)setupDepthBuffer
{
    //glGenRenderbuffersOES(1, &depthRenderbuffer);
    //glBindRenderbufferOES(GL_RENDERBUFFER_OES, depthRenderbuffer);
    //glRenderbufferStorageOES(GL_RENDERBUFFER_OES, GL_DEPTH_COMPONENT16_OES, self.frame.size.width, self.frame.size.height);// 这个深度缓冲是协同渲染一起工作的
    int width, height;
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &width);
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &height);
    glGenRenderbuffers(1, &depthRenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthRenderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                              GL_RENDERBUFFER, depthRenderbuffer);
}
// 设置帧缓冲区
- (void)setupFrameBuffer
{
    glGenFramebuffers(1, &viewFramebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, viewFramebuffer);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                              GL_RENDERBUFFER, viewRenderbuffer);
}

- (void)setupBuffer
{
        // Setup color render buffer
    //
    glGenRenderbuffers(1, &viewRenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, viewRenderbuffer);
    [_context renderbufferStorage:GL_RENDERBUFFER fromDrawable:_eaglLayer];
    
    // Setup depth render buffer
    //
   
    int width, height;
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &width);
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &height);
    
    // Create a depth buffer that has the same size as the color buffer.
    glGenRenderbuffers(1, &depthRenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthRenderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width, height);
   
    // Setup frame buffer
    //
    glGenFramebuffers(1, &viewFramebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, viewFramebuffer);
    
    // Attach color render buffer and depth render buffer to frameBuffer
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                              GL_RENDERBUFFER, viewRenderbuffer);
    
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                              GL_RENDERBUFFER, depthRenderbuffer);
    
    // Set color render buffer as current render buffer
    //
    glBindRenderbuffer(GL_RENDERBUFFER, viewRenderbuffer);

}

- (void)destoryBuffers
{
    glDeleteBuffers(1, &depthRenderbuffer);
    depthRenderbuffer = 0;
    glDeleteRenderbuffers(1, &viewRenderbuffer);
    viewRenderbuffer = 0;
    glDeleteFramebuffers(1, &viewFramebuffer);
    viewFramebuffer = 0;
}

/*
- (void)setupVBOs
{
	glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // 设置视口，一般就是我们屏幕的大小，它就是最终影像投放区域的大小
    CGRect rect = self.bounds;
	glViewport(0, 0, CGRectGetWidth(rect), CGRectGetHeight(rect));
    // 切换到模型模式，接下去就是给我们的模型定型，关于对模型的操作基本上都是在这个模式下进行的
	glMatrixMode(GL_MODELVIEW);
}
*/
- (void)render
{
    glClearColor(.4, .4, .4, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
    glViewport(0, 0, self.frame.size.width, self.frame.size.height);
    
  
    //[self drawGL];
    
    [self updateSurface];
    [self drawGLWithVBO];
    
    [_context presentRenderbuffer:GL_RENDERBUFFER];
}
/*
- (CLVDrawableVBO *) CreatVBO:(FBXLoader *)fbxLoader {
   
    GLsizeiptr sizeofVectex = fbxLoader->modelPositionList.size() * 4 * sizeof(float);
    GLsizeiptr tt = sizeof(GLKVector4);
    GLsizeiptr sizeofIndex = fbxLoader->modelIndexList.size() * sizeof(int);
    
    GLuint vectexBuffer;
    glGenBuffers(1, &vectexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vectexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeofVectex, fbxLoader->modelPositionList.data(), GL_STATIC_DRAW);
    
    
    std::vector<GLuint> ind;
    for(int i = 0;i<(int)_fbxLoader->modelIndexList.size();i++) {
        ind.push_back((GLuint)i);
    }
    
    GLuint indexBuffer;
    glGenBuffers(1, &indexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeofIndex, ind.data(), GL_STATIC_DRAW);
    
    CLVDrawableVBO *vbo = [[CLVDrawableVBO alloc]init];
    vbo.vertexBuffer = vectexBuffer;
    vbo.triangleIndexBuffer = indexBuffer;
    vbo.vertexSize = (GLuint)fbxLoader->modelPositionList.size();
    vbo.triangleIndexSize = (GLuint) fbxLoader->modelIndexList.size();
    return vbo;
}

- (void) drawGL {
    // Setup viewport
    GLfloat vertices[] = {
        0.0f,  0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f,
        0.5f,  -0.5f, 0.0f };
    GLKVector3 glkv3;
    std::vector<GLKVector3> vGLKV3;
    vGLKV3.push_back(GLKVector3Make(0.0f, 0.5f, 0.0f));
    vGLKV3.push_back(GLKVector3Make(-0.5f, -0.5f, 0.0f));
    vGLKV3.push_back(GLKVector3Make(0.5f, -0.5f, 0.0f));
   
    glVertexAttribPointer(_positionSlot, 4, GL_FLOAT, GL_FALSE, 0, _fbxLoader->modelPositionList.data());
    glEnableVertexAttribArray(_positionSlot);
    //glDrawArrays(GL_LINE_LOOP, 0, sizeof(_fbxLoader->modelPositionList));
   
    NSLog(@"%lu",sizeof(_fbxLoader->modelIndexList)/sizeof(GLubyte));
    NSLog(@"%lu",sizeof(_fbxLoader->modelIndexList));
    NSLog(@"%lu",sizeof(GLubyte));
    
    std::vector<GLuint> ind;
    for(int i = 0;i<(int)_fbxLoader->modelIndexList.size();i++) {
        ind.push_back((GLuint)i);
    }
    glDrawElements(GL_LINES, (int)_fbxLoader->modelIndexList.size() , GL_UNSIGNED_INT, ind.data());
}
*/

- (CLVDrawableVBO *) CreatVBO:(FBXLoader *)fbxLoader {
   
//    GLsizeiptr sizeofVectex = fbxLoader->modelPositionList.size() * 4 * sizeof(float);
    //GLsizeiptr sizeofIndex = fbxLoader->modelIndexList.size() * sizeof(int);
    GLsizeiptr sizeofVectex = fbxLoader->getMesh(0).vertexList.size()*sizeof(ModelVertex);
    GLsizeiptr sizeofIndex = fbxLoader->getMesh(0).indexList.size()*sizeof(int);
    
    
    GLuint vectexBuffer;
    glGenBuffers(1, &vectexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vectexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeofVectex, fbxLoader->getMesh(0).vertexList.data(), GL_STATIC_DRAW);
    /*
    GLuint normalBuffer;
    glGenBuffers(1, &normalBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeofVectex, fbxLoader->getMesh(0).vertexList.data(), GL_STATIC_DRAW);

    GLuint uv0Buffer;
    glGenBuffers(1, &uv0Buffer);
    glBindBuffer(GL_ARRAY_BUFFER, uv0Buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeofVectex, fbxLoader->getMesh(0).vertexList.data(), GL_STATIC_DRAW);
    */
   
    GLuint indexBuffer;
    glGenBuffers(1, &indexBuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    if(!fbxLoader->getMesh(0).allByControlPoint) {
        std::vector<GLuint> ind;
        for(int i = 0;i<(int)fbxLoader->getMesh(0).indexList.size();i++) {
            ind.push_back((GLuint)i);
        }
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeofIndex, ind.data(), GL_STATIC_DRAW);
    } else {
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeofIndex, fbxLoader->getMesh(0).indexList.data(), GL_STATIC_DRAW);
    }
    
    

    CLVDrawableVBO *vbo = [[CLVDrawableVBO alloc]init];
    vbo.vertexBuffer = vectexBuffer;
    vbo.triangleIndexBuffer = indexBuffer;
    vbo.vertexSize = (GLuint)fbxLoader->getMesh(0).vertexList.size();
    vbo.triangleIndexSize = (GLuint) fbxLoader->getMesh(0).indexList.size();
    //vbo.normalBuffer = normalBuffer;
    //vbo.uv0Buffer = uv0Buffer;
    
    return vbo;
}

- (void) updateVBOWithFBX:(FBXLoader *)fbxLoader  {
    
    GLsizeiptr sizeofVectex = fbxLoader->getMesh(0).vertexList.size()*sizeof(ModelVertex);
    glBindBuffer(GL_ARRAY_BUFFER, [_renderVBO vertexBuffer]);
    glBufferData(GL_ARRAY_BUFFER, sizeofVectex, fbxLoader->getMesh(0).vertexList.data(), GL_STATIC_DRAW);

}

- (void)drawGLWithVBO {
    if(!_renderVBO)
        return;
    glBindBuffer(GL_ARRAY_BUFFER, [_renderVBO vertexBuffer]);
    glVertexAttribPointer(_positionSlot, 4, GL_FLOAT, GL_FALSE, sizeof(ModelVertex), BUFFER_OFFSET(offsetof(ModelVertex, position)));
    glVertexAttribPointer(_normalSlot, 3, GL_FLOAT, GL_FALSE, sizeof(ModelVertex), BUFFER_OFFSET(offsetof(ModelVertex, normal)));
    //glEnableVertexAttribArray(_positionSlot);
  
    /*
    glBindBuffer(GL_ARRAY_BUFFER, [_renderVBO vertexBuffer]);
    glVertexAttribPointer(_positionSlot, 3, GL_FLOAT, GL_FALSE, sizeof(ModelVertex), BUFFER_OFFSET(offsetof(ModelVertex, normal)));
    glEnableVertexAttribArray(_positionSlot);
    */
    //glBindBuffer(GL_ARRAY_BUFFER, [_renderVBO uv0Buffer]);
    //glVertexAttribPointer(_positionSlot, 2, GL_FLOAT, GL_FALSE, sizeof(ModelVertex), BUFFER_OFFSET(offsetof(ModelVertex, uv0)));
    //glEnableVertexAttribArray(_positionSlot);
    
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, [_renderVBO triangleIndexBuffer]);
    glDrawElements(GL_TRIANGLES, [_renderVBO triangleIndexSize], GL_UNSIGNED_INT, 0);
}

- (void)updateTransform
{
    static CGFloat rot = 0;

    _modelViewMatrix = GLKMatrix4Identity;
    _modelViewMatrix  = GLKMatrix4Scale(_modelViewMatrix, _modelScale, _modelScale, 1.0);
    _modelViewMatrix = GLKMatrix4Translate(_modelViewMatrix, 0, 0, _modelZ);
    
    //_modelViewMatrix =  GLKMatrix4MakeTranslation(0, 0, 2);  //GLKMatrix4MakeRotation(rot, 1.0, 0, 0);//(rr, rot, 1.0, 0.0, 0.0);
    //_modelViewMatrix = GLKMatrix4MakeXRotation(rot);
    _modelViewMatrix = GLKMatrix4Rotate(_modelViewMatrix, _modelRotate*M_PI/180.0 , 1, 0, 0);
    //_modelViewMatrix = GLKMatrix4Rotate(_modelViewMatrix, _modelRotate , 0, 1, 0);
    //_modelViewMatrix = GLKMatrix4Multiply(_modelViewMatrix, rr);
    
    //NSLog(@"%@",NSStringFromGLKMatrix4(_modelViewMatrix));
    rot +=2;
}

- (void)updateSurface
{
    //ksMatrixLoadIdentity(&_modelViewMatrix);
    
    //ksTranslate(&_modelViewMatrix, 0.0, 0.0, -8);
    
    //ksMatrixMultiply(&_modelViewMatrix, &_rotationMatrix, &_modelViewMatrix);
    
    // Load the model-view matrix
    //glUniformMatrix4fv(_modelViewSlot, 1, GL_FALSE, (GLfloat*)&_modelViewMatrix.m[0][0]);
    
    // Load the normal matrix.
    // It's orthogonal, so its Inverse-Transpose is itself!
    //
    //_uniformVs.normalMatrix = GLKMatrix4InvertAndTranspose(modelMatrix, NULL);
    
    glUniformMatrix4fv(_modelViewSlot, 1, GL_FALSE, &_modelViewMatrix.m[0]);
    GLKMatrix3 normalMatrix3 = GLKMatrix4GetMatrix3(_modelViewMatrix);
    //GLKMatrix3 normalMatrix3 = GLKMatrix4GetMatrix3(_modelViewMatrix, YES);
    //ksMatrix4ToMatrix3(&normalMatrix3, &_modelViewMatrix);
    glUniformMatrix3fv(_normalMatrixSlot, 1, GL_FALSE, &normalMatrix3.m[0]);
    [self updateLights];
}

// 让它按设备帧率进行渲染
- (void)layoutSubviews
{
    [EAGLContext setCurrentContext:_context];

    [self destoryBuffers];
    
   // [self setupRenderBuffer];
   // [self setupDepthBuffer];
   // [self setupFrameBuffer];
    [self setupBuffer];
    
    [self updateTransform];
    
    _renderVBO = [self CreatVBO:_fbxLoader];
    
    [self render];
}

- (void)refreshDisp:(CADisplayLink *)displayLink {
    _autoR += displayLink.duration * 90;
    
    [self updateTransform];
    
    [self render];
}

- (void) updateDisplay {
    self.fbxLoader->updateMash();
    [self updateVBOWithFBX:_fbxLoader];
    [self updateTransform];
    [self render];
}

- (void)setupDisplayLink {
    CADisplayLink* displayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(refreshDisp:)];
    [displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
}

- (void) setModelZ:(GLfloat)modelZ {
    _modelZ = modelZ;
    //[self updateTransform];
    //[self render];
}

- (void) setModelScale:(GLfloat)modelScale {
    _modelScale = modelScale;
    //[self updateTransform];
    //[self render];
}

- (void) setModelRotate:(GLfloat)modelRotate {
    _modelRotate = modelRotate;
    //[self updateTransform];
    //[self render];
}

- (void) setLightPosition:(GLKVector3)lightPosition {
    _lightPosition = lightPosition;
    //[self updateTransform];
    //[self render];
}

- (void) setAmbient:(GLKVector4)ambient {
    _ambient = ambient;
    //[self updateTransform];
    //[self render];
}

- (void) setDiffuse:(GLKVector4)diffuse {
    _diffuse = diffuse;
    //[self updateTransform];
    //[self render];
}

- (void) setSpecular:(GLKVector4)specular {
    _specular = specular;
    //[self updateTransform];
    //[self render];
}

- (void) setShininess:(GLfloat)shininess {
    _shininess = shininess;
    //[self updateTransform];
    //[self render];
}

@end
