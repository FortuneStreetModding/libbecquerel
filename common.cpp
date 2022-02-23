#include "common.h"
#include <algorithm>

namespace bq {

Section::~Section() = default;
BasePane::~BasePane() = default;

template<class T>
void vec2<T>::read(std::istream &stream, bool revEndian) {
    x = readNumber<T>(stream, revEndian);
    y = readNumber<T>(stream, revEndian);
}

template<class T>
void vec3<T>::read(std::istream &stream, bool revEndian) {
    x = readNumber<T>(stream, revEndian);
    y = readNumber<T>(stream, revEndian);
    z = readNumber<T>(stream, revEndian);
}

template<bool padding>
void Txl1<padding>::read(std::istream &stream, bool revEndian) {
    auto count = readNumber<std::uint16_t>(stream, revEndian);
    stream.seekg(2, std::ios::cur); // padding
    auto pos = stream.tellg();
    for (int i=0; i<count; ++i) {
        auto off = readNumber<std::uint32_t>(stream, revEndian);
        if (padding) stream.seekg(4, std::ios::cur);
        {
            TemporarySeek ts(stream, pos + std::streamoff(off));
            textures.push_back(readNullTerminatedStr(stream));
        }
    }
    return true;
}

template<bool padding>
void Fnl1<padding>::read(std::istream &stream, bool revEndian) {
    auto count = readNumber<std::uint16_t>(stream, revEndian);
    stream.seekg(2, std::ios::cur); // padding
    auto pos = stream.tellg();
    for (int i=0; i<count; ++i) {
        auto off = readNumber<std::uint32_t>(stream, revEndian);
        if (padding) stream.seekg(4, std::ios::cur);
        {
            TemporarySeek ts(stream, pos + std::streamoff(off));
            fonts.push_back(readNullTerminatedStr(stream));
        }
    }
    return true;
}

void BlendMode::read(std::istream &stream, bool revEndian) {
    blendOp = (Op)readNumber<std::uint8_t>(stream, revEndian);
    srcFactor = (BlendFactor)readNumber<std::uint8_t>(stream, revEndian);
    destFactor = (BlendFactor)readNumber<std::uint8_t>(stream, revEndian);
    logicOp = (Op)readNumber<std::uint8_t>(stream, revEndian);
}

template<class IO>
TemporarySeek<IO>::TemporarySeek(IO &s, std::streampos seekPos, std::ios::seekdir seekDir) : stream(s) {
    oldSeekPos = stream.tellg();
    stream.seekg(seekPos, seekDir);
}

template<class IO>
TemporarySeek<IO>::~TemporarySeek() {
    stream.seekg(oldSeekPos);
}

std::string readFixedStr(std::istream &stream, int len) {
    std::unique_ptr<char> buf(new char[len]);
    stream.read(buf.get(), len);
    return std::string(buf.get(), len);
}

std::string readNullTerminatedStr(std::istream &stream) {
    std::string res;
    std::getline(stream, res, '\0');
    return res;
}

template<class T>
T readNumber(std::istream &stream, bool reverseEndian) {
    T res;
    char *resPtr = reinterpret_cast<char *>(&res);
    stream.read(resPtr, sizeof(T));
    if (reverseEndian) {
        std::reverse(resPtr, resPtr + sizeof(T));
    }
    return res;
}

color8 readColor8(std::istream &stream, bool reverseEndian) {
    color8 res;
    for (int i=0; i<4; ++i) {
        res[i] = readNumber<std::uint8_t>(stream, reverseEndian);
    }
    return res;
}

color16 readColor16(std::istream &stream, bool reverseEndian) {
    color16 res;
    for (int i=0; i<4; ++i) {
        res[i] = readNumber<std::uint16_t>(stream, reverseEndian);
    }
    return res;
}

color8 toColor8(const color16 &color) {
    color8 res;
    std::copy(color.begin(), color.end(), res.begin());
    return res;
}

color16 toColor16(const color8 &color) {
    color16 res;
    std::copy(color.begin(), color.end(), res.begin());
    return res;
}

template<class T>
BitField<T>::BitField(T &value, int bitStart, int numBits) 
    : theValue(value), theBitStart(bitStart), theNumBits(numBits) {}

template<class T>
BitField<T>::operator T() const {
    return (theValue >> theBitStart) & ((T(1) << theNumBits) - 1);
}

template<class T>
BitField<T> &BitField<T>::operator=(T newBits) {
    T mask = ((T(1) << theNumBits) - 1) << theBitStart;
    theValue &= ~mask;
    theValue |= (newBits << theBitStart);
    return *this;
}

}
