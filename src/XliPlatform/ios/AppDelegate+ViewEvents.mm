#include <XliPlatform/Window.h>
#include "AppDelegate.h"

@interface Xli_AppDelegate (ViewEvents)
@end


@implementation Xli_AppDelegate (ViewEvents)

// Notifies the view controller that its view is about to be added to a view hierarchy.
- (void)viewWillAppear:(BOOL)animated
{
    [super viewWillAppear:animated];
    sharedApplication->RootWindow()->Show();
}

// Notifies the view controller that its view was removed from a view hierarchy.
- (void)viewDidDisappear:(BOOL)animated
{
    sharedApplication->RootWindow()->Hide();
    [super viewDidDisappear:animated];
}

- (void)viewWillLayoutSubviews
{
    Xli::Window *window = sharedApplication->RootWindow();
    window->Hide();
    window->Show();
    sharedApplication->OnSizeChanged(window);
}

@end
