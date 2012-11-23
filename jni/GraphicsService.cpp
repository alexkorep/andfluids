#include "GraphicsService.h"

#include <EGL/eglplatform.h>
#include <EGL/egl.h>
#include <GLES/gl.h>
//#include <GLES/glext.h>

#include "log.h"

GraphicsService::GraphicsService( android_app* application )
	: mApplication( application ),
	  mWidth( 0 ), mHeight( 0 ),
	  mDisplay( EGL_NO_DISPLAY ),
	  mSurface( EGL_NO_SURFACE ),
	  mContext( EGL_NO_CONTEXT )
{}

GraphicsService::~GraphicsService()
{
}

const char* GraphicsService::getPath( void )
{
	return "Something";
}

const int32_t& GraphicsService::getHeight( void )
{
	return mHeight;
}

const int32_t& GraphicsService::getWidth( void )
{
	return mHeight;
}

void GraphicsService::start()
{
	EGLint format, numConfigs, errorResult, major, minor;
	EGLConfig config;

	const EGLint attributes[] = {
		EGL_RENDERABLE_TYPE, 1, //EGL_OPENGL_ES_BIT,
		EGL_BLUE_SIZE, 5, EGL_GREEN_SIZE, 6, EGL_RED_SIZE, 5,
		EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
		EGL_NONE
	};

	const EGLNativeDisplayType display_id = EGL_DEFAULT_DISPLAY;
	EGLDisplay display = eglGetDisplay( display_id );

	if ( mDisplay == EGL_NO_DISPLAY )
	{
		LOGI( "EGL returned EGL_NO_DISPLAY. Exiting." );

		goto ERROR;
	}

	if ( !eglInitialize( mDisplay, &major, &minor ) )
	{
		LOGI( "EGL failed to initialize." );

		goto ERROR;
	}

	if( !eglChooseConfig( mDisplay, attributes, &config, 1, &numConfigs ) || ( numConfigs <= 0 ) )
	{
		LOGI( "EGL failed to choose proper configuration." );

		goto ERROR;
	}

	if( !eglGetConfigAttrib( mDisplay, config, EGL_NATIVE_VISUAL_ID, &format ) )
	{
		LOGI( "EGL failed to get configuration attributes." );

		goto ERROR;
	}

	mSurface = eglCreateWindowSurface( mDisplay, config, mApplication->window, NULL );

	if( mSurface == EGL_NO_SURFACE )
	{
		LOGI( "EGL failed to find Surface." );

		goto ERROR;
	}

	if( ( mContext = eglCreateContext( mDisplay, config, EGL_NO_CONTEXT, NULL ) ) == EGL_NO_CONTEXT )
	{
		LOGI( "EGL failed to create context." );

		goto ERROR;
	}

	if( !eglMakeCurrent( mDisplay, mSurface, mSurface, mContext ) )
	{
		LOGI( "EGL could not make context current." );

		goto ERROR;
	}


	if( !eglQuerySurface( mDisplay, mSurface, EGL_WIDTH, &mWidth ) || !eglQuerySurface( mDisplay, mSurface, EGL_HEIGHT, &mHeight ) )
	{
		LOGI( "EGL could not query surface for either width or height" );

		goto ERROR;
	}

	if ( mWidth <= 0 || mHeight <= 0 )
	{
		LOGI( "ERROR: either the window width or window height is read as less than or equal to zero:" );
		LOGI( "\t width => %i; height => %i ", mWidth, mHeight );
	}

	glViewport( 0, 0, mWidth, mHeight );

	ERROR:
		stop( );
}

void GraphicsService::stop()
{

}

/*
void GraphicsService::renderContext( void )
{
	const EGLint attribs[] = {
		EGL_CONTEXT_CLIENT_VERSION, 2,
		EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
		EGL_NONE
	};
}
*/

