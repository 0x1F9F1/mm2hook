#pragma once
#include "mm2_common.h"

namespace MM2
{
    // Forward declarations
    extern class Stream;

    namespace $
    {
        namespace Stream
        {
            HOOK_EXPORT(0x4C9970, _Func<void>, DumpOpenFiles);

            HOOK_EXPORT(0x4C99C0, _Func<MM2::Stream *>, Open$1);
            HOOK_EXPORT(0x4C9A00, _Func<MM2::Stream *>, Create$1);

            HOOK_EXPORT(0x4C9A40, _Func<MM2::Stream *>, Open$2);
            HOOK_EXPORT(0x4C9A70, _Func<MM2::Stream *>, Create$2);

            HOOK_EXPORT(0x4C9AA0, _MemberFunc<int>, Read);
            HOOK_EXPORT(0x4C9BF0, _MemberFunc<int>, Write);
            HOOK_EXPORT(0x4C9D00, _MemberFunc<int>, GetCh);
            HOOK_EXPORT(0x4C9D30, _MemberFunc<int>, PutCh);
            HOOK_EXPORT(0x4C9D60, _MemberFunc<int>, Seek);
            HOOK_EXPORT(0x4C9DB0, _MemberFunc<int>, Tell);
            HOOK_EXPORT(0x4C9DC0, _MemberFunc<int>, Close);
            HOOK_EXPORT(0x4C9E00, _MemberFunc<int>, Size);
            HOOK_EXPORT(0x4C9E60, _MemberFunc<int>, Flush);
        }

        HOOK_EXPORT(0x4C9F20, _Func<int>, fseek);
        HOOK_EXPORT(0x4C9F80, _Func<int>, fgets);
        HOOK_EXPORT(0x4C9FF0, _Func<int>, fscanf);
    }

    typedef void (*EnumFilesCallback)(LPCSTR, bool, LPVOID);

    enum seekWhence {
        seek_begin,
        seek_current,
        seek_end,
    };

    /* most of the methods are optional */
    struct coreFileMethods {
        int (*open)(LPCSTR filename, bool readOnly);
        int (*create)(LPCSTR filename);
        int (*read)(int handle, LPVOID buffer, int length);
        int (*write)(int handle, const LPVOID buffer, int length);
        int (*seek)(int handle, int position, seekWhence whence);
        int (*close)(int handle);
        int (*enumFiles)(LPCSTR, EnumFilesCallback, LPVOID);
        int (*size)(int handle);
        int (*flush)(int handle); // usually set to null
    };

    class Stream {
    public:
        AGE_API static void DumpOpenFiles(void) {
            $::Stream::DumpOpenFiles();
        };

        AGE_API static Stream* Open(LPCSTR filename, bool isZipFile) {
            return $::Stream::Open$1(filename, isZipFile);
        };

        AGE_API static Stream* Open(LPCSTR filename, const coreFileMethods *fileMethods, bool isZipFile) {
            return $::Stream::Open$2(filename, fileMethods, isZipFile);
        };

        AGE_API static Stream* Create(LPCSTR filename) {
            return $::Stream::Create$1(filename);
        };

        AGE_API static Stream* Create(LPCSTR filename, const coreFileMethods *fileMethods) {
            return $::Stream::Create$2(filename, fileMethods);
        };

        AGE_API int Read(LPVOID dstBuf, int size) {
            return $::Stream::Read(this, dstBuf, size);
        };

        AGE_API int Write(const LPVOID srcBuf, int size) {
            return $::Stream::Write(this, srcBuf, size);
        };

        AGE_API int GetCh(void) {
            return $::Stream::GetCh(this);
        };

        AGE_API int PutCh(unsigned char ch) {
            return $::Stream::PutCh(this, ch);
        };

        AGE_API int Seek(int offset) {
            return $::Stream::Seek(this, offset);
        };

        AGE_API int Tell(void) {
            return $::Stream::Tell(this);
        };

        AGE_API int Close(void) {
            return $::Stream::Close(this);
        };

        AGE_API int Size(void) {
            return $::Stream::Size(this);
        };

        AGE_API int Flush(void) {
            return $::Stream::Flush(this);
        };
    };

    namespace
    {
        AGE_EXT int fseek(Stream *stream, int position, seekWhence whence) {
            return $::fseek(stream, position, whence);
        }

        AGE_EXT int fgets(char *buffer, int length, Stream *stream) {
            return $::fgets(buffer, length, stream);
        }

        AGE_EXT void fprintf(Stream *stream, char const *format, ...) {
            /* It's much easier to just rewrite the function since it's so simple */
            char buf[512];
            va_list va;
            va_start(va, format);
            _vsnprintf(buf, sizeof(buf), format, va);
            va_end(va);

            stream->Write(buf, strlen(buf));
        }

        AGE_EXT int fscanf(Stream *stream, char const *format, ...) {
            /*
                I think I need a shower after writing this...
                (Don't blame me, this is how Angel does it when calling sscanf!)
            */
            va_list va;
            va_start(va, format);
            int result = $::fscanf(stream, format, *va, *(va + 1), *(va + 2), *(va + 3), *(va + 4), *(va + 5), *(va + 6), *(va + 7));
            va_end(va);

            return result;
        }
    }
}