#ifndef LOADING_WINDOW_NX_H
#define LOADING_WINDOW_NX_H

#include "LoadingWindow.h"
#include "RageThreads.h"
#include <SDL.h>
#include <SDL_ttf.h>

extern "C" {
	#include <switch/runtime/devices/console.h>
	#include <sys/iosupport.h>

	extern const devoptab_t* g_consoleDevOpTab;
	ConsoleRenderer* getDefaultConsoleRenderer(void);
}

#define LOADING_WINDOW_NX_WIDTH 1280
#define LOADING_WINDOW_NX_HEIGHT 720
#define LOADING_WINDOW_NX_SPLASH_HEIGHT 120
#define LOADING_WINDOW_NX_FTSIZE 30

class LoadingWindow_NX: public LoadingWindow
{
protected:
	RString m_txt;
	SDL_Window *m_window;
	SDL_Renderer *m_renderer;
	SDL_Texture *m_splash;
	TTF_Font *m_font;
	int m_y;
	int m_splash_y;
	int m_splash_h;
	int m_splash_w;

	void PrintText(const char* txt);
	void PrintNewLine();
	bool DisplaySplash();
	void Update();
public:
	LoadingWindow_NX();
	virtual ~LoadingWindow_NX();
	RString Init();
	void SetText(RString txt);
	void SetSplash(const RageSurface * rageSurface);
	void SetProgress(const int progress);
	void SetTotalWork(const int totalWork);
	void SetIndeterminate(bool indeterminate);
};
#define USE_LOADING_WINDOW_NX

#endif

/*
 * (c) 2019 p-sam
 * (c) 2003-2004 Glenn Maynard
 * All rights reserved.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, and/or sell copies of the Software, and to permit persons to
 * whom the Software is furnished to do so, provided that the above
 * copyright notice(s) and this permission notice appear in all copies of
 * the Software and that both the above copyright notice(s) and this
 * permission notice appear in supporting documentation.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT OF
 * THIRD PARTY RIGHTS. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR HOLDERS
 * INCLUDED IN THIS NOTICE BE LIABLE FOR ANY CLAIM, OR ANY SPECIAL INDIRECT
 * OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
 * OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */

