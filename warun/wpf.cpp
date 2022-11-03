/*
WaruNパックファイル作成ツール

●データ構造

[シグニチャ]
WARUNPACK\0：10バイト

[ヘッダ]
\0で終わるファイル名：最大250バイト
ファイルの内容があるパックファイルのシグニチャを除く先頭からの場所：4バイト
ファイルのサイズ：4バイト
※上記がファイルの数だけ並んでいるということになる。
※ファイル名が\0で始まっている場合、ヘッダ終了とし、次のバイトからファイルの内容が格納されている。

●暗号化
シグニチャを除くパックファイル全体をBlowfishを用い、CBCで暗号化する。復号化の際、当然、シグニチャは無視するようにする。
※ファイル毎ではなく、全体で暗号化する。つまり、初期ベクトルはパックファイルのシグニチャを除く先頭の1つだけである。
※Blowfishの特徴により、元データが8で割り切れないなら\0で埋める。暗号化されたデータのサイズは8で割り切れる。
*/
#include <stdio.h>
#include <io.h>
#include <conio.h>
#include "cryptblowfish.cpp"
#include "md5.cpp"
#define PACKFILE "warun.pak"

// ヘッダサイズを取得。
// 引数：pdir 末尾に「/」を含まないパス。
// 返値：ヘッダサイズ。
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

		size += strlen(dir) + strlen(fd.name) + 1 + 4 + 4;	// ファイル名 + \0 + 場所 + サイズ
	} while (_findnext(fh, &fd) == 0);
	_findclose(fh);

	return size;
}

// ヘッダを書き込む。
// 引数：pfile_w 書き込むファイル情報。
//       pdir 末尾に「/」を含まないパス。
//       poffset 位置。
// 返値：成功ならtrue、それ以外はfalse。
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

		// ファイル名
		strcpy(filepath, dir);
		strcat(filepath, fd.name);
		fprintf(pfile_w, "%s", filepath);
		fputc('\0', pfile_w);

		// ファイルの場所
		if (fwrite(poffset, 4, 1, pfile_w) != 1) goto procend;

		// ファイルサイズ
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

// データを書き込む。
// 引数：pfile_w 書き込むファイル情報。
//       pdir 末尾に「/」を含まないパス。
// 返値：成功ならtrue、それ以外はfalse。
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

// MDを計算して出力。
// 引数：pfilename ファイル名。
// 返値：成功ならtrue、それ以外はfalse。
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

	//--- パックファイル作成。

	// シグニチャを書き込む。
	if (fwrite("WARUNPACK\0", 10, 1, pfile1) != 1) goto procend;

	// ヘッダサイズ取得。
	i = getHeaderSize("img");
	i += getHeaderSize("scr");
	i += getHeaderSize("snd");
	i++;	// ヘッダの最後の\0

	// ヘッダを書き込む。
	if (!writeHeader(pfile1, "img", &i)) goto procend;
	if (!writeHeader(pfile1, "scr", &i)) goto procend;
	if (!writeHeader(pfile1, "snd", &i)) goto procend;
	fputc('\0', pfile1);

	// データを書き込む。
	printf("Write data...");
	if (!writeData(pfile1, "img")) goto procend;
	if (!writeData(pfile1, "scr")) goto procend;
	if (!writeData(pfile1, "snd")) goto procend;

	for (i = (8 - i % 8) % 8; i > 0; i--) fputc('\0', pfile1);	// 暗号化するデータが8の倍数になるようにする。

	fclose(pfile1);
	printf("done\n");


	//--- 暗号化。

	printf("Encrypt data...");
	pfile1 = fopen(PACKFILE ".raw", "rb");
	pfile2 = fopen(PACKFILE, "wb");
	if (pfile1 == NULL || pfile2 == NULL) goto procend;

	// シグニチャはそのまま書き込む。
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


	//--- MD5計算。

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
