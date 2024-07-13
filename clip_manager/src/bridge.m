#include <cocoa/Cocoa.h>
#include "bridge.h"

void showMessage(const char* message) {
    NSString *nsMessage = [NSString stringWithUTF8String:message];
    NSAlert *alert = [[NSAlert alloc] init];
    [alert setMessageText:nsMessage];
    [alert runModal];
}

void openTextWindow(const char* text) {
    NSString *nsText = [NSString stringWithUTF8String:text];
    
    // Create the application and set up the environment
    @autoreleasepool {
        NSApplication *app = [NSApplication sharedApplication];
        [app setActivationPolicy:NSApplicationActivationPolicyRegular];
        
        // Create a window
        NSRect frame = NSMakeRect(0, 0, 480, 270);
        NSUInteger style = NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskResizable;
        NSWindow *window = [[NSWindow alloc] initWithContentRect:frame
                                                       styleMask:style
                                                         backing:NSBackingStoreBuffered
                                                           defer:NO];
        [window setTitle:@"Text Window"];
        
        // Create a text view and set the text
        NSTextView *textView = [[NSTextView alloc] initWithFrame:frame];
        [textView setString:nsText];
        [textView setEditable:NO];
        
        // Add the text view to the window's content view
        [window setContentView:textView];
        [window makeKeyAndOrderFront:nil];
        
        // Run the application
        [app activateIgnoringOtherApps:YES];
        [app run];
    }
}

const char* getClipboardContent() {
    @autoreleasepool { // autorelease -> memory is freed on scope exits
        NSPasteboard *pasteboard = [NSPasteboard generalPasteboard];
		// objective-c objects like NSString are managed by autorelease-pools
        NSString *clipboardContent = [pasteboard stringForType:NSPasteboardTypeString]; 
        if (clipboardContent != nil) {
            return strdup([clipboardContent UTF8String]);
        } else {
            return NULL;
        }
    }
}
