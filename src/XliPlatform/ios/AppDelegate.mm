#include "AppDelegate.h"
#include <XliPlatform/Application.h>
#include <XliPlatform/Window.h>
#include <Xli/Console.h>

@implementation Xli_AppDelegate

- (void)Xli_OnUpdateFrame:(id)sender
{
    sharedApplication->OnUpdateFrame();
}

- (id)init
{
    if ((self = [super init]))
    {
        sharedApplication = Xli::Application::SharedApp();

        NSNotificationCenter *center = [NSNotificationCenter defaultCenter];
        [center addObserver:self selector:@selector(Xli_keyboardWillShowOrHide:)
            name:UIKeyboardWillShowNotification object:nil];
        [center addObserver:self selector:@selector(Xli_keyboardWillShowOrHide:)
            name:UIKeyboardWillHideNotification object:nil];
    }
    return self;
}

// Tells the delegate that the launch process has begun but that state restoration has not yet occurred.
- (BOOL)application:(UIApplication *)application willFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
    Xli::PrintLine("------------------------------------------------------------ Test2");
    sharedApplication->Start();
    return YES;
}

// Tells the delegate that the app is about to enter the foreground.
- (void)applicationWillEnterForeground:(UIApplication *)application
{
    sharedApplication->BecomeVisible();
}

// Tells the delegate that the app has become active.
- (void)applicationDidBecomeActive:(UIApplication *)application
{
    sharedApplication->BecomeActive();
}

// Tells the delegate that the app is about to become inactive.
- (void)applicationWillResignActive:(UIApplication *)application
{
    sharedApplication->ResignActive();
}

// Tells the delegate that the app is now in the background.
- (void)applicationDidEnterBackground:(UIApplication *)application
{
    sharedApplication->EnterBackground();
}

// Tells the delegate when the app is about to terminate.
- (void)applicationWillTerminate:(UIApplication *)application
{
    sharedApplication->Terminate();
}

// Tells the delegate when the app receives a memory warning from the system.
- (void)applicationDidReceiveMemoryWarning:(UIApplication *)application
{
    sharedApplication->OnLowMemory();
}

@end
