//
//  OpenGLView.m
//  Preview
//
//  Created by zhuwei on 4/7/18.
//  Copyright © 2018 julian. All rights reserved.
//

#import <OpenGL/gl3.h>
#import "OpenGLView.h"
#include "Preview.h"

@interface OpenGLView() {
    mtp::Preview *_preview;
    NSDate *_previous;
    CVDisplayLinkRef _displayLink; //display link for managing rendering thread
}

@end

@implementation OpenGLView

- (nullable instancetype)initWithFrame:(NSRect)frameRect pixelFormat:(nullable NSOpenGLPixelFormat*)format {
    id instance = [super initWithFrame:frameRect pixelFormat:format];
    if (instance) {
        NSTrackingArea *trackingArea = [[NSTrackingArea alloc] initWithRect:[self bounds]
                                                                    options:(NSTrackingMouseEnteredAndExited 
                                                                             | NSTrackingMouseMoved 
                                                                             | NSTrackingActiveInKeyWindow)
                                                                      owner:self
                                                                   userInfo:nil];
        [self addTrackingArea:trackingArea];

        NSBundle *appBundle = [NSBundle mainBundle];
        string resourcePath = [appBundle.resourcePath UTF8String];
        _preview = new mtp::Preview(frameRect.size.width, frameRect.size.height, resourcePath);
    }
    return instance;
}

// This is the renderer output callback function
static CVReturn MyDisplayLinkCallback(CVDisplayLinkRef displayLink, 
                                      const CVTimeStamp *now, 
                                      const CVTimeStamp *outputTime, 
                                      CVOptionFlags flagsIn, 
                                      CVOptionFlags *flagsOut, 
                                      void *displayLinkContext) {
    return [(__bridge OpenGLView *)displayLinkContext Render];
}

// initialize OpenGL state
- (void)prepareOpenGL {
    _preview->Initialize();
    
    // Synchronize buffer swaps with vertical refresh rate
    GLint swapInt = 1;
    [[self openGLContext] setValues:&swapInt forParameter:NSOpenGLCPSwapInterval];
 
    // Create a display link capable of being used with all active displays
    CVDisplayLinkCreateWithActiveCGDisplays(&_displayLink);
 
    // Set the renderer output callback function
    CVDisplayLinkSetOutputCallback(_displayLink, &MyDisplayLinkCallback, (__bridge void *)self);
 
    // Set the display link for the current renderer
    CGLContextObj cglContext = [[self openGLContext] CGLContextObj];
    CGLPixelFormatObj cglPixelFormat = [[self pixelFormat] CGLPixelFormatObj];
    CVDisplayLinkSetCurrentCGDisplayFromOpenGLContext(_displayLink, cglContext, cglPixelFormat);
 
    // Activate the display link
    CVDisplayLinkStart(_displayLink);
}
 
- (CVReturn) Render {
    [[self openGLContext] makeCurrentContext];
    // We draw on a secondary thread through the display link
    // When resizing the view, -reshape is called automatically on the main
    // thread. Add a mutex around to avoid the threads accessing the context
    // simultaneously when resizing
    CGLLockContext([[self openGLContext] CGLContextObj]);
    
    // Add your drawing codes here
    NSTimeInterval deltaTime = 0.0;
    if (_previous) {
        NSDate *now = [NSDate date];
        deltaTime = [now timeIntervalSinceDate:_previous];
        _previous = now;
    } else {
        _previous = [NSDate date];
    }
    
    // Drawing code here.
    _preview->Update(deltaTime);
    
    CGLFlushDrawable([[self openGLContext] CGLContextObj]);
    CGLUnlockContext([[self openGLContext] CGLContextObj]);
 
    return kCVReturnSuccess;
}
 
- (void)dealloc {
    // Release the display link
    CVDisplayLinkStop(_displayLink);
    CVDisplayLinkRelease(_displayLink);
    
    delete _preview;
}

// moved or resized
- (void)update {
    [super update];
    
    [[self openGLContext] makeCurrentContext];
    [[self openGLContext] update];
    
    NSRect rect = [self bounds];
    glViewport(0, 0, rect.size.width, rect.size.height);
}

// scrolled, moved or resized
- (void)reshape {
    [super reshape];
    
    [[self openGLContext] makeCurrentContext];
    [[self openGLContext] update];
    
    NSRect rect = [self bounds];
    glViewport(0, 0, rect.size.width, rect.size.height);
}

- (BOOL)acceptsFirstResponder {
    return YES;
}

- (void)keyDown:(NSEvent *)theEvent {
//    NSString *characters = [theEvent characters];
//    NSLog(@"keyDown: %@", characters);
}

- (void)keyUp:(NSEvent *)theEvent {
//    NSString *characters = [theEvent characters];
//    NSLog(@"keyUp: %@", characters);
}

- (void)mouseDown:(NSEvent *)theEvent {
    NSPoint location = [theEvent locationInWindow];
    _preview->OnMouseDown(location.x, location.y);
//    NSLog(@"mouseDown: location:%f %f", location.x, location.y);
}

- (void)mouseUp:(NSEvent *)theEvent {
    NSPoint location = [theEvent locationInWindow];
    _preview->OnMouseUp(location.x, location.y);
//    NSLog(@"mouseUp: location:%f %f", location.x, location.y);
}

- (void)rightMouseDown:(NSEvent *)theEvent {
//    NSPoint location = [theEvent locationInWindow];
//    NSLog(@"rightMouseDown: location:%f %f", location.x, location.y);
}

- (void)rightMouseUp:(NSEvent *)theEvent {
//    NSPoint location = [theEvent locationInWindow];
//    NSLog(@"rightMouseUp: location:%f %f", location.x, location.y);
}

- (void)mouseMoved:(NSEvent *)theEvent {
//    CGFloat x = [theEvent deltaX];
//    CGFloat y = [theEvent deltaY];
//    NSLog(@"mouseMoved: x:%f y:%f", x, y);
}

- (void)mouseDragged:(NSEvent *)theEvent {
    CGFloat x = [theEvent deltaX];
    CGFloat y = [theEvent deltaY];
    _preview->OnMouseDrag(x, y);
//    NSLog(@"mouseDragged: x:%f y:%f", x, y);
}

- (void)rightMouseDragged:(NSEvent *)theEvent {
//    CGFloat x = [theEvent deltaX];
//    CGFloat y = [theEvent deltaY];
//    NSLog(@"rightMouseDragged: x:%f y:%f", x, y);
}

- (void)scrollWheel:(NSEvent *)theEvent {
//    CGFloat x = [theEvent scrollingDeltaX];
//    CGFloat y = [theEvent scrollingDeltaY];
//    NSLog(@"scrollWheel: x:%f y:%f", x, y);
}

- (void)mouseEntered:(NSEvent *)theEvent {
//    NSLog(@"mouseEntered");
}

- (void)mouseExited:(NSEvent *)theEvent {
//    NSLog(@"mouseExited");
}

@end
