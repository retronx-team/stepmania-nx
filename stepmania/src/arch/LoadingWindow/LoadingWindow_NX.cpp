#include "global.h"
#include "RageLog.h"
#include "RageFileManager.h"
#include "RageUtil.h"
#include "RageSurface.h"
#include "RageSurfaceUtils.h"
#include "RageSurfaceUtils_Zoom.h"
#include "ver.h"

#include "LoadingWindow_NX.h"

extern "C" {
	#include <switch.h>
}

#define PRINT_TXT_FMT(format, ...) do {\
	std::string _s = ssprintf(format, ##__VA_ARGS__);\
	PrintText(_s.c_str());\
} while(false)

LoadingWindow_NX::LoadingWindow_NX()
{
	m_txt = "";
	m_renderer = NULL;
	m_window = NULL;
	m_font = NULL;
	m_splash = NULL;
	m_y = 0;
	m_splash_w = 0;
	m_splash_h = 0;
}

void LoadingWindow_NX::PrintNewLine() {
	m_y += TTF_FontLineSkip(m_font);
}

void LoadingWindow_NX::PrintText(const char* txt) {
	if(!*txt) {
		PrintNewLine();
		return;
	}
	SDL_Color fgColor = {255, 255, 255};
	SDL_Surface* surface = TTF_RenderText_Blended_Wrapped(m_font, txt, fgColor, LOADING_WINDOW_NX_WIDTH - (2 * LOADING_WINDOW_NX_FTSIZE));

	if(surface == NULL) {
		LOG->Warn("[%s] TTF_RenderText_Shaded: %s", __func__, TTF_GetError());
		return;
	}

	SDL_Texture* texture = SDL_CreateTextureFromSurface(m_renderer, surface);
	if(surface == NULL) {
		LOG->Warn("[%s] SDL_CreateTextureFromSurface: %s", __func__, SDL_GetError());
		SDL_FreeSurface(surface);
		return;
	}

	SDL_Rect rect = {};
	rect.x = LOADING_WINDOW_NX_FTSIZE;
	rect.y = m_y;
	rect.w = surface->w;
	rect.h = surface->h;

	SDL_RenderCopy(m_renderer, texture, NULL, &rect);
	m_y += rect.h;

	SDL_DestroyTexture(texture);
	SDL_FreeSurface(surface);
}

bool LoadingWindow_NX::DisplaySplash() {
	if(!m_splash) {
		return false;
	}

	SDL_Rect rect = {};
	rect.x = m_splash_w <= LOADING_WINDOW_NX_WIDTH ? (LOADING_WINDOW_NX_WIDTH - m_splash_w) / 2 : 0;
	rect.y = m_y;
	rect.w = m_splash_w;
	rect.h = m_splash_h;

	SDL_RenderCopy(m_renderer, m_splash, NULL, &rect);
	m_y += m_splash_h;
	return true;
}

void LoadingWindow_NX::Update() {
	if(!m_renderer) {
		return;
	}

	SDL_SetRenderDrawColor(m_renderer, 0x33, 0x33, 0x33, 0xFF);
	SDL_RenderClear(m_renderer);

	m_y = 0;
	PrintNewLine();
	PRINT_TXT_FMT("Stepmania %s (%s %s)", product_version, version_date, version_time);
	PrintNewLine();

	if(DisplaySplash()) {
		PrintNewLine();
	}

	PrintText(m_txt.c_str());
	if(!m_indeterminate || m_totalWork > 0) {
		float percentage = (float) m_progress / m_totalWork * 100.0f;
		PRINT_TXT_FMT("%.0f%%", percentage);
	}

	SDL_RenderPresent(m_renderer);
}

RString LoadingWindow_NX::Init() {
	Result rc = plInitialize(PlServiceType_User);
	if(R_FAILED(rc)) {
		return ssprintf("[%s] plInitialize: %x", __func__, rc);
	}

	PlFontData fontData;
	rc = plGetSharedFontByType(&fontData, PlSharedFontType_Standard);
	if(R_FAILED(rc)) {
		return ssprintf("[%s] plGetSharedFontByType: %x", __func__, rc);
	}

	if(SDL_InitSubSystem(SDL_INIT_VIDEO) < 0) {
		return ssprintf("[%s] SDL_InitSubSystem: %s", __func__, SDL_GetError());
	}

	if(TTF_Init() < 0) {
		return ssprintf("[%s] TTF_Init: %s", __func__, SDL_GetError());
	}

	SDL_RWops* fontRW = SDL_RWFromMem(fontData.address, fontData.size);
	m_font = TTF_OpenFontRW(fontRW, 1, LOADING_WINDOW_NX_FTSIZE);

	m_window = SDL_CreateWindow("LoadingWindow_NX", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, LOADING_WINDOW_NX_WIDTH, LOADING_WINDOW_NX_HEIGHT, SDL_WINDOW_SHOWN);
	if(!m_window) {
		return ssprintf("[%s] SDL_CreateWindow: %s", __func__, SDL_GetError());
	}

	m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED);
	if(!m_window) {
		return ssprintf("[%s] SDL_CreateRenderer: %s", __func__, SDL_GetError());
	}

	return "";
}

void LoadingWindow_NX::SetSplash(const RageSurface* rageSurface) {
	if(m_splash) {
		SDL_DestroyTexture(m_splash);
		m_splash = NULL;
	}
	if(rageSurface) {
		RageSurface *zoomedRageSurface = CreateSurface(rageSurface->w, rageSurface->h, 32, 0xFF000000, 0x00FF0000, 0x0000FF00, 0);
		RageSurfaceUtils::Blit(rageSurface, zoomedRageSurface, -1, -1);

		float ratio = (float) LOADING_WINDOW_NX_SPLASH_HEIGHT / rageSurface->h;
		RageSurfaceUtils::Zoom(zoomedRageSurface, std::lrint(rageSurface->w * ratio), LOADING_WINDOW_NX_SPLASH_HEIGHT);

		SDL_Texture *texture = SDL_CreateTexture(m_renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, zoomedRageSurface->w, zoomedRageSurface->h);
		if(!texture) {
			LOG->Warn("[%s] SDL_CreateTexture: %s", __func__, SDL_GetError());
			return;
		}

		int pitch;
		uint32_t* pixels;
		SDL_LockTexture(texture, NULL, (void**)&pixels, &pitch);
		SDL_PixelFormat *format = SDL_AllocFormat(SDL_PIXELFORMAT_RGBA8888);

		const uint8_t *srcp = zoomedRageSurface->pixels;
		uint8_t rgbav[4];

		for(size_t i = 0; i < (zoomedRageSurface->w * zoomedRageSurface->h); i++) {
			RageSurfaceUtils::GetRGBAV(srcp, zoomedRageSurface, (uint8_t*)rgbav);
			pixels[i] = SDL_MapRGBA(format, rgbav[0], rgbav[1], rgbav[2], rgbav[3]);
			srcp += zoomedRageSurface->fmt.BytesPerPixel;
		}

		SDL_UnlockTexture(texture);
		SDL_FreeFormat(format);
		delete zoomedRageSurface;
		m_splash = texture;
		m_splash_h = zoomedRageSurface->h;
		m_splash_w = zoomedRageSurface->w;
	}
}

LoadingWindow_NX::~LoadingWindow_NX() {
	SetSplash(NULL);
	if(m_renderer) {
		SDL_DestroyRenderer(m_renderer);
	}
	if(m_window) {
		SDL_DestroyWindow(m_window);
	}
	if(m_font) {
		TTF_CloseFont(m_font);
	}

	SDL_QuitSubSystem(SDL_INIT_VIDEO);
	TTF_Quit();
	plExit();
}

void LoadingWindow_NX::SetText(RString txt) {
	m_txt = txt;
	Update();
}

void LoadingWindow_NX::SetProgress(const int progress) {
	LoadingWindow::SetProgress(progress);
	Update();
}
void LoadingWindow_NX::SetTotalWork(const int totalWork) {
	LoadingWindow::SetTotalWork(totalWork);
	Update();
}
void LoadingWindow_NX::SetIndeterminate(bool indeterminate) {
	LoadingWindow::SetIndeterminate(indeterminate);
	Update();
}

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
