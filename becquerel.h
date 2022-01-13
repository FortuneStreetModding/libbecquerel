#ifndef BECQUEREL_H
#define BECQUEREL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <inttypes.h>
#include <stdio.h>

typedef struct {
    char magic[4];
    uint16_t bom;
    uint16_t version;
    uint32_t file_len;
    uint16_t header_len;
    uint16_t num_sections;
} BrlytHeader;

typedef struct {
    char magic[4];
    uint32_t section_len;
} BrlytSection;

typedef struct {
    BrlytSection section;
    uint8_t centered;
    uint8_t unknown[3];
    float width;
    float height;
} Lyt1;

typedef struct {
    BrlytSection section;
    uint16_t num_entries;
    uint16_t unknown;
} Usd1;

typedef struct {
    char *name;
    char *data;
    uint16_t num_entries;
    uint8_t type;
    uint8_t unknown;
} Usd1Entry;

typedef struct {
    BrlytSection section;
    uint16_t num_tpls;
    uint16_t unknown;
} Txl1;

typedef struct {
    char *tpl_name;
    uint32_t padding;
} Txl1Entry;

typedef struct {
    BrlytSection section;
    uint16_t num_mats;
    uint16_t unknown;
} Mat1;

typedef struct {
    char mat_name[14];
    uint16_t fore_color[4];
    uint16_t back_color[4];
    uint16_t color_r3[4];
    uint8_t tev_colors[4][4];
    uint32_t material_structures;
} Mat1Entry;

typedef struct {
    uint8_t condition;
    uint8_t operation;
    uint8_t values[2];
} AlphaCompare;

typedef struct {
    uint8_t color_source;
    uint8_t alpha_source;
    uint16_t padding;
} ChannelControl;

typedef struct {
    float translation[2];
    float rotation;
    float scale[2];
} IndirectTextureSRT;

typedef struct {
    uint8_t tex_coord_idx;
    uint8_t tex_map_idx;
    uint8_t wrap_s;
    uint8_t wrap_t;
} IndirectState;

typedef struct {
    BrlytSection section;
    uint16_t num_fonts;
    uint16_t unknown;
} Fnl1;

typedef struct {
    char *font_name;
    uint32_t padding;
} Fnl1Entry;

typedef struct {
    BrlytSection section;
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
} Pan1;

typedef struct {
    BrlytSection section;
} Pas1;

typedef struct {
    BrlytSection section;
} Pae1;

typedef struct {
    uint8_t top_left_color[4];
    uint8_t top_right_color[4];
    uint8_t bottom_left_color[4];
    uint8_t bottom_right_color[4];
    uint16_t material_index;
    uint8_t num_uv_sets;
    uint8_t unknown;
} Pic1;

typedef struct {
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
} Txt1;

typedef struct {
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
} Wnd1;

typedef struct {
    uint16_t material;
    uint8_t flip_type;
    uint8_t padding;
} Wnd1Entry;

typedef struct {
    BrlytSection section;
    char grp_name[0x10];
    uint16_t num_entries;
    uint16_t padding;
} Grp1;

typedef struct {
    char entry[0x10];
} Grp1Entry;

typedef struct {
    BrlytSection section;
} Grs1;

typedef struct {
    BrlytSection section;
} Gre1;

typedef struct {
    float top_left_coord[2];
    float top_right_coord[2];
    float bottom_left_coord[2];
    float bottom_right_coord[2];
} UvCoord;

typedef struct {
    BrlytHeader header;
    void *sections;
} BrlytFile;

int becquerel_extract_brlyt(FILE *file, BrlytFile *brlyt);
int becquerel_create_brlyt(FILE *file, BrlytFile *brlyt);
void becquerel_destroy_brlyt(BrlytFile *file);

#ifdef __cplusplus
}
#endif

#endif
