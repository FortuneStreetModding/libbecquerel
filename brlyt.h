#include "common.h"
#include <iostream>

#ifndef BECQUEREL_BRLYT_H
#define BECQUEREL_BRLYT_H

namespace bq::brlyt {

struct Lyt1 : LayoutInfo {
    static inline const std::string MAGIC = "lyt1";
    void read(std::istream &stream, bool revEndian);
    void write(std::ostream &stream, bool revEndian);
};

enum class TexCoordGenTypes {
    GX_TG_MTX3x4 = 0,
    GX_TG_MTX2x4 = 1,
    GX_TG_BUMP0 = 2,
    GX_TG_BUMP1 = 3,
    GX_TG_BUMP2 = 4,
    GX_TG_BUMP3 = 5,
    GX_TG_BUMP4 = 6,
    GX_TG_BUMP5 = 7,
    GX_TG_BUMP6 = 8,
    GX_TG_BUMP7 = 9,
    GX_TG_SRTG = 0xA
};

enum class TexCoordGenSource {
    GX_TG_POS,
    GX_TG_NRM,
    GX_TG_BINRM,
    GX_TG_TANGENT,
    GX_TG_TEX0,
    GX_TG_TEX1,
    GX_TG_TEX2,
    GX_TG_TEX3,
    GX_TG_TEX4,
    GX_TG_TEX5,
    GX_TG_TEX6,
    GX_TG_TEX7,
    GX_TG_TEXCOORD0,
    GX_TG_TEXCOORD1,
    GX_TG_TEXCOORD2,
    GX_TG_TEXCOORD3,
    GX_TG_TEXCOORD4,
    GX_TG_TEXCOORD5,
    GX_TG_TEXCOORD6,
    GX_TG_COLOR0,
    GX_TG_COLOR1
};

enum class TexCoordGenMatrixSource {
    GX_PNMTX0,
    GX_PNMTX1,
    GX_PNMTX2,
    GX_PNMTX3,
    GX_PNMTX4,
    GX_PNMTX5,
    GX_PNMTX6,
    GX_PNMTX7,
    GX_PNMTX8,
    GX_PNMTX9,
    GX_TEXMTX0,
    GX_TEXMTX1,
    GX_TEXMTX2,
    GX_TEXMTX3,
    GX_TEXMTX4,
    GX_TEXMTX5,
    GX_TEXMTX6,
    GX_TEXMTX7,
    GX_TEXMTX8,
    GX_TEXMTX9,
    GX_IDENTITY,
    GX_DTTMTX0,
    GX_DTTMTX1,
    GX_DTTMTX2,
    GX_DTTMTX3,
    GX_DTTMTX4,
    GX_DTTMTX5,
    GX_DTTMTX6,
    GX_DTTMTX7,
    GX_DTTMTX8,
    GX_DTTMTX9,
    GX_DTTMTX10,
    GX_DTTMTX11,
    GX_DTTMTX12,
    GX_DTTMTX13,
    GX_DTTMTX14,
    GX_DTTMTX15,
    GX_DTTMTX16,
    GX_DTTMTX17,
    GX_DTTMTX18,
    GX_DTTMTX19,
    GX_DTTIDENTITY
};

struct TexCoordGenEntry {
    TexCoordGenTypes type;
    TexCoordGenSource source;
    TexCoordGenMatrixSource matrixSource;
    std::uint8_t unknown;
    void read(std::istream &stream, bool revEndian);
    void write(std::ostream &stream, bool revEndian);
};

struct ChanCtrl {
    std::uint8_t colorMatSource;
    std::uint8_t alphaMatSource;
    std::uint8_t unknown1;
    std::uint8_t unknown2;
    void read(std::istream &stream, bool revEndian);
    void write(std::ostream &stream, bool revEndian);
};

enum SwapChannel {
    Red,
    Green,
    Blue,
    Alpha
};

struct SwapMode {
    SwapChannel swapChannels[4];
};

struct TevSwapModeTable {
    SwapMode swapModes[4];
    void read(std::istream &stream, bool revEndian);
    void write(std::ostream &stream, bool revEndian);
};

struct IndirectStage {
    std::uint8_t texCoord;
    std::uint8_t texMap;
    std::uint8_t scaleS;
    std::uint8_t scaleT;
    void read(std::istream &stream, bool revEndian);
    void write(std::ostream &stream, bool revEndian);
};

struct TextureRef : BaseTextureRef {
    void read(std::istream &stream, bool revEndian);
    void write(std::ostream &stream, bool revEndian);
};

struct TevStage : BaseTevStage {
    std::uint8_t texCoord;
    std::uint8_t color;
    // TODO add bit fields for the flags
    std::uint16_t flag1;
    std::array<std::uint8_t, 12> flags;
    void read(std::istream &stream, bool revEndian);
    void write(std::ostream &stream, bool revEndian);
};

struct AlphaCompare : BaseAlphaCompare {
    AlphaFunction comp0;
    AlphaFunction comp1;
    AlphaOp op;
    std::uint8_t ref0;
    std::uint8_t ref1;
    void read(std::istream &stream, bool revEndian);
    void write(std::ostream &stream, bool revEndian);
};

struct Material : BaseMaterial<TextureRef, TevStage, AlphaCompare> {
    color8 colorRegister3;
    color8 matColor;
    std::array<color8, 4> tevColors;
    std::vector<TexCoordGenEntry> texCoordGens;
    ChanCtrl chanCtrl;
    TevSwapModeTable swapModeTable;
    std::vector<TextureTransform> indirectTransforms;
    std::vector<IndirectStage> indirectStages;
    std::uint32_t flags;
    BitField<std::uint32_t> hasMaterialColor = BitField(flags, 4, 1);
    BitField<std::uint32_t> hasChannelControl = BitField(flags, 6, 1);
    BitField<std::uint32_t> hasBlendMode = BitField(flags, 7, 1);
    BitField<std::uint32_t> hasAlphaCompare = BitField(flags, 8, 1);
    BitField<std::uint32_t> tevStagesCount = BitField(flags, 9, 5);
    BitField<std::uint32_t> indTexOrderCount = BitField(flags, 14, 3);
    BitField<std::uint32_t> indSrtCount = BitField(flags, 17, 2);
    BitField<std::uint32_t> hasTevSwapTable = BitField(flags, 19, 1);
    BitField<std::uint32_t> texCoordGenCount = BitField(flags, 20, 4);
    BitField<std::uint32_t> mtxCount = BitField(flags, 24, 4);
    BitField<std::uint32_t> texCount = BitField(flags, 28, 4);
    void read(std::istream &stream, bool revEndian);
    void write(std::ostream &stream, bool revEndian);
};

struct Mat1 {
    static inline const std::string MAGIC = "mat1";
    std::vector<Material> materials;
    void read(std::istream &stream, bool revEndian);
    void write(std::ostream &stream, bool revEndian);
};

struct Pan1 : BasePane {
    static inline const std::string MAGIC = "pan1";
    std::uint8_t flags;
    void read(std::istream &stream, bool revEndian);
    void write(std::ostream &stream, bool revEndian);
};

struct Pic1 : Pan1 {
    static inline const std::string MAGIC = "pic1";
    std::vector<TexCoord> texCoords;
    color8 colorTopLeft;
    color8 colorTopRight;
    color8 colorBottomLeft;
    color8 colorBottomRight;
    std::uint16_t materialIndex;
    void read(std::istream &stream, bool revEndian);
    void write(std::ostream &stream, bool revEndian);
};

struct Txt1 : Pan1 {
    static inline const std::string MAGIC = "txt1";
    std::uint16_t textLen;
    std::uint16_t maxTextLen;
    std::uint16_t materialIndex;
    std::uint16_t fontIndex;
    std::uint8_t textAlign;
    LineAlign lineAlign;
    float italicTilt;
    color8 fontTopColor;
    color8 fontBottomColor;
    vec2<float> fontSize;
    float charSpace;
    float lineSpace;
    vec2<float> shadowXY;
    vec2<float> shadowXYSize;
    color8 shadowForeColor;
    color8 shadowBackColor;
    float shadowItalic;
    std::string textBoxName;
    std::uint8_t flags;
    void read(std::istream &stream, bool revEndian);
    void write(std::ostream &stream, bool revEndian);
};

struct Bnd1 : Pan1 {
    static inline const std::string MAGIC = "bnd1";
    void read(std::istream &stream, bool revEndian);
    void write(std::ostream &stream, bool revEndian);
};

struct Wnd1 : Pan1 {
    static inline const std::string MAGIC = "wnd1";
    WindowKind windowKind;
    std::uint16_t stretchLeft;
    std::uint16_t stretchRight;
    std::uint16_t stretchTop;
    std::uint16_t stretchBottom;
    std::uint16_t frameElementLeft;
    std::uint16_t frameElementRight;
    std::uint16_t frameElementTop;
    std::uint16_t frameElementBottom;
    std::uint8_t flags;
    WindowContent content;
    std::vector<WindowFrame> frames;
    void read(std::istream &stream, bool revEndian);
    void write(std::ostream &stream, bool revEndian);
};

struct Grp1 : GroupPane {
    void read(std::istream &stream, bool revEndian);
    void write(std::ostream &stream, bool revEndian);
};

struct BrlytHeader : BaseHeader {
    static inline const std::string MAGIC = "RLYT";
    std::uint16_t bom;
    std::uint16_t headerSize;
    Lyt1 lyt1;
    Txl1<true> txl1;
    Mat1 mat1;
    Fnl1<true> fnl1;
    void read(std::istream &stream);
    void write(std::ostream &stream);
};

struct Brlyt {
    BrlytHeader header;
    void read(std::istream &stream);
    void write(std::ostream &stream);
};

}

#endif
