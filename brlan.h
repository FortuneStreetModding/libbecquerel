#include "common.h"

#ifndef BECQUEREL_BRLAN_H
#define BECQUEREL_BRLAN_H

namespace bq::brlan {

struct Pat1 : BasePat1 {
    std::string unknownData;
    void read(std::istream &stream, bool revEndian);
    void write(std::ostream &stream, bool revEndian);
};

struct PaiTag : BasePaiTag {
    std::uint32_t unknown;
    void read(std::istream &stream, bool revEndian, AnimationTarget target);
    void write(std::ostream &stream, bool revEndian, AnimationTarget target);
};

struct PaiEntry : BasePaiEntry<PaiTag> {
    void read(std::istream &stream, bool revEndian);
    void write(std::ostream &stream, bool revEndian);
};

struct Pai1 : BasePai1<PaiEntry> {
    void read(std::istream &stream, bool revEndian);
    void write(std::ostream &stream, bool revEndian);
};

}

#endif
