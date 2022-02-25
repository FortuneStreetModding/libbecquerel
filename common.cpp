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
    writeColor8(colorTopLeft, stream, revEndian);
    writeColor8(colorTopRight, stream, revEndian);
    writeColor8(colorBottomLeft, stream, revEndian);
    writeColor8(colorBottomRight, stream, revEndian);
    writeNumber(materialIndex, stream, revEndian);
    writeNumber((std::uint8_t)texCoords.size(), stream, revEndian);
    stream.put('\0');
    for (auto &texCoord: texCoords) {
        texCoord.topLeft.write(stream, revEndian);
        texCoord.topRight.write(stream, revEndian);
        texCoord.bottomLeft.write(stream, revEndian);
        texCoord.bottomRight.write(stream, revEndian);
    } 
}

void WindowFrame::read(std::istream &stream, bool revEndian) {
    materialIndex = readNumber<std::uint16_t>(stream, revEndian);
    texFlip = (WindowFrameTexFlip)readNumber<std::uint8_t>(stream, revEndian);
    stream.seekg(1, std::ios::cur);
}

void WindowFrame::write(std::ostream &stream, bool revEndian) {
    writeNumber(materialIndex, stream, revEndian);
    writeNumber((std::uint8_t)texFlip, stream, revEndian);
    stream.put('\0');
}

static std::vector<std::string> readStringList(std::istream &stream, bool revEndian, bool padding) {
    std::vector<std::string> result;
    auto count = readNumber<std::uint16_t>(stream, revEndian);
    stream.seekg(2, std::ios::cur); // padding
    auto pos = stream.tellg();
    for (int i=0; i<count; ++i) {
        auto off = readNumber<std::uint32_t>(stream, revEndian);
        if (padding) stream.seekg(4, std::ios::cur);
        {
            TemporarySeek ts(stream, pos + std::streamoff(off));
            result.push_back(readNullTerminatedStr(stream));
        }
    }
    return result;
}

static void writeStringList(const std::vector<std::string> &list, std::ostream &stream, bool revEndian, bool padding) {
    writeNumber((std::uint16_t)list.size(), stream, revEndian);
    stream.put('\0');
    stream.put('\0');
    auto pos = stream.tellp();
    auto pos2 = pos + std::streamoff(list.size() * sizeof(uint32_t) * (padding ? 2 : 1));
    for (auto &item: list) {
        writeNumber(std::uint32_t(pos2 - pos), stream, revEndian);
        if (padding) {
            stream.write("\0\0\0\0", 4);
        }

        {
            TemporarySeek ts(stream, pos2);
            writeNullTerminatedStr(item, stream);
            pos2 = stream.tellp();
        }
    }
    stream.seekp(pos2); // seek to end of allocated strings
}

template<bool padding>
void Txl1<padding>::read(std::istream &stream, bool revEndian) {
    textures = readStringList(stream, revEndian, padding);
}

template<bool padding>
void Txl1<padding>::write(std::ostream &stream, bool revEndian) {
    writeStringList(textures, stream, revEndian, padding);
}

template<bool padding>
void Fnl1<padding>::read(std::istream &stream, bool revEndian) {
    fonts = readStringList(stream, revEndian, padding);
}

template<bool padding>
void Fnl1<padding>::write(std::ostream &stream, bool revEndian) {
    writeStringList(fonts, stream, revEndian, padding);
}

void TextureTransform::read(std::istream &stream, bool revEndian) {
    translate.read(stream, revEndian);
    rotate = readNumber<float>(stream, revEndian);
    scale.read(stream, revEndian);
}

void TextureTransform::write(std::ostream &stream, bool revEndian) {
    translate.write(stream, revEndian);
    writeNumber(rotate, stream, revEndian);
    scale.write(stream, revEndian);
}

void BlendMode::read(std::istream &stream, bool revEndian) {
    blendOp = (Op)readNumber<std::uint8_t>(stream, revEndian);
    srcFactor = (BlendFactor)readNumber<std::uint8_t>(stream, revEndian);
    destFactor = (BlendFactor)readNumber<std::uint8_t>(stream, revEndian);
    logicOp = (Op)readNumber<std::uint8_t>(stream, revEndian);
}

void BlendMode::write(std::ostream &stream, bool revEndian) {
    writeNumber((std::uint8_t)blendOp, stream, revEndian);
    writeNumber((std::uint8_t)srcFactor, stream, revEndian);
    writeNumber((std::uint8_t)destFactor, stream, revEndian);
    writeNumber((std::uint8_t)logicOp, stream, revEndian);
}

std::string readFixedStr(std::istream &stream, int len) {
    std::string res(len, '\0');
    stream.read(res.data(), len);
    return res;
}

void writeFixedStr(const std::string &str, std::ostream &stream, int len) {
    stream.write(str.data(), len);
}

std::string readNullTerminatedStr(std::istream &stream) {
    std::string res;
    std::getline(stream, res, '\0');
    return res;
}

void writeNullTerminatedStr(const std::string &str, std::ostream &stream) {
    stream.write(str.data(), str.length() + 1);
}

color8 readColor8(std::istream &stream, bool reverseEndian) {
    color8 res;
    for (auto &colval: res) {
        colval = readNumber<std::uint8_t>(stream, reverseEndian);
    }
    return res;
}

void writeColor8(const color8 &color, std::ostream &stream, bool reverseEndian) {
    for (auto colval: color) {
        writeNumber(colval, stream, reverseEndian);
    }
}

color16 readColor16(std::istream &stream, bool reverseEndian) {
    color16 res;
    for (auto &colval: res) {
        colval = readNumber<std::uint16_t>(stream, reverseEndian);
    }
    return res;
}

void writeColor16(const color16 &color, std::ostream &stream, bool reverseEndian) {
    for (auto colval: color) {
        writeNumber(colval, stream, reverseEndian);
    }
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
