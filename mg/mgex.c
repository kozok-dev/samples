#include <stdio.h>
#include "mg.h"

char write_spr(FILE *rf)
{
	char pal[768], fn[99], *data = NULL, is_succ = 0, is_pal = 0;
	int len, cnt = 0;
	FILE *wf = NULL;
	SFF_HEADER sh;
	SFF_SUBFILE_HEADER ssh;

	if (fseek(rf, 0, SEEK_SET) ||
		fread(&sh, sizeof(sh), 1, rf) < 1 ||
		fseek(rf, sh.subfile_offset, SEEK_SET)) return 0;

	while (cnt++ < sh.number_of_images) {
		if (fread(&ssh, sizeof(ssh), 1, rf) < 1) return 0;
		if (ssh.next_subfile_offset <= 0) break;

		len = ssh.subfile_length;
		if (len > 0) {
			data = (char *)malloc(len);
			if (data == NULL || fread(data, sizeof(char), len, rf) < len) goto proc_end;

			sprintf(fn, "%d-%d.pcx", ssh.group_number, ssh.image_number);
			wf = fopen(fn, "wb");
			if (wf == NULL || fwrite(data, sizeof(char), len, wf) < len) goto proc_end;

			if (ssh.is_same_prev_pal) {
				if (fwrite(pal, sizeof(pal), 1, wf) < 1) goto proc_end;
			} else {
				if (!is_pal) {
					if (fseek(rf, -sizeof(pal), SEEK_CUR) || fread(pal, sizeof(pal), 1, rf) < 1) return 0;
					is_pal = 1;
				}
			}

			free(data);
			data = NULL;
			fclose(wf);
			wf = NULL;
		}

		if (fseek(rf, ssh.next_subfile_offset, SEEK_SET)) return 0;
	}

	is_succ = 1;
proc_end:
	if (data != NULL) free(data);
	if (wf != NULL) fclose(wf);
	return is_succ;
}

char write_snd(FILE *rf)
{
	char fn[99], *data = NULL, is_succ = 0;
	int loc, len, group, sample;
	FILE *wf = NULL;

	if (fseek(rf, 20, SEEK_SET) || fread(&loc, sizeof(int), 1, rf) < 1) return 0;

	while (loc > 0) {
		if (fseek(rf, loc, SEEK_SET) ||
			fread(&loc, sizeof(int), 1, rf) < 1 ||
			fread(&len, sizeof(int), 1, rf) < 1 ||
			fread(&group, sizeof(int), 1, rf) < 1 ||
			fread(&sample, sizeof(int), 1, rf) < 1) return 0;

		data = (char *)malloc(len * sizeof(char));
		if (data == NULL || fread(data, sizeof(char), len, rf) < len) goto proc_end;

		sprintf(fn, "%d-%d.wav", group, sample);
		wf = fopen(fn, "wb");
		if (wf == NULL || fwrite(data, sizeof(char), len, wf) < len) goto proc_end;

		free(data);
		data = NULL;
		fclose(wf);
		wf = NULL;
	}

	is_succ = 1;
proc_end:
	if (data != NULL) free(data);
	if (wf != NULL) fclose(wf);
	return is_succ;
}

void main(int argc, char **argv)
{
	char buf[256], is_succ;
	int i;
	FILE *rf = NULL;

	if (argc < 2) return;

	for (i = 1; i < argc; i++) {
		is_succ = 0;

		printf("%s > ", argv[i]);
		rf = fopen(argv[i], "rb");
		if (rf == NULL) goto proc_next;

		if (fread(buf, sizeof(char), 12, rf) < 12) goto proc_next;

		if (!memcmp(buf, "ElecbyteSpr\0", 12)) {
			if (!write_spr(rf)) goto proc_next;
		} else if (!memcmp(buf, "ElecbyteSnd\0", 12)) {
			if (!write_snd(rf)) goto proc_next;
		} else {
			goto proc_next;
		}

		is_succ = 1;
proc_next:
		if (rf != NULL) fclose(rf);
		printf(is_succ == 1 ? "done\n" : "error\n");
	}
}
