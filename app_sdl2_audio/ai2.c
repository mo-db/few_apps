#import <Cocoa/Cocoa.h>

// Read from clipboard
const char* readFromClipboard() {
    NSPasteboard *pasteboard = [NSPasteboard generalPasteboard];
    NSString *string = [pasteboard stringForType:NSPasteboardTypeString];
    return [string UTF8String];
}

// Write to clipboard
void writeToClipboard(const char* text) {
    NSPasteboard *pasteboard = [NSPasteboard generalPasteboard];
    [pasteboard clearContents];
    [pasteboard setString:[NSString stringWithUTF8String:text] forType:NSPasteboardTypeString];
}

// Example usage
int main() {
    writeToClipboard("Hello, Clipboard!");
    printf("Clipboard contents: %s\n", readFromClipboard());
    return 0;
}
