#include "common.h"

namespace bq {

void Section::read(std::istream &stream, const BaseHeader &header) {
    // nothing to do
}
void Section::write(std::ostream &stream, const BaseHeader &header) {
    // nothing to do
}
Section::~Section() = default;

std::string GroupPane::signature() {
    return GroupPane::MAGIC;
}

bool BaseHeader::revEndian() const { return bom != 0xfeff; }

static std::vector<std::string> readStringList(std::istream &stream, bool revEndian, bool padding) {
    std::vector<std::string> result;
    auto count = readNumber<std::uint16_t>(stream, revEndian);
    stream.seekg(2, std::ios::cur); // padding
    auto pos = stream.tellg();
    for (int i=0; i<count; ++i) {
        auto off = readNumber<std::uint32_t>(stream, revEndian);
        if (padding) stream.seekg(4, std::ios::cur);
        {
            TemporarySeekI ts(stream, pos + std::streamoff(off));
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
    std::streamoff firstOff = list.size() * sizeof(uint32_t) * (padding ? 2 : 1);
    auto pos2 = pos + firstOff;
    for (int i=0; i<firstOff; ++i) {
        stream.put('\0');
    }
    stream.seekp(pos);

    for (auto &item: list) {
        writeNumber(std::uint32_t(pos2 - pos), stream, revEndian);
        if (padding) {
            stream.write("\0\0\0\0", 4);
        }

        {
            TemporarySeekO ts(stream, pos2);
            writeNullTerminatedStr(item, stream);
            pos2 = stream.tellp();
        }
    }
    stream.seekp(pos2); // seek to end of allocated strings
}

template<bool padding>
void Txl1<padding>::read(std::istream &stream, const BaseHeader &header) {
    textures = readStringList(stream, header.revEndian(), padding);
}

template<bool padding>
void Txl1<padding>::write(std::ostream &stream, const BaseHeader &header) {
    writeStringList(textures, stream, header.revEndian(), padding);
}

template<bool padding>
void Fnl1<padding>::read(std::istream &stream, const BaseHeader &header) {
    fonts = readStringList(stream, header.revEndian(), padding);
}

template<bool padding>
void Fnl1<padding>::write(std::ostream &stream, const BaseHeader &header) {
    writeStringList(fonts, stream, header.revEndian(), padding);
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

void KeyFrame::read(std::istream &stream, bool revEndian, CurveType curveType) {
    if (curveType == CurveType::Hermite) {
        frame = readNumber<float>(stream, revEndian);
        value = readNumber<float>(stream, revEndian);
        slope = readNumber<float>(stream, revEndian);
    } else {
        frame = readNumber<float>(stream, revEndian);
        value = readNumber<std::uint16_t>(stream, revEndian);
        stream.seekg(2, std::ios::cur);
    }
}

void KeyFrame::write(std::ostream &stream, bool revEndian, CurveType curveType) {
    if (curveType == CurveType::Hermite) {
        writeNumber(frame, stream, revEndian);
        writeNumber(value, stream, revEndian);
        writeNumber(slope, stream, revEndian);
    } else {
        writeNumber(frame, stream, revEndian);
        writeNumber((std::uint16_t)value, stream, revEndian);
        stream.put('\0');
        stream.put('\0');
    }
}

void PaiTagEntry::read(std::istream &stream, bool revEndian) {
    auto pos = stream.tellg();
    index = readNumber<std::uint8_t>(stream, revEndian);
    target = readNumber<std::uint8_t>(stream, revEndian);
    curveType = (CurveType)readNumber<std::uint8_t>(stream, revEndian);
    stream.seekg(1, std::ios::cur);
    auto keyFrameCount = readNumber<std::uint16_t>(stream, revEndian);
    stream.seekg(2, std::ios::cur);
    auto keyFrameOff = readNumber<std::uint32_t>(stream, revEndian);
    stream.seekg(pos + std::streamoff(keyFrameOff));
    keyFrames.resize(keyFrameCount);
    for (auto &keyFrame: keyFrames) {
        keyFrame.read(stream, revEndian, curveType);
    }
}

void PaiTagEntry::write(std::ostream &stream, bool revEndian) {
    writeNumber(index, stream, revEndian);
    writeNumber(target, stream, revEndian);
    writeNumber((std::uint8_t)curveType, stream, revEndian);
    stream.put('\0');
    writeNumber((std::uint16_t)keyFrames.size(), stream, revEndian);
    stream.put('\0');
    stream.put('\0');
    writeNumber(std::uint32_t(0x0c), stream, revEndian);
    for (auto &keyFrame: keyFrames) {
        keyFrame.write(stream, revEndian, curveType);
    }
}

std::string readFixedStr(std::istream &stream, int len) {
    std::string res(len, '\0');
    stream.read(res.data(), len);
    while (!res.empty() && res.back() == '\0') {
        res.pop_back();
    }
    return res;
}

void writeFixedStr(const std::string &str, std::ostream &stream, int len) {
    int toWriteFromStr = std::min((int)str.size(), len);
    stream.write(str.data(), toWriteFromStr);
    for (int i=0; i<len - toWriteFromStr; ++i) {
        stream.put('\0');
    }
}

std::string readNullTerminatedStr(std::istream &stream) {
    std::string res;
    std::getline(stream, res, '\0');
    return res;
}

void writeNullTerminatedStr(const std::string &str, std::ostream &stream) {
    stream.write(str.data(), str.length() + 1);
}

std::u16string readNullTerminatedStrU16(std::istream &stream, bool revEndian) {
    std::u16string result;
    for (;;) {
        auto c = readNumber<std::uint16_t>(stream, revEndian);
        if (stream.eof() || c == 0) {
            break;
        }
        result.push_back(c);
    }
    return result;
}

void writeNullTerminatedStrU16(const std::u16string &str, std::ostream &stream, bool revEndian) {
    for (auto c: str) {
        writeNumber((std::uint16_t)c, stream, revEndian);
    }
    stream.put('\0');
    stream.put('\0');
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

void writeSection(const std::string &magic, Section &sec, std::ostream &stream, const BaseHeader &header) {
    bool revEndian = header.revEndian();
    auto startPos = stream.tellp();
    writeFixedStr(magic, stream, 4);
    auto sectionSizePos = stream.tellp();
    writeNumber(std::uint32_t(0), stream, revEndian);
    sec.write(stream, header);
    alignFile(stream);
    auto endPos = stream.tellp();
    {
        TemporarySeekO ts(stream, sectionSizePos);
        writeNumber(std::uint32_t(endPos - startPos), stream, revEndian);
    }
}

void alignFile(std::ostream &stream) {
    while (stream.tellp() % 4 != 0) {
        stream.put('\0');
    }
}

}
