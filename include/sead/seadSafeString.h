#ifndef SEAD_SAFE_STRING_H_
#define SEAD_SAFE_STRING_H_

#include <stdarg.h>
#include <string>

namespace sead {

template <typename T>
class SafeStringBase
{
public:
    SafeStringBase() : mStringTop(cNullString) { }
    SafeStringBase(const T* str) : mStringTop(str) { }
    virtual ~SafeStringBase() { }

    virtual void assureTerminationImpl_() const { }

    inline const T* c_str() const
    {
        assureTerminationImpl_();
        return mStringTop;
    }

    static const T cNullChar;
    static const T cNullString[1];
    static const T cLineBreakChar;
    static const SafeStringBase cEmptyString;
    static const s32 cMaximumLength = 0x40000;

    const T* mStringTop;
};

template<typename T>
const T SafeStringBase<T>::cNullChar = 0;

template<typename T>
const T SafeStringBase<T>::cLineBreakChar = static_cast<T>('\n');

template<typename T>
const T SafeStringBase<T>::cNullString[1] = { SafeStringBase<T>::cNullChar };

template<typename T>
const SafeStringBase<T> SafeStringBase<T>::cEmptyString;

template <typename T>
class BufferedSafeStringBase : public SafeStringBase<T>
{
public:
    BufferedSafeStringBase(const T* buffer, s32 size)
    {
        mStringTop = buffer;
        mBufferSize = size;
        assureTerminationImpl_();
    }

    virtual ~BufferedSafeStringBase() { }

    virtual void assureTerminationImpl_() const
    {
        BufferedSafeStringBase<T>* mutableSafeString = const_cast<BufferedSafeStringBase<T>*>(this);
        mutableSafeString->getMutableStringTop_()[mBufferSize - 1] = mutableSafeString->cNullChar;
    };

    void formatV(T const*, va_list);
    void format(T const*, ...);
    void appendWithFormatV(T const*, va_list);
    void appendWithFormat(T const*, ...);

    inline T* getMutableStringTop_()
    {
        return const_cast<T*>(mStringTop);
    }

    inline void clear()
    {
        getMutableStringTop_()[0] = this->cNullChar;
    }

    inline void copy(const SafeStringBase<T>& src)
    {
        std::char_traits<T>::copy(getMutableStringTop_(), src.c_str(), cMaximumLength);
        //...
    }

    s32 mBufferSize;
};

template <typename T, s32 L>
class FixedSafeStringBase : public BufferedSafeStringBase<T>
{
public:
    FixedSafeStringBase() : BufferedSafeStringBase<T>(mBuffer, L)
    {
        clear();
    }

    virtual ~FixedSafeStringBase() { }

    T mBuffer[L];
};

typedef SafeStringBase<char> SafeString;
typedef BufferedSafeStringBase<char> BufferedSafeString;

template <s32 L>
class FixedSafeString : public FixedSafeStringBase<char, L>
{
public:
    FixedSafeString() : FixedSafeStringBase<char, L>() { }

    explicit FixedSafeString(const SafeString& str) : FixedSafeStringBase<char, L>()
    {
        copy(str);
    }
};

template <s32 L>
class FormatFixedSafeString : public FixedSafeStringBase<char, L>
{
public:
    FormatFixedSafeString(const char* str, ...);
    virtual ~FormatFixedSafeString() { }
};

} // namespace sead

#endif // SEAD_SAFE_STRING_H_
