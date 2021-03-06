#include <cinttypes>
#include <algorithm>
#include <iostream>
#include <memory>
#include <string>
#include <array>
#include <vector>
#include <unordered_map>

#ifndef BECQUEREL_COMMON_H
#define BECQUEREL_COMMON_H

static_assert(sizeof(float) == 4, "Size of single precision float must be 4 bytes");

namespace bq {

typedef std::array<std::uint8_t, 4> color8;
typedef std::array<std::uint16_t, 4> color16;

std::string readFixedStr(std::istream &stream, int len);
void writeFixedStr(const std::string &str, std::ostream &stream, int len);
std::string readNullTerminatedStr(std::istream &stream);
void writeNullTerminatedStr(const std::string &str, std::ostream &stream);
std::u16string readNullTerminatedStrU16(std::istream &stream, bool revEndian);
void writeNullTerminatedStrU16(const std::u16string &str, std::ostream &stream, bool revEndian);
template<class T>
T readNumber(std::istream &stream, bool reverseEndian) {
    T res;
    char *resPtr = reinterpret_cast<char *>(&res);
    stream.read(resPtr, sizeof(T));
    if (reverseEndian) {
        std::reverse(resPtr, resPtr + sizeof(T));
    }
    return res;
}
template<class T>
void writeNumber(T number, std::ostream &stream, bool reverseEndian) {
    char *numPtr = reinterpret_cast<char *>(&number);
    if (reverseEndian) {
        std::reverse(numPtr, numPtr + sizeof(T));
    }
    stream.write(numPtr, sizeof(T));
}
color8 readColor8(std::istream &stream, bool reverseEndian);
void writeColor8(const color8 &color, std::ostream &stream, bool reverseEndian);
color16 readColor16(std::istream &stream, bool reverseEndian);
void writeColor16(const color16 &color, std::ostream &stream, bool reverseEndian);
color8 toColor8(const color16 &color);
color16 toColor16(const color8 &color);

/**
 * @brief 2d vector class
 * 
 * @tparam T the component type
 */
template<class T>
struct vec2 {
    T x,y;
    void read(std::istream &stream, bool revEndian) {
        x = readNumber<T>(stream, revEndian);
        y = readNumber<T>(stream, revEndian);
    }
    void write(std::ostream &stream, bool revEndian) const {
        writeNumber(x, stream, revEndian);
        writeNumber(y, stream, revEndian);
    }
};

/**
 * @brief 3d vector class
 * 
 * @tparam T the component type
 */
template<class T>
struct vec3 {
    T x,y,z;
    void read(std::istream &stream, bool revEndian) {
        x = readNumber<T>(stream, revEndian);
        y = readNumber<T>(stream, revEndian);
        z = readNumber<T>(stream, revEndian);
    }
    void write(std::ostream &stream, bool revEndian) const {
        writeNumber(x, stream, revEndian);
        writeNumber(y, stream, revEndian);
        writeNumber(z, stream, revEndian);
    }
};

/**
 * @brief relative x-position of origin
 * 
 */
enum class OriginX : std::uint8_t {
    CENTER, LEFT, RIGHT
};

/**
 * @brief relative y-position of origin
 * 
 */
enum class OriginY : std::uint8_t {
    CENTER, TOP, BOTTOM
};

struct TexCoord {
    vec2<float> topLeft;
    vec2<float> topRight;
    vec2<float> bottomLeft;
    vec2<float> bottomRight;
};

enum class LineAlign : std::uint8_t {
    Unspecified = 0,
    Left = 1,
    Center = 2,
    Right = 3,
};

enum class WindowKind {
    Around = 0,
    Horizontal = 1,
    HorizontalNoContent = 2
};

template<class Material>
struct BaseWindowContent {
    color8 colorTopLeft;
    color8 colorTopRight;
    color8 colorBottomLeft;
    color8 colorBottomRight;
    std::shared_ptr<Material> material;
    std::vector<TexCoord> texCoords;
};

enum WindowFrameTexFlip : std::uint8_t {
    None = 0,
    FlipH = 1,
    FlipV = 2,
    Rotate90 = 3,
    Rotate180 = 4,
    Rotate270 = 5
};

template<class Material>
struct BaseWindowFrame {
    std::shared_ptr<Material> material;
    WindowFrameTexFlip texFlip;
};

struct BaseHeader;

/**
 * @brief base class for sections in a layout file
 * 
 */
struct Section {
    virtual void read(std::istream &stream, const BaseHeader &header);
    virtual void write(std::ostream &stream, const BaseHeader &header);
    virtual ~Section();
};

/**
 * @brief base class for layout panes
 * 
 */
struct BasePane : Section {
    std::string name;
    std::uint8_t paneMagFlags;
    std::string userDataInfo;
    bool visible;
    vec3<float> translate;
    vec3<float> rotate;
    vec2<float> scale;
    float width;
    float height;
    OriginX originX;
    OriginY originY;
    OriginX parentOriginX;
    OriginY parentOriginY;
    std::uint8_t alpha;
    bool influenceAlpha;
    std::weak_ptr<BasePane> parent;
    std::vector<std::shared_ptr<BasePane>> children;
    virtual std::string signature() = 0;
};

struct GroupPane : Section {
    static inline const std::string MAGIC = "grp1";
    std::string name;
    std::vector<std::string> panes;
    std::vector<std::shared_ptr<GroupPane>> children;
    std::weak_ptr<GroupPane> parent;
    std::string signature();
};

/**
 * @brief base class for header
 * 
 */
struct BaseHeader {
    unsigned version;
    std::uint16_t bom;
    std::uint16_t headerSize;
    std::shared_ptr<BasePane> rootPane;
    std::shared_ptr<GroupPane> rootGroup;
    bool revEndian() const;
};

struct LayoutInfo : virtual Section {
    bool drawFromCenter;
    float width;
    float height;
    std::string name;
};

template<bool padding>
struct Txl1 : Section {
    static inline const std::string MAGIC = "txl1";
    std::vector<std::string> textures;
    void read(std::istream &stream, const BaseHeader &header);
    void write(std::ostream &stream, const BaseHeader &header);
};

template struct Txl1<true>;

template<bool padding>
struct Fnl1 : Section {
    static inline const std::string MAGIC = "fnl1";
    std::vector<std::string> fonts;
    void read(std::istream &stream, const BaseHeader &header);
    void write(std::ostream &stream, const BaseHeader &header);
};

template struct Fnl1<true>;

struct TextureTransform {
    vec2<float> translate;
    float rotate;
    vec2<float> scale;
    void read(std::istream &stream, bool revEndian);
    void write(std::ostream &stream, bool revEndian);
};

struct BlendMode {
    enum class Op : std::uint8_t {
        Disable = 0,
        Add = 1,
        Subtract = 2,
        ReverseSubtract = 3,
        SelectMin = 4,
        SelectMax = 5
    };

    enum class BlendFactor : std::uint8_t {
        Factor0 = 0,
        Factor1 = 1,
        DestColor = 2,
        DestInvColor = 3,
        SourceAlpha = 4,
        SourceInvAlpha = 5,
        DestAlpha = 6,
        DestInvAlpha = 7,
        SourceColor = 8,
        SourceInvColor = 9
    };

    Op blendOp;
    BlendFactor srcFactor;
    BlendFactor destFactor;
    Op logicOp;

    void read(std::istream &stream, bool revEndian);
    void write(std::ostream &stream, bool revEndian);
};

enum class AlphaFunction : std::uint8_t {
    Never = 0,
    Less = 1,
    LessOrEqual = 2,
    Equal = 3,
    NotEqual = 4,
    GreaterOrEqual = 5,
    Greater = 6,
    Always = 7,
};

enum class AlphaOp : std::uint8_t {
    And = 0,
    Or = 1,
    Xor = 2,
    Nor = 3,
};

struct BaseAlphaCompare {
    AlphaFunction compareMode;
    float value;
};

enum class WrapMode {
    Clamp = 0,
    Repeat = 1,
    Mirror = 2
};

enum class FilterMode {
    Near = 0,
    Linear = 1
};

struct BaseTextureRef {
    std::string name;
    WrapMode wrapModeU;
    WrapMode wrapModeV;
    FilterMode filterModeMin;
    FilterMode filterModeMax;
};

enum class TevMode : std::uint8_t {
    Replace,
    Modulate,
    Add,
    AddSigned,
    Interpolate,
    Subtract,
    AddMultiplicate,
    MultiplcateAdd,
    Overlay,
    Indirect,
    BlendIndirect,
    EachIndirect,
};

struct BaseTevStage {
    TevMode colorMode;
    TevMode alphaMode;
};

enum class TexGenMatrixType : std::uint8_t {
    Matrix2x4
};

enum class TexGenType : std::uint8_t {
    TextureCoord0 = 0,
    TextureCoord1 = 1,
    TextureCoord2 = 2,
    OrthographicProjection = 3,
    PaneBasedProjection = 4,
    PerspectiveProjection = 5
};

struct TexCoordGen {
    TexGenMatrixType matrix;
    TexGenType source;
};

struct ProjectionTexGenParam {
    float posX;
    float posY;
    float scaleX;
    float scaleY;
    std::uint8_t flags;
};

template<class TexRefType, class TevStageType, class AlphaCompareType>
struct BaseMaterial {
    std::vector<TextureTransform> texTransforms;
    std::string name;
    color8 whiteColor;
    color8 blackColor;
    BlendMode blendMode;
    BlendMode blendModeLogic;
    AlphaCompareType alphaCompare;
    bool alphaInterp;
    std::vector<TexRefType> textureMaps;
    std::vector<TevStageType> tevStages;
    std::vector<TexCoordGen> texCoordGens;
    std::vector<ProjectionTexGenParam> projTexGenParams;
};

struct BasePat1 : Section {
    std::string name;
    std::vector<std::string> groups;
    std::int16_t startFrame;
    std::int16_t endFrame;
    std::uint16_t animationOrder;
    bool childBinding;
};

enum CurveType : std::uint8_t {
    Constant, Step, Hermite
};

struct KeyFrame {
    float frame;
    float value;
    float slope;
    void read(std::istream &stream, bool revEndian, CurveType curveType);
    void write(std::ostream &stream, bool revEndian, CurveType curveType);
};

struct PaiTagEntry {
    std::uint8_t index;
    std::uint8_t target;
    CurveType curveType;
    std::vector<KeyFrame> keyFrames;
    void read(std::istream &stream, bool revEndian);
    void write(std::ostream &stream, bool revEndian);
};

struct BasePaiTag {
    std::string tag;
    std::vector<PaiTagEntry> tagEntries;
};

enum AnimationTarget : std::uint8_t {
    Pane = 0, Material = 1
};

template<class TagType>
struct BasePaiEntry {
    std::string name;
    AnimationTarget target;
    std::vector<TagType> tags;
};

template<class EntryType>
struct BasePai1 : Section {
    std::uint16_t frameSize;
    bool loop;
    std::vector<std::string> textures;
    std::vector<EntryType> entries;
};

template<class IO>
class TemporarySeekI {
    public:
    TemporarySeekI(IO &s, std::streampos seekPos, std::ios::seekdir seekDir = std::ios::beg) : stream(s) {
        oldSeekPos = stream.tellg();
        stream.seekg(seekPos, seekDir);
    };
    ~TemporarySeekI() {
        stream.seekg(oldSeekPos);
    };
    TemporarySeekI<IO> &operator=(const TemporarySeekI<IO> &other) = delete;
    private:
    IO &stream;
    std::streampos oldSeekPos;
};
template<class IO>
class TemporarySeekO {
    public:
    TemporarySeekO(IO &s, std::streampos seekPos, std::ios::seekdir seekDir = std::ios::beg) : stream(s) {
        oldSeekPos = stream.tellp();
        stream.seekp(seekPos, seekDir);
    };
    ~TemporarySeekO() {
        stream.seekp(oldSeekPos);
    };
    TemporarySeekO<IO> &operator=(const TemporarySeekO<IO> &other) = delete;
    private:
    IO &stream;
    std::streampos oldSeekPos;
};
template<class T>
class BitField {
    T &theValue;
    int internalBitStart;
    int theNumBits;
    public:
    BitField(T &value, int bitStart, int numBits)
        : theValue(value), internalBitStart(8 * sizeof(T) - (bitStart + numBits)), theNumBits(numBits) {};
    operator T() const {
        return (theValue >> internalBitStart) & ((T(1) << theNumBits) - 1);
    };
    BitField<T> &operator=(T newBits) {
        T mask = ((T(1) << theNumBits) - 1) << internalBitStart;
        theValue &= ~mask;
        theValue |= (newBits << internalBitStart);
        return *this;
    };
    BitField<T> &operator=(const BitField<T> &other) = delete;
};

void writeSection(const std::string &magic, Section &sec, std::ostream &stream, const BaseHeader &header);

void alignFile(std::ostream &stream);

}

#endif
