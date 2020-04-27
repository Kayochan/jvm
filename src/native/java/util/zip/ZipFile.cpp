#include <minizip/unzip.h>
#include "../../../jni_inner.h"
#include "../../../../symbol.h"
#include "../../../../vmdef.h"
#include "../../../../objects/string_object.h"

/*
 * Author: Yo Ka
 */

// private static native void initIDs();
void initIDs(jclsref clazz)
{
    // todo
}

// private static native long open(String name, int mode, long lastModified, boolean usemmap) throws IOException;
jlong __open(jclsref clazz, jstrref name, jint mode, jlong lastModified, jboolean usemmap)
{
    const char *utf8 = name->toUtf8();

    // todo 其他几个参数怎么搞？？
    unzFile jzfile = unzOpen64(utf8);
    if (jzfile == NULL) {
        throw IOException(utf8);
    }

    return (jlong) (jzfile);
}

// private static native boolean startsWithLOC(long jzfile);
jboolean startsWithLOC(jclsref clazz, jlong jzfile)
{
    // todo
    return jtrue;
}

// private static native int getTotal(long jzfile);
jint getTotal(jclsref clazz, jlong jzfile)
{
    unz_global_info64 info;
    if (unzGetGlobalInfo64((unzFile) jzfile, &info) != UNZ_OK) {
        // todo error
        jvm_abort("unzGetGlobalInfo64 failed\n");
    }
    return info.number_entry;
}

// private static native long getNextEntry(long jzfile, int i);
jlong getNextEntry(jclsref clazz, jlong jzfile, jint i)
{
/*
 * 	vars := frame.LocalVars()
	jzfile := vars.GetLong(0)
	i := vars.GetInt(2)

	jzentry := getJzentry(jzfile, i)

	stack := frame.OperandStack()
	stack.PushLong(jzentry)
 */
    // todo
    jvm_abort("getNextEntry");
}

// // private static native void freeEntry(long jzfile, long jzentry);
void freeEntry(jclsref clazz, jlong jzfile, jlong jzentry)
{
    // todo
}

// private static native long getEntry(long jzfile, byte[] name, boolean addSlash);
jlong getEntry(jclsref clazz, jlong jzfile, jarrref name, jboolean addSlash)
{
    // todo
    /*
     * 	vars := frame.LocalVars()
	jzfile := vars.GetLong(0)
	nameObj := vars.GetRef(2)
	//addSlash := vars.GetBoolean(3)

	// todo
	name := nameObj.GoBytes()
	jzentry := getJzentry2(jzfile, name)

	stack := frame.OperandStack()
	stack.PushLong(jzentry)
     */
    jvm_abort("getEntry");
}

// private static native byte[] getEntryBytes(long jzentry, int type);
jarrref getEntryBytes(jclsref clazz, jlong jzentry, jint type)
{
    jvm_abort("getEntryBytes");
// todo
    /*
     * 	vars := frame.LocalVars()
	jzentry := vars.GetLong(0)
	_type := vars.GetInt(2)

	goBytes := _getEntryBytes(jzentry, _type)
	jBytes := jutil.CastUint8sToInt8s(goBytes)
	byteArr := heap.NewByteArray(jBytes)

	stack := frame.OperandStack()
	stack.PushRef(byteArr)
     */

    /*
    func _getEntryBytes(jzentry int64, _type int32) []byte {
	entry := getEntryFile(jzentry)
	switch _type {
	case JZENTRY_NAME:
		return []byte(entry.Name)
	case JZENTRY_EXTRA:
		return entry.Extra
	case JZENTRY_COMMENT:
		return []byte(entry.Comment)
	}
	jutil.Panicf("BAD type: %v", _type)
	return nil
}
     */
}

// private static native int getEntryFlag(long jzentry);
jint getEntryFlag(jclsref clazz, jlong jzentry)
{
    jvm_abort("getEntryFlag");
/*
 * todo
 * 	entry := _getEntryPop(frame)
	flag := int32(entry.Flags)

	stack := frame.OperandStack()
	stack.PushInt(flag)
 */
}

// private static native long getEntryTime(long jzentry);
jlong getEntryTime(jclsref clazz, jlong jzentry)
{
    jvm_abort("getEntryTime");
/*
 * todo
 * 	entry := _getEntryPop(frame)
	modDate := entry.ModifiedDate
	modTime := entry.ModifiedTime
	time := int64(modDate)<<16 | int64(modTime)

	stack := frame.OperandStack()
	stack.PushLong(time)
 */
}

// private static native long getEntryCrc(long jzentry);
jlong getEntryCrc(jclsref clazz, jlong jzentry)
{
    jvm_abort("getEntryCrc");
    /*
     * todo
     * 	entry := _getEntryPop(frame)
	crc := int64(entry.CRC32)

	stack := frame.OperandStack()
	stack.PushLong(crc)
     */
}

// private static native long getEntrySize(long jzentry);
jlong getEntrySize(jclsref clazz, jlong jzentry)
{
    jvm_abort("getEntrySize");
/*
 * todo
 * 	entry := _getEntryPop(frame)
	size := int64(entry.UncompressedSize64)

	stack := frame.OperandStack()
	stack.PushLong(size)
 */
}

// private static native long getEntryCSize(long jzentry);
jlong getEntryCSize(jclsref clazz, jlong jzentry)
{
    jvm_abort("getEntryCSize");
    /*
     * todo
     *
     * 	// entry := _getEntryPop(frame)
	// size := int64(entry.CompressedSize64)

	// stack := frame.OperandStack()
	// stack.PushLong(size)

	// todo
	getEntrySize(frame)

     */
}

// private static native int getEntryMethod(long jzentry);
jint getEntryMethod(jclsref clazz, jlong jzentry)
{
    jvm_abort("getEntryMethod");
    /*
     * todo
     * 	// entry := _getEntryPop(frame)
	// method := int32(entry.Method)

	// todo
	stack := frame.OperandStack()
	stack.PushInt(0)
     */

    /*
     * func _getEntryPop(frame *runtime.Frame) *gozip.File {
	vars := frame.LocalVars()
	jzentry := vars.GetLong(0)

	entry := getEntryFile(jzentry)
	return entry
}
     */
}

// private static native int read(long jzfile, long jzentry, long pos, byte[] b, int off, int len);
jint __read(jclsref clazz, jlong jzfile, jlong jzentry, jlong pos, jarrref b, jint off, jint len)
{
    jvm_abort("read");
/*
 * todo
 * 	vars := frame.LocalVars()
	//jzfile := vars.GetLong(0)
	jzentry := vars.GetLong(2)
	pos := vars.GetLong(4)
	byteArr := vars.GetRef(6)
	off := vars.GetInt(7)
	_len := vars.GetInt(8)

	goBytes := byteArr.GoBytes()
	goBytes = goBytes[off : off+_len]
	n := readEntry(jzentry, pos, goBytes)

	stack := frame.OperandStack()
	stack.PushInt(int32(n))
 */
}

static JNINativeMethod methods[] = {
        JNINativeMethod_registerNatives,
        { "initIDs", "()V", (void *) initIDs },
        { "open", "(Ljava/lang/String;IJZ)J", (void *) __open },
        { "startsWithLOC", "(J)Z", (void *) startsWithLOC },
        { "getTotal", "(J)I", (void *) getTotal },
        { "getNextEntry", "(JI)J", (void *) getNextEntry },
        { "freeEntry", "(JJ)V", (void *) freeEntry },
        { "getEntry", "(J[BZ)J", (void *) getEntry },
        { "getEntryBytes", "(JI)[B", (void *) getEntryBytes },
        { "getEntryFlag", "(J)I", (void *) getEntryFlag },
        { "getEntryTime", "(J)J", (void *) getEntryTime },
        { "getEntryCrc", "(J)J", (void *) getEntryCrc },
        { "getEntrySize", "(J)J", (void *) getEntrySize },
        { "getEntryCSize", "(J)J", (void *) getEntryCSize },
        { "getEntryMethod", "(J)I", (void *) getEntryMethod },
        { "read", "((JJJ[BII)I", (void *) __read },
};

void java_util_zip_ZipFile_registerNatives()
{
    registerNatives("java/util/zip/ZipFile", methods, ARRAY_LENGTH(methods));
}
