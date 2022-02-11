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
        std::shared_ptr<Pan1> curPane;
        if (sectionHeader == Lyt1::MAGIC) {
            lyt1.read(stream, reverseEndian);
        } else if (sectionHeader == Txl1<true>::MAGIC) {
            txl1.read(stream, reverseEndian);
        } else if (sectionHeader == Fnl1<true>::MAGIC) {
            fnl1.read(stream, reverseEndian);
        } else if (sectionHeader == Mat1::MAGIC) {
            mat1.read(stream, reverseEndian);
        } else if (sectionHeader == Pan1::MAGIC) {
            curPane = std::make_unique<Pan1>();
        }

        if (!rootPane && sectionHeader == Pan1::MAGIC) {
            rootPane = curPane;
        }
    }

    return true;
}