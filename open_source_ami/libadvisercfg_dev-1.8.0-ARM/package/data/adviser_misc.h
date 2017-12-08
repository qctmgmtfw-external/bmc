#ifndef __ADVISER_MISC_H__
#define __ADVISER_MISC_H__

// crash screen capture related
#define	CAPTURE_SCREEN_FILE	"/conf/crashscr.jpg"
#define CAPTURE_SCREEN_QUALITY	10

int capture_screen();
int program_default_edid();
int program_edid_file(char *edid_file);
int create_common_edid_file(char* edid_file_1, char *edid_file_2, char *result_edid_file);
int calculate_common_edid(unsigned char edid_1[], unsigned char edid_2[], unsigned char result_edid[]);

#endif



