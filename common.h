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
