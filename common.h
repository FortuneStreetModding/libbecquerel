#include <cinttypes>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#ifndef BECQUEREL_COMMON_H
#define BECQUEREL_COMMON_H

/**
 * @brief 2d vector class
 * 
 * @tparam T the component type
 */
template<class T>
struct vec2 { T x,y; };

/**
 * @brief 3d vector class
 * 
 * @tparam T the component type
 */
template<class T>
struct vec3 { T x,y,z; };

typedef std::array<std::uint8_t, 4> color8;

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

/**
 * @brief base class for sections in a layout file
 * 
 */
struct Section {
    std::string magic;
    std::uint32_t sectionLen;
};

/**
 * @brief base class for layout panes
 * 
 */
struct BasePane : virtual Section {
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
};

/**
 * @brief base class for header
 * 
 */
struct BaseHeader {
    unsigned version;
    std::vector<std::string> textures;
    std::vector<std::string> fonts;
    std::unique_ptr<BasePane> rootPane;
};

struct LayoutInfo : virtual Section {
    bool drawFromCenter;
    float width;
    float height;
    std::string name;
};

struct Txl1 : virtual Section {
    static inline const std::string MAGIC = "txl1";
    std::vector<std::string> textures;
    bool read(std::istream &stream, bool revEndian, bool padding);
    bool write(std::ostream &stream, bool revEndian, bool padding);
};

struct Fnl1 : virtual Section {
    static inline const std::string MAGIC = "fnl1";
    std::vector<std::string> fonts;
    bool read(std::istream &stream, bool revEndian, bool padding);
    bool write(std::ostream &stream, bool revEndian, bool padding);
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

struct AlphaCompare {
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

struct TextureRef {
    std::uint16_t id;
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

struct TevStage {
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

struct BaseMaterial {
    color8 whiteColor;
    color8 blackColor;
    BlendMode blendMode;
    BlendMode blendModeLogic;
    AlphaCompare alphaCompare;
    bool alphaInterp;
    std::vector<TextureRef> textureMaps;
    std::vector<TevStage> tevStages;
    std::vector<TexCoordGen> texCoordGens;
    std::vector<ProjectionTexGenParam> projTexGenParams;
};

template<class IO>
class TemporarySeek {
    public:
    TemporarySeek(IO &s, long seekPos, int seekDir = std::ios::beg);
    ~TemporarySeek();
    private:
    IO *stream;
    long oldSeekPos;
};

std::string readFixedStr(std::istream &stream, int len);
std::string readNullTerminatedStr(std::istream &stream);
template<class T>
T readNumber(std::istream &stream, bool reverseEndian);

#endif
