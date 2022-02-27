#include "common.h"

#ifndef BECQUEREL_BRLAN_H
#define BECQUEREL_BRLAN_H

namespace bq::brlan {

struct Pat1 : BasePat1 {
    static inline const std::string MAGIC = "pat1";
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
    static inline const std::string MAGIC = "pai1";
    void read(std::istream &stream, bool revEndian);
    void write(std::ostream &stream, bool revEndian);
};

struct Brlan : BaseHeader {
    static inline const std::string MAGIC = "RLAN";
    Pat1 animationTag;
    Pai1 animationInfo;
    std::uint16_t bom;
    std::uint16_t headerSize;
    void read(std::istream &stream);
    void write(std::ostream &stream);
};

}

#endif
