/*
 * Copyright (C) 2010 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include <android_native_app_glue.h>
#include <android/native_window.h>
#include <android/input.h>
#include <android/looper.h>

//#include <errno.h>
//#include <jni.h>
//#include <sys/time.h>
//#include <time.h>
#include <android/log.h>

#include <stdio.h>
#include <stdlib.h>
//#include <math.h>

#include "fluids/fluids/fluid_system.h"

#define  LOG_TAG    "andfluids"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGW(...)  __android_log_print(ANDROID_LOG_WARN,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

class MyFluidSystem: public FluidSystem {
private:
	int m_xMin;
	int m_yMin;
	double m_xRatio;
	double m_yRatio;

	void normalizeCoords(const ANativeWindow_Buffer& buffer)
	{

	}
public:
	void DrawToBuffer (ANativeWindow_Buffer* buffer)
	{
		normalizeCoords(*buffer);

		uint16_t* pixels = (uint16_t*)buffer->bits;
		const char* dat = mBuf[0].data;
		for (int n=0; n < NumPoints(); n++) {
			const Point* p = (Point*) dat;

			// Getting coordinates
			int x = (p->pos.x + p->pos.y/2 + 50) * 10;
			if (x > buffer->width) {
				x = buffer->width - 1;
			} else if (x < 0) {
				x = 0;
			}

			int y = (p->pos.z + p->pos.y/2+ 20) * 10;
			if (y > buffer->height) {
				y = buffer->height - 1;
			} else if (y < 0) {
				y = 0;
			}

			// Actual drawing
			uint16_t* line = pixels + y*buffer->stride;
			line[x] = 0xFFFF;
		    //LOGI("====== draw(%d, %d)\n", x, y);

			dat += mBuf[0].stride;
		}
	}
};

MyFluidSystem psys;
float view_matrix[16] = {0};					// View matrix (V)

static void fill_plasma(ANativeWindow_Buffer* buffer)
{

    void* pixels = buffer->bits;
    //LOGI("width=%d height=%d stride=%d format=%d", buffer->width, buffer->height,
    //        buffer->stride, buffer->format);

    int  yy;
    for (yy = 0; yy < buffer->height; yy++) {
        uint16_t*  line = (uint16_t*)pixels;
        int xx;
        for (xx = 0; xx < buffer->width; xx++) {
            line[xx] = 0x0000;
        }

        // go to next line
        pixels = (uint16_t*)pixels + buffer->stride;
    }

    psys.DrawToBuffer(buffer);
}


// ----------------------------------------------------------------------

struct engine {
    struct android_app* app;
    int animating;
};

static void engine_draw_frame(struct engine* engine) {
    if (engine->app->window == NULL) {
        // No window.
        return;
    }

    ANativeWindow_Buffer buffer;
    if (ANativeWindow_lock(engine->app->window, &buffer, NULL) < 0) {
        LOGW("Unable to lock window buffer");
        return;
    }

    fill_plasma(&buffer);
    psys.Draw ( &view_matrix[0], 0.55 );			// Draw particles

    ANativeWindow_unlockAndPost(engine->app->window);

}

static int engine_term_display(struct engine* engine) {
    engine->animating = 0;
    return 0;
}

static int32_t engine_handle_input(struct android_app* app, AInputEvent* event) {
    struct engine* engine = (struct engine*)app->userData;
    if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION) {
        engine->animating = 1;
        return 1;
    } else if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_KEY) {
        LOGI("Key event: action=%d keyCode=%d metaState=0x%x",
                AKeyEvent_getAction(event),
                AKeyEvent_getKeyCode(event),
                AKeyEvent_getMetaState(event));
    }

    return 0;
}

static void engine_handle_cmd(struct android_app* app, int32_t cmd) {
    struct engine* engine = (struct engine*)app->userData;
    switch (cmd) {
        case APP_CMD_INIT_WINDOW:
            if (engine->app->window != NULL) {
                engine_draw_frame(engine);
            }
            break;
        case APP_CMD_TERM_WINDOW:
            engine_term_display(engine);
            break;
        case APP_CMD_LOST_FOCUS:
            engine->animating = 0;
            engine_draw_frame(engine);
            break;
    }
}

void android_main(struct android_app* state) {
    static int init;

    struct engine engine;

    const int psys_nmax = 2048;

    Vector3DF	obj_from, obj_angs;
	obj_from.x = 0;		obj_from.y = 0;		obj_from.z = 20;		// emitter
	obj_angs.x = 118.7;	obj_angs.y = 200;	obj_angs.z = 1.0;

	psys.Initialize ( BFLUID, psys_nmax );
	psys.SPH_CreateExample ( 0, psys_nmax );
	psys.SetVec ( EMIT_ANG, Vector3DF ( obj_angs.x, obj_angs.y, obj_angs.z ) );
	psys.SetVec ( EMIT_POS, Vector3DF ( obj_from.x, obj_from.y, obj_from.z ) );

	psys.SetParam ( PNT_DRAWMODE, int(1) );
	psys.SetParam ( CLR_MODE, 0 );

    // Make sure glue isn't stripped.
    app_dummy();

    memset(&engine, 0, sizeof(engine));
    state->userData = &engine;
    state->onAppCmd = engine_handle_cmd;
    state->onInputEvent = engine_handle_input;
    engine.app = state;

    // loop waiting for stuff to do.

    while (1) {
        // Read all pending events.
        int ident;
        int events;
        struct android_poll_source* source;

        // If not animating, we will block forever waiting for events.
        // If animating, we loop until all events are read, then continue
        // to draw the next frame of animation.
        while ((ident=ALooper_pollAll(engine.animating ? 0 : -1, NULL, &events,
                (void**)&source)) >= 0) {

            // Process this event.
            if (source != NULL) {
                source->process(state, source);
            }

            // Check if we are exiting.
            if (state->destroyRequested != 0) {
                LOGI("Engine thread destroy requested!");
                engine_term_display(&engine);
                return;
            }
        }

        if (engine.animating) {
        	psys.Run ();
            engine_draw_frame(&engine);
        }
    }
}
