#include "common.h"
#include <algorithm>

template<bool padding>
bool Txl1<padding>::read(std::istream &stream, bool revEndian) {
    auto count = readNumber<std::uint16_t>(stream, revEndian);
    stream.seekg(2, std::ios::cur); // padding
    int pos = stream.tellg();
    for (int i=0; i<count; ++i) {
        auto off = readNumber<std::uint32_t>(stream, revEndian);
        if (padding) stream.seekg(4, std::ios::cur);
        {
            TemporarySeek ts(stream, pos + off);
            textures.push_back(readNullTerminatedStr(stream));
        }
    }
    return true;
}

template<bool padding>
bool Fnl1<padding>::read(std::istream &stream, bool revEndian) {
    auto count = readNumber<std::uint16_t>(stream, revEndian);
    stream.seekg(2, std::ios::cur); // padding
    int pos = stream.tellg();
    for (int i=0; i<count; ++i) {
        auto off = readNumber<std::uint32_t>(stream, revEndian);
        if (padding) stream.seekg(4, std::ios::cur);
        {
            TemporarySeek ts(stream, pos + off);
            fonts.push_back(readNullTerminatedStr(stream));
        }
    }
    return true;
}

template<class IO>
TemporarySeek<IO>::TemporarySeek(IO &s, long seekPos, std::ios::seekdir seekDir) {
    stream = &s;
    oldSeekPos = stream->tellg();
    stream->seekg(seekPos, seekDir);
}

template<class IO>
TemporarySeek<IO>::~TemporarySeek() {
    stream->seekg(oldSeekPos);
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
    stream.read(reinterpret_cast<char *>(&res), sizeof(T));
    if (reverseEndian) {
        T newRes = T();
        for (int i=0; i<sizeof(T)/2; ++i) {
            int s0 = (i*8);
            int s1 = ((sizeof(T)-i-1)*8);
            newRes |= ((res >> s0) & 0xff) << s1;
            newRes |= ((res >> s1) & 0xff) << s0;
        }
        res = newRes;
    }
    return res;
}
