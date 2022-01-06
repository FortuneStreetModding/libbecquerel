#ifndef BECQUEREL_H
#define BECQUEREL_H

#include <inttypes.h>

struct BrlytHeader {
    char magic[4];
    uint16_t bom;
    uint16_t version;
    uint32_t file_len;
    uint16_t header_len;
    uint16_t num_sections;
};

struct BrlytSection {
    char magic[4];
    uint32_t section_len;
};

struct Lyt1 {
    struct BrlytSection section;
    uint8_t centered;
    uint8_t unknown[3];
    float width;
    float height;
};

struct Usd1 {
    struct BrlytSection section;
    uint16_t num_entries;
    uint16_t unknown;
};

struct Usd1Entry {
    char *name;
    char *data;
    uint16_t num_entries;
    uint8_t type;
    uint8_t unknown;
};

struct Txl1 {
    struct BrlytSection section;
    uint16_t num_tpls;
    uint16_t unknown;
};

struct Txl1Entry {
    char *tpl_name;
    uint32_t padding;
};

struct Mat1 {
    struct BrlytSection section;
    uint16_t num_mats;
    uint16_t unknown;
};

struct Mat1Entry {
    char mat_name[14];
    uint16_t fore_color[4];
    uint16_t back_color[4];
    uint16_t color_r3[4];
    uint8_t tev_colors[4][4];
    uint32_t material_structures;
};

struct AlphaCompare {
    uint8_t condition;
    uint8_t operation;
    uint8_t values[2];
};

struct ChannelControl {
    uint8_t color_source;
    uint8_t alpha_source;
    uint16_t padding;
};

struct IndirectTextureSRT {
    float translation[2];
    float rotation;
    float scale[2];
};

struct IndirectState {
    uint8_t tex_coord_idx;
    uint8_t tex_map_idx;
    uint8_t wrap_s;
    uint8_t wrap_t;
};

struct Fnl1 {
    struct BrlytSection section;
    uint16_t num_fonts;
    uint16_t unknown;
};

struct Fnl1Entry {
    char *font_name;
    uint32_t padding;
};

struct Pan1 {
    struct BrlytSection section;
    uint8_t flag;
    uint8_t origin_type;
    uint8_t alpha;
    uint8_t padding;
    char pane_name[0x10];
    char user_info[0x8];
    float translation[3];
    float rotation[3];
    float scale[2];
    float width;
    float height;
};

struct Pas1 {
    struct BrlytSection section;
};

struct Pae1 {
    struct BrlytSection section;
};

struct Pic1 {
    uint8_t top_left_color[4];
    uint8_t top_right_color[4];
    uint8_t bottom_left_color[4];
    uint8_t bottom_right_color[4];
    uint16_t material_index;
    uint8_t num_uv_sets;
    uint8_t unknown;
};

struct Txt1 {
    uint16_t string_size;
    uint16_t max_string_size;
    uint16_t mat_idx;
    uint16_t font_idx;
    uint8_t string_origin;
    uint8_t alignment;
    uint16_t padding;
    char *text;
    uint8_t top_color[4];
    uint8_t bottom_color[4];
    float font_size_x;
    float font_size_y;
    float character_size;
    float line_size;
};

struct Wnd1 {
    float coords[4];
    uint8_t num_frames;
    uint8_t flag;
    uint16_t padding;
    uint32_t off_window_content;
    // offsets here
    uint8_t top_left_color[4];
    uint8_t top_right_color[4];
    uint8_t bottom_left_color[4];
    uint8_t bottom_right_color[4];
    uint16_t mat_idx;
    uint8_t num_uv_sets;
    uint8_t unknown;
};

struct Wnd1Entry {
    uint16_t material;
    uint8_t flip_type;
    uint8_t padding;
};

struct Grp1 {
    struct BrlytSection section;
    char grp_name[0x10];
    uint16_t num_entries;
    uint16_t padding;
};

struct Grp1Entry {
    char entry[0x10];
};

struct Grs1 {
    struct BrlytSection section;
};

struct Gre1 {
    struct BrlytSection section;
};

struct UvCoord {
    float top_left_coord[2];
    float top_right_coord[2];
    float bottom_left_coord[2];
    float bottom_right_coord[2];
};

#endif
