/*
** Haaf's Game Engine 1.8
** Copyright (C) 2003-2007, Relish Games
** hge.relishgames.com
**
** Common core implementation header
*/

#ifndef HGE_IMPL_UNIX_H
#define HGE_IMPL_UNIX_H

#include "unix_compat.h"

#include "../../include/hge.h"
#include <stdio.h>
#include <dirent.h>
#include "SDL/SDL.h"

#define GL_GLEXT_LEGACY 1
#include "gl.h"

#ifndef WINGDIAPI
#define WINGDIAPI
#endif

#ifndef APIENTRY
#define APIENTRY
#endif

#ifndef APIENTRYP
#define APIENTRYP APIENTRY *
#endif

#define GL_PROC(ext,fn,call,ret,params) typedef call ret (APIENTRYP _HGE_PFN_##fn) params;
#include "hge_glfuncs.h"
#undef GL_PROC

class COpenGLDevice
{
public:
    #define GL_PROC(ext,fn,call,ret,params) _HGE_PFN_##fn fn;
	#include "hge_glfuncs.h"
	#undef GL_PROC

	GLenum TextureTarget;  // texture rectangle vs (npot) 2D.
	bool have_base_opengl;
	bool have_GL_ARB_texture_rectangle;
	bool have_GL_ARB_texture_non_power_of_two;
	bool have_GL_EXT_framebuffer_object;
	bool have_GL_EXT_texture_compression_s3tc;
	bool have_GL_ARB_vertex_buffer_object;
	bool have_GL_APPLE_ycbcr_422;
};


//#define DEMO

#define VERTEX_BUFFER_SIZE 4000


struct gltexture;  // used internally.

struct CRenderTargetList
{
	int					width;
	int					height;
	HTEXTURE			tex;
	GLuint				depth;
	GLuint				frame;
	CRenderTargetList*	next;
};

struct CTextureList
{
	HTEXTURE			tex;
	int					width;
	int					height;
	CTextureList*		next;
};

struct CResourceList
{
	char				filename[_MAX_PATH];
	char				password[64];
	CResourceList*		next;
};

struct CStreamList
{
	HSTREAM				hstream;
	void*				data;
	CStreamList*		next;
};

struct CInputEventList
{
	hgeInputEvent		event;
	CInputEventList*	next;
};


void DInit();
void DDone();
bool DFrame();


/*
** HGE Interface implementation
*/
class HGE_Impl : public HGE
{
public:
	virtual	void		CALL	Release();

	virtual bool		CALL	System_Initiate();
	virtual void		CALL	System_Shutdown();
	virtual bool		CALL	System_Start();
	virtual void		CALL	System_SetStateBool  (hgeBoolState   state, bool        value);
	virtual void		CALL	System_SetStateFunc  (hgeFuncState   state, hgeCallback value);
	virtual void		CALL	System_SetStateHwnd  (hgeHwndState   state, HWND        value);
	virtual void		CALL	System_SetStateInt   (hgeIntState    state, int         value);
	virtual void		CALL	System_SetStateString(hgeStringState state, const char *value);
	virtual bool		CALL	System_GetStateBool  (hgeBoolState  );
	virtual hgeCallback	CALL	System_GetStateFunc  (hgeFuncState  );
	virtual HWND		CALL	System_GetStateHwnd  (hgeHwndState  );
	virtual int			CALL	System_GetStateInt   (hgeIntState   );
	virtual const char*	CALL	System_GetStateString(hgeStringState);
	virtual const char*	CALL	System_GetErrorMessage();
	virtual	void		CALL	System_Log(const char *format, ...);
	virtual bool		CALL	System_Launch(const char *url);
	virtual void		CALL	System_Snapshot(const char *filename=0);

	virtual void*		CALL	Resource_Load(const char *filename, DWORD *size=0);
	virtual void		CALL	Resource_Free(void *res);
	virtual bool		CALL	Resource_AttachPack(const char *filename, const char *password=0);
	virtual void		CALL	Resource_RemovePack(const char *filename);
	virtual void		CALL	Resource_RemoveAllPacks();
	virtual char*		CALL	Resource_MakePath(const char *filename=0);
	virtual char*		CALL	Resource_EnumFiles(const char *wildcard=0);
	virtual char*		CALL	Resource_EnumFolders(const char *wildcard=0);

	virtual	void		CALL	Ini_SetInt(const char *section, const char *name, int value);
	virtual	int 		CALL	Ini_GetInt(const char *section, const char *name, int def_val);
	virtual	void		CALL	Ini_SetFloat(const char *section, const char *name, float value);
	virtual	float		CALL	Ini_GetFloat(const char *section, const char *name, float def_val);
	virtual	void		CALL	Ini_SetString(const char *section, const char *name, const char *value);
	virtual	char*		CALL	Ini_GetString(const char *section, const char *name, const char *def_val);

	virtual void		CALL	Random_Seed(int seed=0);
	virtual int			CALL	Random_Int(int min, int max);
	virtual float		CALL	Random_Float(float min, float max);

	virtual float		CALL	Timer_GetTime();
	virtual float		CALL	Timer_GetDelta();
	virtual int			CALL	Timer_GetFPS();

	virtual HEFFECT		CALL	Effect_Load(const char *filename, DWORD size=0);
	virtual void		CALL	Effect_Free(HEFFECT eff);
	virtual HCHANNEL	CALL 	Effect_Play(HEFFECT eff);
	virtual HCHANNEL	CALL 	Effect_PlayEx(HEFFECT eff, int volume=100, int pan=0, float pitch=1.0f, bool loop=false);

	virtual HMUSIC		CALL 	Music_Load(const char *filename, DWORD size=0);
	virtual void		CALL	Music_Free(HMUSIC mus);
	virtual HCHANNEL	CALL 	Music_Play(HMUSIC mus, bool loop, int volume = 100, int order = 0, int row = 0);
	virtual void		CALL	Music_SetAmplification(HMUSIC music, int ampl);
	virtual int			CALL	Music_GetAmplification(HMUSIC music);
	virtual int			CALL	Music_GetLength(HMUSIC music);
	virtual void		CALL	Music_SetPos(HMUSIC music, int order, int row);
	virtual bool		CALL	Music_GetPos(HMUSIC music, int *order, int *row);
	virtual void		CALL	Music_SetInstrVolume(HMUSIC music, int instr, int volume);
	virtual int			CALL	Music_GetInstrVolume(HMUSIC music, int instr);
	virtual void		CALL	Music_SetChannelVolume(HMUSIC music, int channel, int volume);
	virtual int			CALL	Music_GetChannelVolume(HMUSIC music, int channel);

	virtual HSTREAM		CALL	Stream_Load(const char *filename, DWORD size=0);
	virtual void		CALL	Stream_Free(HSTREAM stream);
	virtual HCHANNEL	CALL	Stream_Play(HSTREAM stream, bool loop, int volume = 100);

	virtual void		CALL 	Channel_SetPanning(HCHANNEL chn, int pan);
	virtual void		CALL 	Channel_SetVolume(HCHANNEL chn, int volume);
	virtual void		CALL 	Channel_SetPitch(HCHANNEL chn, float pitch);
	virtual void		CALL 	Channel_Pause(HCHANNEL chn);
	virtual void		CALL 	Channel_Resume(HCHANNEL chn);
	virtual void		CALL 	Channel_Stop(HCHANNEL chn);
	virtual void		CALL 	Channel_PauseAll();
	virtual void		CALL 	Channel_ResumeAll();
	virtual void		CALL 	Channel_StopAll();
	virtual bool		CALL	Channel_IsPlaying(HCHANNEL chn);
	virtual float		CALL	Channel_GetLength(HCHANNEL chn);
	virtual float		CALL	Channel_GetPos(HCHANNEL chn);
	virtual void		CALL	Channel_SetPos(HCHANNEL chn, float fSeconds);
	virtual void		CALL	Channel_SlideTo(HCHANNEL channel, float time, int volume, int pan = -101, float pitch = -1);
	virtual bool		CALL	Channel_IsSliding(HCHANNEL channel);

	virtual void		CALL	Input_GetMousePos(float *x, float *y);
	virtual void		CALL	Input_SetMousePos(float x, float y);
	virtual int			CALL	Input_GetMouseWheel();
	virtual bool		CALL	Input_IsMouseOver();
	virtual bool		CALL	Input_KeyDown(int key);
	virtual bool		CALL	Input_KeyUp(int key);
	virtual bool		CALL	Input_GetKeyState(int key);
	virtual const  char*		CALL	Input_GetKeyName(int key);
	virtual int			CALL	Input_GetKey();
	virtual int			CALL	Input_GetChar();
	virtual bool		CALL	Input_GetEvent(hgeInputEvent *event);

	virtual bool		CALL	Gfx_BeginScene(HTARGET target=0);
	virtual void		CALL	Gfx_EndScene();
	virtual void		CALL	Gfx_Clear(DWORD color);
	virtual void		CALL	Gfx_RenderLine(float x1, float y1, float x2, float y2, DWORD color=0xFFFFFFFF, float z=0.5f);
	virtual void		CALL	Gfx_RenderTriple(const hgeTriple *triple);
	virtual void		CALL	Gfx_RenderQuad(const hgeQuad *quad);
	virtual hgeVertex*	CALL	Gfx_StartBatch(int prim_type, HTEXTURE tex, int blend, int *max_prim);
	virtual void		CALL	Gfx_FinishBatch(int nprim);
	virtual void		CALL	Gfx_SetClipping(int x=0, int y=0, int w=0, int h=0);
	virtual void		CALL	Gfx_SetTransform(float x=0, float y=0, float dx=0, float dy=0, float rot=0, float hscale=0, float vscale=0);

	virtual HTARGET		CALL	Target_Create(int width, int height, bool zbuffer);
	virtual void		CALL	Target_Free(HTARGET target);
	virtual HTEXTURE	CALL	Target_GetTexture(HTARGET target);

	virtual HTEXTURE	CALL	Texture_Create(int width, int height);
	virtual HTEXTURE	CALL	Texture_Load(const char *filename, DWORD size=0, bool bMipmap=false);
	virtual void		CALL	Texture_Free(HTEXTURE tex);
	virtual int			CALL	Texture_GetWidth(HTEXTURE tex, bool bOriginal=false);
	virtual int			CALL	Texture_GetHeight(HTEXTURE tex, bool bOriginal=false);
	virtual DWORD*		CALL	Texture_Lock(HTEXTURE tex, bool bReadOnly=true, int left=0, int top=0, int width=0, int height=0);
	virtual void		CALL	Texture_Unlock(HTEXTURE tex);

	bool CALL HGEEXT_Texture_PushYUV422(HTEXTURE tex, const BYTE *yuv);

	//////// Implementation ////////

	static HGE_Impl*	_Interface_Get();
	void				_FocusChange(bool bAct);
	void				_PostError(const char *error);

	// ini ...
	void				_LoadIniFile(const char *fname);
	const char*			_BuildProfilePath(const char *section, const char *name, const char *szIniFile);
	bool				_WritePrivateProfileString(const char *section, const char *name, const char *buf, const char *szIniFile);
	bool				_GetPrivateProfileString(const char *section, const char *name, const char *deflt, char *buf, size_t bufsize, const char *szIniFile);

	long				MacOSXVersion;
	HWND				hwnd;
	bool				bActive;
	char				szError[256];
	char				szAppPath[_MAX_PATH];
	char				szIniString[256];


	// System States
	bool				(*procFrameFunc)();
	bool				(*procRenderFunc)();
	bool				(*procFocusLostFunc)();
	bool				(*procFocusGainFunc)();
	bool				(*procGfxRestoreFunc)();
	bool				(*procExitFunc)();
	const char*			szIcon;
	char				szWinTitle[256];
	int					nScreenWidth;
	int					nScreenHeight;
	int					nOrigScreenWidth;
	int					nOrigScreenHeight;
	int					nScreenBPP;
	bool				bWindowed;
	bool				bVsync;
	bool				bZBuffer;
	bool				bTextureFilter;
	char				szIniFile[_MAX_PATH];
	char				szLogFile[_MAX_PATH];
	bool				bUseSound;
	int					nSampleRate;
	int					nFXVolume;
	int					nMusVolume;
	int					nStreamVolume;
	int					nHGEFPS;
	bool				bHideMouse;
	bool				bDontSuspend;
	HWND				hwndParent;
	bool				bForceTextureCompression;
	GLuint				IndexBufferObject;

	bool				_PrimsOutsideClipping(const hgeVertex *v, const int verts);
	bool				bTransforming;
	int					clipX;
	int					clipY;
	int					clipW;
	int					clipH;

	#ifdef DEMO
	bool				bDMO;
	#endif


	// Power
	int							nPowerStatus;

	void				_InitPowerStatus();
	void				_UpdatePowerStatus();
	void				_DonePowerStatus();


	// Graphics
	COpenGLDevice*			pOpenGLDevice;  // GL entry points, dynamically loaded.
	hgeVertex*				pVB;  // vertex buffer is a client-side array in the OpenGL renderer.
	GLushort*				pIB;  // index buffer is a client-side array in the OpenGL renderer.

	CRenderTargetList*	pTargets;
	CRenderTargetList*	pCurTarget;

	//D3DXMATRIX			matView;
	//D3DXMATRIX			matProj;

	CTextureList*		textures;
	hgeVertex*			VertArray;

	int					nPrim;
	int					CurPrimType;
	int					CurBlendMode;
	HTEXTURE			CurTexture;

	bool				_HaveOpenGLExtension(const char *extlist, const char *ext);
	void 				_UnloadOpenGLEntryPoints();
	bool 				_LoadOpenGLEntryPoints();

	bool				_GfxInit();
	void				_GfxDone();
	bool				_GfxRestore();
	void				_AdjustWindow();
	void				_Resize(int width, int height);
	bool				_init_lost();
	void				_render_batch(bool bEndScene=false);
	void				_SetTextureFilter();
	void				_ConfigureTexture(gltexture *t, int width, int height, DWORD *pixels);
	void				_BindTexture(gltexture *t);
	bool				_BuildTarget(CRenderTargetList *pTarget, GLuint texname, int width, int height, bool zbuffer);
	HTEXTURE			_BuildTexture(int width, int height, DWORD *pixels);
	//int					_format_id(D3DFORMAT fmt);
	void				_SetBlendMode(int blend);
	void				_SetProjectionMatrix(int width, int height);


	// Audio
	void*				hBass;
	void*				hOpenAL;
	bool				bSilent;
	CStreamList*		streams;
	bool				_SoundInit();
	void				_SoundDone();
	void				_SetMusVolume(int vol);
	void				_SetStreamVolume(int vol);
	void				_SetFXVolume(int vol);


	// Input
	int					VKey;
	int					Char;
	int					Zpos;
	float				Xpos;
	float				Ypos;
	SDLMod				keymods;
	bool				bMouseOver;
	bool				bCaptured;
	char				keyz[256];
	CInputEventList*	queue;
	void				_UpdateMouse();
	void				_InputInit();
	void				_ClearQueue();
	void				_BuildEvent(int type, int key, int scan, int flags, int x, int y);
	bool				_ProcessSDLEvent(const SDL_Event &e);

	// Resources
	char				szTmpFilename[_MAX_PATH];
	CResourceList*		res;

	bool				_WildcardMatch(const char *str, const char *wildcard);
	bool				_PrepareFileEnum(const char *wildcard);
	char*				_DoEnumIteration(const bool wantdir);
	DIR*				hSearch;
	char				szSearchDir[_MAX_PATH];
	char				szSearchWildcard[_MAX_PATH];
	char				szSearchResult[_MAX_PATH];

	// Timer
	float				fTime;
	float				fDeltaTime;
	DWORD				nFixedDelta;
	int					nFPS;
	DWORD				t0, t0fps, dt;
	int					cfps;


private:
	HGE_Impl();

	#if PLATFORM_MACOSX
	void				_MacMinimizeWindow();
	void				_MacHideOtherWindows();
	void				_MacHideWindow();
	#endif
};

extern HGE_Impl*		pHGE;

#endif  // include-once blocker

// end of hg_impl_unix.h ...

