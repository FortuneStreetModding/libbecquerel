#include "common.h"
#include <iostream>

#ifndef BECQUEREL_BRLYT_H
#define BECQUEREL_BRLYT_H

struct BrlytHeader : BaseHeader {
    static inline const std::string MAGIC = "RLYT";
    std::uint16_t bom;
    std::uint16_t headerSize;
    Lyt1 lyt1;
    Txl1 txl1;
    Fnl1 fnl1;
    bool read(std::istream &stream);
    bool write(std::ostream &stream);
};

struct Brlyt {
    BrlytHeader header;
    bool read(std::istream &stream);
    bool write(std::ostream &stream);
};

struct Lyt1 : LayoutInfo {
    static inline const std::string MAGIC = "lyt1";
    bool read(std::istream &stream, bool revEndian);
    bool write(std::ostream &stream, bool revEndian);
};

#endif
