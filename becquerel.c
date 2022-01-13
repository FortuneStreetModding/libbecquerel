#include "becquerel.h"
#include <stdbool.h>
#include <string.h>

static void readval(FILE *file, void *val, size_t size, bool rev_endian) {
    fread(val, size, 1, file);
    if (rev_endian) {
        char *cval = (char *)val;
        for (int i=0; i<size-i-1; ++i) {
            char tmp = cval[i];
            cval[i] = cval[size-i-1];
            cval[size-i-1] = tmp;
        }
    }
}

#define RV(file, val, rev_endian) readval((file), (val), sizeof(*(val)), (rev_endian));

static int read_header(FILE *file, BrlytHeader *header, bool *rev_endian) {
    fread(header->magic, 1, 4, file);
    if (strncmp(header->magic, "RLYT", 4) != 0) {
        return -1;
    }
    fread(&header->bom, 2, 1, file);
    *rev_endian = header->bom != 0xFEFF;
    RV(file, &header->version, rev_endian);
    RV(file, &header->file_len, rev_endian);
    RV(file, &header->header_len, rev_endian);
    RV(file, &header->num_sections, rev_endian);

    return 0;
}

int becquerel_extract_brlyt(FILE *file, BrlytFile *brlyt) {
    bool rev_endian;
    read_header(file, &brlyt->header, &rev_endian);
    // TODO read_section
    return 0;
}