#include "brlyt.h"

namespace bq::brlyt {

void Lyt1::read(std::istream &stream, const BaseHeader &header) {
    bool revEndian = header.revEndian();
    drawFromCenter = readNumber<std::uint8_t>(stream, revEndian);
    stream.seekg(3, std::ios::cur); // padding
    width = readNumber<float>(stream, revEndian);
    height = readNumber<float>(stream, revEndian);
}

void Lyt1::write(std::ostream &stream, const BaseHeader &header) {
    bool revEndian = header.revEndian();
    writeNumber(drawFromCenter, stream, revEndian);
    stream.write("\0\0\0", 3);
    writeNumber(width, stream, revEndian);
    writeNumber(height, stream, revEndian);
}

void TexCoordGenEntry::read(std::istream &stream, bool revEndian) {
    type = (TexCoordGenTypes)readNumber<std::uint8_t>(stream, revEndian);
    source = (TexCoordGenSource)readNumber<std::uint8_t>(stream, revEndian);
    matrixSource = (TexCoordGenMatrixSource)readNumber<std::uint8_t>(stream, revEndian);
    unknown = readNumber<std::uint8_t>(stream, revEndian);
}

void TexCoordGenEntry::write(std::ostream &stream, bool revEndian) {
    writeNumber((std::uint8_t)type, stream, revEndian);
    writeNumber((std::uint8_t)source, stream, revEndian);
    writeNumber((std::uint8_t)matrixSource, stream, revEndian);
    writeNumber((std::uint8_t)unknown, stream, revEndian);
}

void ChanCtrl::read(std::istream &stream, bool revEndian) {
    colorMatSource = readNumber<std::uint8_t>(stream, revEndian);
    alphaMatSource = readNumber<std::uint8_t>(stream, revEndian);
    unknown1 = readNumber<std::uint8_t>(stream, revEndian);
    unknown2 = readNumber<std::uint8_t>(stream, revEndian);
}

void ChanCtrl::write(std::ostream &stream, bool revEndian) {
    writeNumber(colorMatSource, stream, revEndian);
    writeNumber(alphaMatSource, stream, revEndian);
    writeNumber(unknown1, stream, revEndian);
    writeNumber(unknown2, stream, revEndian);
}

void SwapMode::read(std::istream &stream, bool revEndian) {
    auto val = readNumber<std::uint8_t>(stream, revEndian);
    r = SwapChannel(val & 0x3);
    g = SwapChannel((val >> 2) & 0x3);
    b = SwapChannel((val >> 4) & 0x3);
    a = SwapChannel((val >> 6) & 0x3);
}

void SwapMode::write(std::ostream &stream, bool revEndian) {
    std::uint8_t val = r + (g << 2) + (b << 4) + (a << 6);
    writeNumber(val, stream, revEndian);
}

void TevSwapModeTable::read(std::istream &stream, bool revEndian) {
    for (auto &swapMode: swapModes) {
        swapMode.read(stream, revEndian);
    }
}

void TevSwapModeTable::write(std::ostream &stream, bool revEndian) {
    for (auto &swapMode: swapModes) {
        swapMode.write(stream, revEndian);
    }
}

void IndirectStage::read(std::istream &stream, bool revEndian) {
    texCoord = readNumber<std::uint8_t>(stream, revEndian);
    texMap = readNumber<std::uint8_t>(stream, revEndian);
    scaleS = readNumber<std::uint8_t>(stream, revEndian);
    scaleT = readNumber<std::uint8_t>(stream, revEndian);
}

void IndirectStage::write(std::ostream &stream, bool revEndian) {
    writeNumber(texCoord, stream, revEndian);
    writeNumber(texMap, stream, revEndian);
    writeNumber(scaleS, stream, revEndian);
    writeNumber(scaleT, stream, revEndian);
}

void TextureRef::read(std::istream &stream, const BaseHeader &header) {
    bool revEndian = header.revEndian();
    auto id = readNumber<std::uint16_t>(stream, revEndian);
    name = static_cast<const Brlyt &>(header).txl1.textures[id];
    wrapModeU = (WrapMode)readNumber<std::uint8_t>(stream, revEndian);
    wrapModeV = (WrapMode)readNumber<std::uint8_t>(stream, revEndian);
    filterModeMin = filterModeMax = FilterMode::Linear;
}

void TextureRef::write(std::ostream &stream, const BaseHeader &header) {
    bool revEndian = header.revEndian();
    auto &textures = static_cast<const Brlyt &>(header).txl1.textures;
    std::uint16_t id = std::find(textures.begin(), textures.end(), name) - textures.begin();
    writeNumber(id, stream, revEndian);
    writeNumber((std::uint8_t)wrapModeU, stream, revEndian);
    writeNumber((std::uint8_t)wrapModeV, stream, revEndian);
}

void TevStage::read(std::istream &stream, bool revEndian) {
    texCoord = readNumber<std::uint8_t>(stream, revEndian);
    color = readNumber<std::uint8_t>(stream, revEndian);
    flag1 = readNumber<std::uint16_t>(stream, revEndian);
    for (auto &flag: flags) {
        flag = readNumber<std::uint8_t>(stream, revEndian);
    }
}

void TevStage::write(std::ostream &stream, bool revEndian) {
    writeNumber(texCoord, stream, revEndian);
    writeNumber(color, stream, revEndian);
    writeNumber(flag1, stream, revEndian);
    for (auto flag: flags) {
        writeNumber(flag, stream, revEndian);
    }
}

void AlphaCompare::read(std::istream &stream, bool revEndian) {
    auto c = readNumber<std::uint8_t>(stream, revEndian);
    comp0 = AlphaFunction(c & 0x7);
    comp1 = AlphaFunction((c >> 4) & 0x7);
    op = (AlphaOp)readNumber<std::uint8_t>(stream, revEndian);
    ref0 = readNumber<std::uint8_t>(stream, revEndian);
    ref1 = readNumber<std::uint8_t>(stream, revEndian);
}

void AlphaCompare::write(std::ostream &stream, bool revEndian) {
    std::uint8_t c = std::uint8_t(comp0) + (std::uint8_t(comp1) << 4);
    writeNumber(c, stream, revEndian);
    writeNumber((std::uint8_t)op, stream, revEndian);
    writeNumber(ref0, stream, revEndian);
    writeNumber(ref1, stream, revEndian);
}

void Material::read(std::istream &stream, const BaseHeader &header) {
    bool revEndian = header.revEndian();

    name = readFixedStr(stream, 0x14);
    blackColor = toColor8(readColor16(stream, revEndian));
    whiteColor = toColor8(readColor16(stream, revEndian));
    colorRegister3 = toColor8(readColor16(stream, revEndian));
    for (auto &tevColor: tevColors) {
        tevColor = readColor8(stream, revEndian);
    }
    flags = readNumber<std::uint32_t>(stream, revEndian);
    textureMaps.resize(texCount);
    for (auto &textureMap: textureMaps) {
        textureMap.read(stream, header);
    }
    texTransforms.resize(mtxCount);
    for (auto &texTransform: texTransforms) {
        texTransform.read(stream, revEndian);
    }
    texCoordGens.resize(texCoordGenCount);
    for (auto &texCoordGen: texCoordGens) {
        texCoordGen.read(stream, revEndian);
    }
    if (hasChannelControl) {
        chanCtrl.read(stream, revEndian);
    }
    if (hasMaterialColor) {
        matColor = readColor8(stream, revEndian);
    }
    if (hasTevSwapTable) {
        swapModeTable.read(stream, revEndian);
    }
    indirectTransforms.resize(indSrtCount);
    for (auto &indTransform: indirectTransforms) {
        indTransform.read(stream, revEndian);
    }
    indirectStages.resize(indTexOrderCount);
    for (auto &indStage: indirectStages) {
        indStage.read(stream, revEndian);
    }
    tevStages.resize(tevStagesCount);
    for (auto &tevStage: tevStages) {
        tevStage.read(stream, revEndian);
    }
    if (hasAlphaCompare) {
        alphaCompare.read(stream, revEndian);
    }
    if (hasBlendMode) {
        blendMode.read(stream, revEndian);
    }
}

void Material::write(std::ostream &stream, const BaseHeader &header) {
    bool revEndian = header.revEndian();

    writeFixedStr(name, stream, 0x14);
    writeColor16(toColor16(blackColor), stream, revEndian);
    writeColor16(toColor16(whiteColor), stream, revEndian);
    writeColor16(toColor16(colorRegister3), stream, revEndian);
    for (auto &tevColor: tevColors) {
        writeColor8(tevColor, stream, revEndian);
    }
    // update flag
    texCount = textureMaps.size();
    mtxCount = texTransforms.size();
    texCoordGenCount = texCoordGens.size();
    indSrtCount = indirectTransforms.size();
    indTexOrderCount = indirectStages.size();
    tevStagesCount = tevStages.size();
    // write the flag integer
    writeNumber(flags, stream, revEndian);

    for (auto &textureMap: textureMaps) {
        textureMap.write(stream, header);
    }
    for (auto &texTransform: texTransforms) {
        texTransform.write(stream, revEndian);
    }
    for (auto &texCoordGen: texCoordGens) {
        texCoordGen.write(stream, revEndian);
    }
    if (hasChannelControl) {
        chanCtrl.write(stream, revEndian);
    }
    //std::cout << std::hex << stream.tellp() << std::endl;
    if (hasMaterialColor) {
        writeColor8(matColor, stream, revEndian);
    }
    //std::cout << std::hex << stream.tellp() << std::endl;
    if (hasTevSwapTable) {
        swapModeTable.write(stream, revEndian);
    }
    for (auto &indTransform: indirectTransforms) {
        indTransform.write(stream, revEndian);
    }
    for (auto &indStage: indirectStages) {
        indStage.write(stream, revEndian);
    }
    for (auto &tevStage: tevStages) {
        tevStage.write(stream, revEndian);
    }
    if (hasAlphaCompare) {
        alphaCompare.write(stream, revEndian);
    }
    if (hasBlendMode) {
        blendMode.write(stream, revEndian);
    }
}

void Mat1::read(std::istream &stream, const BaseHeader &header) {
    bool revEndian = header.revEndian();
    auto pos = stream.tellg();
    auto numMats = readNumber<std::uint16_t>(stream, revEndian);
    stream.seekg(2, std::ios::cur); // padding
    for (int i=0; i<numMats; ++i) {
        auto off = readNumber<std::uint32_t>(stream, revEndian);
        {
            TemporarySeekI ts(stream, pos + std::streamoff(off - 8));
            materials.push_back(std::make_shared<Material>());
            materials.back()->read(stream, header);
        }
    }
}

void Mat1::write(std::ostream &stream, const BaseHeader &header) {
    bool revEndian = header.revEndian();
    auto pos = stream.tellp() - std::streamoff(8);
    writeNumber((std::uint16_t)materials.size(), stream, revEndian);
    stream.put('\0');
    stream.put('\0');
    std::streamoff firstOff = materials.size() * sizeof(uint32_t);
    auto offsetStartPos = stream.tellp();
    auto pos2 = offsetStartPos + firstOff;
    for (int i=0; i<firstOff; ++i) {
        stream.put('\0');
    }
    stream.seekp(offsetStartPos);
    for (auto &mat: materials) {
        writeNumber(std::uint32_t(pos2 - pos), stream, revEndian);

        {
            TemporarySeekO ts(stream, pos2);
            mat->write(stream, header);
            alignFile(stream);
            pos2 = stream.tellp();
        }
    }
    stream.seekp(pos2); // seek to end of allocated materials
}

#if 0
void UsdEntry::read(std::istream &stream, bool revEndian) {
    auto pos = stream.tellg();
    auto nameOffset = readNumber<std::uint32_t>(stream, revEndian);
    auto dataOffset = readNumber<std::uint32_t>(stream, revEndian);
    auto numEntries = readNumber<std::uint16_t>(stream, revEndian);
    UsdType entryType = (UsdType)readNumber<std::uint8_t>(stream, revEndian);
    stream.seekg(1, std::ios::cur); // padding
    {
        TemporarySeekI ts(stream, pos + std::streamoff(nameOffset));
        name = readNullTerminatedStr(stream);
    }
    {
        TemporarySeekI ts(stream, pos + std::streamoff(dataOffset));
        switch (entryType) {
            case UsdType::String:
            data = readFixedStr(stream, numEntries);
            break;

            case UsdType::Int32:
            {
                auto &intVec = data.emplace<std::vector<std::uint32_t>>();
                for (int i=0; i<numEntries; ++i) {
                    intVec.push_back(readNumber<std::uint32_t>(stream, revEndian));
                }
            }
            break;

            case UsdType::Float:
            {
                auto &floatVec = data.emplace<std::vector<float>>();
                for (int i=0; i<numEntries; ++i) {
                    floatVec.push_back(readNumber<float>(stream, revEndian));
                }
            }
            break;
        }
    }
}

void UsdEntry::write(std::ostream &stream, bool revEndian) {
    auto pos = stream.tellp();
    auto nameOffsetPos = stream.tellp();
    writeNumber(std::uint32_t(0), stream, revEndian);
    auto dataOffsetPos = stream.tellp();
    writeNumber(std::uint32_t(0), stream, revEndian);
    std::uint16_t numEntries = std::visit([](const auto &entries) { return entries.size(); }, data);
    writeNumber(numEntries, stream, revEndian);
    UsdType entryType = (UsdType)data.index();
    writeNumber((std::uint8_t)entryType, stream, revEndian);
    stream.put('\0'); // padding
    auto pos2 = stream.tellp();
    {
        TemporarySeekO ts(stream, nameOffsetPos);
        writeNumber(std::uint32_t(pos2 - pos), stream, revEndian);
    }
    writeNullTerminatedStr(name, stream);
    pos2 = stream.tellp();
    {
        TemporarySeekO ts(stream, dataOffsetPos);
        writeNumber(std::uint32_t(pos2 - pos), stream, revEndian);
    }
    switch (entryType) {
        case UsdType::String:
        {
            auto &str = std::get<std::string>(data);
            writeFixedStr(str, stream, str.size());
        }
        break;

        case UsdType::Int32:
        for (auto val: std::get<std::vector<std::uint32_t>>(data)) {
            writeNumber(val, stream, revEndian);
        }
        break;

        case UsdType::Float:
        for (auto val: std::get<std::vector<float>>(data)) {
            writeNumber(val, stream, revEndian);
        }
        break;
    };
}
#endif

void Usd1::read(std::istream &stream, const BaseHeader &header) {
    data.resize(sectionSize - 8);
    stream.read(data.data(), data.size());
}

void Usd1::write(std::ostream &stream, const BaseHeader &header) {
    sectionSize = data.size() + 8;
    stream.write(data.data(), data.size());
}

void Pan1::read(std::istream &stream, const BaseHeader &header) {
    bool revEndian = header.revEndian();
    flags = readNumber<std::uint8_t>(stream, revEndian);
    auto origin = readNumber<std::uint8_t>(stream, revEndian);
    alpha = readNumber<std::uint8_t>(stream, revEndian);
    paneMagFlags = readNumber<std::uint8_t>(stream, revEndian);
    name = readFixedStr(stream, 0x10);
    userDataInfo = readFixedStr(stream, 0x8);
    translate.read(stream, revEndian);
    rotate.read(stream, revEndian);
    scale.read(stream, revEndian);
    width = readNumber<float>(stream, revEndian);
    height = readNumber<float>(stream, revEndian);
    originX = ORIGIN_X_MAP[origin % 3];
    originY = ORIGIN_Y_MAP[origin / 3];
}

void Pan1::write(std::ostream &stream, const BaseHeader &header) {
    bool revEndian = header.revEndian();

    uint8_t originXIdx = std::find(ORIGIN_X_MAP.begin(), ORIGIN_X_MAP.end(), originX) - ORIGIN_X_MAP.begin();
    uint8_t originYIdx = std::find(ORIGIN_Y_MAP.begin(), ORIGIN_Y_MAP.end(), originY) - ORIGIN_Y_MAP.begin();
    uint8_t origin = originXIdx + 3*originYIdx;

    writeNumber(flags, stream, revEndian);
    writeNumber(origin, stream, revEndian);
    writeNumber(alpha, stream, revEndian);
    writeNumber(paneMagFlags, stream, revEndian);
    writeFixedStr(name, stream, 0x10);
    writeFixedStr(userDataInfo, stream, 0x8);
    translate.write(stream, revEndian);
    rotate.write(stream, revEndian);
    scale.write(stream, revEndian);
    writeNumber(width, stream, revEndian);
    writeNumber(height, stream, revEndian);
}

std::string Pan1::signature() {
    return Pan1::MAGIC;
}

void Pic1::read(std::istream &stream, const BaseHeader &header) {
    bool revEndian = header.revEndian();

    Pan1::read(stream, header);
    colorTopLeft = readColor8(stream, revEndian);
    colorTopRight = readColor8(stream, revEndian);
    colorBottomLeft = readColor8(stream, revEndian);
    colorBottomRight = readColor8(stream, revEndian);
    auto materialIndex = readNumber<std::uint16_t>(stream, revEndian);
    material = static_cast<const Brlyt &>(header).mat1.materials[materialIndex];
    auto numUVs = readNumber<std::uint8_t>(stream, revEndian);
    stream.seekg(1, std::ios::cur);
    texCoords.resize(numUVs);
    for (auto &texCoord: texCoords) {
        texCoord.topLeft.read(stream, revEndian);
        texCoord.topRight.read(stream, revEndian);
        texCoord.bottomLeft.read(stream, revEndian);
        texCoord.bottomRight.read(stream, revEndian);
    }
}

void Pic1::write(std::ostream &stream, const BaseHeader &header) {
    bool revEndian = header.revEndian();

    Pan1::write(stream, header);
    writeColor8(colorTopLeft, stream, revEndian);
    writeColor8(colorTopRight, stream, revEndian);
    writeColor8(colorBottomLeft, stream, revEndian);
    writeColor8(colorBottomRight, stream, revEndian);
    auto &materials = static_cast<const Brlyt &>(header).mat1.materials;
    std::uint16_t materialIndex = std::find(materials.begin(), materials.end(), material) - materials.begin();
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

std::string Pic1::signature() {
    return Pic1::MAGIC;
}

void Txt1::read(std::istream &stream, const BaseHeader &header) {
    bool revEndian = header.revEndian();

    Pan1::read(stream, header);
    textLen = readNumber<std::uint16_t>(stream, revEndian);
    maxTextLen = readNumber<std::uint16_t>(stream, revEndian);
    auto materialIndex = readNumber<std::uint16_t>(stream, revEndian);
    material = static_cast<const Brlyt &>(header).mat1.materials[materialIndex];
    auto fontIndex = readNumber<std::uint16_t>(stream, revEndian);
    font = static_cast<const Brlyt &>(header).fnl1.fonts[fontIndex];
    textAlign = readNumber<std::uint8_t>(stream, revEndian);
    lineAlign = (LineAlign)readNumber<std::uint8_t>(stream, revEndian);
    flagsTxt1 = readNumber<std::uint8_t>(stream, revEndian);
    stream.seekg(1, std::ios::cur);
    auto textOffset = readNumber<std::uint32_t>(stream, revEndian);
    fontTopColor = readColor8(stream, revEndian);
    fontBottomColor = readColor8(stream, revEndian);
    fontSize.read(stream, revEndian);
    charSpace = readNumber<float>(stream, revEndian);
    lineSpace = readNumber<float>(stream, revEndian);
    text = readNullTerminatedStrU16(stream, revEndian);
}

void Txt1::write(std::ostream &stream, const BaseHeader &header) {
    bool revEndian = header.revEndian();

    Pan1::write(stream, header);
    writeNumber(textLen, stream, revEndian);
    writeNumber(maxTextLen, stream, revEndian);
    auto &materials = static_cast<const Brlyt &>(header).mat1.materials;
    std::uint16_t materialIndex = std::find(materials.begin(), materials.end(), material) - materials.begin();
    writeNumber(materialIndex, stream, revEndian);
    auto &fonts = static_cast<const Brlyt &>(header).fnl1.fonts;
    std::uint16_t fontIndex = std::find(fonts.begin(), fonts.end(), font) - fonts.begin();
    writeNumber(fontIndex, stream, revEndian);
    writeNumber(textAlign, stream, revEndian);
    writeNumber((std::uint8_t)lineAlign, stream, revEndian);
    writeNumber(flagsTxt1, stream, revEndian);
    stream.put('\0');
    writeNumber(std::uint32_t(0x74), stream, revEndian);
    writeColor8(fontTopColor, stream, revEndian);
    writeColor8(fontBottomColor, stream, revEndian);
    fontSize.write(stream, revEndian);
    writeNumber(charSpace, stream, revEndian);
    writeNumber(lineSpace, stream, revEndian);
    writeNullTerminatedStrU16(text, stream, revEndian);
}

std::string Txt1::signature() {
    return Txt1::MAGIC;
}

void Bnd1::read(std::istream &stream, const BaseHeader &header) {
    Pan1::read(stream, header);
}

void Bnd1::write(std::ostream &stream, const BaseHeader &header) {
    Pan1::write(stream, header);
}

std::string Bnd1::signature() {
    return Bnd1::MAGIC;
}

void WindowContent::read(std::istream &stream, const BaseHeader &header) {
    bool revEndian = header.revEndian();
    colorTopLeft = readColor8(stream, revEndian);
    colorTopRight = readColor8(stream, revEndian);
    colorBottomLeft = readColor8(stream, revEndian);
    colorBottomRight = readColor8(stream, revEndian);
    auto materialIndex = readNumber<std::uint16_t>(stream, revEndian);
    material = static_cast<const Brlyt &>(header).mat1.materials[materialIndex];
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

void WindowContent::write(std::ostream &stream, const BaseHeader &header) {
    bool revEndian = header.revEndian();
    writeColor8(colorTopLeft, stream, revEndian);
    writeColor8(colorTopRight, stream, revEndian);
    writeColor8(colorBottomLeft, stream, revEndian);
    writeColor8(colorBottomRight, stream, revEndian);
    auto &materials = static_cast<const Brlyt &>(header).mat1.materials;
    std::uint16_t materialIndex = std::find(materials.begin(), materials.end(), material) - materials.begin();
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

void WindowFrame::read(std::istream &stream, const BaseHeader &header) {
    bool revEndian = header.revEndian();
    auto materialIndex = readNumber<std::uint16_t>(stream, revEndian);
    material = static_cast<const Brlyt &>(header).mat1.materials[materialIndex];
    texFlip = (WindowFrameTexFlip)readNumber<std::uint8_t>(stream, revEndian);
    stream.seekg(1, std::ios::cur);
}

void WindowFrame::write(std::ostream &stream, const BaseHeader &header) {
    bool revEndian = header.revEndian();
    auto &materials = static_cast<const Brlyt &>(header).mat1.materials;
    std::uint16_t materialIndex = std::find(materials.begin(), materials.end(), material) - materials.begin();
    writeNumber(materialIndex, stream, revEndian);
    writeNumber((std::uint8_t)texFlip, stream, revEndian);
    stream.put('\0');
}

void Wnd1::read(std::istream &stream, const BaseHeader &header) {
    bool revEndian = header.revEndian();

    Pan1::read(stream, header);
    auto pos = stream.tellg() - std::streamoff(0x4c);
    stretchLeft = readNumber<std::uint16_t>(stream, revEndian);
    stretchRight = readNumber<std::uint16_t>(stream, revEndian);
    stretchTop = readNumber<std::uint16_t>(stream, revEndian);
    stretchBottom = readNumber<std::uint16_t>(stream, revEndian);
    frameElementLeft = readNumber<std::uint16_t>(stream, revEndian);
    frameElementRight = readNumber<std::uint16_t>(stream, revEndian);
    frameElementTop = readNumber<std::uint16_t>(stream, revEndian);
    frameElementBottom = readNumber<std::uint16_t>(stream, revEndian);
    auto frameCount = readNumber<std::uint8_t>(stream, revEndian);
    flagsWnd1 = readNumber<std::uint8_t>(stream, revEndian);
    stream.seekg(2, std::ios::cur);
    auto contentOffset = readNumber<std::uint32_t>(stream, revEndian);
    auto frameOffsetTbl = readNumber<std::uint32_t>(stream, revEndian);
    stream.seekg(pos + std::streamoff(contentOffset));
    content.read(stream, header);
    stream.seekg(pos + std::streamoff(frameOffsetTbl));
    frames.resize(frameCount);
    for (auto &frame: frames) {
        auto off = readNumber<std::uint32_t>(stream, revEndian);
        {
            TemporarySeekI ts(stream, pos + std::streamoff(off));
            frame.read(stream, header);
        }
    }
}

void Wnd1::write(std::ostream &stream, const BaseHeader &header) {
    bool revEndian = header.revEndian();

    Pan1::write(stream, header);
    auto pos = stream.tellp() - std::streamoff(0x4c);
    writeNumber(stretchLeft, stream, revEndian);
    writeNumber(stretchRight, stream, revEndian);
    writeNumber(stretchTop, stream, revEndian);
    writeNumber(stretchBottom, stream, revEndian);
    writeNumber(frameElementLeft, stream, revEndian);
    writeNumber(frameElementRight, stream, revEndian);
    writeNumber(frameElementTop, stream, revEndian);
    writeNumber(frameElementBottom, stream, revEndian);
    writeNumber((std::uint8_t)frames.size(), stream, revEndian);
    writeNumber(flagsWnd1, stream, revEndian);
    stream.put('\0');
    stream.put('\0');
    
    auto offsetStartPos = stream.tellp();
    stream.write("\0\0\0\0\0\0\0\0", 8);
    auto pos2 = stream.tellp();
    stream.seekp(offsetStartPos);
    writeNumber(std::uint32_t(pos2 - pos), stream, revEndian);
    {
        TemporarySeekO ts(stream, pos2);
        content.write(stream, header);
        pos2 = stream.tellp();
    }
    writeNumber(std::uint32_t(pos2 - pos), stream, revEndian);
    stream.seekp(pos2);
    auto frameOffStartPos = stream.tellp();
    for (int i=0; i<frames.size(); ++i) {
        writeNumber(std::uint32_t(0), stream, revEndian);
    }
    auto pos3 = stream.tellp();
    stream.seekp(frameOffStartPos);
    for (auto &frame: frames) {
        writeNumber(std::uint32_t(pos3 - pos), stream, revEndian);
        {
            TemporarySeekO ts1(stream, pos3);
            frame.write(stream, header);
            pos3 = stream.tellp();
        }
    }
    stream.seekp(pos3); // seek to end of allocated stuff
}

std::string Wnd1::signature() {
    return Wnd1::MAGIC;
}

void Grp1::read(std::istream &stream, const BaseHeader &header) {
    bool revEndian = header.revEndian();
    name = readFixedStr(stream, 0x10);
    auto numNodes = readNumber<std::uint16_t>(stream, revEndian);
    stream.seekg(2, std::ios::cur);
    for (int i=0; i<numNodes; ++i) {
        panes.push_back(readFixedStr(stream, 0x10));
    }
}

void Grp1::write(std::ostream &stream, const BaseHeader &header) {
    bool revEndian = header.revEndian();
    writeFixedStr(name, stream, 0x10);
    writeNumber((std::uint16_t)panes.size(), stream, revEndian);
    stream.put('\0');
    stream.put('\0');
    for (auto &node: panes) {
        writeFixedStr(node, stream, 0x10);
    }
}

template<class T>
static void setPane(std::shared_ptr<T> pane, std::shared_ptr<T> parentPane) {
    if (parentPane) {
        parentPane->children.push_back(pane);
        pane->parent = parentPane;
    }
}

void Brlyt::read(std::istream &stream) {
    auto magic = readFixedStr(stream, 4);
    bool reverseEndian;
    if (magic != MAGIC) {
        // TODO throw exception here
    }
    bom = readNumber<std::uint16_t>(stream, false);
    reverseEndian = revEndian();
    version = readNumber<std::uint16_t>(stream, reverseEndian);
    auto fileSize = readNumber<std::uint32_t>(stream, reverseEndian);
    headerSize = readNumber<std::uint16_t>(stream, reverseEndian);
    auto sectionCount = readNumber<std::uint16_t>(stream, reverseEndian);

    stream.seekg(headerSize);

    std::shared_ptr<BasePane> curPane, parentPane;
    std::shared_ptr<GroupPane> curGroupPane, parentGroupPane;

    for (int i=0; i<sectionCount; ++i) {
        auto pos = stream.tellg();

        auto sectionHeader = readFixedStr(stream, 4);
        auto sectionSize = readNumber<std::uint32_t>(stream, reverseEndian);

        bool addPane = false;
        
        if (sectionHeader == Lyt1::MAGIC) {
            lyt1.read(stream, *this);
        } else if (sectionHeader == Txl1<true>::MAGIC) {
            txl1.read(stream, *this);
        } else if (sectionHeader == Fnl1<true>::MAGIC) {
            fnl1.read(stream, *this);
        } else if (sectionHeader == Mat1::MAGIC) {
            mat1.read(stream, *this);
        } else if (sectionHeader == Pan1::MAGIC) {
            curPane = std::make_shared<Pan1>();
            addPane = true;
        } else if (sectionHeader == Pic1::MAGIC) {
            curPane = std::make_shared<Pic1>();
            addPane = true;
        } else if (sectionHeader == Txt1::MAGIC) {
            curPane = std::make_shared<Txt1>();
            addPane = true;
        } else if (sectionHeader == Bnd1::MAGIC) {
            curPane = std::make_shared<Bnd1>();
            addPane = true;
        } else if (sectionHeader == Wnd1::MAGIC) {
            curPane = std::make_shared<Wnd1>();
            addPane = true;
        } else if (sectionHeader == "pas1") {
            if (curPane) {
                parentPane = curPane;
            }
        } else if (sectionHeader == "pae1") {
            curPane = parentPane;
            parentPane = curPane->parent.lock();
        } else if (sectionHeader == Grp1::MAGIC) {
            curGroupPane = std::make_shared<Grp1>();
            curGroupPane->read(stream, *this);
            setPane(curGroupPane, parentGroupPane);
        } else if (sectionHeader == "grs1") {
            if (curGroupPane) {
                parentGroupPane = curGroupPane;
            }
        } else if (sectionHeader == "gre1") {
            curGroupPane = parentGroupPane;
            parentGroupPane = curGroupPane->parent.lock();
        } else if (sectionHeader == "usd1") {
            auto associatedPane = std::dynamic_pointer_cast<Pan1>(curPane);
            if (associatedPane) {
                auto &usd1 = associatedPane->userData.emplace();
                usd1.sectionSize = sectionSize;
                usd1.read(stream, *this);
            }
        }

        if (addPane) {
            curPane->read(stream, *this);
            setPane(curPane, parentPane);
        }

        if (!rootPane && sectionHeader == Pan1::MAGIC) {
            rootPane = curPane;
        }

        if (!rootGroup && sectionHeader == Grp1::MAGIC) {
            rootGroup = curGroupPane;
        }

        stream.seekg(pos + std::streamoff(sectionSize));
    }
}

template<class Pane, class SecCount>
static void writePanes(Pane &pane, std::ostream &stream, const BaseHeader &header, const std::string &startTag, const std::string &endTag, SecCount &secCount) {
    writeSection(pane.signature(), pane, stream, header);
    ++secCount;
    auto pan1 = dynamic_cast<Pan1 *>(&pane);
    if (pan1) {
        if (pan1->userData.has_value()) {
            writeSection(Usd1::MAGIC, pan1->userData.value(), stream, header);
            ++secCount;
        }
    }

    if (!pane.children.empty()) {
        secCount += 2;
        Section nullSec;
        writeSection(startTag, nullSec, stream, header);
        for (auto &child: pane.children) {
            writePanes(*child, stream, header, startTag, endTag, secCount);
        }
        writeSection(endTag, nullSec, stream, header);
    }
}

void Brlyt::write(std::ostream &stream) {
    writeFixedStr(MAGIC, stream, 4);
    bool reverseEndian = revEndian();
    writeNumber(bom, stream, false);
    writeNumber((std::uint16_t)version, stream, reverseEndian);
    auto fileSizePos = stream.tellp();
    writeNumber(std::uint32_t(0), stream, reverseEndian);
    // header size
    writeNumber(std::uint16_t(stream.tellp() + std::streamoff(4)), stream, reverseEndian);
    auto sectionCountPos = stream.tellp();
    stream.put('\0');
    stream.put('\0');

    uint16_t sectionCount = 1;

    writeSection(Lyt1::MAGIC, lyt1, stream, *this);
    if (!txl1.textures.empty()) {
        writeSection(Txl1<true>::MAGIC, txl1, stream, *this);
        ++sectionCount;
    }
    if (!fnl1.fonts.empty()) {
        writeSection(Fnl1<true>::MAGIC, fnl1, stream, *this);
        ++sectionCount;
    }
    if (!mat1.materials.empty()) {
        writeSection(Mat1::MAGIC, mat1, stream, *this);
        ++sectionCount;
    }

    if (rootPane) {
        writePanes(*rootPane, stream, *this, "pas1", "pae1", sectionCount);
    }
    if (rootGroup) {
        writePanes(*rootGroup, stream, *this, "grs1", "gre1", sectionCount);
    }

    {
        TemporarySeekO ts(stream, sectionCountPos);
        writeNumber(sectionCount, stream, reverseEndian);
    }

    alignFile(stream);

    auto fileEnd = stream.tellp();

    {
        TemporarySeekO ts(stream, fileSizePos);
        writeNumber((std::uint32_t)fileEnd, stream, reverseEndian);
    }
}

}