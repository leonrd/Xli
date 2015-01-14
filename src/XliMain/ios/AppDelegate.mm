#include "AppDelegate.h"
#include <XliPlatform/Application.h>
#include <XliPlatform/Window.h>

@implementation uObjC_AppDelegate

- (void)uObjC_OnDidStart:(id)sender
{
    Xli::Application::SharedApp()->BecomeVisible();
}

- (void)uObjC_OnUpdateFrame:(id)sender
{
    Xli::Application::SharedApp()->OnUpdateFrame();
}

// Tells the delegate that the launch process has begun but that state restoration has not yet occurred.
- (BOOL)application:(UIApplication *)application willFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
    // if (launchOptions) {TODO} parse launch options
    //     return NO;

    // Xli::Application::SharedApp()->Start(); // remove in favour of main
    main(0, (char**)0);
    
    return YES;
}

// Tells the delegate that the launch process is almost done and the app is almost ready to run.
- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
    [self performSelector:@selector(uObjC_OnDidStart:)
        withObject:self afterDelay:0.0];
    return YES;
}

// Tells the delegate that the app is about to enter the foreground.
- (void)applicationWillEnterForeground:(UIApplication *)application
{
    Xli::Application::SharedApp()->BecomeVisible();
}

// Tells the delegate that the app has become active.
- (void)applicationDidBecomeActive:(UIApplication *)application
{
    Xli::Application::SharedApp()->BecomeActive();
}

// Tells the delegate that the app is about to become inactive.
- (void)applicationWillResignActive:(UIApplication *)application
{
    Xli::Application::SharedApp()->ResignActive();
}

// Tells the delegate that the app is now in the background.
- (void)applicationDidEnterBackground:(UIApplication *)application
{
    Xli::Application::SharedApp()->EnterBackground();
}

// Tells the delegate when the app is about to terminate.
- (void)applicationWillTerminate:(UIApplication *)application
{
    Xli::Application::SharedApp()->Terminate();
}

// Tells the delegate when the app receives a memory warning from the system.
- (void)applicationDidReceiveMemoryWarning:(UIApplication *)application
{
    Xli::Application::SharedApp()->OnLowMemory();
}

@end
