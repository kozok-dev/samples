/*
WaruN�p�b�N�t�@�C���쐬�c�[��

���f�[�^�\��

[�V�O�j�`��]
WARUNPACK\0�F10�o�C�g

[�w�b�_]
\0�ŏI���t�@�C�����F�ő�250�o�C�g
�t�@�C���̓��e������p�b�N�t�@�C���̃V�O�j�`���������擪����̏ꏊ�F4�o�C�g
�t�@�C���̃T�C�Y�F4�o�C�g
����L���t�@�C���̐���������ł���Ƃ������ƂɂȂ�B
���t�@�C������\0�Ŏn�܂��Ă���ꍇ�A�w�b�_�I���Ƃ��A���̃o�C�g����t�@�C���̓��e���i�[����Ă���B

���Í���
�V�O�j�`���������p�b�N�t�@�C���S�̂�Blowfish��p���ACBC�ňÍ�������B�������̍ہA���R�A�V�O�j�`���͖�������悤�ɂ���B
���t�@�C�����ł͂Ȃ��A�S�̂ňÍ�������B�܂�A�����x�N�g���̓p�b�N�t�@�C���̃V�O�j�`���������擪��1�����ł���B
��Blowfish�̓����ɂ��A���f�[�^��8�Ŋ���؂�Ȃ��Ȃ�\0�Ŗ��߂�B�Í������ꂽ�f�[�^�̃T�C�Y��8�Ŋ���؂��B
*/
#include <stdio.h>
#include <io.h>
#include <conio.h>
#include "cryptblowfish.cpp"
#include "md5.cpp"
#define PACKFILE "warun.pak"

// �w�b�_�T�C�Y���擾�B
// �����Fpdir �����Ɂu/�v���܂܂Ȃ��p�X�B
// �Ԓl�F�w�b�_�T�C�Y�B
UINT getHeaderSize(LPCSTR pdir) {
	char dir[256];
	int fh;
	UINT size = 0;
	struct _finddata_t fd;

	strncpy(dir, pdir, 50);
	strcat(dir, "/*");
	fh = _findfirst(dir, &fd);
	if (fh == -1) return 0;

	strncpy(dir, pdir, 50);
	strcat(dir, "/");
	do {
		if (fd.name[0] == '.' || (fd.attrib & _A_SUBDIR) != 0) continue;

		size += strlen(dir) + strlen(fd.name) + 1 + 4 + 4;	// �t�@�C���� + \0 + �ꏊ + �T�C�Y
	} while (_findnext(fh, &fd) == 0);
	_findclose(fh);

	return size;
}

// �w�b�_���������ށB
// �����Fpfile_w �������ރt�@�C�����B
//       pdir �����Ɂu/�v���܂܂Ȃ��p�X�B
//       poffset �ʒu�B
// �Ԓl�F�����Ȃ�true�A����ȊO��false�B
bool writeHeader(FILE *pfile_w, LPCSTR pdir, DWORD *poffset) {
	char dir[256], filepath[256];
	int fh, size;
	bool flag = false;
	struct _finddata_t fd;
	FILE *pfile_r;

	strncpy(dir, pdir, 50);
	strcat(dir, "/*");
	fh = _findfirst(dir, &fd);
	if (fh == -1) return false;

	strncpy(dir, pdir, 50);
	strcat(dir, "/");
	do {
		if (fd.name[0] == '.' || (fd.attrib & _A_SUBDIR) != 0) continue;
		if (strlen(fd.name) > 200) goto procend;

		// �t�@�C����
		strcpy(filepath, dir);
		strcat(filepath, fd.name);
		fprintf(pfile_w, "%s", filepath);
		fputc('\0', pfile_w);

		// �t�@�C���̏ꏊ
		if (fwrite(poffset, 4, 1, pfile_w) != 1) goto procend;

		// �t�@�C���T�C�Y
		pfile_r = fopen(filepath, "rb");
		if (pfile_r == NULL) goto procend;
		size = _filelength(_fileno(pfile_r));
		fclose(pfile_r);
		if (fwrite(&size, 4, 1, pfile_w) != 1) goto procend;

		printf("%s %u %u\n", filepath, *poffset, size);
		*poffset += size;
	} while (_findnext(fh, &fd) == 0);

	flag = true;
procend:
	_findclose(fh);
	return flag;
}

// �f�[�^���������ށB
// �����Fpfile_w �������ރt�@�C�����B
//       pdir �����Ɂu/�v���܂܂Ȃ��p�X�B
// �Ԓl�F�����Ȃ�true�A����ȊO��false�B
bool writeData(FILE *pfile_w, LPCSTR pdir) {
	char dir[256], filepath[256], buf[1024];
	int fh;
	UINT size;
	bool flag = false;
	struct _finddata_t fd;
	FILE *pfile_r;

	strncpy(dir, pdir, 50);
	strcat(dir, "/*");
	fh = _findfirst(dir, &fd);
	if (fh == -1) return false;

	strncpy(dir, pdir, 50);
	strcat(dir, "/");
	do {
		if (fd.name[0] == '.' || (fd.attrib & _A_SUBDIR) != 0) continue;
		if (strlen(fd.name) > 200) goto procend;

		strcpy(filepath, dir);
		strcat(filepath, fd.name);
		pfile_r = fopen(filepath, "rb");
		if (pfile_r == NULL) goto procend;

		for (;;) {
			size = fread(buf, 1, sizeof(buf), pfile_r);
			if (size == 0) break;

			if (fwrite(buf, 1, size, pfile_w) == size) continue;

			fclose(pfile_r);
			goto procend;
		}

		fclose(pfile_r);
	} while (_findnext(fh, &fd) == 0);

	flag = true;
procend:
	_findclose(fh);
	return flag;
}

// MD���v�Z���ďo�́B
// �����Fpfilename �t�@�C�����B
// �Ԓl�F�����Ȃ�true�A����ȊO��false�B
bool calcMD5(LPCSTR pfilename) {
	BYTE buf[4096];
	WORD i;
	FILE *pfile;
	MD5 md5;

	pfile = fopen(pfilename, "rb");
	if (pfile == NULL) return false;

	md5.Init();
	for (;;) {
		i = fread(buf, 1, sizeof(buf), pfile);
		if (i == 0) break;
		md5.Update(buf, i);
	}
	md5.Final(buf);

	fclose(pfile);
	for (i = 0; i < 16; i ++) printf("%02x", buf[i]);
	printf("\n");
	for (i = 0; i < 16; i += 4) {
		if (i > 0) printf(" ");
		printf("0%08xh", *(DWORD *)(buf + i) ^ 0x2db5e0a1);
	}
	return true;
}

void main()
{
	bool flag = false;
	DWORD buf[4], i;
	FILE *pfile1 = fopen(PACKFILE ".raw", "wb"), *pfile2 = NULL;
	CryptBlowfish *pcb;

	if (pfile1 == NULL) goto procend;

	//--- �p�b�N�t�@�C���쐬�B

	// �V�O�j�`�����������ށB
	if (fwrite("WARUNPACK\0", 10, 1, pfile1) != 1) goto procend;

	// �w�b�_�T�C�Y�擾�B
	i = getHeaderSize("img");
	i += getHeaderSize("scr");
	i += getHeaderSize("snd");
	i++;	// �w�b�_�̍Ō��\0

	// �w�b�_���������ށB
	if (!writeHeader(pfile1, "img", &i)) goto procend;
	if (!writeHeader(pfile1, "scr", &i)) goto procend;
	if (!writeHeader(pfile1, "snd", &i)) goto procend;
	fputc('\0', pfile1);

	// �f�[�^���������ށB
	printf("Write data...");
	if (!writeData(pfile1, "img")) goto procend;
	if (!writeData(pfile1, "scr")) goto procend;
	if (!writeData(pfile1, "snd")) goto procend;

	for (i = (8 - i % 8) % 8; i > 0; i--) fputc('\0', pfile1);	// �Í�������f�[�^��8�̔{���ɂȂ�悤�ɂ���B

	fclose(pfile1);
	printf("done\n");


	//--- �Í����B

	printf("Encrypt data...");
	pfile1 = fopen(PACKFILE ".raw", "rb");
	pfile2 = fopen(PACKFILE, "wb");
	if (pfile1 == NULL || pfile2 == NULL) goto procend;

	// �V�O�j�`���͂��̂܂܏������ށB
	if (fread(buf, 10, 1, pfile1) != 1) goto procend;
	if (fwrite(buf, 10, 1, pfile2) != 1) goto procend;

	pcb = CryptBlowfish::getObject();
	for (;;) {
		i = fread(buf, 1, 8, pfile1);
		if (i == 0) break;

		buf[0] ^= pcb->m_vec[0];
		buf[1] ^= pcb->m_vec[1];

		pcb->Encrypt(buf, buf + 1);
		if (fwrite(buf, 8, 1, pfile2) != 1) goto procend;

		pcb->m_vec[0] = buf[0];
		pcb->m_vec[1] = buf[1];
	}

	fclose(pfile1);
	fclose(pfile2);
	pfile1 = pfile2 = NULL;
	printf("done\n");


	//--- MD5�v�Z�B

	printf("MD5(encrypt data) = ");
	if (!calcMD5(PACKFILE)) goto procend;

	printf("\nMD5(raw data) = ");
	if (!calcMD5(PACKFILE ".raw")) goto procend;


	flag = true;
procend:
	if (pfile1 != NULL) fclose(pfile1);
	if (pfile2 != NULL) fclose(pfile2);
	if (flag) printf("\nComplete");
	else printf("\nError");
	getch();
}
