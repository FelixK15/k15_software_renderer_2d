#define _GNU_SOURCE 1

#define K15_SOFTWARE_RENDERER_2D_IMPLEMENTATION
#include "k15_software_renderer_2d.h"

#include "stdio.h"
#include "stddef.h"
#include "time.h"
#include "string.h"
#include "unistd.h"
#include "malloc.h"
#include "X11/Xlib.h"

#define K15_FALSE 0
#define K15_TRUE 1

typedef unsigned char bool8;
typedef unsigned char byte;
typedef unsigned int uint32;
typedef unsigned short uint16;
typedef unsigned char uint8;

Display* mainDisplay = 0;
GC mainGC;
Atom deleteMessage = 0;
int nanoSecondsPerFrame = 16000000;
ksr2_contexthandle renderer;

int screenWidth = 800;
int screenHeight = 600;

void handleKeyPress(XEvent* p_Event)
{
}

void handleKeyRelease(XEvent* p_Event)
{
}

void handleButtonPress(XEvent* p_Event)
{
}

void handleButtonRelease(XEvent* p_Event)
{
}

void handleMouseMotion(XEvent* p_Event)
{
}

void handleWindowResize(XEvent* p_Event)
{
	screenWidth 	= p_Event->xconfigure.width;
	screenHeight 	= p_Event->xconfigure.height;

	ksr2_resize_swap_chain(renderer, screenWidth, screenHeight, K15_RENDERER_2D_PIXEL_FORMAT_RGB8);
}

bool8 filterEvent(XEvent* p_Event)
{
	if (p_Event->type == KeyRelease)
	{
		if (XEventsQueued(mainDisplay, QueuedAfterReading))
		{
			XEvent tempEvent;
			XPeekEvent(mainDisplay, &tempEvent);

			if (tempEvent.type == KeyPress && tempEvent.xkey.time == p_Event->xkey.time &&
				tempEvent.xkey.keycode == p_Event->xkey.keycode)
			{
				XNextEvent(mainDisplay, &tempEvent);
				return K15_TRUE;
			}
		}
	}


	return K15_FALSE;
}

void handleEvent(XEvent* p_Event)
{
	if (filterEvent(p_Event))
		return;

	if (p_Event->type == KeyPress)
		handleKeyPress(p_Event);
	else if (p_Event->type == KeyRelease)
		handleKeyRelease(p_Event);
	else if (p_Event->type == ButtonPress)
		handleButtonPress(p_Event);
	else if (p_Event->type == ButtonRelease)
		handleButtonRelease(p_Event);
	else if (p_Event->type == MotionNotify)
		handleMouseMotion(p_Event);
	else if (p_Event->type == ConfigureNotify)
		handleWindowResize(p_Event);
}

int errorHandler(Display* p_Display, XErrorEvent* p_Event)
{
	uint32 errorCode = p_Event->error_code;
	char errorMessage[256];
	XGetErrorText(p_Display, errorCode, errorMessage, 256);

	printf("Error (%d): '%s'\n", errorCode, errorMessage);

	return 0;
}

void setupWindow(Window* p_WindowOut, int p_Width, int p_Height)
{
	int borderWidth = 1;
	int eventMask = ButtonPressMask | ButtonReleaseMask | PointerMotionMask | 
		KeyPressMask | KeyReleaseMask | StructureNotifyMask;

	Window window = XCreateSimpleWindow(mainDisplay, XRootWindow(mainDisplay, XDefaultScreen(mainDisplay)),
		0, 0, p_Width, p_Height, 0, 0, 0);

	*p_WindowOut = window;

	mainGC = XCreateGC(mainDisplay, window, 0, 0);
	XSetForeground(mainDisplay, mainGC, (~0));
	XStoreName(mainDisplay, window, "Test Window!");
	XSelectInput(mainDisplay, window, eventMask);
	XSetWMProtocols(mainDisplay, window, &deleteMessage, 1);
	XMapWindow(mainDisplay, window);
}

bool8 setup(Window* p_WindowOut)
{
	const size_t rendererMemorySize = ksr2_megabyte(5);

	ksr2_context_parameters contextParameters;
	contextParameters.backBufferWidth 	= screenWidth;
	contextParameters.backBufferHeight 	= screenHeight;
	contextParameters.backBufferFormat	= K15_RENDERER_2D_PIXEL_FORMAT_RGB8;
	contextParameters.pMemory			= malloc(rendererMemorySize);
	contextParameters.memorySizeInBytes	= rendererMemorySize;
	contextParameters.flags				= K15_RENDERER_2D_DOUBLE_BUFFERED_FLAG;

	ksr2_result result = ksr2_init_context(&contextParameters, &renderer);

	XSetErrorHandler(errorHandler);
	mainDisplay = XOpenDisplay(0);

	if (mainDisplay)
	{
		deleteMessage = XInternAtom(mainDisplay, "WM_DELETE_WINDOW", False);
		setupWindow(p_WindowOut, screenWidth, screenHeight);
		return K15_TRUE;
	}

	return K15_FALSE;
}

void drawDeltaTime(Window* p_MainWindow, long p_DeltaTimeInNs)
{
	char buffer[256];
	sprintf(buffer, "Milliseconds:%ld", p_DeltaTimeInNs / 1000000);

	XDrawString(mainDisplay, *p_MainWindow, mainGC, 20, 20, buffer, strlen(buffer));
}

void doFrame(Window* p_MainWindow, long p_DeltaTimeInNs)
{
	XClearWindow(mainDisplay, *p_MainWindow);
	drawDeltaTime(p_MainWindow, p_DeltaTimeInNs);
	ksr2_swap_buffers(renderer);
	XFlush(mainDisplay);
	XSync(mainDisplay, *p_MainWindow);
}

int main(int argc, char** argv)
{
	Window mainWindow;
	setup(&mainWindow);

	struct timespec timeFrameStarted = {0};
	struct timespec timeFrameEnded = {0};
	long deltaNs = 0;

	bool8 loopRunning = K15_TRUE;
	XEvent event = {0};
	while (loopRunning)
	{
		clock_gettime(CLOCK_MONOTONIC, &timeFrameStarted);
		while (XPending(mainDisplay))
		{
			XNextEvent(mainDisplay, &event);

			if (event.type == ClientMessage && 
				event.xclient.data.l[0] == deleteMessage)
			{
				loopRunning = K15_FALSE;
			} 

			handleEvent(&event);
		}

		doFrame(&mainWindow, deltaNs);

		clock_gettime(CLOCK_MONOTONIC, &timeFrameEnded);

		deltaNs = timeFrameEnded.tv_nsec - timeFrameStarted.tv_nsec;

		if (deltaNs < nanoSecondsPerFrame)
		{
			struct timespec sleepTime = {0, nanoSecondsPerFrame - deltaNs};
			nanosleep(&sleepTime, 0);
		}
	}

	return 0;
}
