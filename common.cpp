#include "common.h"
#include <algorithm>

namespace bq {

Section::~Section() = default;

void WindowContent::read(std::istream &stream, bool revEndian) {
    colorTopLeft = readColor8(stream, revEndian);
    colorTopRight = readColor8(stream, revEndian);
    colorBottomLeft = readColor8(stream, revEndian);
    colorBottomRight = readColor8(stream, revEndian);
    materialIndex = readNumber<std::uint16_t>(stream, revEndian);
    auto uvCount = readNumber<std::uint8_t>(stream, revEndian);
    stream.seekg(1, std::ios::cur);
    texCoords.resize(uvCount);
    for (auto &texCoord: texCoords) {
        texCoord.topLeft.read(stream, revEndian);
        texCoord.topRight.read(stream, revEndian);
        texCoord.bottomLeft.read(stream, revEndian);
        texCoord.bottomRight.read(stream, revEndian);
    }
}

void WindowContent::write(std::ostream &stream, bool revEndian) {
    // TODO implement
}

void WindowFrame::read(std::istream &stream, bool revEndian) {
    materialIndex = readNumber<std::uint16_t>(stream, revEndian);
    texFlip = (WindowFrameTexFlip)readNumber<std::uint8_t>(stream, revEndian);
    stream.seekg(1, std::ios::cur);
}

void WindowFrame::write(std::ostream &stream, bool revEndian) {
    // TODO implement
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
}

template<bool padding>
void Txl1<padding>::write(std::ostream &stream, bool revEndian) {
    // TODO implement
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
}

template<bool padding>
void Fnl1<padding>::write(std::ostream &stream, bool revEndian) {
    // TODO implement
}

void TextureTransform::read(std::istream &stream, bool revEndian) {
    translate.read(stream, revEndian);
    rotate = readNumber<float>(stream, revEndian);
    scale.read(stream, revEndian);
}

void TextureTransform::write(std::ostream &stream, bool revEndian) {
    // TODO implement
}

void BlendMode::read(std::istream &stream, bool revEndian) {
    blendOp = (Op)readNumber<std::uint8_t>(stream, revEndian);
    srcFactor = (BlendFactor)readNumber<std::uint8_t>(stream, revEndian);
    destFactor = (BlendFactor)readNumber<std::uint8_t>(stream, revEndian);
    logicOp = (Op)readNumber<std::uint8_t>(stream, revEndian);
}

void BlendMode::write(std::ostream &stream, bool revEndian) {
    // TODO implement
}

std::string readFixedStr(std::istream &stream, int len) {
    std::string res(len, '\0');
    stream.read(res.data(), len);
    return res;
}

std::string readNullTerminatedStr(std::istream &stream) {
    std::string res;
    std::getline(stream, res, '\0');
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

}
