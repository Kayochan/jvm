#include <limits.h>
#include <assert.h>
#include "loader/classloader.h"
#include "rtda/thread/jthread.h"
#include "rtda/ma/access.h"
#include "interpreter/interpreter.h"

/*
 * Author: Jia Yang
 */

char bootstrap_classpath[PATH_MAX] = { 0 };
char extension_classpath[PATH_MAX] = { 0 };
char *user_classpath = "D:\\code\\jvm\\test"; // todo;


static void init_jvm(struct classloader *loader, struct jthread *main_thread)
{
    // 先加载sun.mis.VM类，然后执行其类初始化方法
    struct jclass *vm_class = classloader_load_class(loader, "sun/misc/VM");
    if (vm_class == NULL) {
        jvm_abort("vmClass is null\n");  // todo throw exception
        return;
    }

    struct jmethod *init = jclass_lookup_static_method(vm_class, "initialize", "()V");
    if (init == NULL) {
        jvm_abort("error %s\n", jclass_to_string(vm_class));
        return;
    }

    struct stack_frame *init_frame = sf_create(main_thread, init);
    jclass_clinit(vm_class, init_frame);  // todo clinit 方法到底在何时调用
    jthread_push_frame(main_thread, init_frame);
    interpret(main_thread);
}

// todo 这函数干嘛的
void create_main_thread_group(struct classloader *loader, struct jthread *main_thread)
{
    struct jclass *thread_group_class = classloader_load_class(loader, "java/lang/ThreadGroup"); // todo is NULL
    struct jobject *main_thread_group = jobject_create(thread_group_class);

    struct jmethod * constructor = jclass_get_constructor(thread_group_class, "()V");
    struct stack_frame *frame = sf_create(main_thread, constructor);
//    jclass_clinit(thread_group_class, frame);  // todo
    struct slot arg = rslot(main_thread_group);
    sf_set_local_var(frame, 0, &arg);

    jthread_push_frame(main_thread, frame);
    interpret(main_thread);

    // 无需在这里 delete frame, interpret函数会delete调执行过的frame
}

void start_jvm(const char *main_class_name)
{
    assert(main_class_name != NULL);

    struct classloader *loader = classloader_create();
    struct jthread *main_thread = jthread_create();

#if 0
    createMainThreadGroup();
    mainThread->joinToMainThreadGroup();
#endif

//    init_jvm(loader, main_thread);

    if (verbose)
        printvm("loading main class: %s !!!!!!!!!!!!!!!!!!!!!!!!!!!!\n", main_class_name);

    struct jclass *main_class = classloader_load_class(loader, main_class_name);
    if (main_class == NULL) {
        jvm_abort("error %s\n", main_class_name);  // todo
        return;
    }

    struct jmethod *main_method = jclass_lookup_static_method(main_class, "main", "([Ljava/lang/String;)V");
    if (main_method == NULL) {
        jvm_abort("没有找到main方法\n");  // todo
    } else {
        if (!IS_PUBLIC(main_method->access_flags)) {
            // todo
            jvm_abort("..................");
        }
        if (!IS_STATIC(main_method->access_flags)) {
            // todo
            jvm_abort("...................");
        }
    }

    jthread_push_frame(main_thread, sf_create(main_thread, main_method));

    // 开始在主线程中执行 main 方法
    interpret(main_thread);

    // main_thread 退出，做一些清理工作。
    jthread_destroy(main_thread);
    classloader_destroy(loader);
}