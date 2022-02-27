#include "brlan.h"

namespace bq::brlan {

void Pat1::read(std::istream &stream, bool revEndian) {
    auto startPos = stream.tellg() - std::streamoff(8);
    animationOrder = readNumber<std::uint16_t>(stream, revEndian);
    auto groupCount = readNumber<std::uint16_t>(stream, revEndian);
    auto animNameOffset = readNumber<std::uint32_t>(stream, revEndian);
    auto groupNamesOffset = readNumber<std::uint32_t>(stream, revEndian);
    startFrame = readNumber<std::int16_t>(stream, revEndian);
    endFrame = readNumber<std::int16_t>(stream, revEndian);
    childBinding = readNumber<std::uint8_t>(stream, revEndian);
    unknownData = readFixedStr(stream, startPos + std::streamoff(animNameOffset) - stream.tellg());
    
    stream.seekg(startPos + std::streamoff(animNameOffset));
    name = readNullTerminatedStr(stream);

    stream.seekg(startPos + std::streamoff(groupNamesOffset));
    for (int i=0; i<groupCount; ++i) {
        groups.push_back(readFixedStr(stream, 0x14));
    }
}

void Pat1::write(std::ostream &stream, bool revEndian) {
    auto startPos = stream.tellp() - std::streamoff(8);
    writeNumber(animationOrder, stream, revEndian);
    writeNumber((std::uint16_t)groups.size(), stream, revEndian);
    auto animNameOffPos = stream.tellp();
    writeNumber((std::uint32_t)0, stream, revEndian);
    auto groupNamesOffPos = stream.tellp();
    writeNumber((std::uint32_t)0, stream, revEndian);
    writeNumber(startFrame, stream, revEndian);
    writeNumber(endFrame, stream, revEndian);
    writeNumber((std::uint8_t)childBinding, stream, revEndian);
    writeFixedStr(unknownData, stream, unknownData.size());
    auto pos2 = stream.tellp();
    {
        TemporarySeekO ts(stream, animNameOffPos);
        writeNumber(std::uint32_t(pos2 - startPos), stream, revEndian);
    }
    writeNullTerminatedStr(name, stream);
    alignFile(stream);
    pos2 = stream.tellp();
    {
        TemporarySeekO ts(stream, groupNamesOffPos);
        writeNumber(std::uint32_t(pos2 - startPos), stream, revEndian);
    }
    for (auto &group: groups) {
        writeFixedStr(group, stream, 0x14);
    }
}

void PaiTag::read(std::istream &stream, bool revEndian, AnimationTarget target) {
    if (target == 2) {
        unknown = readNumber<std::uint32_t>(stream, revEndian);
    }
    auto startPos = stream.tellg();
    tag = readFixedStr(stream, 4);
    auto numEntries = readNumber<std::uint8_t>(stream, revEndian);
    stream.seekg(3, std::ios::cur);
    tagEntries.resize(numEntries);
    for (auto &tagEntry: tagEntries) {
        auto off = readNumber<std::uint32_t>(stream, revEndian);
        {
            TemporarySeekI ts(stream, startPos + std::streamoff(off));
            tagEntry.read(stream, revEndian);
        }
    }
}

void PaiTag::write(std::ostream &stream, bool revEndian, AnimationTarget target) {
    if (target == 2) {
        writeNumber(unknown, stream, revEndian);
    }
    auto startPos = stream.tellp();
    writeFixedStr(tag, stream, 4);
    writeNumber((std::uint8_t)tagEntries.size(), stream, revEndian);
    stream.write("\0\0\0", 3);
    auto offStart = stream.tellp();
    for (int i=0; i<tagEntries.size() * sizeof(std::uint32_t); ++i) {
        stream.put('\0');
    }
    auto pos2 = stream.tellp();
    stream.seekp(offStart);
    for (auto &tagEntry: tagEntries) {
        writeNumber(std::uint32_t(pos2 - startPos), stream, revEndian);
        {
            TemporarySeekO ts(stream, pos2);
            tagEntry.write(stream, revEndian);
            pos2 = stream.tellp();
        }
    }
    stream.seekp(pos2); // seek to end of allocated stuff
}

void PaiEntry::read(std::istream &stream, bool revEndian) {
    auto startPos = stream.tellg();
    name = readFixedStr(stream, 0x14);
    auto numTags = readNumber<std::uint8_t>(stream, revEndian);
    target = (AnimationTarget)readNumber<std::uint8_t>(stream, revEndian);
    stream.seekg(2, std::ios::cur);
    tags.resize(numTags);
    for (auto &tag: tags) {
        auto off = readNumber<std::uint32_t>(stream, revEndian);
        {
            TemporarySeekI ts(stream, startPos + std::streamoff(off));
            tag.read(stream, revEndian, target);
        }
    }
}

void PaiEntry::write(std::ostream &stream, bool revEndian) {
    auto startPos = stream.tellp();
    writeFixedStr(name, stream, 0x14);
    writeNumber((std::uint8_t)tags.size(), stream, revEndian);
    writeNumber((std::uint8_t)target, stream, revEndian);
    stream.put('\0');
    stream.put('\0');
    auto offStart = stream.tellp();
    for (int i=0; i<tags.size() * sizeof(std::uint32_t); ++i) {
        stream.put('\0');
    }
    auto pos2 = stream.tellp();
    stream.seekp(offStart);
    for (auto &tag: tags) {
        writeNumber(std::uint32_t(pos2 - startPos), stream, revEndian);
        {
            TemporarySeekO ts(stream, pos2);
            tag.write(stream, revEndian, target);
            pos2 = stream.tellp();
        }
    }
    stream.seekp(pos2); // seek to end of allocated stuff
}

void Pai1::read(std::istream &stream, bool revEndian) {
    auto startPos = stream.tellg() - std::streamoff(8);
    frameSize = readNumber<std::uint16_t>(stream, revEndian);
    loop = readNumber<std::uint8_t>(stream, revEndian);
    stream.seekg(1, std::ios::cur);
    auto numTextures = readNumber<std::uint16_t>(stream, revEndian);
    auto numEntries = readNumber<std::uint16_t>(stream, revEndian);
    auto entryOffsetTbl = readNumber<std::uint32_t>(stream, revEndian);
    for (int i=0; i<numTextures; ++i) {
        auto off = readNumber<std::uint32_t>(stream, revEndian);
        {
            TemporarySeekI ts(stream, startPos + std::streamoff(off));
            textures.push_back(readNullTerminatedStr(stream));
        }
    }
    stream.seekg(startPos + std::streamoff(entryOffsetTbl));
    entries.resize(numEntries);
    for (auto &entry: entries) {
        auto off = readNumber<std::uint32_t>(stream, revEndian);
        {
            TemporarySeekI ts(stream, startPos + std::streamoff(off));
            entry.read(stream, revEndian);
        }
    }
}

void Pai1::write(std::ostream &stream, bool revEndian) {
    auto startPos = stream.tellp() - std::streamoff(8);
    writeNumber(frameSize, stream, revEndian);
    writeNumber((std::uint8_t)loop, stream, revEndian);
    stream.put('\0');
    writeNumber((std::uint16_t)textures.size(), stream, revEndian);
    writeNumber((std::uint16_t)entries.size(), stream, revEndian);
    // TODO add more stuff
}

}
