#pragma once

#include <UIKit/UIKit.h>
#include <XliPlatform/Application.h>

@interface Xli_AppDelegate : UIViewController<UIApplicationDelegate>
{
    @package Xli::Application *sharedApplication;
}

- (void)Xli_OnUpdateFrame:(id)sender;

@end
