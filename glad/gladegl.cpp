#if !defined(__APPLE__) && !defined(__HAIKU__) \
  && !defined(_WIN32) && !defined(__CYGWIN__)

#include <glad/glad.h>
#include <EGL/egl.h>
#include <dlfcn.h>

static void* libEGL;

typedef void* (APIENTRYP PFNEGLGETPROCADDRESSPROC_PRIVATE)(const char*);
static PFNEGLGETPROCADDRESSPROC_PRIVATE gladGetProcAddressPtr;

static
int open_egl(void) {
    static const char *NAMES[] = {"libEGL.so.1", "libEGL.so"};

    unsigned int index = 0;
    for(index = 0; index < (sizeof(NAMES) / sizeof(NAMES[0])); index++) {
        libEGL = dlopen(NAMES[index], RTLD_NOW | RTLD_GLOBAL);

        if(libEGL != NULL) {
            gladGetProcAddressPtr = (PFNEGLGETPROCADDRESSPROC_PRIVATE)dlsym(libEGL,
                "eglGetProcAddress");
            return gladGetProcAddressPtr != NULL;
        }
    }

    return 0;
}

static
void close_egl(void) {
    if(libEGL != NULL) {
        dlclose(libEGL);
        libEGL = NULL;
    }
}

static
void* get_proc(const char *namez) {
    void* result = NULL;
    if(libEGL == NULL) return NULL;

    if(gladGetProcAddressPtr != NULL) {
        result = gladGetProcAddressPtr(namez);
    }
    if(result == NULL) {
        result = dlsym(libEGL, namez);
    }

    return result;
}

int gladLoadEGL(void) {
    int status = 0;
    PFNEGLBINDAPIPROC bindAPI = NULL;

    if(open_egl()) {
        bindAPI = (PFNEGLBINDAPIPROC)get_proc("eglBindAPI");
        if (bindAPI != NULL && bindAPI(EGL_OPENGL_API)) {
            status = gladLoadGLLoader(&get_proc);
        }
        close_egl();
    }

    return status;
}

#else

int gladLoadEGL(void) {
    return 0;
}

#endif
