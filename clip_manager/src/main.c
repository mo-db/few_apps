#include "bridge.h"
#include <stdio.h>

int main()
{
	/* openTextWindow("Hello, Cocoa from C!"); */
	const char *clip_content;
	clip_content = getClipboardContent();

	printf("clipboard content: %s\n", clip_content);

	// if clipboard content changes record the time.
	return 0;
}
