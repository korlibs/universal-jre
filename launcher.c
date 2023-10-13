// clang -O3 -arch x86_64 -arch arm64 -Ijdk-21+35/Contents/Home/include -Ijdk-21+35/Contents/Home/include/darwin -framework Cocoa -o app launcher.c
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <err.h>
#include <dlfcn.h>
#include <pthread.h>

#include <mach-o/dyld.h>
#include <CoreFoundation/CoreFoundation.h>

#include "./jdk-21+35/Contents/Home/include/jni.h"

typedef jint (JNICALL CreateJavaVM_t)(JavaVM **, void **, void *);

static char app_dir[PATH_MAX];

/* Dummy callback for the main thread loop. */
static void
dummy_callback(void *info) { }

static char *
get_application_directory(char *buffer, uint32_t len)
{
    char *last_slash = NULL;
    int n = 2;

    if ( !_NSGetExecutablePath(buffer, &len) ) {
        //getcwd(buffer, len);
        //buffer[strlen(buffer)] = '/';
        //buffer[strlen(buffer) + 1] = 0;
    }

    //printf("APP[a]: '%s'\n", buffer);

    if ( (last_slash = strrchr(buffer, '/')) ) {
        *last_slash = '\0';
    }

    //printf("APP[b]: '%s'\n", buffer);

    return last_slash ? buffer : NULL;
}

/* Execute the main method of our application. */
static int
start_java_main(JNIEnv *env)
{
    jclass main_class;
    jmethodID main_method;
    jobjectArray main_args;
    char *mainClassName = "MainKt";

    if ( ! (main_class = (*env)->FindClass(env, mainClassName)) ) {
        errx(EXIT_FAILURE, "Can't find class: %s", mainClassName);
        return -1;
    }

    if ( ! (main_method = (*env)->GetStaticMethodID(env, main_class, "main",
                                                    "([Ljava/lang/String;)V")) )
        return -1;

    main_args = (*env)->NewObjectArray(env, 0,
                                       (*env)->FindClass(env, "java/lang/String"),
                                       (*env)->NewStringUTF(env, ""));

    (*env)->CallStaticVoidMethod(env, main_class, main_method, main_args);

    return 0;
}

#define ADD_OPENS(module) "--add-opens=" module "=ALL-UNNAMED"

/* Load and start the Java virtual machine. */
static void *
start_jvm(void *arg)
{
    char lib_path[PATH_MAX];
    void *lib;
    JavaVMInitArgs jvm_args;
    JavaVMOption jvm_opts[32];
    JavaVM *jvm;
    JNIEnv *env;
    CreateJavaVM_t *create_java_vm;

    (void) arg;

    /* Load the Java library in the bundled JRE. */
    snprintf(lib_path, PATH_MAX, "%s/jre/lib/libjli.dylib", app_dir);
    if ( ! (lib = dlopen(lib_path, RTLD_LAZY)) )
        errx(EXIT_FAILURE, "Cannot load Java library: %s", dlerror());

    if ( ! (create_java_vm = (CreateJavaVM_t *)dlsym(lib, "JNI_CreateJavaVM")) )
        errx(EXIT_FAILURE, "Cannot find JNI_CreateJavaVM: %s", dlerror());

    /* Prepare options for the JVM. */
    int nopts = 0;
    jvm_opts[nopts++].optionString = "-Djava.class.path=./app.jar";

    jvm_opts[nopts++].optionString = ADD_OPENS("java.desktop/sun.java2d.opengl");
    jvm_opts[nopts++].optionString = ADD_OPENS("java.desktop/java.awt");
    jvm_opts[nopts++].optionString = ADD_OPENS("java.desktop/sun.awt");
    jvm_opts[nopts++].optionString = ADD_OPENS("java.desktop/sun.lwawt");
    jvm_opts[nopts++].optionString = ADD_OPENS("java.desktop/sun.lwawt.macosx");
    jvm_opts[nopts++].optionString = ADD_OPENS("java.desktop/com.apple.eawt");
    jvm_opts[nopts++].optionString = ADD_OPENS("java.desktop/com.apple.eawt.event");
    jvm_opts[nopts++].optionString = ADD_OPENS("java.desktop/sun.awt.X11");

    jvm_args.version = JNI_VERSION_1_2;
    jvm_args.ignoreUnrecognized = JNI_TRUE;
    jvm_args.options = jvm_opts;
    jvm_args.nOptions = nopts;

    if ( create_java_vm(&jvm, (void **)&env, &jvm_args) == JNI_ERR )
        errx(EXIT_FAILURE, "Cannot create Java virtual machine");

    if ( start_java_main(env) != 0 ) {
        (*jvm)->DestroyJavaVM(jvm);
        errx(EXIT_FAILURE, "Cannot start Java main method");
    }

    if ( (*env)->ExceptionCheck(env) ) {
        (*env)->ExceptionDescribe(env);
        (*env)->ExceptionClear(env);
    }

    (*jvm)->DetachCurrentThread(jvm);
    (*jvm)->DestroyJavaVM(jvm);

    /* Calling exit() here will terminate both this JVM thread and the
     * infinite loop in the main thread. */
    exit(EXIT_SUCCESS);
}

int
main(int argc, char **argv)
{
    pthread_t jvm_thread;
    pthread_attr_t jvm_thread_attr;
    CFRunLoopSourceContext loop_context;
    CFRunLoopSourceRef loop_ref;

    (void) argc;
    (void) argv;

    if ( ! get_application_directory(app_dir, PATH_MAX) )
        errx(EXIT_FAILURE, "Cannot get application directory");

    if ( chdir(app_dir) == -1 )
        err(EXIT_FAILURE, "Cannot change current directory");

    /* Start the thread where the JVM will run. */
    pthread_attr_init(&jvm_thread_attr);
    pthread_attr_setscope(&jvm_thread_attr, PTHREAD_SCOPE_SYSTEM);
    pthread_attr_setdetachstate(&jvm_thread_attr, PTHREAD_CREATE_DETACHED);
    if ( pthread_create(&jvm_thread, &jvm_thread_attr, start_jvm, NULL) != 0 )
        err(EXIT_FAILURE, "Cannot start JVM thread");
    pthread_attr_destroy(&jvm_thread_attr);

    /* Run a dummy loop in the main thread. */
    memset(&loop_context, 0, sizeof(loop_context));
    loop_context.perform = &dummy_callback;
    loop_ref = CFRunLoopSourceCreate(NULL, 0, &loop_context);
    CFRunLoopAddSource(CFRunLoopGetCurrent(), loop_ref, kCFRunLoopCommonModes);
    CFRunLoopRun();

    return EXIT_SUCCESS;
}

