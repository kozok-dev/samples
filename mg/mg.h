/* SFF file header (512 bytes) */
typedef struct {
	char signature[12];	/* "ElecbyteSpr\0" signature */
	int version;	/* 1 verhi, 1 verlo, 1 verlo2, 1 verlo3 */
	int number_of_groups;	/* Number of groups */
	int number_of_images;	/* Number of images */
	int subfile_offset;	/* File offset where first subfile is located */
	int size_of_subheader;	/* Size of subheader in bytes */
	char palette_type;	/* Palette type (1=SPRPALTYPE_SHARED or 0=SPRPALTYPE_INDIV) */
	char blank[3];	/* Blank; set to zero */
	char comment[476];	/* Blank; can be used for comments */
} SFF_HEADER;

/* SFF file subfile header (32 bytes) */
typedef struct {
	int next_subfile_offset;	/* File offset where next subfile in the "linked list" is located. Null if last subfile */
	int subfile_length;	/* Subfile length (not including header) Length is 0 if it is a linked sprite */
	short x;	/* Image axis X coordinate */
	short y;	/* Image axis Y coordinate */
	short group_number;	/* Group number */
	short image_number;	/* Image number (in the group) */
	short prev_cpy_spr_idx;	/* Index of previous copy of sprite (linked sprites only) This is the actual */
	char is_same_prev_pal;	/* True if palette is same as previous image */
	char comment[13];	/* Blank; can be used for comments */
	/* unsigned char data[1];	PCX graphic data. If palette data is available, it is the last 768 bytes */
} SFF_SUBFILE_HEADER;
