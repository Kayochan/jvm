/*
 * Author: kayo
 */

//#include <thread>
#include "thread_info.h"
#include "../kayo.h"
#include "../debug.h"
#include "../objects/class_loader.h"
#include "../objects/object.h"
#include "../objects/class.h"
#include "../objects/field.h"
#include "../interpreter/interpreter.h"
#include "Frame.h"

#if TRACE_THREAD
#define TRACE PRINT_TRACE
#else
#define TRACE(x)
#endif

using namespace std;

// Thread specific key holding a Thread
static pthread_key_t thread_key;

//thread_local Thread *curr_thread;

Thread *getCurrentThread()
{
    return (Thread *) pthread_getspecific(thread_key);
//    return curr_thread;
}

static inline void saveCurrentThread(Thread *thread)
{
    pthread_setspecific(thread_key, thread);
//    curr_thread = thread;
}

// Various field and method into java.lang.Thread cached at startup and used in thread creation
static Field *eetopField;
static Field *threadStatusField;
static Method *runMethod;

// Cached java.lang.Thread class
static Class *threadClass;

Thread *mainThread;

Thread *initMainThread()
{
    pthread_key_create(&thread_key, nullptr);

    threadClass = loadBootClass(S(java_lang_Thread));

    eetopField = threadClass->lookupInstField("eetop", S(J));
    threadStatusField = threadClass->lookupInstField("threadStatus", S(I));
    runMethod = threadClass->lookupInstMethod(S(run), S(___V));

    mainThread = new Thread();

    initClass(threadClass);
//    threadClass->clinit();

    Class *threadGroupClass = loadBootClass(S(java_lang_ThreadGroup));
    sysThreadGroup = newObject(threadGroupClass);

    // 初始化 system_thread_group
    // java/lang/ThreadGroup 的无参数构造函数主要用来：
    // Creates an empty Thread group that is not in any Thread group.
    // This method is used to create the system Thread group.
    initClass(threadGroupClass);
//    threadGroupClass->clinit();
    execJavaFunc(threadGroupClass->getConstructor(S(___V)), sysThreadGroup);

    mainThread->setThreadGroupAndName(sysThreadGroup, MAIN_THREAD_NAME);
    saveCurrentThread(mainThread);
    return mainThread;
}

void createVMThread(void *(*start)(void *), const utf8_t *thread_name)
{
    assert(start != nullptr && thread_name != nullptr); // vm thread must have a name

//    static auto start = [](void *args) {
//        auto a = (VMThreadInitInfo *) args;
//        auto newThread = new Thread();
//        newThread->setThreadGroupAndName(sysThreadGroup, a->threadName);
//        return a->start(nullptr);
//    };
//
//    pthread_t tid;
//    int ret = pthread_create(&tid, nullptr, start, (void *) info);
//    if (ret != 0) {
//        thread_throw(new InternalError("create Thread failed"));
//    }

//    static auto __start = [](void *(*start)(void *), const utf8_t *thread_name) {
//        auto new_thread = new Thread();
//        new_thread->setThreadGroupAndName(sysThreadGroup, thread_name);
//        return start(nullptr);
//    };
//
//    std::thread t(__start, start, thread_name);
//    t.detach();
}

void createCustomerThread(Object *jThread)
{
    assert(jThread != nullptr);

    static auto start = [](void *args0) {
        auto __jThread = (jref) args0;
        auto newThread = new Thread(__jThread);
        return (void *) execJavaFunc(runMethod, __jThread);
    };

    pthread_t tid;
    int ret = pthread_create(&tid, nullptr, start, jThread);
    if (ret != 0) {
        thread_throw(new InternalError("create Thread failed"));
    }

//    static auto __start = [](Object *jThread) {
//        auto new_thread = new Thread(jThread);
//        return (void *) execJavaFunc(runMethod, jThread);
//    };
//
//    std::thread t(__start, jThread);
//    t.detach();
}

static pthread_mutex_t newThreadMutex = PTHREAD_MUTEX_INITIALIZER;
//static mutex new_thread_mutex;

Thread::Thread(Object *tobj0, jint priority): tobj(tobj0)
{
    assert(THREAD_MIN_PRIORITY <= priority && priority <= THREAD_MAX_PRIORITY);

//    scoped_lock lock(new_thread_mutex);
    pthread_mutex_lock(&newThreadMutex);

    saveCurrentThread(this);
    g_all_threads.push_back(this);

    tid = pthread_self();
//    tid = this_thread::get_id();

    if (tobj == nullptr)
        tobj = newObject(threadClass);

    tobj->setFieldValue(eetopField, (jlong) this);
    tobj->setFieldValue(S(priority), S(I), priority);
//    if (vmEnv.sysThreadGroup != nullptr)   todo
//        setThreadGroupAndName(vmEnv.sysThreadGroup, nullptr);

    pthread_mutex_unlock(&newThreadMutex);
}

Thread *Thread::from(Object *tobj0)
{
    assert(tobj0 != nullptr);
    assert(0 <= eetopField->id && eetopField->id < tobj0->clazz->instFieldsCount);
    return *(Thread **)(tobj0->data + eetopField->id);//jThread0->getInstFieldValue<Thread *>(eetopField);
}

Thread *Thread::from(jlong threadId)
{
    jvm_abort("ffffffffffffffffffffffff");
    for (Thread *t : g_all_threads) {
        // private long tid; // Thread ID
        auto tid = t->tobj->getInstFieldValue<jlong>("tid", "J");
        if (tid == threadId)
            return t;
    }

    TRACE(NEW_MSG("invaild: %lld\n", threadId)); // todo 无效的 threadId
    return nullptr;
}

void Thread::setThreadGroupAndName(Object *threadGroup, const char *threadName)
{
    assert(threadGroup != nullptr);
    assert(threadName != nullptr);

    // 调用 java/lang/Thread 的构造函数
    Method *constructor = tobj->clazz->getConstructor("(Ljava/lang/ThreadGroup;Ljava/lang/String;)V");
    execJavaFunc(constructor, tobj, threadGroup, newString(threadName));
}

void Thread::setStatus(jint status)
{
    tobj->setFieldValue(threadStatusField, status);
}

jint Thread::getStatus()
{
    return tobj->getInstFieldValue<jint>(threadStatusField);
}

bool Thread::isAlive()
{
    assert(tobj != nullptr);
    auto status = tobj->getInstFieldValue<jint>("threadStatus", "I");
    // todo
    return false;
}

Frame *Thread::allocFrame(Method *m, bool vm_invoke)
{
    assert(m != nullptr);

    intptr_t mem = topFrame == nullptr ? (intptr_t) vmStack : topFrame->end();
    auto size = sizeof(Frame) + (m->maxLocals + m->maxStack) * sizeof(slot_t);
    if (mem + size - (intptr_t) vmStack > VM_STACK_SIZE) {
//        thread_throw(new StackOverflowError);
        // todo 栈已经溢出无法执行程序了。不要抛异常了，无法执行了。
        jvm_abort("StackOverflowError");
    }

    auto lvars = (slot_t *)(mem);
    auto newFrame = (Frame *)(lvars + m->maxLocals);
    auto ostack = (slot_t *)(newFrame + 1);
    topFrame = new(newFrame) Frame(m, vm_invoke, lvars, ostack, topFrame);
    return topFrame;
}

void Thread::popFrame()
{
    assert(topFrame != nullptr);
    topFrame = topFrame->prev;
}

int Thread::countStackFrames()
{
    int count = 0;
    for (Frame *frame = topFrame; frame != nullptr; frame = frame->prev) {
        count++;
    }
    return count;
}

vector<Frame *> Thread::getStackFrames()
{
    vector<Frame *> vec;
    for (auto frame = topFrame; frame != nullptr; frame = frame->prev) {
        vec.push_back(frame);
    }
    vec.reserve(vec.size());
    assert(vec.size() == countStackFrames());
    return vec;
}

jref Thread::to_java_lang_management_ThreadInfo(jbool lockedMonitors, jbool lockedSynchronizers, jint maxDepth)
{
    // todo
//    jvm_abort("to_java_lang_management_ThreadInfo\n");
//    return nullptr;

    // private volatile String name;
    auto name = tobj->getInstFieldValue<jstrref>("name", "Ljava/lang/String;");
    // private long tid;
    auto tid = tobj->getInstFieldValue<jlong>("tid", "J");

    Class *c = loadBootClass("java/lang/management/ThreadInfo");
    c->clinit();
    jref threadInfo = newObject(c);
    // private String threadName;
    threadInfo->setFieldValue("threadName", "Ljava/lang/String;", name);
    // private long threadId;
    threadInfo->setFieldValue("threadId", "J", tid);

    return threadInfo;

}

Array *Thread::dump(int maxDepth)
{
    vector<Frame *> vec = getStackFrames();
    size_t size = vec.size();
    if (maxDepth >= 0 && size > maxDepth) {
        size = (size_t) maxDepth;
    }

    auto c = loadBootClass(S(java_lang_StackTraceElement));
    // public StackTraceElement(String declaringClass, String methodName, String fileName, int lineNumber);
    Method *constructor = c->getConstructor("(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;I)V");

    Array *arr = newArray(loadArrayClass(S(array_java_lang_StackTraceElement)), size);
    for (size_t i = 0; i < size; i++) {
        Frame *f = vec[i];
        jref o = newObject(c);
        execJavaFunc(constructor, { to_rslot(o),
                                    to_rslot(newString(f->method->clazz->className)),
                                    to_rslot(newString(f->method->name)),
                                    to_rslot(newString(f->method->clazz->sourceFileName)),
                                    to_islot(f->method->getLineNumber(f->reader.pc)) }
        );
        arr->set(i, o);
    }

    return arr;
}

[[noreturn]] void thread_uncaught_exception(Object *exception)
{
    assert(exception != nullptr);

    Thread *thread = getCurrentThread();
    thread->clearVMStack();
    Method *pst = exception->clazz->lookupInstMethod(S(printStackTrace), S(___V));
    assert(pst != nullptr);
    execJavaFunc(pst, exception);

    // 结束 this thread todo
    jvm_abort("thread_uncaught_exception\n");
}

[[noreturn]] void thread_throw(Throwable *t)
{
    assert(t != nullptr);
    thread_uncaught_exception(t->getJavaThrowable());
}