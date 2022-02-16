#include "brlyt.h"

bool Brlyt::read(std::istream &stream) {
    return header.read(stream);
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
        } else if (sectionHeader == Pic1::MAGIC) {
            curPane = std::make_shared<Pic1>();
        } else if (sectionHeader == Txt1::MAGIC) {
            curPane = std::make_shared<Txt1>();
        } else if (sectionHeader == Bnd1::MAGIC) {
            curPane = std::make_shared<Bnd1>();
        } else if (sectionHeader == Wnd1::MAGIC) {
            curPane = std::make_shared<Wnd1>();
        } else if (sectionHeader == "pas1") {
            if (curPane) {
                parentPane = curPane;
            }
        } else if (sectionHeader == "pae1") {
            curPane = parentPane;
            parentPane = std::shared_ptr<BasePane>(curPane->parent);
        } else if (sectionHeader == Grp1::MAGIC) {
            curGroupPane = std::make_shared<Grp1>();
        } else if (sectionHeader == "grs1") {
            if (curGroupPane) {
                parentGroupPane = curGroupPane;
            }
        } else if (sectionHeader == "gre1") {
            curGroupPane = parentGroupPane;
            parentGroupPane = std::shared_ptr<GroupPane>(curGroupPane->parent);
        }

        if (curPane) {
            curPane->read(stream, reverseEndian);
        }

        if (!rootPane && sectionHeader == Pan1::MAGIC) {
            rootPane = curPane;
        }

        if (!rootGroup && sectionHeader == Grp1::MAGIC) {
            rootGroup = curGroupPane;
        }        
    }

    return true;
}