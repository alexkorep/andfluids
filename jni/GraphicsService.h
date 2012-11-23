#ifndef _GRAPHICS_SERVICE_H_
#define _GRAPHICS_SERVICE_H_

#include <android_native_app_glue.h>
#include <EGL/egl.h>

class GraphicsService
{
public:
	GraphicsService( android_app* application );

	virtual ~GraphicsService();

	const char* getPath( void );
	const int32_t& getHeight( void );
	const int32_t& getWidth( void );

	void start();
	void stop();
	//status update( void );
	//void renderContext( void );

private:
	android_app* mApplication;

	int32_t mWidth, mHeight;
	EGLDisplay mDisplay;
	EGLSurface mSurface;
	EGLContext mContext;
};

#endif // _GRAPHICS_SERVICE_H_
