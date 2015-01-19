#include "AppDelegate.h"
#include <XliPlatform/Application.h>
#include <XliPlatform/Window.h>
#include <Xli/Console.h>

@implementation Xli_AppDelegate

- (void)Xli_OnUpdateFrame:(id)sender
{
    sharedApplication->OnUpdateFrame();
}

// Tells the delegate that the launch process has begun but that state restoration has not yet occurred.
- (BOOL)application:(UIApplication *)application willFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
    Xli::PrintLine("------------------------------------------------------------ Test2");
    sharedApplication = Xli::Application::SharedApp();
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
