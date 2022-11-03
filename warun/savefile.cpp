#pragma once
#include <stdio.h>
#include <d3d9.h>
#include "cryptblowfish.cpp"
#include "md5.cpp"
#define SCR_SAVE_FILE "warun.sav"
#define SCR_PLAY_FILE "warun.tmp"
#define SCR_SIGN "WARUNSAVE\0"

// セーブデータクラス。
class SaveFile {
public:
	static const BYTE SAVE_CNT = 16;	// セーブできる数。

	// オブジェクト取得。
	static SaveFile *getObject() {
		static SaveFile sf;
		return &sf;
	}

	// デストラクタ。
	~SaveFile() {
		unlink(SCR_PLAY_FILE);
	}

	// セーブデータを保存。
	// 引数：idx セーブの番号(キャラのゲームクリア状態だけを変更したいなら負の値を指定する)。
	// 返値：成功ならtrue、それ以外はfalse。
	bool Save(short idx) {
		BYTE data[16], *pdata;
		FILE *pfile;
		SYSTEMTIME st;
		SaveData sd;
		MD5 md5;

		pfile = fopen(SCR_SAVE_FILE ".tmp", "wb");
		if (pfile == NULL) return false;

		// 暗号化データ領域を確保。
		pdata = new BYTE[sizeof(m_ss)];
		if (pdata == NULL) return false;

		if (idx >= 0 && idx < SAVE_CNT) {
			// 現在時刻を取得して設定。
			GetLocalTime(&st);
			wsprintfW(m_ssp.sd.date, L"%d-%02d-%02d %02d:%02d:%02d"
				, st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

			sd = m_ss.sd[idx];
			m_ss.sd[idx] = m_ssp.sd;
		}
		CopyMemory(pdata, &m_ss, sizeof(m_ss));
		Encrypt(pdata, sizeof(m_ss));

		// 元データのMD5を計算。
		md5.Init();
		md5.Update((BYTE *)&m_ss, sizeof(m_ss));
		md5.Final(data);

		// シグニチャと暗号化したデータと元データのMD5を書き込む。
		if (fwrite(SCR_SIGN, 10, 1, pfile) != 1 || fwrite(pdata, sizeof(m_ss), 1, pfile) != 1
			|| fwrite(data, sizeof(data), 1, pfile) != 1) {

			delete[] pdata;
			fclose(pfile);
			unlink(SCR_SAVE_FILE ".tmp");
			if (idx >= 0 && idx < SAVE_CNT) m_ss.sd[idx] = sd;
			return false;
		}

		delete[] pdata;
		fclose(pfile);
		unlink(SCR_SAVE_FILE);
		if (rename(SCR_SAVE_FILE ".tmp", SCR_SAVE_FILE) != 0) {
			unlink(SCR_SAVE_FILE ".tmp");
			if (idx >= 0 && idx < SAVE_CNT) m_ss.sd[idx] = sd;
			return false;
		}
		return true;
	}

	// プレイ中のデータを保存/削除。
	// 引数：del_flag falseなら保存、trueなら削除。
	void savePlaying(bool del_flag) {
		BYTE data[16], *pdata;
		FILE *pfile;
		MD5 md5;

		if (del_flag) {
			unlink(SCR_PLAY_FILE);
			return;
		}

		pfile = fopen(SCR_PLAY_FILE, "wb");
		if (pfile == NULL) return;

		// 暗号化データ領域を確保して設定し、暗号化する。
		pdata = new BYTE[sizeof(m_ssp)];
		if (pdata == NULL) return;
		CopyMemory(pdata, &m_ssp, sizeof(m_ssp));
		Encrypt(pdata, sizeof(m_ssp));

		// 元データのMD5を計算。
		md5.Init();
		md5.Update((BYTE *)&m_ssp, sizeof(m_ssp));
		md5.Final(data);

		fwrite(SCR_SIGN, 10, 1, pfile);	// シグニチャを書き込む。
		fwrite(pdata, sizeof(m_ssp), 1, pfile);	// 暗号化したデータを書き込む。
		fwrite(data, sizeof(data), 1, pfile);	// 元データのMD5を書き込む。
		delete[] pdata;
		fclose(pfile);
	}

protected:
	struct SaveData {	// スクリプトのセーブデータのクラス。
		BYTE chara;	// 主人公種別。
		UINT pos;	// スクリプトの何番目のWAITとSELECTか(1以上ならセーブデータとして有効とする)。
		WCHAR bg_name[32];	// 背景名。
		WCHAR day[16];	// 日。
		WCHAR date[32];	// 日付。

		D3DCOLOR bg_color;	// 背景色。
		char bg_filename[MAX_PATH];	// 背景画像。

		char chara_filename1[MAX_PATH], chara_filename2[MAX_PATH];	// キャラ。
		short chara_pos1, chara_pos2;	// キャラの表示X位置。

		D3DCOLOR color;	// キャラより上の背景色。

		WCHAR text[512];	// 文章。
		WCHAR name[32];	// 文章の名前。
		BYTE bracket;	// 括弧種別。

		char bgm_filename[MAX_PATH], bgm_intro_filename[MAX_PATH];	// BGM。

		char scr_var;	// スクリプト用変数。
	};

	struct ScriptSave {	// セーブデータのクラス(サイズは8の倍数)。
		BYTE chara[5];	// キャラのゲームクリア状態。
		SaveData sd[SAVE_CNT];	// スクリプトのセーブデータ。
	};

	struct ScriptSavePlaying {	// プレイ中のセーブデータのクラス(サイズは8の倍数)。
		SaveData sd;
		int dmy;	// 8の倍数にするためのダミー変数。
	};

	// コンストラクタ。
	SaveFile() {
		Load(SCR_SAVE_FILE, &m_ss, sizeof(m_ss));	// セーブデータを読み込む。
		Load(SCR_PLAY_FILE, &m_ssp, sizeof(m_ssp));	// プレイ中だったデータを読み込む。
	}

	// セーブデータを読み込む。読み込めなかったら初期化する。
	// 引数：pfilename ファイル名。
	//       pdata ScriptSave型かScriptSavePlaying型のデータ。
	//       len データ長。
	void Load(LPCSTR pfilename, void *pdata, UINT len) {
		bool flag = false;
		BYTE data1[16], data2[16];
		DWORD vec1_0, vec1_1, vec2_0, vec2_1, *pbuf, *plen;
		FILE *pfile;
		CryptBlowfish *pcb;
		MD5 md5;

		pfile = fopen(pfilename, "rb");
		if (pfile == NULL) goto procend;

		// シグニチャ確認。
		if (fread(data1, 10, 1, pfile) != 1) goto procend;
		if (memcmp(data1, SCR_SIGN, 10) != 0) goto procend;

		if (fread(pdata, len, 1, pfile) != 1) goto procend;
		if (fread(data1, sizeof(data1), 1, pfile) != 1) goto procend;

		// 復号化。
		pcb = CryptBlowfish::getObject();
		pbuf = (DWORD *)pdata;
		plen = (DWORD *)((char *)pdata + len);
		vec1_0 = pcb->m_vec[0];
		vec1_1 = pcb->m_vec[1];
		for (; pbuf < plen; pbuf += 2) {
			vec2_0 = *pbuf;
			vec2_1 = pbuf[1];

			pcb->Decrypt(pbuf, pbuf + 1);
			*pbuf ^= vec1_0;
			pbuf[1] ^= vec1_1;

			vec1_0 = vec2_0;
			vec1_1 = vec2_1;
		}

		// 復号化したデータのMD5を計算して確認。
		md5.Init();
		md5.Update((BYTE *)pdata, len);
		md5.Final(data2);
		if (memcmp(data1, data2, sizeof(data1)) != 0) goto procend;

		flag = true;
procend:
		if (pfile != NULL) fclose(pfile);
		if (!flag) ZeroMemory(pdata, len);
	}

	// 暗号化。
	// 引数：pdata データ。
	//       len データ長。
	void Encrypt(void *pdata, UINT len) {
		DWORD vec1, vec2, *pbuf, *plen;
		CryptBlowfish *pcb = CryptBlowfish::getObject();

		pbuf = (DWORD *)pdata;
		plen = (DWORD *)((char *)pdata + len);

		vec1 = pcb->m_vec[0];
		vec2 = pcb->m_vec[1];
		for (; pbuf < plen; pbuf += 2) {
			*pbuf ^= vec1;
			pbuf[1] ^= vec2;

			pcb->Encrypt(pbuf, pbuf + 1);

			vec1 = *pbuf;
			vec2 = pbuf[1];
		}
	}

public:
	ScriptSave m_ss;
	ScriptSavePlaying m_ssp;
};
