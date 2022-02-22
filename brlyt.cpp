#include "brlyt.h"

namespace bq::brlyt {

bool Lyt1::read(std::istream &stream, bool revEndian) {
    drawFromCenter = readNumber<std::uint8_t>(stream, revEndian);
    stream.seekg(3, std::ios::cur); // padding
    width = readNumber<float>(stream, revEndian);
    height = readNumber<float>(stream, revEndian);
    return true;
}

bool TextureRef::read(std::istream &stream, bool revEndian) {
    id = readNumber<std::uint16_t>(stream, revEndian);
    wrapModeU = (WrapMode)readNumber<std::uint8_t>(stream, revEndian);
    wrapModeV = (WrapMode)readNumber<std::uint8_t>(stream, revEndian);
    filterModeMin = filterModeMax = FilterMode::Linear;
}

bool Material::read(std::istream &stream, bool revEndian) {
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
    
    // TODO add more stuff
    return true;
}

bool Mat1::read(std::istream &stream, bool revEndian) {
    auto pos = stream.tellg();
    auto numMats = readNumber<std::uint16_t>(stream, revEndian);
    stream.seekg(2, std::ios::cur); // padding
    for (int i=0; i<numMats; ++i) {
        auto off = readNumber<std::uint32_t>(stream, revEndian);
        {
            TemporarySeek ts(stream, pos + std::streamoff(off - 8));
            materials.emplace_back();
            if (!materials.back().read(stream, revEndian)) {
                return false;
            }
        }
    }
    return true;
}

bool Brlyt::read(std::istream &stream) {
    return header.read(stream);
}

template<class T>
static void setPane(std::shared_ptr<T> pane, std::shared_ptr<T> parentPane) {
    if (parentPane) {
        parentPane->children.push_back(pane);
        pane->parent = parentPane;
    }
}

bool BrlytHeader::read(std::istream &stream) {
    auto magic = readFixedStr(stream, 4);
    bool reverseEndian;
    if (magic != MAGIC) {
        return false;
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
            curPane->read(stream, reverseEndian);
            paneTable.emplace(curPane->name, curPane);
            setPane(curPane, parentPane);
        } else if (sectionHeader == Pic1::MAGIC) {
            curPane = std::make_shared<Pic1>();
            curPane->read(stream, reverseEndian);
            paneTable.emplace(curPane->name, curPane);
            setPane(curPane, parentPane);
        } else if (sectionHeader == Txt1::MAGIC) {
            curPane = std::make_shared<Txt1>();
            curPane->read(stream, reverseEndian);
            paneTable.emplace(curPane->name, curPane);
            setPane(curPane, parentPane);
        } else if (sectionHeader == Bnd1::MAGIC) {
            curPane = std::make_shared<Bnd1>();
            curPane->read(stream, reverseEndian);
            paneTable.emplace(curPane->name, curPane);
            setPane(curPane, parentPane);
        } else if (sectionHeader == Wnd1::MAGIC) {
            curPane = std::make_shared<Wnd1>();
            curPane->read(stream, reverseEndian);
            paneTable.emplace(curPane->name, curPane);
            setPane(curPane, parentPane);
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
        }

        if (!rootPane && sectionHeader == Pan1::MAGIC) {
            rootPane = curPane;
        }

        if (!rootGroup && sectionHeader == Grp1::MAGIC) {
            rootGroup = curGroupPane;
        }

        stream.seekg(pos + std::streamoff(sectionSize));
    }

    return true;
}

}