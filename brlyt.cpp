#include "brlyt.h"

namespace bq::brlyt {

void Lyt1::read(std::istream &stream, bool revEndian) {
    drawFromCenter = readNumber<std::uint8_t>(stream, revEndian);
    stream.seekg(3, std::ios::cur); // padding
    width = readNumber<float>(stream, revEndian);
    height = readNumber<float>(stream, revEndian);
}

void TextureRef::read(std::istream &stream, bool revEndian) {
    id = readNumber<std::uint16_t>(stream, revEndian);
    wrapModeU = (WrapMode)readNumber<std::uint8_t>(stream, revEndian);
    wrapModeV = (WrapMode)readNumber<std::uint8_t>(stream, revEndian);
    filterModeMin = filterModeMax = FilterMode::Linear;
}

void TevStage::read(std::istream &stream, bool revEndian) {
    texCoord = readNumber<std::uint8_t>(stream, revEndian);
    color = readNumber<std::uint8_t>(stream, revEndian);
    flag1 = readNumber<std::uint16_t>(stream, revEndian);
    for (auto &flag: flags) {
        flag = readNumber<std::uint8_t>(stream, revEndian);
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

void Material::read(std::istream &stream, bool revEndian) {
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
        textureMap.read(stream, revEndian);
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

void Mat1::read(std::istream &stream, bool revEndian) {
    auto pos = stream.tellg();
    auto numMats = readNumber<std::uint16_t>(stream, revEndian);
    stream.seekg(2, std::ios::cur); // padding
    for (int i=0; i<numMats; ++i) {
        auto off = readNumber<std::uint32_t>(stream, revEndian);
        {
            TemporarySeek ts(stream, pos + std::streamoff(off - 8));
            materials.emplace_back();
            materials.back().read(stream, revEndian);
        }
    }
}

void Pan1::read(std::istream &stream, bool revEndian) {
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

void Pic1::read(std::istream &stream, bool revEndian) {
    Pan1::read(stream, revEndian);
    colorTopLeft = readColor8(stream, revEndian);
    colorTopRight = readColor8(stream, revEndian);
    colorBottomLeft = readColor8(stream, revEndian);
    colorBottomRight = readColor8(stream, revEndian);
    materialIndex = readNumber<std::uint16_t>(stream, revEndian);
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

void Txt1::read(std::istream &stream, bool revEndian) {
    Pan1::read(stream, revEndian);
    textLen = readNumber<std::uint16_t>(stream, revEndian);
    maxTextLen = readNumber<std::uint16_t>(stream, revEndian);
    materialIndex = readNumber<std::uint16_t>(stream, revEndian);
    fontIndex = readNumber<std::uint16_t>(stream, revEndian);
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
    text = readNullTerminatedStr(stream);
}

void Bnd1::read(std::istream &stream, bool revEndian) {
    Pan1::read(stream, revEndian);
}

void Wnd1::read(std::istream &stream, bool revEndian) {
    Pan1::read(stream, revEndian);
    auto pos = stream.tellg() - std::streamoff(0x4c);
    stretchLeft = readNumber<std::uint16_t>(stream, revEndian);
    stretchRight = readNumber<std::uint16_t>(stream, revEndian);
    stretchTop = readNumber<std::uint16_t>(stream, revEndian);
    stretchBottom = readNumber<std::uint16_t>(stream, revEndian);
    frameElementLeft = readNumber<std::uint16_t>(stream, revEndian);
    frameElementRight = readNumber<std::uint16_t>(stream, revEndian);
    frameElementTop = readNumber<std::uint16_t>(stream, revEndian);
    frameElementBottom = readNumber<std::uint16_t>(stream, revEndian);
    frameCount = readNumber<std::uint8_t>(stream, revEndian);
    flagsWnd1 = readNumber<std::uint8_t>(stream, revEndian);
    stream.seekg(2, std::ios::cur);
    auto contentOffset = readNumber<std::uint32_t>(stream, revEndian);
    auto frameOffsetTbl = readNumber<std::uint32_t>(stream, revEndian);
    stream.seekg(pos + std::streamoff(contentOffset));
    content.read(stream, revEndian);
    stream.seekg(pos + std::streamoff(frameOffsetTbl));
    frames.resize(frameCount);
    for (auto &frame: frames) {
        auto off = readNumber<std::uint32_t>(stream, revEndian);
        {
            TemporarySeek ts(stream, pos + std::streamoff(off));
            frame.read(stream, revEndian);
        }
    }
}

void Brlyt::read(std::istream &stream) {
    return header.read(stream);
}

template<class T>
static void setPane(std::shared_ptr<T> pane, std::shared_ptr<T> parentPane) {
    if (parentPane) {
        parentPane->children.push_back(pane);
        pane->parent = parentPane;
    }
}

void BrlytHeader::read(std::istream &stream) {
    auto magic = readFixedStr(stream, 4);
    bool reverseEndian;
    if (magic != MAGIC) {
        // TODO throw exception here
    }
    bom = readNumber<std::uint16_t>(stream, false);
    reverseEndian = (bom != 0xfeff);
    version = readNumber<std::uint16_t>(stream, reverseEndian);
    auto fileSize = readNumber<std::uint32_t>(stream, reverseEndian);
    headerSize = readNumber<std::uint16_t>(stream, reverseEndian);
    auto sectionCount = readNumber<std::uint16_t>(stream, reverseEndian);

    stream.seekg(headerSize);

    for (int i=0; i<sectionCount; ++i) {
        auto pos = stream.tellg();

        auto sectionHeader = readFixedStr(stream, 4);
        auto sectionSize = readNumber<std::uint32_t>(stream, reverseEndian);
        bool addPane = false;
        std::shared_ptr<BasePane> curPane, parentPane;
        std::shared_ptr<GroupPane> curGroupPane, parentGroupPane;
        if (sectionHeader == Lyt1::MAGIC) {
            lyt1.read(stream, reverseEndian);
        } else if (sectionHeader == Txl1<true>::MAGIC) {
            txl1.read(stream, reverseEndian);
        } else if (sectionHeader == Fnl1<true>::MAGIC) {
            fnl1.read(stream, reverseEndian);
        } else if (sectionHeader == Mat1::MAGIC) {
            mat1.read(stream, reverseEndian);
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
            parentPane = std::shared_ptr<BasePane>(curPane->parent);
        } else if (sectionHeader == Grp1::MAGIC) {
            curGroupPane = std::make_shared<Grp1>();
            curGroupPane->read(stream, reverseEndian);
            setPane(curGroupPane, parentGroupPane);
        } else if (sectionHeader == "grs1") {
            if (curGroupPane) {
                parentGroupPane = curGroupPane;
            }
        } else if (sectionHeader == "gre1") {
            curGroupPane = parentGroupPane;
            parentGroupPane = std::shared_ptr<GroupPane>(curGroupPane->parent);
        } else if (sectionHeader == "usd1") {
            // TODO add support for usd1
        }

        if (addPane) {
            curPane->read(stream, reverseEndian);
            paneTable.emplace(curPane->name, curPane);
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

}