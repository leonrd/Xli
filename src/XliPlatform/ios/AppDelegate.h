#pragma once

#include "Application.h"
#include <UIKit/UIKit.h>

@interface uObjC_AppDelegate : NSObject<UIApplicationDelegate>
{
    @package uObjC::Application *sharedApplication;
}

- (void)uObjC_OnUpdateFrame:(id)sender;

@end
