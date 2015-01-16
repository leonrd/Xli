#pragma once

#include <UIKit/UIKit.h>
#include <XliPlatform/Application.h>

@interface uObjC_AppDelegate : NSObject<UIApplicationDelegate>
{
    @package Xli::Application *sharedApplication;
}

- (void)uObjC_OnUpdateFrame:(id)sender;

@end
