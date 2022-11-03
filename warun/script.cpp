#pragma once
#include "spritemanager.cpp"
#include "savefile.cpp"
#define BLANK_CHAR " \t"
#define CMPOP_CHAR "=!<>"

// スクリプトクラス。
class Script {
public:
	FILE *m_ppackfile;
	bool m_packfile_valid;

	// オブジェクト取得。
	// 引数：hwnd ウィンドウハンドル。
	//       pspr_mng スプライト管理オブジェクト。
	//       psnd_mng 音声管理オブジェクト。
	static Script *getObject(HWND hwnd, SpriteManager *pspr_mng, SoundManager *psnd_mng) {
		static Script script(hwnd, pspr_mng, psnd_mng);
		return &script;
	}

	// デストラクタ。
	~Script() {
		if (m_thread != NULL) {
			WaitForSingleObject(m_thread, INFINITE);
			CloseHandle(m_thread);
		}
		if (m_pfile != NULL) delete m_pfile;
		if (m_ppackfile != NULL) fclose(m_ppackfile);
	}

	// イントロ画面設定。
	// 返値：成功ならtrue、それ以外はfalse。
	//   スプライト番号使用表
	//    0:背景
	//    1:ロゴ
	//   48:前景
	bool setIntro() {
#if PACKFILE_TYPE < 2
		if (!m_packfile_valid) {
			if (m_pfile == NULL || m_ppackfile == NULL) return false;
			if (_filelength(_fileno(m_ppackfile)) != 78685698) return false;
			m_thread = CreateThread(NULL, 0, procCheckPackfile, this, 0, NULL);
		}
#else
		m_packfile_valid = true;
#endif

		if (m_psf->m_ssp.sd.pos > 0) {
			// その時点でプレイ中のデータが設定されているということは、以前不正終了によりゲームが中断され、
			// その時プレイ中だったデータを読み込み済みとみなし、すぐにゲームロード処理をする。
			m_scene = 41;
			m_frame = 100;
			m_pspr_mng->createSpriteColor(48, D3DCOLOR_XRGB(0, 0, 0));
			m_pspr_mng->addFadeInAction(48, 0);
			setGame();
		} else {
			m_scene = 1;
			m_frame = 0;

			m_pspr_mng->createSpriteColor(0, D3DCOLOR_XRGB(255, 255, 255));
			m_pspr_mng->createSpriteImg(1, "img/intro.bmp", 300, -50);
			m_pspr_mng->addFadeInAction(0, 20);
			m_pspr_mng->addFadeInAction(1, 0);
			m_pspr_mng->addPauseAction(1, 50);
			m_pspr_mng->addMoveYAction(1, -50, 275, 2);
		}
		return true;
	}

	// 終了設定。
	// 引数：close_flag 特定の画面ならtrueにすると終了する。それ以外の画面なら無視される。
	//   スプライト番号使用表
	//   44: 背景
	//   45: 確認メッセージ
	//   46: はい
	//   47: いいえ
	//   49: 前景
	void Close(bool close_flag) {
		BYTE i;

		if ((m_scene == 4 || m_scene == 5 || m_scene == 6) && !close_flag) {
			if (m_close > -2) return;
			m_close = -1;
			m_skip_flag = false;

			if (m_scene == 6) {
				for (i = 17; i <= 20; i++) m_pspr_mng->setEnable(i, false);
				m_pspr_mng->createSpriteString(21, 0, 0, 800, 600, 1, 0, 0, 99);
				m_pspr_mng->addFadeInAction(21, 0);
			} else {
				for (i = 23; i <= 40; i++) m_pspr_mng->setEnable(i, false);
			}
			m_pspr_mng->mouseOut(-1, false, false);

			if (m_confirm_flag) {
				m_confirm_flag = false;
			} else {
				m_pspr_mng->createSpriteColor(44, D3DCOLOR_ARGB(224, 8, 8, 16));
				m_pspr_mng->addFadeInAction(44, 10);
			}

			m_pspr_mng->createSpriteString(45, 320, 190, 200, 50, 25, D3DCOLOR_XRGB(192, 192, 255));
			m_pspr_mng->createSpriteString(46, 308, 240, 50, 27, 25, D3DCOLOR_XRGB(255, 255, 255), D3DCOLOR_XRGB(255, 255, 64), 22);
			m_pspr_mng->createSpriteString(47, 430, 240, 75, 27, 25, D3DCOLOR_XRGB(255, 255, 255), D3DCOLOR_XRGB(255, 255, 64), 20);
			m_pspr_mng->writeString(45, L"終了しますか？");
			m_pspr_mng->writeString(46, L"はい");
			m_pspr_mng->writeString(47, L"いいえ");
			m_pspr_mng->addFadeInAction(45, 10);
			m_pspr_mng->addFadeInAction(46, 10);
			m_pspr_mng->addFadeInAction(47, 10);

			if (m_scene == 6) m_pspr_mng->stopAction(6);
		} else {
			if (m_close >= 0) return;
			m_close = 0;
			m_packfile_valid = true;

			for (i = 1; i <= 47; i++) m_pspr_mng->setEnable(i, false);
			m_pspr_mng->createSpriteColor(49, D3DCOLOR_XRGB(0, 0, 0));
			m_pspr_mng->addFadeInAction(49, 20);
			m_psnd_mng->Close(20, true);
		}
	}

	// 毎フレーム実行される処理。
	void Process() {
		BYTE i;

		switch (m_scene) {
		case 1:	// イントロ画面。
			switch (m_frame) {
			case 210:
				m_psnd_mng->Play(8, "snd/intro.wav");
				break;
			case 400:
				m_pspr_mng->addFadeOutAction(0, 30);
				m_pspr_mng->addFadeOutAction(1, 30);
				m_pspr_mng->addPauseAction(0, 50);
				break;
			case 480:
				if (m_thread != NULL) {
					WaitForSingleObject(m_thread, INFINITE);
					CloseHandle(m_thread);
					m_thread = NULL;
				}
				if (!m_packfile_valid) {
					MessageBox(m_hwnd, TEXT("エラーが発生しました。"), TEXT("warun"), MB_OK | MB_ICONEXCLAMATION);
					DestroyWindow(m_hwnd);
					return;
				}

				setMenu(false);
				break;
			}
			break;
		case 6:	// ゲーム画面。
			if (m_frame == m_scr_frame && m_scr_frame > 0) parseScript();
			break;
		case 7:	// エンディング画面。
			if (m_frame == 200) {
				setEnding(1);

			} else if (m_frame == 2300 && m_ending_type == 0) {
				m_scr_frame = 0;
				m_pspr_mng->createSpriteColor(2, D3DCOLOR_XRGB(255, 255, 255));
				m_pspr_mng->addFadeInAction(2, 150);
				m_pspr_mng->addPauseAction(3, 150);
				m_pspr_mng->addFadeInAction(3, 150);
			} else if (m_frame == 2600 && m_ending_type == 0) {
				m_pspr_mng->createSpriteString(4, 50, 100, 400, 60, 30, D3DCOLOR_XRGB(0, 0, 0));
				m_pspr_mng->writeString(4, L"[XXXXX]\n  XXXXX");
				m_pspr_mng->addFadeInAction(4, 60);
				m_pspr_mng->addPauseAction(4, 346);
				m_pspr_mng->addFadeOutAction(4, 60);

				m_pspr_mng->createSpriteString(5, 400, 300, 350, 60, 30, D3DCOLOR_XRGB(0, 0, 0));
				m_pspr_mng->writeString(5, L"[XX]\n  XXXXX");
				m_pspr_mng->addPauseAction(5, 20);
				m_pspr_mng->addFadeInAction(5, 60);
				m_pspr_mng->addPauseAction(5, 346);
				m_pspr_mng->addFadeOutAction(5, 60);

				m_pspr_mng->createSpriteString(6, 70, 440, 300, 60, 30, D3DCOLOR_XRGB(0, 0, 0));
				m_pspr_mng->writeString(6, L"[XX]\n  XXXXX");
				m_pspr_mng->addPauseAction(6, 40);
				m_pspr_mng->addFadeInAction(6, 60);
				m_pspr_mng->addPauseAction(6, 346);
				m_pspr_mng->addFadeOutAction(6, 60);
			} else if (m_frame == 3126 && m_ending_type == 0) {
				m_pspr_mng->createSpriteString(4, 400, 100, 200, 60, 30, D3DCOLOR_XRGB(0, 0, 0));
				m_pspr_mng->writeString(4, L"[XXXXX]\n  XXXXX");
				m_pspr_mng->addPauseAction(4, 40);
				m_pspr_mng->addFadeInAction(4, 60);
				m_pspr_mng->addPauseAction(4, 306);
				m_pspr_mng->addFadeOutAction(4, 60);

				m_pspr_mng->createSpriteString(5, 50, 300, 100, 60, 30, D3DCOLOR_XRGB(0, 0, 0));
				m_pspr_mng->writeString(5, L"[XX]\n  XXX");
				m_pspr_mng->addPauseAction(5, 20);
				m_pspr_mng->addFadeInAction(5, 60);
				m_pspr_mng->addPauseAction(5, 346);
				m_pspr_mng->addFadeOutAction(5, 60);

				m_pspr_mng->createSpriteString(6, 380, 440, 100, 60, 30, D3DCOLOR_XRGB(0, 0, 0));
				m_pspr_mng->writeString(6, L"[XX]\n  XXX");
				m_pspr_mng->addFadeInAction(6, 60);
				m_pspr_mng->addPauseAction(6, 386);
				m_pspr_mng->addFadeOutAction(6, 60);
			} else if (m_frame == 3702 && m_ending_type == 0) {
				m_pspr_mng->createSpriteString(4, 310, 250, 200, 100, 50, D3DCOLOR_XRGB(0, 0, 0));
				m_pspr_mng->writeString(4, L"[XXXXX]\n  XXXXX");
				m_pspr_mng->addFadeInAction(4, 110);
				m_pspr_mng->addPauseAction(4, 296);
				m_pspr_mng->addFadeOutAction(4, 30);
			} else if (m_frame == 4138 && m_ending_type == 0) {
				m_pspr_mng->createSpriteString(4, 170, 225, 500, 50, 50, D3DCOLOR_XRGB(0, 0, 0));
				m_pspr_mng->writeString(4, L"Copyright 2009, kozok");
				m_pspr_mng->addFadeInAction(4, 20);

				m_pspr_mng->createSpriteString(5, 270, 325, 350, 30, 30, D3DCOLOR_XRGB(0, 0, 0));
				m_pspr_mng->writeString(5, L"Thank you for playing!!");
				m_pspr_mng->addFadeInAction(5, 20);

				m_pspr_mng->createSpriteColor(6, D3DCOLOR_XRGB(0, 0, 0));
				m_pspr_mng->addPauseAction(6, 500);
				m_pspr_mng->addFadeInAction(6, 60);

			} else if (m_frame == 1520 && m_ending_type == 1) {
				m_scr_frame = 0;
				m_pspr_mng->createSpriteColor(6, D3DCOLOR_XRGB(0, 0, 0));
				m_pspr_mng->addFadeInAction(6, 250);
			} else if (m_frame == 925 && m_ending_type == 2) {
				m_scr_frame = 0;
				m_pspr_mng->createSpriteString(0, 170, 250, 500, 50, 50, D3DCOLOR_XRGB(255, 255, 255));
				m_pspr_mng->createSpriteColor(1, D3DCOLOR_XRGB(255, 255, 255));
				m_pspr_mng->writeString(0, L"Copyright 2009, kozok");
				m_pspr_mng->addFadeInAction(0, 0);
				m_pspr_mng->addFadeInAction(1, 0);
				m_pspr_mng->addFadeOutAction(1, 5);
			} else if (m_frame == 941 && m_ending_type == 2) {
				m_pspr_mng->createSpriteString(0, 180, 220, 450, 100, 100, D3DCOLOR_XRGB(255, 255, 255));
				m_pspr_mng->writeString(0, L"Try again!!");
				m_pspr_mng->addFadeInAction(0, 0);
				m_pspr_mng->addPauseAction(0, 250);
				m_pspr_mng->addFadeOutAction(0, 30);
				m_pspr_mng->addFadeInAction(1, 0);
				m_pspr_mng->addFadeOutAction(1, 5);

			} else if (m_frame == 4800 && m_ending_type == 0 || m_frame == 1900 && m_ending_type == 1) {
				for (i = 2; i <= 6; i++) m_pspr_mng->deleteSprite(i);
				setIntro();
			} else if (m_frame == 1300 && m_ending_type == 2 || m_frame == 5700 && m_ending_type == 3) {
				setIntro();
			} else if (m_frame == m_scr_frame && m_scr_frame > 0) {
				setEnding(2);
			}
			break;
		case 20:	// メニュー画面の「New Game」押下時。
			if (m_frame == 30) setPlaySel();
			break;
		case 21:	// メニュー画面の「Load Game」押下時。
			if (m_frame == 30) setLoadGame();
			break;
		case 30:	// 主人公選択画面のキャラ押下時。
			if (m_frame == 90) setGame();
			break;
		case 31:	// 主人公選択画面の「戻る」押下時。
			if (m_frame != 30) break;
			for (i = 4; i <= 7; i++) m_pspr_mng->deleteSprite(i);
			setMenu(true);
			break;
		case 40:	// ゲームロード画面かゲームセーブ画面の「戻る」押下時。
			switch (m_frame) {
			case 30:
				for (i = 21; i <= 41; i++) m_pspr_mng->deleteSprite(i);
				m_pspr_mng->addFadeOutAction(48, 30);
				break;
			case 60:
				for (i = 1; i <= 12; i++) m_pspr_mng->setEnable(i, true);
				m_pspr_mng->setEnable(13, m_wait);
				for (i = 14; i <= 20; i++) m_pspr_mng->setEnable(i, true);

				m_scene = m_scene_before;
				break;
			}
			break;
		case 41:	// ゲームロード画面の「はい」押下時。
			switch (m_frame) {
			case 50:
				for (i = 21; i <= 47; i++) m_pspr_mng->deleteSprite(i);
				m_psf->m_ssp.sd = m_psf->m_ss.sd[m_id];
				setGame();
				break;
			case 100:
				m_pspr_mng->addFadeOutAction(48, 30);
				break;
			case 130:
				m_scene = 6;
				break;
			}
			break;
		case 50:	// ゲームセーブ画面の「はい」押下時。
			if (m_frame != 30) break;
			for (i = 44; i <= 48; i++) m_pspr_mng->deleteSprite(i);
			if (m_psf->Save(m_id)) m_psnd_mng->Play(7, "snd/push.wav");
			setSaveGame();
			break;
		case 60:	// ゲーム画面の「Load」押下時。
			if (m_frame == 50) setLoadGame();
			break;
		case 61:	// ゲーム画面の「Save」押下時。
			if (m_frame == 50) setSaveGame();
			break;
		case 62:	// ゲーム画面の「はい」(メニュー)押下時。
			if (m_frame != 60) break;
			for (i = 4; i <= 48; i++) m_pspr_mng->deleteSprite(i);
			m_psnd_mng->Close(0, false);
			setMenu(false);
			break;
		}

		if (m_close >= 0) {
			m_close++;
			if (m_close >= 30) {
				DestroyWindow(m_hwnd);
				return;
			}
		}

		if (!m_pspr_mng->Process() || !m_psnd_mng->Process()) {
			MessageBox(m_hwnd, TEXT("エラーが発生しました。"), TEXT("warun"), MB_OK | MB_ICONEXCLAMATION);
			DestroyWindow(m_hwnd);
			return;
		}
		if (m_frame < 0xffffffff) m_frame++;
	}

	// マウスイベントに応じた処理。
	// 引数：type マウスイベントタイプ。
	//       pos_x X位置。
	//       pos_y Y位置。
	//       noprepare_flag trueにするとウィンドウサイズによる座標の調整をしない。
	void eventMouse(BYTE type, WORD pos_x, WORD pos_y, bool noprepare_flag) {
		BYTE id = m_pspr_mng->eventMouse(type, pos_x, pos_y, noprepare_flag);
		if (id > 0) onClick(id);
	}

	// キーボードを押下した時の処理。
	// 引数：key 押下したキー。
	void onKeyDown(BYTE key) {
		if (m_scene != 6) return;

		switch (key) {
		case VK_RETURN:
		case VK_RIGHT:
			eventMouse(1, 100, 455, true);	// パネル(文章)。
			break;
		case 49:
			eventMouse(1, 225 + 10, 70 + 70, true);	// 選択肢(1)。
			break;
		case 50:
			eventMouse(1, 225 + 10, 70 + 120, true);	// 選択肢(2)。
			break;
		case 51:
			eventMouse(1, 225 + 10, 70 + 170, true);	// 選択肢(3)。
			break;
		case 52:
			eventMouse(1, 225 + 10, 70 + 220, true);	// 選択肢(4)。
			break;
		}
	}

	// パックファイルが正しいかチェック。
	static DWORD WINAPI procCheckPackfile(LPVOID lpParameter) {
		BYTE data[16], buf[4096];
		UINT md[4], r;
		Script *pscript = (Script *)lpParameter;
		MD5 md5;

		md5.Init();
		for (;;) {
			r = fread(buf, 1, sizeof(buf), pscript->m_ppackfile);
			if (r == 0 || pscript->m_packfile_valid) break;
			md5.Update(buf, r);
		}
		md5.Final(data);

#if PACKFILE_TYPE == 0
		_asm {
			mov md, 048ac30beh
			xor md, 02db5e0a1h
			mov [md + 4], 09ad168a5h
			xor [md + 4], 02db5e0a1h
			mov [md + 8], 0518373c6h
			xor [md + 8], 02db5e0a1h
			mov [md + 12], 04aebbee1h
			xor [md + 12], 02db5e0a1h
		}
#else
		_asm {
			mov md, 0e29d7b5eh
			xor md, 02db5e0a1h
			mov [md + 4], 03e345502h
			xor [md + 4], 02db5e0a1h
			mov [md + 8], 00e4e0cffh
			xor [md + 8], 02db5e0a1h
			mov [md + 12], 053d288dch
			xor [md + 12], 02db5e0a1h
		}
#endif
		if (memcmp(md, data, 16) == 0) pscript->m_packfile_valid = true;
		return 0;
	}

protected:
	HWND m_hwnd;
	SpriteManager *m_pspr_mng;
	SoundManager *m_psnd_mng;

	UINT m_frame, m_scr_frame;
	BYTE m_scene, m_scene_before;
	int m_close;	// 終了設定用フレームカウント。
	PackFile *m_pfile;

	bool m_wait;	// 次の文章ユーザ入力待ち用。
	char m_sel_goto[4][32];	// 選択肢ユーザ入力待ち用。
	char m_goto[32];	// ラベルへの移動用。
	char m_scr_var;	// スクリプト用変数。
	bool m_skip_flag;	// スキップ状態用フラグ。
	bool m_bg, m_confirm_flag;

	BYTE m_id, m_ending_type;
	struct ScrInfo {
		char *pfilename;
		WCHAR *pname;
	} m_scr_info[5];
	SaveFile *m_psf;
	HANDLE m_thread;

	// コンストラクタ。
	// 引数：hwnd ウィンドウハンドル。
	//       pspr_mng スプライト管理オブジェクト。
	//       psnd_mng 音声管理オブジェクト。
	Script(HWND hwnd, SpriteManager *pspr_mng, SoundManager *psnd_mng) {
		m_hwnd = hwnd;
		m_pspr_mng = pspr_mng;
		m_psnd_mng = psnd_mng;

		m_frame = m_scr_frame = 0;
		m_scene = m_scene_before = 0;
		m_close = -2;
		m_ppackfile = fopen(PACKFILE, "rb+");	// 他のアプリケーションが書き込み等の操作をできないようにする。
		m_pfile = new PackFile(1024);

		m_wait = false;
		ZeroMemory(m_sel_goto, sizeof(m_sel_goto));
		ZeroMemory(m_goto, sizeof(m_goto));
		m_scr_var = 0;
		m_skip_flag = false;
		m_bg = m_confirm_flag = false;

		m_id = m_ending_type = 0;
		m_scr_info[0].pfilename = "scr/waru.txt";
		m_scr_info[0].pname = L"Waru";
		m_scr_info[1].pfilename = "scr/iita.txt";
		m_scr_info[1].pname = L"Iita";
		m_scr_info[2].pfilename = "scr/shizumi.txt";
		m_scr_info[2].pname = L"Shizumi";
		m_scr_info[3].pfilename = "scr/al.txt";
		m_scr_info[3].pname = L"Al";
		m_scr_info[4].pfilename = "scr/kd.txt";
		m_scr_info[4].pname = L"KD";
		m_psf = SaveFile::getObject();
		m_packfile_valid = false;
		m_thread = NULL;
	}

	// メニュー画面設定。
	// 引数：nobgm_flag 既にBGMを再生中ならtrueにする。
	//   スプライト番号使用表
	//   0: 背景
	//   1: ボタン(New Game)
	//   2: ボタン(Load Game)
	//   3: ボタン(Exit)
	void setMenu(bool nobgm_flag) {
		m_scene = 2;
		m_frame = 0;
		m_confirm_flag = false;
		m_psf->savePlaying(true);

		if (!nobgm_flag) m_psnd_mng->Play(8, "snd/bgm_01.wav", true, 0, "snd/bgm_01i.wav");

		m_pspr_mng->createSpriteImg(0, "img/menu.bmp");
		m_pspr_mng->createSpriteImgButton(1, "img/menu_btn_new1.bmp", "img/menu_btn_new2.bmp"
			, "img/menu_btn_new3.bmp", 325, 410, 1, m_psnd_mng, NULL, "snd/push.wav");
		m_pspr_mng->createSpriteImgButton(2, "img/menu_btn_load1.bmp", "img/menu_btn_load2.bmp"
			, "img/menu_btn_load3.bmp", 325, 410 + 40, 2, m_psnd_mng, NULL, "snd/push.wav");
		m_pspr_mng->createSpriteImgButton(3, "img/menu_btn_exit1.bmp", "img/menu_btn_exit2.bmp"
			, "img/menu_btn_exit3.bmp", 325, 410 + 80, 3, m_psnd_mng, NULL, "snd/push.wav");

		m_pspr_mng->addFadeInAction(0, 20);
		m_pspr_mng->addFadeInAction(1, 20);
		m_pspr_mng->addFadeInAction(2, 20);
		m_pspr_mng->addFadeInAction(3, 20);
	}

	// 主人公選択画面設定。
	//   スプライト番号使用表
	//   0: 背景
	//   1: キャラ(Waru)
	//   2: キャラ(Iita)
	//   3: キャラ(Shizumi)
	//   4: キャラ(Al)
	//   5: キャラ(KD)
	//   6: 戻る
	//   7: 前景
	void setPlaySel() {
		m_scene = 3;

		m_pspr_mng->createSpriteImg(0, "img/playsel.bmp");
		m_pspr_mng->addFadeInAction(0, 30);
		m_pspr_mng->createSpriteImgButton(1, m_psf->m_ss.chara[0] == 1 ? "img/playsel_w2.bmp" : "img/playsel_w1.bmp"
			, "img/playsel_w3.bmp", NULL, 358, 283, 1, m_psnd_mng, "snd/sel.wav", "snd/push.wav");
		m_pspr_mng->addPauseAction(1, 25);
		m_pspr_mng->addFadeInAction(1, 50);
		m_pspr_mng->createSpriteImgButton(2, m_psf->m_ss.chara[1] == 1 ? "img/playsel_i2.bmp" : "img/playsel_i1.bmp"
			, "img/playsel_i3.bmp", NULL, 170, 283, 2, m_psnd_mng, "snd/sel.wav", "snd/push.wav");
		m_pspr_mng->addPauseAction(2, 30);
		m_pspr_mng->addFadeInAction(2, 50);

		if (m_psf->m_ss.chara[0] == 1 && m_psf->m_ss.chara[1] == 1) {
			m_pspr_mng->createSpriteImgButton(3, m_psf->m_ss.chara[2] == 1 ? "img/playsel_s2.bmp" : "img/playsel_s1.bmp"
				, "img/playsel_s3.bmp", NULL, 545, 283, 3, m_psnd_mng, "snd/sel.wav", "snd/push.wav");
			m_pspr_mng->addPauseAction(3, 40);
			m_pspr_mng->addFadeInAction(3, 50);
			m_pspr_mng->createSpriteImgButton(4, m_psf->m_ss.chara[3] == 1 ? "img/playsel_a2.bmp" : "img/playsel_a1.bmp"
				, "img/playsel_a3.bmp", NULL, 358, 96, 4, m_psnd_mng, "snd/sel.wav", "snd/push.wav");
			m_pspr_mng->addPauseAction(4, 35);
			m_pspr_mng->addFadeInAction(4, 50);
			m_pspr_mng->createSpriteImgButton(5, m_psf->m_ss.chara[4] == 1 ? "img/playsel_k2.bmp" : "img/playsel_k1.bmp"
				, "img/playsel_k3.bmp", NULL, 358, 471, 5, m_psnd_mng, "snd/sel.wav", "snd/push.wav");
			m_pspr_mng->addPauseAction(5, 45);
			m_pspr_mng->addFadeInAction(5, 50);
		} else {
			m_pspr_mng->deleteSprite(3);
		}

		m_pspr_mng->createSpriteString(6, 20, 560, 47, 27, 25, D3DCOLOR_XRGB(255, 255, 255), D3DCOLOR_XRGB(255, 255, 64), 6);
		m_pspr_mng->writeString(6, L"戻る");
		m_pspr_mng->addPauseAction(6, 20);
		m_pspr_mng->addFadeInAction(6, 10);
	}

	// ゲームロード画面設定。
	void setLoadGame() {
		m_scene_before = m_scene == 21 ? 2 : 6;
		m_scene = 4;

		setLoadSave(false);
		m_pspr_mng->createSpriteString(41, 350, 0, 90, 35, 30, D3DCOLOR_XRGB(255, 255, 255));
		m_pspr_mng->writeString(41, L"ロード");
		m_pspr_mng->addFadeInAction(41, 30);
	}

	// ゲームセーブ画面設定。
	void setSaveGame() {
		m_scene_before = 6;
		m_scene = 5;

		setLoadSave(true);
		m_pspr_mng->createSpriteString(41, 350, 0, 90, 35, 30, D3DCOLOR_XRGB(255, 255, 255));
		m_pspr_mng->writeString(41, L"セーブ");
		m_pspr_mng->addFadeInAction(41, 30);
	}

	// ゲームロード画面とゲームセーブ画面の共通設定。
	// 引数：save_flag ゲームセーブ画面ならtrue、それ以外はfalseを指定すること。
	//   スプライト番号使用表
	//   22: 背景
	//   23～39: 保存されたゲームの文章
	//   40: 戻る
	//   41: タイトル
	//   44: 背景
	//   45: 確認メッセージ
	//   46: はい
	//   47: いいえ
	//   48: 前景
	void setLoadSave(bool save_flag) {
		BYTE i;

		m_pspr_mng->createSpriteImg(22, "img/load.bmp");
		m_pspr_mng->addFadeInAction(22, 30);

		for (i = 0; i < SaveFile::SAVE_CNT; i++) {
			m_pspr_mng->createSpriteString(
				i + 23
				, 40 + (i / 8) * 400
				, 50 + (i % 8) * 67
				, 300, 35
				, 18
				, D3DCOLOR_XRGB(0, 0, 0)
				, !save_flag && m_psf->m_ss.sd[i].pos == 0 ? 0 : D3DCOLOR_XRGB(0, 128, 0)
				, !save_flag && m_psf->m_ss.sd[i].pos == 0 ? 0 : i + 1
			);

			if (m_psf->m_ss.sd[i].pos == 0) {
				m_pspr_mng->writeString(i + 23, L"－－－－－－－－－－", NULL, 0, false, 0, 17);
			} else {
				m_pspr_mng->writeString(i + 23, m_scr_info[m_psf->m_ss.sd[i].chara].pname);
				m_pspr_mng->writeString(i + 23, m_psf->m_ss.sd[i].bg_name[0] == L'\0' ? L"－" : m_psf->m_ss.sd[i].bg_name, NULL, 0, true, 100, 0);
				m_pspr_mng->writeString(i + 23, m_psf->m_ss.sd[i].day[0] == L'\0' ? L"－" : m_psf->m_ss.sd[i].day, NULL, 0, true, 0, 17);
				m_pspr_mng->writeString(i + 23, m_psf->m_ss.sd[i].date, NULL, 0, true, 100, 17);
			}

			m_pspr_mng->addFadeInAction(i + 23, 30);
		}

		m_pspr_mng->createSpriteString(40, 35, 571, 47, 27, 25, D3DCOLOR_XRGB(255, 255, 255), D3DCOLOR_XRGB(255, 255, 64), 17);
		m_pspr_mng->writeString(40, L"戻る");
		m_pspr_mng->addFadeInAction(40, 30);
	}

	// ゲーム画面設定。
	//   スプライト番号使用表
	//    0: 背景
	//    1: 背景
	//    2: キャラ
	//    3: キャラ
	//    4: 背景
	//    5: パネル(半透明)
	//    6: パネル(クリック可能目印)
	//    7: パネル
	//    8: パネル(文章)
	//    9: パネル(背景名)
	//   10: パネル(日付)
	//   11: パネル(Load)
	//   12: パネル(Save)
	//   13: パネル(Skip)
	//   14: パネル(Menu)
	//   15: 選択肢(半透明)
	//   16: 選択肢(文章)
	//   17: 選択肢(1)
	//   18: 選択肢(2)
	//   19: 選択肢(3)
	//   20: 選択肢(4)
	//   21: 前景
	//   44: 背景
	//   45: 確認メッセージ
	//   46: はい
	//   47: いいえ
	//   48: 前景
	void setGame() {
		char buf[1024], *pbuf, *plen;
		BYTE i;
		UINT pos;

		m_scr_frame = 0;

		for (i = 0; i <= 4; i++) m_pspr_mng->deleteSprite(i);

		m_pspr_mng->createSpriteColor(5, D3DCOLOR_ARGB(128, 0, 0, 128), 100, 455, 600, 140);
		m_pspr_mng->createSpriteImg(6, "img/wait.bmp", 685, 585);
		m_pspr_mng->createSpriteImg(7, "img/panel.bmp", 0, 450);
		m_pspr_mng->createSpriteString(8, 100, 455, 600, 140, 20, D3DCOLOR_XRGB(255, 255, 255), 0, 1);
		m_pspr_mng->createSpriteString(9, 22, 450 + 25, 70, 30, 15, D3DCOLOR_XRGB(80, 80, 80));
		m_pspr_mng->createSpriteString(10, 19, 450 + 100, 70, 25, 25, D3DCOLOR_XRGB(80, 80, 80));

		m_pspr_mng->createSpriteImgButton(11, "img/game_btn_load1.bmp", "img/game_btn_load2.bmp"
			, "img/game_btn_load3.bmp", 719, 450 + 12, 10, m_psnd_mng, NULL, "snd/push.wav");
		m_pspr_mng->createSpriteImgButton(12, "img/game_btn_save1.bmp", "img/game_btn_save2.bmp"
			, "img/game_btn_save3.bmp", 719, 450 + 46, 11, m_psnd_mng, NULL, "snd/push.wav");
		m_pspr_mng->createSpriteImgButton(13, "img/game_btn_skip1.bmp", "img/game_btn_skip2.bmp"
			, "img/game_btn_skip3.bmp", 719, 450 + 80, 12, m_psnd_mng, NULL, "snd/push.wav");
		m_pspr_mng->createSpriteImgButton(14, "img/game_btn_menu1.bmp", "img/game_btn_menu2.bmp"
			, "img/game_btn_menu3.bmp", 719, 450 + 114, 13, m_psnd_mng, NULL, "snd/push.wav");

		m_pspr_mng->createSpriteColor(15, D3DCOLOR_ARGB(192, 0, 0, 128), 225, 70, 350, 280);
		m_pspr_mng->createSpriteString(16, 225 + 10, 70 + 10, 330, 50, 23, D3DCOLOR_XRGB(255, 255, 255));
		m_pspr_mng->createSpriteString(17, 225 + 10, 70 + 70, 330, 50, 23, D3DCOLOR_XRGB(255, 255, 255), D3DCOLOR_XRGB(255, 255, 64), 2);
		m_pspr_mng->createSpriteString(18, 225 + 10, 70 + 120, 330, 50, 23, D3DCOLOR_XRGB(255, 255, 255), D3DCOLOR_XRGB(255, 255, 64), 3);
		m_pspr_mng->createSpriteString(19, 225 + 10, 70 + 170, 330, 50, 23, D3DCOLOR_XRGB(255, 255, 255), D3DCOLOR_XRGB(255, 255, 64), 4);
		m_pspr_mng->createSpriteString(20, 225 + 10, 70 + 220, 330, 50, 23, D3DCOLOR_XRGB(255, 255, 255), D3DCOLOR_XRGB(255, 255, 64), 5);

		m_pspr_mng->setEnable(11, false);
		m_pspr_mng->setEnable(12, false);

		if (m_psf->m_ssp.sd.pos > 0) {	// 続きから？
			// スクリプトの位置を設定。
			if (!m_pfile->Open(m_scr_info[m_psf->m_ssp.sd.chara].pfilename, "r")) return;
			pos = 0;
			for (;;) {
				if (!m_pfile->readLine(buf, 1024)) return;
				pbuf = buf;
				plen = buf + lstrlenA(buf);
				if (plen[-1] == '\n') *--plen = '\0';	// 最後の文字が改行文字ならその改行文字を抜く。

				pbuf += strspn(pbuf, BLANK_CHAR);
				if (pbuf >= plen || *pbuf == ';') continue;

				if (strnicmp(pbuf, "SELECT", 6) == 0 && pbuf + 6 < plen && strcspn(pbuf + 6, BLANK_CHAR) == 0) {
					pos++;
					if (m_psf->m_ssp.sd.pos != pos) continue;
					pbuf += 6;
					for (i = 15; i <= 20; i++) m_pspr_mng->addPauseAction(i, 70);
					parseSelect(pbuf, plen);
					break;
				} else if (strnicmp(pbuf, "WAIT", 4) == 0 && (pbuf + 4 >= plen || strcspn(pbuf + 4, BLANK_CHAR ";") == 0)) {
					pos++;
					if (m_psf->m_ssp.sd.pos != pos) continue;
					m_wait = true;
					setWait();
					break;
				}
			}

			// 背景。
			if (m_psf->m_ssp.sd.bg_color > 0) {
				m_pspr_mng->createSpriteColor(0, m_psf->m_ssp.sd.bg_color);
				m_pspr_mng->addFadeInAction(0, 0);
				m_bg = true;
			} else if (m_psf->m_ssp.sd.bg_filename[0] != '\0') {
				m_pspr_mng->createSpriteImg(0, m_psf->m_ssp.sd.bg_filename);
				m_pspr_mng->addFadeInAction(0, 0);
				m_bg = true;
			}

			// キャラ。
			if (m_psf->m_ssp.sd.chara_filename1[0] != '\0') {
				m_pspr_mng->createSpriteChara(2, m_psf->m_ssp.sd.chara_filename1, m_psf->m_ssp.sd.chara_pos1);
				m_pspr_mng->addFadeInAction(2, 0);
			}
			if (m_psf->m_ssp.sd.chara_filename2[0] != '\0') {
				m_pspr_mng->createSpriteChara(3, m_psf->m_ssp.sd.chara_filename2, m_psf->m_ssp.sd.chara_pos2);
				m_pspr_mng->addFadeInAction(3, 0);
			}

			// キャラより上の背景色。
			if (m_psf->m_ssp.sd.color > 0) {
				m_pspr_mng->createSpriteColor(4, m_psf->m_ssp.sd.color);
				m_pspr_mng->addFadeInAction(4, 0);
			}

			// 文章。
			if (m_psf->m_ssp.sd.text[0] != '\0') {
				m_pspr_mng->writeString(8, m_psf->m_ssp.sd.text
					, m_psf->m_ssp.sd.name[0] == '\0' ? NULL : m_psf->m_ssp.sd.name, m_psf->m_ssp.sd.bracket);
				m_pspr_mng->addFadeInAction(8, 0);
			}

			// 背景名。
			m_pspr_mng->writeString(9, m_psf->m_ssp.sd.bg_name);
			m_pspr_mng->addFadeInAction(9, 5);

			// 日。
			m_pspr_mng->writeString(10, m_psf->m_ssp.sd.day);
			m_pspr_mng->addFadeInAction(10, 5);

			// BGM。
			if (m_psf->m_ssp.sd.bgm_filename[0] != '\0') {
				m_psnd_mng->Play(0, m_psf->m_ssp.sd.bgm_filename, true, 0
					, m_psf->m_ssp.sd.bgm_intro_filename[0] == '\0' ? NULL : m_psf->m_ssp.sd.bgm_intro_filename);
			}

			m_scr_var = m_psf->m_ssp.sd.scr_var;	// スクリプト用変数。

			m_psf->savePlaying(false);

			m_pspr_mng->addFadeInAction(5, 0);
			m_pspr_mng->addFadeInAction(7, 0);
			m_pspr_mng->addPauseAction(11, 100);
			m_pspr_mng->addFadeInAction(11, 20);
			m_pspr_mng->addPauseAction(12, 110);
			m_pspr_mng->addFadeInAction(12, 20);
			m_pspr_mng->addPauseAction(13, 120);
			m_pspr_mng->addFadeInAction(13, 20);
			m_pspr_mng->addPauseAction(14, 130);
			m_pspr_mng->addFadeInAction(14, 20);
		} else {	// 始めから。
			m_pspr_mng->addFadeInAction(5, 30);
			m_pspr_mng->addFadeInAction(7, 30);
			m_pspr_mng->addPauseAction(11, 55);
			m_pspr_mng->addFadeInAction(11, 20);
			m_pspr_mng->addPauseAction(12, 65);
			m_pspr_mng->addFadeInAction(12, 20);
			m_pspr_mng->addPauseAction(13, 75);
			m_pspr_mng->addFadeInAction(13, 20);
			m_pspr_mng->addPauseAction(14, 85);
			m_pspr_mng->addFadeInAction(14, 20);

			m_scene = 6;

			// parseScriptを実行させるようにする。
			m_frame = 0;
			m_scr_frame = 30;
		}
	}

	// エンディング画面設定。
	// 引数：type 0ならエンディング準備、1なら初回エンディング処理、それ以外は2回目以降のエンディング処理。
	//   スプライト番号使用表
	//    0: エンドロール
	//    1: エンドロール
	//   エンディング種別
	//    0: グッドエンド1
	//    1: バッドエンド
	//    2: コミカルエンド
	//    3: グッドエンド2
	void setEnding(BYTE type) {
		char buf[64];
		WCHAR wbuf[64];
		BYTE i = 0;
		D3DCOLOR color;

		m_scene = 7;
		m_scr_frame = 0;
		m_skip_flag = false;

		switch (type) {
		case 0:
			m_pspr_mng->mouseOut(-1, false, false);
			m_pspr_mng->setEnable(11, false);
			m_pspr_mng->setEnable(12, false);
			m_pspr_mng->setEnable(13, false);
			m_pspr_mng->setEnable(14, false);
			m_pspr_mng->createSpriteColor(21, D3DCOLOR_XRGB(0, 0, 0));
			m_pspr_mng->addFadeInAction(21, 120);

			m_pspr_mng->addFadeOutAction(44, 30, true);
			m_pspr_mng->addFadeOutAction(45, 30, true);
			m_pspr_mng->addFadeOutAction(46, 30, true);
			m_pspr_mng->addFadeOutAction(47, 30, true);

			m_psf->m_ssp.sd.pos = 0;	// 最終的にイントロ画面に戻るので、ゲームロード処理しないように初期化しておく。
			m_psf->savePlaying(true);
			break;
		case 1:
			for (i = 0; i <= 21; i++) m_pspr_mng->deleteSprite(i);

			switch (m_ending_type) {
			case 1:
				m_pfile->Open("scr/end1.txt", "r");

				m_psnd_mng->Play(8, "snd/bgm_03.wav");
				color = D3DCOLOR_XRGB(255, 64, 64);
				m_pspr_mng->createSpriteString(0, 300, 600, 370, 600, 20, color);
				m_pspr_mng->createSpriteString(1, 300, 600, 370, 600, 20, color);
				m_pspr_mng->writeString(0, m_scr_info[m_psf->m_ssp.sd.chara].pname, NULL, 0, false, 0, 20);
				m_pspr_mng->writeString(0, L"Bad End...", NULL, 0, true, 0, 40);
				break;
			case 2:
				m_pfile->Open("scr/end2.txt", "r");

				m_psnd_mng->Play(8, "snd/bgm_04.wav");
				color = D3DCOLOR_XRGB(255, 255, 128);
				m_pspr_mng->createSpriteString(0, 300, 600, 370, 600, 20, color);
				m_pspr_mng->createSpriteString(1, 300, 600, 370, 600, 20, color);
				m_pspr_mng->writeString(0, m_scr_info[m_psf->m_ssp.sd.chara].pname, NULL, 0, false, 0, 20);
				m_pspr_mng->writeString(0, L"Bad End", NULL, 0, true, 0, 40);
				break;
			case 3:
				m_psf->m_ss.chara[m_psf->m_ssp.sd.chara] = 1;
				m_psf->Save(-1);
				m_pfile->Open("scr/end3.txt", "r");

				m_psnd_mng->Play(8, "snd/bgm_19.wav");
				color = D3DCOLOR_XRGB(255, 255, 255);
				m_pspr_mng->createSpriteString(0, 300, 600, 370, 600, 20, color);
				m_pspr_mng->createSpriteString(1, 300, 600, 370, 600, 20, color);
				m_pspr_mng->writeString(0, m_scr_info[m_psf->m_ssp.sd.chara].pname, NULL, 0, false, 0, 20);
				m_pspr_mng->writeString(0, L"Good End!!", NULL, 0, true, 0, 40);
				break;
			default:
				m_psf->m_ss.chara[m_psf->m_ssp.sd.chara] = 1;
				m_psf->Save(-1);
				m_pfile->Open("scr/end0.txt", "r");

				m_psnd_mng->Play(8, "snd/bgm_02.wav");
				color = D3DCOLOR_XRGB(255, 255, 255);
				m_pspr_mng->createSpriteString(0, 300, 600, 370, 600, 20, color);
				m_pspr_mng->createSpriteString(1, 300, 600, 370, 600, 20, color);
				m_pspr_mng->createSpriteImg(3, "img/bg_13.bmp");
				m_pspr_mng->writeString(0, m_scr_info[m_psf->m_ssp.sd.chara].pname, NULL, 0, false, 0, 20);
				m_pspr_mng->writeString(0, L"Good End!!", NULL, 0, true, 0, 40);
				break;
			}

			m_id = 0;
			i = 6;
			// ↓も実行するのでbreakはしない。
		default:
			if (i == 0) m_pspr_mng->writeString(m_id, L"");
			for (; i < 30; i++) {
				if (!m_pfile->readLine(buf, 64)) break;
				if (buf[0] == '\n') continue;
				if (MultiByteToWideChar(CP_ACP, 0, buf, -1, wbuf, sizeof(wbuf) / sizeof(wbuf[0])) == 0) continue;

				m_pspr_mng->writeString(m_id, wbuf, NULL, 0, true, 0, i * 20);
			}
			if (i == 0) break;

			m_pspr_mng->addFadeInAction(m_id, 0);
			switch (m_ending_type) {
			case 0:
				m_pspr_mng->addMoveYAction(m_id, 600, -600, -2);
				m_scr_frame = m_frame + 300;
				break;
			case 2:
				m_pspr_mng->addMoveYAction(m_id, 600, -600, -10);
				m_scr_frame = m_frame + 60;
				break;
			default:
				m_pspr_mng->addMoveYAction(m_id, 600, -600, -1);
				m_scr_frame = m_frame + 600;
				break;
			}
			m_id = m_id == 0 ? 1 : 0;
			break;
		}
	}

	// 現在の背景を作成する色スプライトに変更。
	// 引数：red 赤。
	//       green 緑。
	//       blue 青。
	//       time フェードインが完了するまでのフレーム単位の時間。
	void changeBGColor(BYTE red, BYTE green, BYTE blue, short time) {
		D3DCOLOR color = D3DCOLOR_XRGB(red, green, blue);

		if (m_bg) {
			m_pspr_mng->createSpriteColor(1, color);
			m_pspr_mng->addFadeInAction(1, time);

			m_bg = false;
		} else {
			m_pspr_mng->createSpriteColor(0, color);
			if (m_pspr_mng->addFadeOutAction(1, time)) m_pspr_mng->addFadeInAction(0, 0);
			else m_pspr_mng->addFadeInAction(0, time);

			m_bg = true;
		}

		m_psf->m_ssp.sd.bg_color = color;
	}

	// 現在の背景を作成する画像スプライトに変更。
	// 引数：pfilename ファイル名。
	//       time フェードインが完了するまでのフレーム単位の時間。
	void changeBGImg(LPCSTR pfilename, short time) {
		if (m_bg) {
			m_pspr_mng->createSpriteImg(1, pfilename);
			m_pspr_mng->addFadeInAction(1, time);

			m_bg = false;
		} else {
			m_pspr_mng->createSpriteImg(0, pfilename);
			if (m_pspr_mng->addFadeOutAction(1, time)) m_pspr_mng->addFadeInAction(0, 0);
			else m_pspr_mng->addFadeInAction(0, time);

			m_bg = true;
		}

		m_psf->m_ssp.sd.bg_color = 0;
		lstrcpyA(m_psf->m_ssp.sd.bg_filename, pfilename);
	}

	// 表示優先順位の低いキャラスプライトを表示/非表示。
	// 引数：pfilename ファイル名。
	//       pos_x スプライトのX位置。
	//       time フェードイン/アウトが完了するまでのフレーム単位の時間。
	//       disp_flag 表示するならtrue、非表示にするならfalseを指定する。
	void setChara1(LPCSTR pfilename, short pos_x, short time, bool disp_flag) {
		if (disp_flag) {
			m_pspr_mng->createSpriteChara(2, pfilename, pos_x);
			m_pspr_mng->addFadeInAction(2, time);
			lstrcpyA(m_psf->m_ssp.sd.chara_filename1, pfilename);
			m_psf->m_ssp.sd.chara_pos1 = pos_x;
		} else {
			m_pspr_mng->addFadeOutAction(2, time);
			m_psf->m_ssp.sd.chara_filename1[0] = '\0';
		}
	}

	// 表示優先順位の高いキャラスプライトを表示/非表示。
	// 引数：pfilename ファイル名。
	//       pos_x スプライトのX位置。
	//       time フェードイン/アウトが完了するまでのフレーム単位の時間。
	//       disp_flag 表示するならtrue、非表示にするならfalseを指定する。
	void setChara2(LPCSTR pfilename, short pos_x, short time, bool disp_flag) {
		if (disp_flag) {
			m_pspr_mng->createSpriteChara(3, pfilename, pos_x);
			m_pspr_mng->addFadeInAction(3, time);
			lstrcpyA(m_psf->m_ssp.sd.chara_filename2, pfilename);
			m_psf->m_ssp.sd.chara_pos2 = pos_x;
		} else {
			m_pspr_mng->addFadeOutAction(3, time);
			m_psf->m_ssp.sd.chara_filename2[0] = '\0';
		}
	}

	// 背景とキャラの上に作成する色スプライトを表示/非表示。
	// 引数：red 赤。
	//       green 緑。
	//       blue 青。
	//       time フェードイン/アウトが完了するまでのフレーム単位の時間。
	//       disp_flag 表示するならtrue、非表示にするならfalseを指定する。
	void setBGColorOverChara(BYTE red, BYTE green, BYTE blue, short time, bool disp_flag) {
		D3DCOLOR color;

		if (disp_flag) {
			color = D3DCOLOR_XRGB(red, green, blue);
			m_pspr_mng->createSpriteColor(4, color);
			m_pspr_mng->addFadeInAction(4, time);
			m_psf->m_ssp.sd.color = color;
		} else {
			m_pspr_mng->addFadeOutAction(4, time);
			m_psf->m_ssp.sd.color = 0;
		}
	}

	// 文章を表示/非表示。
	// 引数：ptext 文章。NULLを指定すると非表示(≠空文字)にする。
	//       pname 名前。
	//       bracket 文章を囲む括弧の種類。
	void setText(LPCWSTR ptext, LPCWSTR pname, BYTE bracket) {
		m_pspr_mng->addFadeOutAction(8, 0);
		if (ptext == NULL) {
			m_psf->m_ssp.sd.text[0] = L'\0';
		} else {
			m_pspr_mng->writeString(8, ptext, pname, bracket);
			m_pspr_mng->addPauseAction(8, m_skip_flag ? 2 : 10);
			m_pspr_mng->addFadeInAction(8, m_skip_flag ? 1 : 5);

			lstrcpyW(m_psf->m_ssp.sd.text, ptext);
			if (pname == NULL) m_psf->m_ssp.sd.name[0] = L'\0';
			else lstrcpyW(m_psf->m_ssp.sd.name, pname);
			m_psf->m_ssp.sd.bracket = bracket;
		}
	}

	// 選択肢を表示/非表示。
	// 引数：ptext 文章。
	//       psel1 選択文字列1。
	//       psel2 選択文字列2。
	//       psel3 選択文字列3。
	//       psel4 選択文字列4。
	//       disp_flag 表示するならtrue、非表示にするならfalseを指定する。
	void setSelect(LPCWSTR ptext, LPCWSTR psel1, LPCWSTR psel2, LPCWSTR psel3, LPCWSTR psel4, bool disp_flag) {
		BYTE i;

		m_skip_flag = false;

		if (disp_flag) {
			m_pspr_mng->setEnable(11, true);
			m_pspr_mng->setEnable(12, true);
			m_pspr_mng->mouseOut(13, false, false);
			m_pspr_mng->setEnable(13, false);
			for (i = 17; i <= 20; i++) m_pspr_mng->setEnable(i, true);

			m_pspr_mng->addFadeInAction(15, 20);

			if (ptext[0] != L'\0') {
				m_pspr_mng->writeString(16, ptext);
				m_pspr_mng->addFadeInAction(16, 20);
			}

			m_pspr_mng->writeString(17, psel1, L"①", 3);
			m_pspr_mng->addFadeInAction(17, 20);
			m_pspr_mng->writeString(18, psel2, L"②", 3);
			m_pspr_mng->addFadeInAction(18, 20);
			if (psel3[0] == L'\0') return;
			m_pspr_mng->writeString(19, psel3, L"③", 3);
			m_pspr_mng->addFadeInAction(19, 20);
			if (psel4[0] == L'\0') return;
			m_pspr_mng->writeString(20, psel4, L"④", 3);
			m_pspr_mng->addFadeInAction(20, 20);
		} else {
			m_frame = 0;
			m_scr_frame = 20;

			m_pspr_mng->addFadeOutAction(15, 20);
			m_pspr_mng->addFadeOutAction(16, 20, true);
			m_pspr_mng->addFadeOutAction(17, 20);
			m_pspr_mng->addFadeOutAction(18, 20);
			m_pspr_mng->addFadeOutAction(19, 20, true);
			m_pspr_mng->addFadeOutAction(20, 20, true);

			m_pspr_mng->setEnable(11, false);
			m_pspr_mng->setEnable(12, false);
			m_pspr_mng->setEnable(13, true);
			for (i = 17; i <= 20; i++) m_pspr_mng->setEnable(i, false);
		}
	}

	// 待ち状態を設定する。
	void setWait() {
		m_pspr_mng->resetAction(6);

		if (m_wait) {
			m_pspr_mng->addFadeInAction(6, 30);
			m_pspr_mng->addFadeOutAction(6, 30);
			m_pspr_mng->setActionLoop(6, 0);

			m_pspr_mng->setEnable(11, true);
			m_pspr_mng->setEnable(12, true);
		} else {
			m_pspr_mng->addFadeOutAction(6, 0);

			m_pspr_mng->mouseOut(11, false, false);
			m_pspr_mng->mouseOut(12, false, false);
			m_pspr_mng->setEnable(11, false);
			m_pspr_mng->setEnable(12, false);

			// parseScriptを実行させるようにする。
			m_frame = 0;
			m_scr_frame = 1;
		}
	}

	// スキップ状態に応じたフレーム単位の時間を取得。
	// 引数：time フレーム単位の時間。
	WORD getScrTime(WORD time) {
		if (!m_skip_flag || time == 0) return time;

		time = (float)time / 5;
		return time > 0 ? time : 1;
	}

	// シナリオスクリプト構文解析。
	void parseScript() {
		char buf[1024], *pbuf, *plen;

		for (;;) {
			if (!m_pfile->readLine(buf, 1024)) return;
			pbuf = buf;
			plen = buf + lstrlenA(buf);
			if (plen[-1] == '\n') *--plen = '\0';	// 最後の文字が改行文字ならその改行文字を抜く。

			pbuf += strspn(pbuf, BLANK_CHAR);
			if (pbuf >= plen || *pbuf == ';') continue;

			if (m_goto[0] != '\0') {	// ラベルへの移動をするか？
				if (strnicmp(pbuf, "SELECT", 6) == 0 && pbuf + 6 < plen && strcspn(pbuf + 6, BLANK_CHAR) == 0) {
					m_psf->m_ssp.sd.pos++;
				} else if (strnicmp(pbuf, "WAIT", 4) == 0 && (pbuf + 4 >= plen || strcspn(pbuf + 4, BLANK_CHAR ";") == 0)) {
					m_psf->m_ssp.sd.pos++;
				} else if (*pbuf == ':' && stricmp(pbuf + 1, m_goto) == 0) {
					m_goto[0] = '\0';	// ラベル名が一致したので次からラベルへの移動をしないようにする。
				}
				continue;
			}

			m_scr_frame = 0;

			if (strnicmp(pbuf, "BGCOLOR", 7) == 0 && pbuf + 7 < plen && strcspn(pbuf + 7, BLANK_CHAR) == 0) {
				pbuf += 7;
				m_scr_frame = parseBGColor(pbuf, plen, 1);
			} else if (strnicmp(pbuf, "BG", 2) == 0 && pbuf + 2 < plen && strcspn(pbuf + 2, BLANK_CHAR) == 0) {
				pbuf += 2;
				m_scr_frame = parseBGImg(pbuf, plen);
			} else if (strnicmp(pbuf, "CHARA1", 6) == 0 && pbuf + 6 < plen && strcspn(pbuf + 6, BLANK_CHAR) == 0) {
				pbuf += 6;
				m_scr_frame = parseChara(pbuf, plen, 1);
			} else if (strnicmp(pbuf, "CHARA2", 6) == 0 && pbuf + 6 < plen && strcspn(pbuf + 6, BLANK_CHAR) == 0) {
				pbuf += 6;
				m_scr_frame = parseChara(pbuf, plen, 2);
			} else if (strnicmp(pbuf, "COLOR", 5) == 0 && pbuf + 5 < plen && strcspn(pbuf + 5, BLANK_CHAR) == 0) {
				pbuf += 5;
				m_scr_frame = parseBGColor(pbuf, plen, 2);
			} else if (strnicmp(pbuf, "TEXT", 4) == 0 && pbuf + 4 < plen && strcspn(pbuf + 4, BLANK_CHAR) == 0) {
				pbuf += 4;
				m_scr_frame = parseText(pbuf, plen);
			} else if (strnicmp(pbuf, "TEXTERASE", 9) == 0 && (pbuf + 9 >= plen || strcspn(pbuf + 9, BLANK_CHAR ";") == 0)) {
				setText(NULL, NULL, false);
			} else if (strnicmp(pbuf, "SELECT", 6) == 0 && pbuf + 6 < plen && strcspn(pbuf + 6, BLANK_CHAR) == 0) {
				m_psf->m_ssp.sd.pos++;
				m_psf->savePlaying(false);
				pbuf += 6;
				if (parseSelect(pbuf, plen)) break;
			} else if (strnicmp(pbuf, "WAIT", 4) == 0 && (pbuf + 4 >= plen || strcspn(pbuf + 4, BLANK_CHAR ";") == 0)) {
				m_psf->m_ssp.sd.pos++;
				m_psf->savePlaying(false);
				if (!m_skip_flag) {
					m_wait = true;
					setWait();
					break;
				}
			} else if (strnicmp(pbuf, "PAUSE", 5) == 0 && pbuf + 5 < plen && strcspn(pbuf + 5, BLANK_CHAR) == 0) {
				pbuf += 5;
				m_scr_frame = parsePause(pbuf, plen);
			} else if (strnicmp(pbuf, "GOTO", 4) == 0 && pbuf + 4 < plen && strcspn(pbuf + 4, BLANK_CHAR) == 0) {
				pbuf += 4;
				parseGoto(pbuf, plen);
			} else if (strnicmp(pbuf, "BGNAME", 6) == 0 && pbuf + 6 < plen && strcspn(pbuf + 6, BLANK_CHAR) == 0) {
				pbuf += 6;
				parseBGName(pbuf, plen);
			} else if (strnicmp(pbuf, "DAY", 3) == 0 && pbuf + 3 < plen && strcspn(pbuf + 3, BLANK_CHAR) == 0) {
				pbuf += 3;
				parseDay(pbuf, plen);
			} else if (strnicmp(pbuf, "SOUND", 5) == 0 && pbuf + 5 < plen && strcspn(pbuf + 5, BLANK_CHAR) == 0) {
				if (!m_skip_flag) {
					pbuf += 5;
					parseSound(pbuf, plen, 1);
				}
			} else if (strnicmp(pbuf, "BGM", 3) == 0 && pbuf + 3 < plen && strcspn(pbuf + 3, BLANK_CHAR) == 0) {
				pbuf += 3;
				parseSound(pbuf, plen, 2);
			} else if (strnicmp(pbuf, "BGMSTOP", 7) == 0 && (pbuf + 7 >= plen || strcspn(pbuf + 7, BLANK_CHAR ";") == 0)) {
				m_psnd_mng->Stop(0, 30);
				m_psf->m_ssp.sd.bgm_filename[0] = '\0';
			} else if (strnicmp(pbuf, "VAR", 3) == 0 && pbuf + 3 < plen && strcspn(pbuf + 3, BLANK_CHAR) == 0) {
				pbuf += 3;
				parseVar(pbuf, plen);
			} else if (strnicmp(pbuf, "IF", 2) == 0 && pbuf + 2 < plen && strcspn(pbuf + 2, BLANK_CHAR) == 0) {
				pbuf += 2;
				parseIf(pbuf, plen);
			} else if (strnicmp(pbuf, "END", 3) == 0 && pbuf + 3 < plen && strcspn(pbuf + 3, BLANK_CHAR) == 0) {
				pbuf += 3;
				parseEnding(pbuf, plen);
				break;
			}

			if (m_scr_frame > 0) break;
		}

		m_frame = 0;
	}

	// シナリオスクリプトの「BGCOLOR、COLOR」の構文解析。
	// 引数：pbuf 文字列ポインタ。
	//       plen 文字列長ポインタ。
	//       type 1なら背景、それ以外はキャラの上の背景。
	// 返値：フレーム単位の時間。
	WORD parseBGColor(char *pbuf, char *plen, BYTE type) {
		char *ptmp;
		int value;
		short param[4], i;
		bool hide_flag;

		for (i = 0; i < 4; i++) {
			pbuf += strspn(pbuf, BLANK_CHAR);
			value = strtol(pbuf, &ptmp, 10);
			if (pbuf == ptmp) return 0;

			if (i < 3) {
				// 色の場合。
				if (value < 0) param[i] = 0;
				else if (value > 255) param[i] = 255;
				else param[i] = value;
			} else {
				// timeの場合。
				if (value < 0) param[i] = 0;
				else if (value > 999) param[i] = 999;
				else param[i] = value;
				param[i] = getScrTime(param[i]);
			}

			pbuf = ptmp;
		}

		if (type == 1) {
			if (strcspn(pbuf, BLANK_CHAR ";") > 0) return 0;
			changeBGColor(param[0], param[1], param[2], param[3]);
		} else {
			value = strspn(pbuf, BLANK_CHAR);
			if (value == 0) return 0;
			pbuf += value;

			// hide_flag。
			ptmp = pbuf + strcspn(pbuf, BLANK_CHAR);
			if (ptmp < plen) *ptmp = '\0';
			if (stricmp(pbuf, "true") == 0) hide_flag = true;
			else if (stricmp(pbuf, "false") == 0) hide_flag = false;
			else return 0;

			setBGColorOverChara(param[0], param[1], param[2], param[3], hide_flag);
		}
		return param[3];
	}

	// シナリオスクリプトの「BG」の構文解析。
	// 引数：pbuf 文字列ポインタ。
	//       plen 文字列長ポインタ。
	// 返値：フレーム単位の時間。
	WORD parseBGImg(char *pbuf, char *plen) {
		char *pfilename, *ptmp;
		short time;
		int value;

		pbuf += strspn(pbuf, BLANK_CHAR);
		if (pbuf >= plen || *pbuf == ';') return 0;

		// filename。
		if (*pbuf == '"') {
			pbuf++;
			ptmp = strchr(pbuf, '"');
			if (ptmp == NULL) return 0;
		} else {
			ptmp = pbuf + strcspn(pbuf, BLANK_CHAR ";");
			if (ptmp >= plen || *ptmp == ';') return 0;
		}
		if (ptmp - pbuf > MAX_PATH - 1) pbuf[MAX_PATH - 1] = '\0';	// 制限バイト数を超えるなら超えないように調整する。
		else *ptmp = '\0';
		pfilename = pbuf;
		pbuf = ptmp + 1;

		value = strspn(pbuf, BLANK_CHAR);
		if (value == 0) return 0;
		pbuf += value;

		// time。
		value = strtol(pbuf, &ptmp, 10);
		if (pbuf == ptmp) return 0;
		if (value < 0) time = 0;
		else if (value > 999) time = 999;
		else time = value;

		pbuf = ptmp;
		if (strcspn(pbuf, BLANK_CHAR ";") > 0) return 0;

		time = getScrTime(time);
		changeBGImg(pfilename, time);
		return time;
	}

	// シナリオスクリプトの「CHARA1、CHARA2」の構文解析。
	// 引数：pbuf 文字列ポインタ。
	//       plen 文字列長ポインタ。
	//       type 1なら表示優先順位の低いキャラ、それ以外は1なら表示優先順位の高いキャラ。
	// 返値：フレーム単位の時間。
	WORD parseChara(char *pbuf, char *plen, BYTE type) {
		char *pfilename, *ptmp;
		short pos_x, time;
		int value;
		bool hide_flag;

		pbuf += strspn(pbuf, BLANK_CHAR);
		if (pbuf >= plen || *pbuf == ';') return 0;

		// filename。
		if (*pbuf == '"') {
			pbuf++;
			ptmp = strchr(pbuf, '"');
			if (ptmp == NULL) return 0;
		} else {
			ptmp = pbuf + strcspn(pbuf, BLANK_CHAR ";");
			if (ptmp >= plen || *ptmp == ';') return 0;
		}
		if (ptmp - pbuf > MAX_PATH - 1) pbuf[MAX_PATH - 1] = '\0';	// 制限バイト数を超えるなら超えないように調整する。
		else *ptmp = '\0';
		pfilename = pbuf;

		pbuf = ptmp + 1;
		value = strspn(pbuf, BLANK_CHAR);
		if (value == 0) return 0;
		pbuf += value;

		// pos_x。
		value = strtol(pbuf, &ptmp, 10);
		if (pbuf == ptmp) return 0;
		if (value < -999) pos_x = -999;
		else if (value > 999) pos_x = 999;
		else pos_x = value;
		pbuf = ptmp;

		pbuf += strspn(pbuf, BLANK_CHAR);

		// time。
		value = strtol(pbuf, &ptmp, 10);
		if (pbuf == ptmp) return 0;
		if (value < 0) time = 0;
		else if (value > 999) time = 999;
		else time = value;
		pbuf = ptmp;

		value = strspn(pbuf, BLANK_CHAR);
		if (value == 0) return 0;
		pbuf += value;

		// hide_flag。
		ptmp = pbuf + strcspn(pbuf, BLANK_CHAR);
		if (ptmp < plen) *ptmp = '\0';
		if (stricmp(pbuf, "true") == 0) hide_flag = true;
		else if (stricmp(pbuf, "false") == 0) hide_flag = false;
		else return 0;

		time = getScrTime(time);
		if (type == 1) setChara1(pfilename, pos_x, time, hide_flag);
		else setChara2(pfilename, pos_x, time, hide_flag);
		return time;
	}

	// シナリオスクリプトの「TEXT～TEXTEND」の構文解析。
	// 引数：pbuf 文字列ポインタ。
	//       plen 文字列長ポインタ。
	// 返値：フレーム単位の時間。
	WORD parseText(char *pbuf, char *plen) {
		char buf[512], *ptmp;
		int len, value;
		BYTE bracket;
		WCHAR text[512], name[32], wbuf[512];

		pbuf += strspn(pbuf, BLANK_CHAR);
		if (pbuf >= plen || *pbuf == ';') return 0;

		// name。
		if (*pbuf == '"') {
			pbuf++;
			ptmp = strchr(pbuf, '"');
			if (ptmp == NULL) return 0;
		} else {
			ptmp = pbuf + strcspn(pbuf, BLANK_CHAR ";");
			if (ptmp >= plen || *ptmp == ';') return 0;
		}
		value = ptmp - pbuf;
		if (value > 20) pbuf[20] = '\0';	// 制限バイト数を超えるなら超えないように調整する。
		else *ptmp = '\0';

		if (value > 0) {
			if (MultiByteToWideChar(CP_ACP, 0, pbuf, -1, name, sizeof(name) / sizeof(name[0])) == 0) return 0;
		} else {
			name[0] = L'\0';
		}

		pbuf = ptmp + 1;
		value = strspn(pbuf, BLANK_CHAR);
		if (value == 0) return 0;
		pbuf += value;

		// bracket。
		value = strtol(pbuf, &ptmp, 10);
		if (pbuf == ptmp) return 0;
		if (value < 0 || value > 3) bracket = 0;
		else bracket = value;

		if (strcspn(ptmp, BLANK_CHAR ";") > 0) return 0;

		// text。
		text[0] = L'\0';
		len = 0;
		for (;;) {
			if (!m_pfile->readLine(buf, 512)) return 0;
			value = lstrlenA(buf);

			if (strnicmp(buf, "TEXTEND", 7) == 0 && (value <= 7 || strcspn(buf + 7, BLANK_CHAR ";\n") == 0)) {
				break;
			}
			if (len >= 400) continue;

			if (len + value > 400) {	// 制限バイト数を超えるか？
				buf[400 - len] = '\0';
				len = 400;
			} else {
				len += value;
			}

			if (MultiByteToWideChar(CP_ACP, 0, buf, -1, wbuf, sizeof(wbuf) / sizeof(wbuf[0])) == 0) return 0;
			lstrcatW(text, wbuf);
		}
		len = lstrlenW(text);
		if (len > 0 && text[len - 1] == L'\n') text[len - 1] = L'\0';

		setText(text[0] == L'\0' ? NULL : text, name[0] == L'\0' ? NULL : name, bracket);
		return getScrTime(20);
	}

	// シナリオスクリプトの「SELECT～SELECTEND」の構文解析。
	// 引数：pbuf 文字列ポインタ。
	//       plen 文字列長ポインタ。
	// 返値：成功ならtrue、それ以外はfalse。
	bool parseSelect(char *pbuf, char *plen) {
		char buf[256], i, *ptmp;
		int size;
		WCHAR text[64], sel[4][64];

		pbuf += strspn(pbuf, BLANK_CHAR);
		if (pbuf >= plen || *pbuf == ';') return false;

		// text。
		if (*pbuf == '"') {
			pbuf++;
			ptmp = strchr(pbuf, '"');
			if (ptmp == NULL) return false;
		} else {
			ptmp = pbuf + strcspn(pbuf, BLANK_CHAR ";");
		}
		size = ptmp - pbuf;
		if (size > 60) pbuf[60] = '\0';	// 制限バイト数を超えるなら超えないように調整する。
		else *ptmp = '\0';

		if (size > 0) {
			if (MultiByteToWideChar(CP_ACP, 0, pbuf, -1, text, sizeof(text) / sizeof(text[0])) == 0) return false;
		} else {
			text[0] = L'\0';
		}

		if (strcspn(ptmp + 1, BLANK_CHAR ";") > 0) return false;

		// sel、sel_goto。
		for (i = 0; i < 4; i++) sel[i][0] = m_sel_goto[i][0] = '\0';
		for (i = 0; i < 5; i++) {
			if (!m_pfile->readLine(buf, 256)) return false;
			pbuf = buf;
			plen = buf + lstrlenA(buf);
			if (plen[-1] == '\n') *--plen = '\0';	// 最後の文字が改行文字ならその改行文字を抜く。

			if (strnicmp(pbuf, "SELECTEND", 9) == 0 && (pbuf + 9 >= plen || strcspn(pbuf + 9, BLANK_CHAR ";") == 0)) {
				break;
			}
			if (i >= 4) return false;

			pbuf += strspn(pbuf, BLANK_CHAR);
			if (pbuf >= plen || *pbuf == ';') return false;

			// sel。
			if (*pbuf == '"') {
				pbuf++;
				ptmp = strchr(pbuf, '"');
				if (ptmp == NULL) return false;
			} else {
				ptmp = pbuf + strcspn(pbuf, BLANK_CHAR ";");
				if (ptmp >= plen || *ptmp == ';') return false;
			}
			size = ptmp - pbuf;
			if (size <= 0) return false;
			if (size > 60) pbuf[60] = '\0';	// 制限バイト数を超えるなら超えないように調整する。
			else *ptmp = '\0';

			if (MultiByteToWideChar(CP_ACP, 0, pbuf, -1, sel[i], sizeof(sel[0]) / sizeof(sel[0][0])) == 0) return false;

			pbuf = ptmp + 1;
			size = strspn(pbuf, BLANK_CHAR);
			if (size == 0) return false;
			pbuf += size;

			// sel_goto。
			if (*pbuf == '"') {
				pbuf++;
				ptmp = strchr(pbuf, '"');
				if (ptmp == NULL) return false;
			} else {
				size = strcspn(pbuf, BLANK_CHAR ";");
				if (size == 0) return false;
				ptmp = pbuf + size;
			}
			size = ptmp - pbuf;
			if (size > 30) pbuf[30] = '\0';	// 制限バイト数を超えるなら超えないように調整する。
			else *ptmp = '\0';

			if (size > 0) lstrcpyA(m_sel_goto[i], pbuf);

			if (strcspn(ptmp + 1, BLANK_CHAR ";") > 0) return false;
		}
		if (i < 2) return false;

		setText(NULL, NULL, false);
		setSelect(text, sel[0], sel[1], sel[2], sel[3], true);
		return true;
	}

	// シナリオスクリプトの「PAUSE」の構文解析。
	// 引数：pbuf 文字列ポインタ。
	//       plen 文字列長ポインタ。
	// 返値：フレーム単位の時間。
	WORD parsePause(char *pbuf, char *plen) {
		char *ptmp;
		int time;

		pbuf += strspn(pbuf, BLANK_CHAR);

		// time。
		time = strtol(pbuf, &ptmp, 10);
		if (pbuf == ptmp) return 0;
		if (time < 0) time = 0;
		else if (time > 999) time = 999;

		if (strcspn(ptmp, BLANK_CHAR ";") > 0) return 0;

		return getScrTime(time);
	}

	// シナリオスクリプトの「GOTO」の構文解析。
	// 引数：pbuf 文字列ポインタ。
	//       plen 文字列長ポインタ。
	void parseGoto(char *pbuf, char *plen) {
		char *ptmp;
		int size;

		pbuf += strspn(pbuf, BLANK_CHAR);
		if (pbuf >= plen || *pbuf == ';') return;

		// label。
		if (*pbuf == '"') {
			pbuf++;
			ptmp = strchr(pbuf, '"');
			if (ptmp == NULL) return;
		} else {
			ptmp = pbuf + strcspn(pbuf, BLANK_CHAR ";");
		}
		size = ptmp - pbuf;
		if (size <= 0) return;
		if (size > 30) pbuf[30] = '\0';	// 制限バイト数を超えるなら超えないように調整する。
		else *ptmp = '\0';

		if (strcspn(ptmp + 1, BLANK_CHAR ";") > 0) return;

		lstrcpyA(m_goto, pbuf);
	}

	// シナリオスクリプトの「BGNAME」の構文解析。
	// 引数：pbuf 文字列ポインタ。
	//       plen 文字列長ポインタ。
	void parseBGName(char *pbuf, char *plen) {
		char *ptmp;
		int value;
		WCHAR text[32];

		pbuf += strspn(pbuf, BLANK_CHAR);
		if (pbuf >= plen || *pbuf == ';') return;

		// text。
		if (*pbuf == '"') {
			pbuf++;
			ptmp = strchr(pbuf, '"');
			if (ptmp == NULL) return;
		} else {
			ptmp = pbuf + strcspn(pbuf, BLANK_CHAR ";");
			if (ptmp >= plen || *ptmp == ';') return;
		}
		if (ptmp - pbuf > 20) pbuf[20] = '\0';	// 制限バイト数を超えるなら超えないように調整する。
		else *ptmp = '\0';

		if (strcspn(ptmp + 1, BLANK_CHAR ";") > 0) return;
		if (MultiByteToWideChar(CP_ACP, 0, pbuf, -1, text, sizeof(text) / sizeof(text[0])) == 0) return;

		m_pspr_mng->writeString(9, text);
		m_pspr_mng->addFadeInAction(9, 0);
		lstrcpyW(m_psf->m_ssp.sd.bg_name, text);
	}

	// シナリオスクリプトの「DAY」の構文解析。
	// 引数：pbuf 文字列ポインタ。
	//       plen 文字列長ポインタ。
	void parseDay(char *pbuf, char *plen) {
		char *ptmp;
		int value;
		WCHAR text[16];

		pbuf += strspn(pbuf, BLANK_CHAR);
		if (pbuf >= plen || *pbuf == ';') return;

		// text。
		if (*pbuf == '"') {
			pbuf++;
			ptmp = strchr(pbuf, '"');
			if (ptmp == NULL) return;
		} else {
			ptmp = pbuf + strcspn(pbuf, BLANK_CHAR ";");
			if (ptmp >= plen || *ptmp == ';') return;
		}
		if (ptmp - pbuf > 10) pbuf[10] = '\0';	// 制限バイト数を超えるなら超えないように調整する。
		else *ptmp = '\0';

		if (strcspn(ptmp + 1, BLANK_CHAR ";") > 0) return;
		if (MultiByteToWideChar(CP_ACP, 0, pbuf, -1, text, sizeof(text) / sizeof(text[0])) == 0) return;

		m_pspr_mng->writeString(10, text);
		m_pspr_mng->addFadeInAction(10, 0);
		lstrcpyW(m_psf->m_ssp.sd.day, text);
	}

	// シナリオスクリプトの「SOUND」の構文解析。
	// 引数：pbuf 文字列ポインタ。
	//       plen 文字列長ポインタ。
	//       type 1なら効果音、それ以外はBGM。
	void parseSound(char *pbuf, char *plen, BYTE type) {
		char *pfilename, *pfilename_intro, *ptmp;
		int value;

		pbuf += strspn(pbuf, BLANK_CHAR);
		if (pbuf >= plen || *pbuf == ';') return;

		// filename。
		if (*pbuf == '"') {
			pbuf++;
			ptmp = strchr(pbuf, '"');
			if (ptmp == NULL) return;
		} else {
			ptmp = pbuf + strcspn(pbuf, BLANK_CHAR ";");
		}
		value = ptmp - pbuf;
		if (value <= 0) return;
		if (value > MAX_PATH - 1) pbuf[MAX_PATH - 1] = '\0';	// 制限バイト数を超えるなら超えないように調整する。
		else *ptmp = '\0';
		pfilename = pbuf;

		pbuf = ptmp + 1;
		value = strspn(pbuf, BLANK_CHAR);
		if (value == 0) return;
		pbuf += value;

		if (type == 1) {
			// idx。
			value = strtol(pbuf, &ptmp, 10);
			if (pbuf == ptmp) return;
			if (value < 1) value = 1;
			else if (value > 5) value = 5;

			if (strcspn(ptmp, BLANK_CHAR ";") > 0) return;
			m_psnd_mng->Play(value, pfilename);
		} else {
			// intro_filename。
			if (*pbuf == '"') {
				pbuf++;
				ptmp = strchr(pbuf, '"');
				if (ptmp == NULL) return;
			} else {
				value = strcspn(pbuf, BLANK_CHAR ";");
				if (value == 0) return;
				ptmp = pbuf + value;
			}
			value = ptmp - pbuf;
			if (value > MAX_PATH - 1) pbuf[MAX_PATH - 1] = '\0';	// 制限バイト数を超えるなら超えないように調整する。
			else *ptmp = '\0';
			pfilename_intro = value > 0 ? pbuf : NULL;

			if (strcspn(ptmp + 1, BLANK_CHAR ";") > 0) return;
			m_psnd_mng->Play(0, pfilename, true, 0, pfilename_intro);

			lstrcpyA(m_psf->m_ssp.sd.bgm_filename, pfilename);
			if (pfilename_intro == NULL) m_psf->m_ssp.sd.bgm_intro_filename[0] = '\0';
			else lstrcpyA(m_psf->m_ssp.sd.bgm_intro_filename, pfilename_intro);
		}
	}

	// シナリオスクリプトの「VAR」の構文解析。
	// 引数：pbuf 文字列ポインタ。
	//       plen 文字列長ポインタ。
	void parseVar(char *pbuf, char *plen) {
		char *ptmp;
		int value;
		bool set_flag;

		// 変数に設定か、変数に加算か。
		pbuf += strspn(pbuf, BLANK_CHAR);
		if (pbuf + 3 >= plen || strcspn(pbuf + 3, BLANK_CHAR) > 0) return;
		if (strnicmp(pbuf, "SET", 3) == 0) set_flag = true;
		else if (strnicmp(pbuf, "ADD", 3) == 0) set_flag = false;
		else return;
		pbuf += 3;

		value = strspn(pbuf, BLANK_CHAR);
		if (value == 0) return;
		pbuf += value;

		// num。
		value = strtol(pbuf, &ptmp, 10);
		if (pbuf == ptmp || strcspn(ptmp, BLANK_CHAR ";") > 0) return;

		if (set_flag) {	// 変数に設定？
			if (value < -100) m_scr_var = -100;
			else if (value > 100) m_scr_var = 100;
			else m_scr_var = value;
		} else {	// 変数に加算。
			if (value < -200) {
				m_scr_var = -100;
			} else if (value > 200) {
				m_scr_var = 100;
			} else {
				value = (int)m_scr_var + value;

				if (value < -100) m_scr_var = -100;
				else if (value > 100) m_scr_var = 100;
				else m_scr_var = value;
			}
		}
		m_psf->m_ssp.sd.scr_var = m_scr_var;
	}

	// シナリオスクリプトの「IF」の構文解析。
	// 引数：pbuf 文字列ポインタ。
	//       plen 文字列長ポインタ。
	void parseIf(char *pbuf, char *plen) {
		char cmpop_type, *ptmp;
		int value;

		pbuf += strspn(pbuf, BLANK_CHAR);
		if (strnicmp(pbuf, "VAR", 3) != 0) return;
		pbuf += 3;

		pbuf += strspn(pbuf, BLANK_CHAR);

		// 比較演算子。
		if (strnicmp(pbuf, "=", 1) == 0 && pbuf + 1 < plen && strspn(pbuf + 1, CMPOP_CHAR) == 0) {
			cmpop_type = 1;
			pbuf++;
		} else if (strnicmp(pbuf, "!=", 2) == 0 && pbuf + 2 < plen && strspn(pbuf + 2, CMPOP_CHAR) == 0) {
			cmpop_type = 2;
			pbuf += 2;
		} else if (strnicmp(pbuf, "<", 1) == 0 && pbuf + 1 < plen && strspn(pbuf + 1, CMPOP_CHAR) == 0) {
			cmpop_type = 3;
			pbuf++;
		} else if (strnicmp(pbuf, ">", 1) == 0 && pbuf + 1 < plen && strspn(pbuf + 1, CMPOP_CHAR) == 0) {
			cmpop_type = 4;
			pbuf++;
		} else if (strnicmp(pbuf, "<=", 2) == 0 && pbuf + 2 < plen && strspn(pbuf + 2, CMPOP_CHAR) == 0) {
			cmpop_type = 5;
			pbuf += 2;
		} else if (strnicmp(pbuf, ">=", 2) == 0 && pbuf + 2 < plen && strspn(pbuf + 2, CMPOP_CHAR) == 0) {
			cmpop_type = 6;
			pbuf += 2;
		} else {
			return;
		}

		pbuf += strspn(pbuf, BLANK_CHAR);

		value = strtol(pbuf, &ptmp, 10);
		if (pbuf == ptmp) return;
		if (value < -100) value = -100;
		else if (value > 100) value = 100;

		switch (cmpop_type) {
		case 1:	// =。
			if (m_scr_var == value) break;
			return;
		case 2:	// !=。
			if (m_scr_var != value) break;
			return;
		case 3:	// <。
			if (m_scr_var < value) break;
			return;
		case 4:	// >。
			if (m_scr_var > value) break;
			return;
		case 5:	// <=。
			if (m_scr_var <= value) break;
			return;
		case 6:	// >=。
			if (m_scr_var >= value) break;
			return;
		default:
			return;
		}

		pbuf = ptmp;
		value = strspn(pbuf, BLANK_CHAR);
		if (value == 0) return;
		pbuf += value;

		if (strnicmp(pbuf, "GOTO", 4) != 0 || pbuf + 4 >= plen || strcspn(pbuf + 4, BLANK_CHAR) > 0) return;
		pbuf += 4;
		parseGoto(pbuf, plen);
	}

	// シナリオスクリプトの「END」の構文解析。
	// 引数：pbuf 文字列ポインタ。
	//       plen 文字列長ポインタ。
	void parseEnding(char *pbuf, char *plen) {
		char *ptmp;
		short time;
		int value;

		pbuf += strspn(pbuf, BLANK_CHAR);

		// type。
		value = strtol(pbuf, &ptmp, 10);
		if (pbuf == ptmp) return;
		if (strcspn(ptmp, BLANK_CHAR ";") > 0) return;

		if (value < 0 || value > 3) m_ending_type = 0;
		else m_ending_type = value;
		setEnding(0);
	}

	// スプライトをクリックした時の処理。
	// 引数：id スプライトID。
	void onClick(BYTE id) {
		char *ptmp;
		BYTE i;

		switch (m_scene) {
		case 2:	// メニュー画面。
			switch (id) {
			case 1:	// ボタン(New Game)。
				m_scene = 20;
				m_frame = 0;

				m_pspr_mng->addFadeOutAction(0, 30);
				m_pspr_mng->addFadeOutAction(1, 30);
				m_pspr_mng->addFadeOutAction(2, 30);
				m_pspr_mng->addFadeOutAction(3, 30);
				break;
			case 2:	// ボタン(Load Game)。
				m_scene = 21;
				m_frame = 0;

				for (i = 1; i <= 3; i++) m_pspr_mng->setEnable(i, false);
				m_pspr_mng->createSpriteColor(21, D3DCOLOR_XRGB(0, 0, 0));
				m_pspr_mng->addFadeInAction(21, 30);
				break;
			case 3:	// ボタン(Exit)。
				Close(false);
				break;
			}
			break;
		case 3:	// 主人公選択画面。
			switch (id) {
			case 1:	// キャラ(Waru)。
				ZeroMemory(&m_psf->m_ssp.sd, sizeof(m_psf->m_ssp.sd));
				m_psf->m_ssp.sd.chara = 0;
				if (!m_pfile->Open(m_scr_info[m_psf->m_ssp.sd.chara].pfilename, "r")) break;
				m_scene = 30;
				m_frame = 0;

				m_pspr_mng->addFadeOutAction(0, 30);
				m_pspr_mng->addPauseAction(1, 20);
				m_pspr_mng->addFadeOutAction(1, 30);
				m_pspr_mng->setEnable(1, false);
				m_pspr_mng->addFadeOutAction(2, 30);
				m_pspr_mng->addFadeOutAction(3, 30);
				m_pspr_mng->addFadeOutAction(4, 30);
				m_pspr_mng->addFadeOutAction(5, 30);
				m_pspr_mng->addFadeOutAction(6, 30);
				m_psnd_mng->Stop(8, 60);
				break;
			case 2:	// キャラ(Iita)。
				ZeroMemory(&m_psf->m_ssp.sd, sizeof(m_psf->m_ssp.sd));
				m_psf->m_ssp.sd.chara = 1;
				if (!m_pfile->Open(m_scr_info[m_psf->m_ssp.sd.chara].pfilename, "r")) break;
				m_scene = 30;
				m_frame = 0;

				m_pspr_mng->addFadeOutAction(0, 30);
				m_pspr_mng->addFadeOutAction(1, 30);
				m_pspr_mng->addPauseAction(2, 20);
				m_pspr_mng->addFadeOutAction(2, 30);
				m_pspr_mng->setEnable(2, false);
				m_pspr_mng->addFadeOutAction(3, 30);
				m_pspr_mng->addFadeOutAction(4, 30);
				m_pspr_mng->addFadeOutAction(5, 30);
				m_pspr_mng->addFadeOutAction(6, 30);
				m_psnd_mng->Stop(8, 60);
				break;
			case 3:	// キャラ(Shizumi)。
				ZeroMemory(&m_psf->m_ssp.sd, sizeof(m_psf->m_ssp.sd));
				m_psf->m_ssp.sd.chara = 2;
				if (!m_pfile->Open(m_scr_info[m_psf->m_ssp.sd.chara].pfilename, "r")) break;
				m_scene = 30;
				m_frame = 0;

				m_pspr_mng->addFadeOutAction(0, 30);
				m_pspr_mng->addFadeOutAction(1, 30);
				m_pspr_mng->addFadeOutAction(2, 30);
				m_pspr_mng->addPauseAction(3, 20);
				m_pspr_mng->addFadeOutAction(3, 30);
				m_pspr_mng->setEnable(3, false);
				m_pspr_mng->addFadeOutAction(4, 30);
				m_pspr_mng->addFadeOutAction(5, 30);
				m_pspr_mng->addFadeOutAction(6, 30);
				m_psnd_mng->Stop(8, 60);
				break;
			case 4:	// キャラ(Al)。
				ZeroMemory(&m_psf->m_ssp.sd, sizeof(m_psf->m_ssp.sd));
				m_psf->m_ssp.sd.chara = 3;
				if (!m_pfile->Open(m_scr_info[m_psf->m_ssp.sd.chara].pfilename, "r")) break;
				m_scene = 30;
				m_frame = 0;

				m_pspr_mng->addFadeOutAction(0, 30);
				m_pspr_mng->addFadeOutAction(1, 30);
				m_pspr_mng->addFadeOutAction(2, 30);
				m_pspr_mng->addFadeOutAction(3, 30);
				m_pspr_mng->addPauseAction(4, 20);
				m_pspr_mng->addFadeOutAction(4, 30);
				m_pspr_mng->setEnable(4, false);
				m_pspr_mng->addFadeOutAction(5, 30);
				m_pspr_mng->addFadeOutAction(6, 30);
				m_psnd_mng->Stop(8, 60);
				break;
			case 5:	// キャラ(KD)。
				ZeroMemory(&m_psf->m_ssp.sd, sizeof(m_psf->m_ssp.sd));
				m_psf->m_ssp.sd.chara = 4;
				if (!m_pfile->Open(m_scr_info[m_psf->m_ssp.sd.chara].pfilename, "r")) break;
				m_scene = 30;
				m_frame = 0;

				m_pspr_mng->addFadeOutAction(0, 30);
				m_pspr_mng->addFadeOutAction(1, 30);
				m_pspr_mng->addFadeOutAction(2, 30);
				m_pspr_mng->addFadeOutAction(3, 30);
				m_pspr_mng->addFadeOutAction(4, 30);
				m_pspr_mng->addPauseAction(5, 20);
				m_pspr_mng->addFadeOutAction(5, 30);
				m_pspr_mng->setEnable(5, false);
				m_pspr_mng->addFadeOutAction(6, 30);
				m_psnd_mng->Stop(8, 60);
				break;
			case 6:	// 戻る。
				m_scene = 31;
				m_frame = 0;

				m_pspr_mng->createSpriteColor(7, D3DCOLOR_XRGB(0, 0, 0));
				m_pspr_mng->addFadeInAction(7, 30);

				m_pspr_mng->setEnable(1, false);
				m_pspr_mng->setEnable(2, false);
				m_pspr_mng->setEnable(3, false);
				m_pspr_mng->setEnable(4, false);
				m_pspr_mng->setEnable(5, false);
				m_pspr_mng->setEnable(6, false);
				break;
			}
			break;
		case 4:	// ゲームロード画面。
		case 5:	// ゲームセーブ画面。
			switch (id) {
			case 17:	// 戻る。
				m_scene = 40;
				m_frame = 0;

				m_pspr_mng->mouseOut(2, true, false);
				m_pspr_mng->mouseOut(11, true, false);
				m_pspr_mng->mouseOut(12, true, false);
				for (i = 23; i <= 40; i++) m_pspr_mng->setEnable(i, false);
				m_pspr_mng->createSpriteColor(48, D3DCOLOR_XRGB(0, 0, 0));
				m_pspr_mng->addFadeInAction(48, 30);
				break;
			case 20:	// いいえ。
				m_close = -2;
				for (i = 23; i <= 40; i++) m_pspr_mng->setEnable(i, true);
				m_pspr_mng->addFadeOutAction(44, 10);
				m_pspr_mng->addFadeOutAction(45, 10);
				m_pspr_mng->addFadeOutAction(46, 10);
				m_pspr_mng->addFadeOutAction(47, 10);
				m_pspr_mng->mouseOut(40, false, true);
				break;
			case 21:	// はい。
				if (m_scene == 4) {
					m_scene = 41;
					m_psnd_mng->Stop(0, 30);
					m_psnd_mng->Stop(8, 30);
				} else {
					m_scene = 50;
				}
				m_frame = 0;

				m_pspr_mng->setEnable(46, false);
				m_pspr_mng->setEnable(47, false);
				m_pspr_mng->createSpriteColor(48, D3DCOLOR_XRGB(0, 0, 0));
				m_pspr_mng->addFadeInAction(48, 30);
				break;
			case 22:	// はい(終了)。
				Close(true);
				break;
			default:
				if (id < 1 || id > 16) break;
				m_id = id - 1;
				for (i = 23; i <= 40; i++) m_pspr_mng->setEnable(i, false);

				if (m_scene == 4 || m_psf->m_ss.sd[m_id].pos > 0) {

					m_pspr_mng->createSpriteColor(44, D3DCOLOR_ARGB(192, 8, 8, 16), 200, 150, 400, 150);
					m_pspr_mng->addFadeInAction(44, 10);

					m_pspr_mng->createSpriteString(45, 310, 190, 200, 50, 25, D3DCOLOR_XRGB(192, 192, 255));
					m_pspr_mng->createSpriteString(46, 308, 240, 50, 27, 25, D3DCOLOR_XRGB(255, 255, 255), D3DCOLOR_XRGB(255, 255, 64), 21);
					m_pspr_mng->createSpriteString(47, 430, 240, 75, 27, 25, D3DCOLOR_XRGB(255, 255, 255), D3DCOLOR_XRGB(255, 255, 64), 20);
					m_pspr_mng->writeString(45, m_scene == 4 ? L"ロードしますか？" : L"上書きしますか？");
					m_pspr_mng->writeString(46, L"はい");
					m_pspr_mng->writeString(47, L"いいえ");
					m_pspr_mng->addFadeInAction(45, 10);
					m_pspr_mng->addFadeInAction(46, 10);
					m_pspr_mng->addFadeInAction(47, 10);
				} else {
					onClick(21);
				}
				break;
			}
			break;
		case 6:	// ゲーム画面。
			switch (id) {
			case 1:	// 次の文章。
				if (!m_wait) break;
				m_wait = false;
				setWait();
				for (i = 1; i <= 5; i++) m_psnd_mng->Stop(i, 20);
				break;
			case 2:	// 選択肢(1)。
			case 3:	// 選択肢(2)。
			case 4:	// 選択肢(3)。
			case 5:	// 選択肢(4)。
				ptmp = m_sel_goto[id - 2];
				if (ptmp[0] != '\0') lstrcpyA(m_goto, ptmp);
				setSelect(NULL, NULL, NULL, NULL, NULL, false);
				break;
			case 10:	// パネル(Load)。
				m_scene = 60;
				m_frame = 0;

				for (i = 1; i <= 20; i++) m_pspr_mng->setEnable(i, false);
				m_pspr_mng->createSpriteColor(21, D3DCOLOR_XRGB(0, 0, 0));
				m_pspr_mng->addFadeInAction(21, 30);
				for (i = 1; i <= 5; i++) m_psnd_mng->Stop(i, 30);
				break;
			case 11:	// パネル(Save)。
				m_scene = 61;
				m_frame = 0;

				for (i = 1; i <= 20; i++) m_pspr_mng->setEnable(i, false);
				m_pspr_mng->createSpriteColor(21, D3DCOLOR_XRGB(0, 0, 0));
				m_pspr_mng->addFadeInAction(21, 30);
				for (i = 1; i <= 5; i++) m_psnd_mng->Stop(i, 30);
				break;
			case 12:	// パネル(Skip)。
				m_skip_flag = !m_skip_flag;
				if (m_wait) {
					onClick(1);	// 待ち状態なら解除する。
				} else {
					for (i = 1; i <= 5; i++) m_psnd_mng->Stop(i, 20);
				}
				break;
			case 13:	// パネル(Menu)。
				m_pspr_mng->setEnable(14, false);
				for (i = 17; i <= 20; i++) m_pspr_mng->setEnable(i, false);
				m_pspr_mng->createSpriteString(21, 0, 0, 800, 600, 1, 0, 0, 99);
				m_pspr_mng->addFadeInAction(21, 0);
				m_skip_flag = false;

				m_pspr_mng->createSpriteColor(44, D3DCOLOR_ARGB(224, 8, 8, 16));
				m_pspr_mng->addFadeInAction(44, 10);
				m_confirm_flag = true;

				m_pspr_mng->createSpriteString(45, 290, 190, 250, 50, 25, D3DCOLOR_XRGB(192, 192, 255));
				m_pspr_mng->createSpriteString(46, 308, 240, 50, 27, 25, D3DCOLOR_XRGB(255, 255, 255), D3DCOLOR_XRGB(255, 255, 64), 21);
				m_pspr_mng->createSpriteString(47, 430, 240, 75, 27, 25, D3DCOLOR_XRGB(255, 255, 255), D3DCOLOR_XRGB(255, 255, 64), 20);
				m_pspr_mng->writeString(45, L"メニューに戻りますか？");
				m_pspr_mng->writeString(46, L"はい");
				m_pspr_mng->writeString(47, L"いいえ");
				m_pspr_mng->addFadeInAction(45, 10);
				m_pspr_mng->addFadeInAction(46, 10);
				m_pspr_mng->addFadeInAction(47, 10);

				m_pspr_mng->stopAction(6);
				break;
			case 20:	// いいえ。
				m_close = -2;
				m_confirm_flag = false;

				m_pspr_mng->setEnable(14, true);
				for (i = 17; i <= 20; i++) m_pspr_mng->setEnable(i, true);
				m_pspr_mng->addFadeOutAction(21, 0);

				m_pspr_mng->addFadeOutAction(44, 10);
				m_pspr_mng->addFadeOutAction(45, 10);
				m_pspr_mng->addFadeOutAction(46, 10);
				m_pspr_mng->addFadeOutAction(47, 10);

				m_pspr_mng->restartAction(6);
				m_pspr_mng->mouseOut(20, false, true);
				break;
			case 21:	// はい(メニュー)。
				m_scene = 62;
				m_frame = 0;

				m_pspr_mng->setEnable(46, false);
				m_pspr_mng->setEnable(47, false);
				m_pspr_mng->createSpriteColor(48, D3DCOLOR_XRGB(0, 0, 0));
				m_pspr_mng->addFadeInAction(48, 30);
				m_psnd_mng->Close(50, true);
				break;
			case 22:	// はい(終了)。
				Close(true);
				break;
			}
			break;
		}
	}
};
