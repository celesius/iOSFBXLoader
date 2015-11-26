//
//  ViewController.m
//  iOSFBXLoader
//
//  Created by vk on 15/11/11.
//  Copyright © 2015年 clover. All rights reserved.
//

#import "ViewController.h"
#import "GLView.h"
#include "FBXLoader.hpp"

@interface ViewController ()

@property (nonatomic, assign) FBXLoader *fbxloader;
@property (nonatomic, strong) UISlider *sliderZ;
@property (nonatomic, strong) GLView *glv;
@property (nonatomic, assign) GLKVector3 lightPoint;
@property (nonatomic, assign) GLKVector4 lightAmbient;
@property (nonatomic, assign) GLKVector4 lightDiffuse;
@property (nonatomic, assign) GLKVector4 lightSpecular;
@property (nonatomic, assign) GLfloat lightShininess;
@property (nonatomic, assign) float frame;

@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    //self.view.backgroundColor = [UIColor cyanColor];
    NSLog(@" vc bound %@",  NSStringFromCGRect([UIScreen mainScreen].bounds));
   // NSString *fbxPath = [[NSBundle mainBundle] pathForResource:@"resource/ExportScene01" ofType:@"fbx"];
    NSString *fbxPath = [[NSBundle mainBundle] pathForResource:@"resource/humanoid" ofType:@"fbx"];
    _fbxloader = new FBXLoader(fbxPath.UTF8String);

    _glv = [[GLView alloc]initWithFrame:[UIScreen mainScreen].bounds andFBXLoader:_fbxloader];
    [self.view addSubview:_glv];
    
    _sliderZ = [[UISlider alloc]initWithFrame:CGRectMake(0, CGRectGetMidY(self.view.bounds), CGRectGetMidX(self.view.bounds)- 50, 30)];
    [_sliderZ setMinimumValue:1];
    [_sliderZ setMaximumValue:199];
    [_sliderZ addTarget:self action:@selector(changeZ:) forControlEvents:UIControlEventValueChanged];
    [self.view addSubview:_sliderZ];
    [_sliderZ setValue:10];
    [_glv setModelZ:-_sliderZ.value];
    
    UILabel *modelZLabel = [[UILabel alloc]init];
    [self layoutLabel:modelZLabel withRefObj:_sliderZ];
    modelZLabel.text = @"Z轴";
    [self.view addSubview:modelZLabel];
    
    UISlider *sliderScale = [[UISlider alloc]initWithFrame:CGRectMake(CGRectGetMinX(_sliderZ.frame), CGRectGetMaxY(_sliderZ.frame) , CGRectGetWidth(_sliderZ.frame), CGRectGetHeight(_sliderZ.frame))];
    [sliderScale setMinimumValue:1];
    [sliderScale setMaximumValue:20];
    [sliderScale addTarget:self action:@selector(changeScale:) forControlEvents:UIControlEventValueChanged];
    [self.view addSubview:sliderScale];
    [sliderScale setValue:10];
    [_glv setModelScale:sliderScale.value/10.0];
   
    UILabel *sliderScaleLabel = [[UILabel alloc]init];
    [self layoutLabel:sliderScaleLabel withRefObj:sliderScale];
    sliderScaleLabel.text = @"缩放";
    [self.view addSubview:sliderScaleLabel];
    
    UISlider *sliderRotate = [[UISlider alloc]init];
    [self layoutSlider:sliderRotate withRefObj:sliderScale];
    [sliderRotate setMinimumValue:0];
    [sliderRotate setMaximumValue:359];
    [sliderRotate addTarget:self action:@selector(changeRotate:) forControlEvents:UIControlEventValueChanged];
    [self.view addSubview:sliderRotate];
    [sliderRotate setValue:0];
    
    //light test
    UISlider *sliderLightPosionX = [[UISlider alloc]init];
    [self layoutSlider:sliderLightPosionX withRefObj:sliderRotate];
    [sliderLightPosionX setMinimumValue:-100.0];
    [sliderLightPosionX setMaximumValue:100.0];
    [sliderLightPosionX addTarget:self action:@selector(changeLightPosionX:) forControlEvents:UIControlEventValueChanged];
    [self.view addSubview:sliderLightPosionX];
    [sliderLightPosionX setValue:0.0];
    _lightPoint.x = sliderLightPosionX.value;
    
    UILabel *lightPosionX = [[UILabel alloc]init];
    [self layoutLabel:lightPosionX withRefObj:sliderLightPosionX];
    lightPosionX.text = @"LX";
    [self.view addSubview:lightPosionX];
   
    UISlider *sliderLightPosionY = [[UISlider alloc]init];
    [self layoutSlider:sliderLightPosionY withRefObj:sliderLightPosionX];
    [sliderLightPosionY setMinimumValue:-100.0];
    [sliderLightPosionY setMaximumValue:100.0];
    [sliderLightPosionY addTarget:self action:@selector(changeLightPosionY:) forControlEvents:UIControlEventValueChanged];
    [self.view addSubview:sliderLightPosionY];
    [sliderLightPosionY setValue:0.0];
    _lightPoint.y = sliderLightPosionY.value;

    UILabel *lightPosionY = [[UILabel alloc]init];
    [self layoutLabel:lightPosionY withRefObj:sliderLightPosionY];
    lightPosionY.text = @"LY";
    [self.view addSubview:lightPosionY];
    
    UISlider *sliderLightPosionZ = [[UISlider alloc]init];
    [self layoutSlider:sliderLightPosionZ withRefObj:sliderLightPosionY];
    [sliderLightPosionZ setMinimumValue:-100.0];
    [sliderLightPosionZ setMaximumValue:100.0];
    [sliderLightPosionZ addTarget:self action:@selector(changeLightPosionZ:) forControlEvents:UIControlEventValueChanged];
    [self.view addSubview:sliderLightPosionZ];
    [sliderLightPosionZ setValue:0.0];
    _lightPoint.z = sliderLightPosionZ.value;

    UILabel *lightPosionZ = [[UILabel alloc]init];
    [self layoutLabel:lightPosionZ withRefObj:sliderLightPosionZ];
    lightPosionZ.text = @"LZ";
    [self.view addSubview:lightPosionZ];
    
    //环境光
    UISlider *sliderLightAmbientR = [[UISlider alloc] initWithFrame:CGRectMake(CGRectGetMidX(self.view.bounds), CGRectGetMinY(_sliderZ.frame), CGRectGetWidth(_sliderZ.frame), CGRectGetHeight(_sliderZ.frame))];
    [sliderLightAmbientR setMinimumValue:0.0];
    [sliderLightAmbientR setMaximumValue:1.0];
    [sliderLightAmbientR addTarget:self action:@selector(changeLightAmbientR:) forControlEvents:UIControlEventValueChanged];
    [self.view addSubview:sliderLightAmbientR];
    [sliderLightAmbientR setValue:0.1];
    _lightAmbient.r = sliderLightAmbientR.value;
    UILabel *lightAmbientR = [[UILabel alloc]init];
    [self layoutLabel:lightAmbientR withRefObj:sliderLightAmbientR];
    lightAmbientR.text = @"AR";
    [self.view addSubview:lightAmbientR];
   
    UISlider *sliderLightAmbientG = [[UISlider alloc] init];
    [self layoutSlider:sliderLightAmbientG withRefObj:sliderLightAmbientR];
    [sliderLightAmbientG setMinimumValue:0.0];
    [sliderLightAmbientG setMaximumValue:1.0];
    [sliderLightAmbientG addTarget:self action:@selector(changeLightAmbientG:) forControlEvents:UIControlEventValueChanged];
    [self.view addSubview:sliderLightAmbientG];
    [sliderLightAmbientG setValue:0.1];
    _lightAmbient.g = sliderLightAmbientG.value;
    UILabel *lightAmbientG = [[UILabel alloc]init];
    [self layoutLabel:lightAmbientG withRefObj:sliderLightAmbientG];
    lightAmbientG.text = @"AG";
    [self.view addSubview:lightAmbientG];
    
    UISlider *sliderLightAmbientB = [[UISlider alloc] init];
    [self layoutSlider:sliderLightAmbientB withRefObj:sliderLightAmbientG];
    [sliderLightAmbientB setMinimumValue:0.0];
    [sliderLightAmbientB setMaximumValue:1.0];
    [sliderLightAmbientB addTarget:self action:@selector(changeLightAmbientB:) forControlEvents:UIControlEventValueChanged];
    [self.view addSubview:sliderLightAmbientB];
    [sliderLightAmbientB setValue:0.1];
    _lightAmbient.b = sliderLightAmbientB.value;
    UILabel *lightAmbientB = [[UILabel alloc]init];
    [self layoutLabel:lightAmbientB withRefObj:sliderLightAmbientB];
    lightAmbientB.text = @"AB";
    [self.view addSubview:lightAmbientB];
    _lightAmbient.a = 0.0;
    
    //漫反射光 R
    UISlider *sliderLightDiffuseR = [[UISlider alloc]init];
    [self layoutSlider:sliderLightDiffuseR withRefObj:sliderLightAmbientB];
    [sliderLightDiffuseR setMinimumValue:0.0];
    [sliderLightDiffuseR setMaximumValue:1.0];
    [sliderLightDiffuseR addTarget:self action:@selector(changeLightDiffuseR:) forControlEvents:UIControlEventValueChanged];
    [self.view addSubview:sliderLightDiffuseR];
    [sliderLightDiffuseR setValue:0.0];
    _lightDiffuse.r = sliderLightDiffuseR.value;
    UILabel *lightDiffuseR = [[UILabel alloc]init];
    [self layoutLabel:lightDiffuseR withRefObj:sliderLightDiffuseR];
    lightDiffuseR.text = @"DR";
    [self.view addSubview:lightDiffuseR];
    
    UISlider *sliderLightDiffuseG = [[UISlider alloc]init];
    [self layoutSlider:sliderLightDiffuseG withRefObj:sliderLightDiffuseR];
    [sliderLightDiffuseG setMinimumValue:0.0];
    [sliderLightDiffuseG setMaximumValue:1.0];
    [sliderLightDiffuseG addTarget:self action:@selector(changeLightDiffuseG:) forControlEvents:UIControlEventValueChanged];
    [self.view addSubview:sliderLightDiffuseG];
    [sliderLightDiffuseG setValue:0.0];
    _lightDiffuse.g = sliderLightDiffuseG.value;
    UILabel *lightDiffuseG = [[UILabel alloc]init];
    [self layoutLabel:lightDiffuseG withRefObj:sliderLightDiffuseG];
    lightDiffuseG.text = @"DG";
    [self.view addSubview:lightDiffuseG];
    
    UISlider *sliderLightDiffuseB = [[UISlider alloc]init];
    [self layoutSlider:sliderLightDiffuseB withRefObj:sliderLightDiffuseG];
    [sliderLightDiffuseB setMinimumValue:0.0];
    [sliderLightDiffuseB setMaximumValue:1.0];
    [sliderLightDiffuseB addTarget:self action:@selector(changeLightDiffuseB:) forControlEvents:UIControlEventValueChanged];
    [self.view addSubview:sliderLightDiffuseB];
    [sliderLightDiffuseB setValue:0.0];
    _lightDiffuse.b = sliderLightDiffuseB.value;
    UILabel *lightDiffuseB = [[UILabel alloc]init];
    [self layoutLabel:lightDiffuseB withRefObj:sliderLightDiffuseB];
    lightDiffuseB.text = @"DB";
    [self.view addSubview:lightDiffuseB];
    _lightDiffuse.a = 0;
    
    //镜面高光 specular
    UISlider *sliderLightSpecularR = [[UISlider alloc]init];
    [self layoutSlider:sliderLightSpecularR withRefObj:sliderLightPosionZ];
    [sliderLightSpecularR setMinimumValue:0.0];
    [sliderLightSpecularR setMaximumValue:1.0];
    [sliderLightSpecularR addTarget:self action:@selector(changeLightSpecularR:) forControlEvents:UIControlEventValueChanged];
    [self.view addSubview:sliderLightSpecularR];
    [sliderLightSpecularR setValue:0.0];
    _lightSpecular.r = sliderLightSpecularR.value;
    UILabel *lightSpecularR = [[UILabel alloc]init];
    [self layoutLabel:lightSpecularR withRefObj:sliderLightSpecularR];
    lightSpecularR.text = @"SR";
    [self.view addSubview:lightSpecularR];
    
    UISlider *sliderLightSpecularG = [[UISlider alloc]init];
    [self layoutSlider:sliderLightSpecularG withRefObj:sliderLightSpecularR];
    [sliderLightSpecularG setMinimumValue:0.0];
    [sliderLightSpecularG setMaximumValue:1.0];
    [sliderLightSpecularG addTarget:self action:@selector(changeLightSpecularG:) forControlEvents:UIControlEventValueChanged];
    [self.view addSubview:sliderLightSpecularG];
    [sliderLightSpecularG setValue:0.0];
    _lightSpecular.g = sliderLightSpecularG.value;
    UILabel *lightSpecularG = [[UILabel alloc]init];
    [self layoutLabel:lightSpecularG withRefObj:sliderLightSpecularG];
    lightSpecularG.text = @"SG";
    [self.view addSubview:lightSpecularG];
    
    UISlider *sliderLightSpecularB = [[UISlider alloc]init];
    [self layoutSlider:sliderLightSpecularB withRefObj:sliderLightSpecularG];
    [sliderLightSpecularB setMinimumValue:0.0];
    [sliderLightSpecularB setMaximumValue:1.0];
    [sliderLightSpecularB addTarget:self action:@selector(changeLightSpecularB:) forControlEvents:UIControlEventValueChanged];
    [self.view addSubview:sliderLightSpecularB];
    [sliderLightSpecularB setValue:0.0];
    _lightSpecular.b = sliderLightSpecularB.value;
    UILabel *lightSpecularB = [[UILabel alloc]init];
    [self layoutLabel:lightSpecularB withRefObj:sliderLightSpecularB];
    lightSpecularB.text = @"SB";
    [self.view addSubview:lightSpecularB];
    _lightSpecular.a = 0.0;
    
    //光泽度 shininess 决定 高光程度
    UISlider *sliderLightsShininess = [[UISlider alloc]init];
    [self layoutSlider:sliderLightsShininess withRefObj:sliderLightDiffuseB];
    [sliderLightsShininess setMinimumValue:0.0];
    [sliderLightsShininess setMaximumValue:10.0];
    [sliderLightsShininess addTarget:self action:@selector(changeLightShiniess:) forControlEvents:UIControlEventValueChanged];
    [self.view addSubview:sliderLightsShininess];
    [sliderLightsShininess setValue:5.0];
    _lightShininess = sliderLightsShininess.value;
    UILabel *lightShininess = [[UILabel alloc]init];
    [self layoutLabel:lightShininess withRefObj:sliderLightsShininess];
    lightShininess.text = @"LS";
    [self.view addSubview:lightShininess];
    
    self.preferredFramesPerSecond = 60.0;
    // Do any additional setup after loading the view, typically from a nib.
}

- (void)update {
    _frame += self.timeSinceLastUpdate * 60.0f;
    if(_frame >= self.glv.fbxLoader->GetAnimationEndFrame()) {
        _frame -= self.glv.fbxLoader->GetAnimationEndFrame();
    }
    NSLog(@" _frame = %f ",_frame);
    
    //self.glv.fbxLoader->updateMash();
    [self.glv updateDisplay];
    
    //[self _glv.fbxloader->update()];
    
}

- (void) layoutLabel:(UILabel *)label withRefObj:(UISlider *) refObj {
//    label.frame = CGRectMake(CGRectGetMaxX(refObj.frame) + 5, CGRectGetMinY(refObj.frame), CGRectGetWidth(self.view.bounds) - ( CGRectGetMaxX(refObj.frame)  ) , 30);
    label.frame = CGRectMake(CGRectGetMaxX(refObj.frame) + 5, CGRectGetMinY(refObj.frame), 40, 30);
    label.textAlignment = NSTextAlignmentCenter;
}

- (void) layoutSlider:(UISlider*)slider withRefObj:(UISlider *)refObj {
    slider.frame = CGRectMake(CGRectGetMinX(refObj.frame), CGRectGetMaxY(refObj.frame), CGRectGetWidth(refObj.frame), CGRectGetHeight(refObj.frame));
}

- (void)changeZ:(id)sender {
    UISlider *slider = (UISlider *)sender;
    [_glv setModelZ:-slider.value];
}

- (void)changeScale:(id)sender {
    UISlider *slider = (UISlider *)sender;
    [_glv setModelScale:slider.value/10.0];
}

- (void)changeRotate:(id)sender {
    UISlider *slider = (UISlider *)sender;
    [_glv setModelRotate:slider.value];
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (void) changeLightPosionX:(id)sender {
    UISlider *slider = (UISlider *)sender;
    _lightPoint.x = slider.value;
    [self.glv setLightPosition:_lightPoint];
    //self.glv.lightPosition = _lightPoint;
}

- (void) changeLightPosionY:(id)sender {
    UISlider *slider = (UISlider *)sender;
    _lightPoint.y = slider.value;
    [self.glv setLightPosition:_lightPoint];
}

- (void) changeLightPosionZ:(id)sender {
    UISlider *slider = (UISlider *)sender;
    _lightPoint.z = slider.value;
    [self.glv setLightPosition:_lightPoint];
}

- (void) changeLightAmbientR:(id)sender {
    UISlider *slider = (UISlider *)sender;
    _lightAmbient.r = slider.value;
    [self.glv setAmbient:_lightAmbient];
}

- (void) changeLightAmbientG:(id)sender {
    UISlider *slider = (UISlider *)sender;
    _lightAmbient.g = slider.value;
    [self.glv setAmbient:_lightAmbient];
}

- (void) changeLightAmbientB:(id)sender {
    UISlider *slider = (UISlider *)sender;
    _lightAmbient.b = slider.value;
    [self.glv setAmbient:_lightAmbient];
}

- (void) changeLightDiffuseR:(id)sender {
    UISlider *slider = (UISlider *)sender;
    _lightDiffuse.r = slider.value;
    [self.glv setDiffuse:_lightDiffuse];
}

- (void) changeLightDiffuseG:(id)sender {
    UISlider *slider = (UISlider *)sender;
    _lightDiffuse.g = slider.value;
    [self.glv setDiffuse:_lightDiffuse];
}

- (void) changeLightDiffuseB:(id)sender {
    UISlider *slider = (UISlider *)sender;
    _lightDiffuse.b = slider.value;
    [self.glv setDiffuse:_lightDiffuse];
}

- (void) changeLightSpecularR:(id)sender {
    UISlider *slider = (UISlider *)sender;
    _lightSpecular.r = slider.value;
    [self.glv setSpecular:_lightSpecular];
}

- (void) changeLightSpecularG:(id)sender {
    UISlider *slider = (UISlider *)sender;
    _lightSpecular.g = slider.value;
    [self.glv setSpecular:_lightSpecular];
}

- (void) changeLightSpecularB:(id)sender {
    UISlider *slider = (UISlider *)sender;
    _lightSpecular.b = slider.value;
    [self.glv setSpecular:_lightSpecular];
}

- (void) changeLightShiniess:(id)sender {
    UISlider *slider = (UISlider *)sender;
    _lightShininess = slider.value;
    [self.glv setShininess:_lightShininess];
}

@end
