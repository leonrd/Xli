#pragma once

#include <UIKit/UIKit.h>
#include <XliPlatform/Application.h>

@interface uObjC_AppDelegate : UIViewController<UIApplicationDelegate>
{
    @package Xli::Application *sharedApplication;
}

- (void)uObjC_OnUpdateFrame:(id)sender;

@end
