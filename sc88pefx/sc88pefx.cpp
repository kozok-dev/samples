#define _CRT_SECURE_NO_DEPRECATE
#define _CRT_NON_CONFORMING_SWPRINTFS
#define _WIN32_IE 0x0500
#include <stdio.h>
#include <tchar.h>
#include <windows.h>
#include <commctrl.h>
#define WNDID_EFXNAME 1

struct efxname {
	LPTSTR name, desc;
} g_efxname[] = {
	{TEXT("00：Reverb"),
		TEXT("リバーブは音に残響や余韻を与え奥行きのある音像を創り出すエフェクトです。コンサート・ホールで演奏を聴いているような効果があります。")},
	{TEXT("00：Chorus"),
		TEXT("コーラスは音像を拡大し、音に厚みと潤いを与えます。")},
	{TEXT("00：Delay"),
		TEXT("ディレイは音を遅らせ、山びこのような効果を演出します。また、短いディレイ音を原音と重ねて、音に厚みと広がりを持たせることができます。")},
	{TEXT("01：音色を変える(フィルター系) Stereo-EQ(ステレオ・イコライザー)"),
		TEXT("4バンド(低域、中域×2、高域)のステレオのイコライザーです。")},
	{TEXT("02：音色を変える(フィルター系) Spectrum(スペクトラム)"),
		TEXT("スペクトラムはフィルターの一種で、特定の周波数のレベルを増減させて音色を変えます。イコライザーと働きが似ていますが、音色のクセをつけるのに最適な8つの周波数が決められていますので、より特徴のある音が作れます。")},
	{TEXT("03：音色を変える(フィルター系) Enhancer(エンハンサー)"),
		TEXT("エンハンサーは高域の倍音成分をコントロールすることにより、音にメリハリをつけたり、音ヌケをよくしたりします。")},
	{TEXT("04：音色を変える(フィルター系) Humanizer(ヒューマナイザー)"),
		TEXT("人間の声のように、音に母音をつけることができます。")},
	{TEXT("05：音を歪ませる(歪み系) Overdrive(オーバードライブ)"),
		TEXT("オーバードライブは真空管アンプのようなやわらかな歪みが得られます。")},
	{TEXT("06：音を歪ませる(歪み系) Distortion(ディストーション)"),
		TEXT("ディストーションはオーバードライブよりも激しい歪みが得られます。")},
	{TEXT("07：音を揺らす(モジュレーション系) Phaser(フェイザー)"),
		TEXT("フェイザーは原音に位相をずらした音を加えることで音にうねりを与え、広がり感や奥行き感が得られます。")},
	{TEXT("08：音を揺らす(モジュレーション系) Auto Wah(オート・ワウ)"),
		TEXT("オート・ワウはフィルターを周期的に動かすことで、音色を周期的に変化させます。")},
	{TEXT("09：音を揺らす(モジュレーション系) Rotary(ロータリー)"),
		TEXT("ロータリーは往年の回転スピーカー・サウンドをシミュレートしたエフェクターです。高域と低域のローターの動作をそれぞれ独立して設定できるので、独特のうねり感をリアルに再現できます。オルガンに最も効果があります。")},
	{TEXT("10：音を揺らす(モジュレーション系) Stereo Flanger(ステレオ・フランジャー)"),
		TEXT("ステレオ仕様のフランジャーです。ジェット機の上昇音/下降音のような金属的な響きが得られます。フィルターを使ってフランジャー音の音質を調節できます。")},
	{TEXT("11：音を揺らす(モジュレーション系) Step Flanger(ステップ・フランジャー)"),
		TEXT("ステップ・フランジャーはフランジャー音のピッチが段階的に変化するフランジャーです。")},
	{TEXT("12：音を揺らす(モジュレーション系) Tremolo(トレモロ)"),
		TEXT("トレモロは音量を周期的に動かすことで、音にトレモロ効果を与えます。")},
	{TEXT("13：音を揺らす(モジュレーション系) Auto Pan(オート・パン)"),
		TEXT("オート・パンは音の定位を周期的に変化させます。")},
	{TEXT("14：レベルを変える(コンプレッサー系) Compressor(コンプレッサー)"),
		TEXT("コンプレッサーは大きなレベルの音を抑え、小さなレベルの音を持ち上げることで、音量のバラツキを抑えます。")},
	{TEXT("15：レベルを変える(コンプレッサー系) Limiter(リミッター)"),
		TEXT("リミッターは指定の音量レベルより大きな音のみを圧縮し、音の歪みを抑えます。")},
	{TEXT("16：音を拡げる(コーラス系) Hexa Chorus(ヘキサ・コーラス)"),
		TEXT("ヘキサ・コーラスは音に厚みと広がりを与える6相コーラス(6つのコーラス音が重なる)です。")},
	{TEXT("17：音を拡げる(コーラス系) Tremolo Chorus(トレモロ・コーラス)"),
		TEXT("トレモロ・コーラスはトレモロ効果(音量を周期的に揺らす)がつけられるコーラスです。")},
	{TEXT("18：音を拡げる(コーラス系) Stereo Chorus(ステレオ・コーラス)"),
		TEXT("ステレオ仕様のコーラスです。フィルターを使ってコーラス音の音質を調節できます。")},
	{TEXT("19：音を拡げる(コーラス系) Space D(スペースD)"),
		TEXT("スペースDは2相のモジュレーションをステレオでかける多重コーラスです。変調感はありませんが、透明感のあるコーラス効果が得られます。")},
	{TEXT("20：音を拡げる(コーラス系) 3D Chorus(3Dコーラス)"),
		TEXT("コーラス音に3D効果がかかります。コーラス音が90度左と90度右に定位します。")},
	{TEXT("21：音を響かせる(ディレイ/リバーブ系) Stereo Delay(ステレオ・ディレイ)"),
		TEXT("ステレオ仕様のディレイです。")},
	{TEXT("22：音を響かせる(ディレイ/リバーブ系) Mod Delay(モジュレーション・ディレイ)"),
		TEXT("モジュレーション・ディレイはディレイ音に揺れが加えられるエフェクターです。フランジャーのような効果が得られます。")},
	{TEXT("23：音を響かせる(ディレイ/リバーブ系) 3 Tap Delay(トリプル・タップ・ディレイ)"),
		TEXT("トリプル・タップ・ディレイは、中央、左、右の3方向にディレイ音が鳴らせるエフェクターです。")},
	{TEXT("24：音を響かせる(ディレイ/リバーブ系) 4 Tap Delay(クアドラプル・タップ・ディレイ)"),
		TEXT("クアドラプル・タップ・ディレイは4つのディレイを持っています。")},
	{TEXT("25：音を響かせる(ディレイ/リバーブ系) Tm Ctrl Delay(タイム・コントロール・ディレイ)"),
		TEXT("EFX C.Srcで設定した指定のコントローラーを使ってディレイ・タイムとピッチをリアルタイムでコントロールできます。ディレイ・タイムを長くするとピッチが下がり、短くするとピッチが上がります。")},
	{TEXT("26：音を響かせる(ディレイ/リバーブ系) Reverb(リバーブ)"),
		TEXT("リバーブは原音に残響音を加え、空間の広さをシミュレートします。")},
	{TEXT("27：音を響かせる(ディレイ/リバーブ系) Gate Reverb(ゲート・リバーブ)"),
		TEXT("ゲート・リバーブは残響音を途中でカットするリバーブです。")},
	{TEXT("28：音を響かせる(ディレイ/リバーブ系) 3D Delay(3Dディレイ)"),
		TEXT("ディレイ音に3D効果がかかります。ディレイ音が90度左と90度右に定位します。")},
	{TEXT("29：音程を変える(ピッチ・シフト系) 2 Pitch Shifter(2ボイス・ピッチ・シフター)"),
		TEXT("ピッチ・シフターは原音のピッチをずらすエフェクターです。2ボイス・ピッチ・シフターは2つのピッチ・シフターを持ち、ピッチをずらせた2つの音を原音に重ねて鳴らすことができます。")},
	{TEXT("30：音程を変える(ピッチ・シフト系) Fb P.Shifter(フィードバック・ピッチ・シフター)"),
		TEXT("ピッチ・シフト音を入力に戻すことができるピッチ・シフターです。")},
	{TEXT("31：その他 3D Auto(3Dオート)"),
		TEXT("3Dオートは定位を回転させるエフェクトです。")},
	{TEXT("32：その他 3D Manual(3Dマニュアル)"),
		TEXT("3Dの効果を任意の位置に定位させます。")},
	{TEXT("33：その他 Lo-Fi 1(ロー・ファイ1)"),
		TEXT("ロー・ファイ1は音質を粗くするエフェクターです。")},
	{TEXT("34：その他 Lo-Fi 2(ロー・ファイ2)"),
		TEXT("ロー・ファイ2は音質を粗くした音に、いろいろなノイズを加えることができます。\n＊ R.Detune(ラジオ・デチューン)、W/P Level(ホワイト/ピンク・ノイズ・レベル)、Disc Nz Lev(ディスク・ノイズ・レベル)、Hum Level(ハム・ノイズ・レベル)の値を大きくすると、音が鳴っていないときにもノイズ音が鳴ります。")},
	{TEXT("35：2種類のエフェクト直列接続(直列2) OD→Chorus(オーバードライブ→コーラス)"),
		TEXT("オーバードライブとコーラスを直列に接続しています。")},
	{TEXT("36：2種類のエフェクト直列接続(直列2) OD→Flanger(オーバードライブ→フランジャー)"),
		TEXT("オーバードライブとフランジャーを直列に接続しています。")},
	{TEXT("37：2種類のエフェクト直列接続(直列2) OD→Delay(オーバードライブ→ディレイ)"),
		TEXT("オーバードライブとディレイを直列に接続しています。")},
	{TEXT("38：2種類のエフェクト直列接続(直列2) DS→Chorus(ディストーション→コーラス)"),
		TEXT("ディストーションとコーラスを直列に接続しています。")},
	{TEXT("39：2種類のエフェクト直列接続(直列2) DS→Flanger(ディストーション→フランジャー)"),
		TEXT("ディストーションとフランジャーを直列に接続しています。")},
	{TEXT("40：2種類のエフェクト直列接続(直列2) DS→Delay(ディストーション→ディレイ)"),
		TEXT("ディストーションとディレイを直列に接続しています。")},
	{TEXT("41：2種類のエフェクト直列接続(直列2) EH→Chorus(エンハンサー→コーラス)"),
		TEXT("エンハンサーとコーラスを直列に接続しています。")},
	{TEXT("42：2種類のエフェクト直列接続(直列2) EH→Flanger(エンハンサー→フランジャー)"),
		TEXT("エンハンサーとフランジャーを直列に接続しています。")},
	{TEXT("43：2種類のエフェクト直列接続(直列2) EH→Delay(エンハンサー→ディレイ)"),
		TEXT("エンハンサーとディレイを直列に接続しています。")},
	{TEXT("44：2種類のエフェクト直列接続(直列2) Cho→Delay(コーラス→ディレイ)"),
		TEXT("コーラスとディレイを直列に接続しています。")},
	{TEXT("45：2種類のエフェクト直列接続(直列2) FL→Delay(フランジャー→ディレイ)"),
		TEXT("フランジャーとディレイを直列に接続しています。")},
	{TEXT("46：2種類のエフェクト直列接続(直列2) Cho→Flanger(コーラス→フランジャー)"),
		TEXT("コーラスとフランジャーを直列に接続しています。")},
	{TEXT("47：3種類以上のエフェクト直列接続(直列3/直列4/直列5) Rotary Multi(ロータリー・マルチ)"),
		TEXT("オーバードライブ(OD)と3バンド・イコライザー(EQ)とロータリー(RT)を直列に接続しています。")},
	{TEXT("48：3種類以上のエフェクト直列接続(直列3/直列4/直列5) GTR Multi 1(ギター・マルチ1)"),
		TEXT("ギター・マルチ1はコンプレッサー(Cmp)、オーバードライブまたはディストーション(OD)、コーラスまたはフランジャー(CF)、ディレイ(Dly)を直列に接続しています。")},
	{TEXT("49：3種類以上のエフェクト直列接続(直列3/直列4/直列5) GTR Multi 2(ギター・マルチ2)"),
		TEXT("ギター・マルチ2はコンプレッサー(Cmp)、オーバードライブまたはディストーション(OD)、イコライザー(EQ)、コーラスまたはフランジャー(CF)を直列に接続しています。")},
	{TEXT("50：3種類以上のエフェクト直列接続(直列3/直列4/直列5) GTR Multi 3(ギター・マルチ3)"),
		TEXT("ギター・マルチ3はワウ(Wah)、オーバードライブまたはディストーション(OD)、コーラスまたはフランジャー(CF)、ディレイ(Dly)を直列に接続しています。")},
	{TEXT("51：3種類以上のエフェクト直列接続(直列3/直列4/直列5) Clean Gt Multi1(クリーン・ギター・マルチ1)"),
		TEXT("コンプレッサー(Cmp)、イコライザー(EQ)、コーラスまたはフランジャー(CF)、ディレイ(Dly)を直列に接続しています。")},
	{TEXT("52：3種類以上のエフェクト直列接続(直列3/直列4/直列5) Clean Gt Multi2(クリーン・ギター・マルチ2)"),
		TEXT("オート・ワウ(AW)、イコライザー(EQ)、コーラスまたはフランジャー(CF)、ディレイ(Dly)を直列に接続しています。")},
	{TEXT("53：3種類以上のエフェクト直列接続(直列3/直列4/直列5) Bass Multi(ベース・マルチ)"),
		TEXT("コンプレッサー(Cmp)、オーバードライブまたはディストーション(OD)、イコライザー(EQ)、コーラスまたはフランジャー(CF)を直列に接続しています。")},
	{TEXT("54：3種類以上のエフェクト直列接続(直列3/直列4/直列5) Rhodes Multi(ローズ・マルチ)"),
		TEXT("エンハンサー(EH)、フェイザー(PH)、コーラスまたはフランジャー(CF)、トレモロまたはパン(TP)を直列に接続しています。")},
	{TEXT("55：3種類以上のエフェクト直列接続(直列3/直列4/直列5) Keyboard Multi(キーボード・マルチ)"),
		TEXT("リング・モジュレーター(RM)、イコライザー(EQ)、ピッチ・シフター(PS)、フェイザー(PH)、ディレイ(Dly)を直列に接続しています。リング・モジュレーターは、入力信号に振幅変調(AM変調)をかけることによりベルのような音を出すことができるエフェクターです。")},
	{TEXT("56：2種類のエフェクトを並列接続(並列2) Cho/Delay(コーラス、ディレイ)"),
		TEXT("コーラスとディレイを並列に接続しています。")},
	{TEXT("57：2種類のエフェクトを並列接続(並列2) FL/Delay(フランジャー、ディレイ)"),
		TEXT("フランジャーとディレイを並列に接続しています。")},
	{TEXT("58：2種類のエフェクトを並列接続(並列2) Cho/Flanger(コーラス、フランジャー)"),
		TEXT("コーラスとフランジャーを並列に接続しています。")},
	{TEXT("59：2種類のエフェクトを並列接続(並列2) OD1/OD2(オーバードライブ/ディストーション1、2)"),
		TEXT("オーバードライブまたはディストーションを選べるエフェクター2セットを並列に接続しています。")},
	{TEXT("60：2種類のエフェクトを並列接続(並列2) OD/Rotary(オーバードライブ/ディストーション、ロータリー)"),
		TEXT("オーバードライブまたはディストーションとロータリーを並列に接続しています。")},
	{TEXT("61：2種類のエフェクトを並列接続(並列2) OD/Phaser(オーバードライブ/ディストーション、フェイザー)"),
		TEXT("オーバードライブまたはディストーションとフェイザーを並列に接続しています。")},
	{TEXT("62：2種類のエフェクトを並列接続(並列2) OD/AutoWah(オーバードライブ/ディストーション、オート・ワウ)"),
		TEXT("オーバードライブまたはディストーションとオート・ワウを並列に接続しています。")},
	{TEXT("63：2種類のエフェクトを並列接続(並列2) PH/Rotary(フェイザー、ロータリー)"),
		TEXT("フェイザーとロータリーを並列に接続しています。")},
	{TEXT("64：2種類のエフェクトを並列接続(並列2) PH/Auto Wah(フェイザー、オート・ワウ)"),
		TEXT("フェイザーとオート・ワウを並列に接続しています。")}
};
HINSTANCE g_hinst;
HFONT g_hfont;

class Efx {
	static const char MAX_WND = 30;	// 最大パラメータ数。
	static const char MAX_VALUE = 15;	// 値の最大文字数。
	static const char MAX_SYSEX = 35;	// システムエクスクルーシブの最大文字数。
	static const short VALUE_RANGE = 128;	// 設定できる値の範囲。
	/*
	* [コントロールID仕様]
	*  0001  nnnn             nnnnnnnn
	*  1固定 コントロール種別 パラメータインデックス
	*/
	static const WORD IDMASK_CB_VAL = 0x1100;	// 値設定コンボボックスIDマスク。
	static const WORD IDMASK_EDT_SYSEX = 0x1200;	// システムエクスクルーシブテキストボックスIDマスク。
	static const WORD IDMASK_BTN_COPY = 0x1300;	// システムエクスクルーシブコピーボタンIDマスク。
	static const WORD MASK_CTRLID = 0xff00;	// コントロールIDマスク。
	static const WORD MASK_INDEX = 0x00ff;	// パラメータインデックスマスク。

public:
	static struct ParamValue {
		typedef BYTE value_type;
		value_type value;
		LPTSTR text;
	};

private:
	class Param {
		char m_addr, m_def_idx;
		ParamValue::value_type m_value[VALUE_RANGE];
		HWND m_hwstc_name, m_hwtt_name, m_hwcb_value, m_hwedt_sysex, m_hwbtn_copy;

	public:
		Param() {
			m_hwstc_name = m_hwtt_name = m_hwcb_value = m_hwedt_sysex = m_hwbtn_copy = NULL;
		}

		/*
		* パラメータを設定できるウィンドウ群を作成します。
		* @param hwnd 親ウィンドウハンドル。
		* @param index パラメータインデックス
		* @param addr システムエクスクルーシブアドレス。
		* @param pos_y 表示位置Y
		* @param name パラメータ名。
		* @param descname 説明パラメータ名。
		* @param desc 説明。
		* @param ppv パラメータリスト。
		* @param range パラメータ範囲。
		* @param def_idx デフォルト値。
		* @return bool 成功:true、失敗:false。
		*/
		bool Create(HWND hwnd, char index, char addr, short pos_y, LPCTSTR name,
		LPCTSTR descname, LPTSTR desc, const ParamValue *ppv, short range, char def_idx) {
			short i;
			TOOLINFO ti;

			m_addr = addr;
			m_def_idx = def_idx;

			// パラメータ名スタティック作成。
			if ((m_hwstc_name = CreateWindowEx(WS_EX_STATICEDGE, TEXT("STATIC"), name,
			WS_CHILD | WS_VISIBLE, 10, pos_y + 2, 100, 15, hwnd, NULL, g_hinst, NULL)) == NULL) return false;
			SendMessage(m_hwstc_name, WM_SETFONT, (WPARAM)g_hfont, FALSE);

			// パラメータ名ツールチップ作成。
			if ((m_hwtt_name = CreateWindow(TOOLTIPS_CLASS, NULL,
			TTS_ALWAYSTIP | TTS_BALLOON | TTS_NOPREFIX,
			CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
			hwnd, NULL, g_hinst, NULL)) == NULL)
				return false;
			ti.cbSize = sizeof(TOOLINFO);
			ti.uFlags = TTF_SUBCLASS | TTF_TRANSPARENT;
			ti.hwnd = hwnd;
			ti.uId = 0;
			ti.rect.left = 10;
			ti.rect.top = pos_y + 2;
			ti.rect.right = 110;
			ti.rect.bottom = pos_y + 17;
			ti.hinst = g_hinst;
			ti.lpszText = desc;
			ti.lParam = 0;
			SendMessage(m_hwtt_name, TTM_SETDELAYTIME, TTDT_AUTOPOP, MAKELPARAM(32767, 0));
			SendMessage(m_hwtt_name, TTM_SETDELAYTIME, TTDT_INITIAL, MAKELPARAM(50, 0));
			SendMessage(m_hwtt_name, TTM_SETMAXTIPWIDTH, 0, 300);
			if (!SendMessage(m_hwtt_name, TTM_SETTITLE, 1, (LPARAM)descname)) return false;
			if (!SendMessage(m_hwtt_name, TTM_ADDTOOL, 0, (LPARAM)&ti)) return false;

			// 値設定コンボボックス作成。
			if ((m_hwcb_value = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("COMBOBOX"), NULL,
			WS_CHILD | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWNLIST, 115, pos_y, 70, CW_USEDEFAULT,
			hwnd, (HMENU)(IDMASK_CB_VAL + index), g_hinst, NULL)) == NULL) return false;
			SendMessage(m_hwcb_value, WM_SETFONT, (WPARAM)g_hfont, FALSE);
			for (i = 0; i < range; i++) {
				m_value[i] = ppv[i].value;
				if (SendMessage(m_hwcb_value, CB_ADDSTRING, 0, (LPARAM)ppv[i].text) < 0) return false;
			}
			SendMessage(m_hwcb_value, CB_SETCURSEL, def_idx, 0);

			// システムエクスクルーシブテキストボックス作成。
			if ((m_hwedt_sysex = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), NULL,
			WS_CHILD | WS_VISIBLE | ES_READONLY, 190, pos_y + 1, 205, 18,
			hwnd, (HMENU)(IDMASK_EDT_SYSEX + index), g_hinst, NULL)) == NULL) return false;
			SendMessage(m_hwedt_sysex, WM_SETFONT, (WPARAM)g_hfont, FALSE);
			setSysEx();

			// システムエクスクルーシブコピーボタン作成。
			if ((m_hwbtn_copy = CreateWindow(TEXT("BUTTON"), TEXT("Copy"),
			WS_CHILD | WS_DISABLED | WS_VISIBLE, 400, pos_y, 50, 20,
			hwnd, (HMENU)(IDMASK_BTN_COPY + index), g_hinst, NULL)) == NULL) return false;
			SendMessage(m_hwbtn_copy, WM_SETFONT, (WPARAM)g_hfont, FALSE);

			return true;
		}

		/*
		* パラメータを設定できるウィンドウ群を破棄します。
		* @return bool 成功:true、失敗:false。
		*/
		bool Delete() {
			bool succ = true;
			if (m_hwstc_name != NULL && DestroyWindow(m_hwstc_name)) m_hwstc_name = NULL;
			else succ = false;
			if (m_hwtt_name != NULL && DestroyWindow(m_hwtt_name)) m_hwtt_name = NULL;
			else succ = false;
			if (m_hwcb_value != NULL && DestroyWindow(m_hwcb_value)) m_hwcb_value = NULL;
			else succ = false;
			if (m_hwedt_sysex != NULL && DestroyWindow(m_hwedt_sysex)) m_hwedt_sysex = NULL;
			else succ = false;
			if (m_hwbtn_copy != NULL && DestroyWindow(m_hwbtn_copy)) m_hwbtn_copy = NULL;
			else succ = false;
			return succ;
		}

		/*
		* システムエクスクルーシブを設定します。
		*/
		void setSysEx() {
			short i;
			TCHAR buf[MAX_SYSEX];

			if (m_hwcb_value == NULL || m_hwedt_sysex == NULL) return;
			if ((i = SendMessage(m_hwcb_value, CB_GETCURSEL, 0, 0)) == CB_ERR) return;
			if (_stprintf(buf, TEXT("F0 41 10 42 12 40 03 %02X %02X %02X F7"), m_addr, m_value[i],
			128 - (0x40 + 0x03 + m_addr + m_value[i]) % 128) == -1) return;

			EnableWindow(m_hwbtn_copy, m_def_idx != i);
			SetWindowText(m_hwedt_sysex, buf);
		}

		/*
		* システムエクスクルーシブをクリップボードにコピーします。
		*/
		void copySysEx() {
			char len;
			TCHAR buf[MAX_SYSEX];
			LPTSTR pstr;
			HGLOBAL hg;

			if (!OpenClipboard(NULL)) return;
			if (EmptyClipboard() &&
			(len = GetWindowText(m_hwedt_sysex, buf, sizeof(buf) / sizeof(buf[0]))) > 0 &&
			(hg = GlobalAlloc(GMEM_MOVEABLE, (len + 1) * sizeof(TCHAR))) != NULL)
				if ((pstr = (LPTSTR)GlobalLock(hg)) != NULL) {
					CopyMemory(pstr, buf, len);
					buf[len] = TEXT('\0');
					GlobalUnlock(hg);
					SetClipboardData(CF_TEXT, hg);
				} else
					GlobalFree(hg);
			CloseClipboard();
		}
	};

	char m_param_cnt;
	HWND m_hwnd;
	Param m_param[MAX_WND];

	/*
	* エラーを表示して親ウィンドウを破棄します。
	* この関数の実行後に処理を継続しないでください。
	* @return bool 常にfalse。
	*/
	bool Error() {
		MessageBox(m_hwnd, TEXT("致命的エラーが発生しました。終了します。"), NULL, MB_ICONERROR);
		DestroyWindow(m_hwnd);
		return false;
	}

public:
	Efx(HWND hwnd) {
		m_param_cnt = 0;
		m_hwnd = hwnd;
	}

	/*
	* パラメータを設定できるウィンドウ群を追加します。
	* この関数の失敗後に処理を継続しないでください。
	* @param addr システムエクスクルーシブアドレス。
	* @param name パラメータ名。
	* @param descname 説明パラメータ名。
	* @param desc 説明。
	* @param ppv パラメータリスト。
	* @param range パラメータ範囲。
	* @param def_idx デフォルト値。
	* @return bool 成功:true、失敗:false。
	*/
	bool Add(char addr, LPCTSTR name, LPCTSTR descname, LPTSTR desc,
	const ParamValue *ppv, short range, char def_idx) {
		if (!m_param[m_param_cnt].Create(m_hwnd, m_param_cnt, addr, m_param_cnt * 22 + 50,
		name, descname, desc, ppv, range, def_idx)) return Error();
		m_param_cnt++;
		return true;
	}

	/*
	* パラメータを設定できるウィンドウ群を範囲指定で追加します。
	* この関数の失敗後に処理を継続しないでください。
	* @param addr システムエクスクルーシブアドレス。
	* @param name パラメータ名。
	* @param descname 説明パラメータ名。
	* @param desc 説明。
	* @param def_idx デフォルト値。
	* @param val_min 値の最小値。
	* @param val_max 値の最大値。
	* @param text_min 表示での最小値。
	* @param text_fmt 表示形式。
	* @param text_step 表示増加数。
	* @return bool 成功:true、失敗:false。
	*/
	bool Add(char addr, LPCTSTR name, LPCTSTR descname, LPTSTR desc, char def_idx,
	char val_min = 0, char val_max = 127, char text_min = 0, LPCTSTR text_fmt = TEXT("%d"),
	char text_step = 1) {
		short i = 0;
		bool succ;
		ParamValue pv[VALUE_RANGE];

		while (val_min + i <= val_max) {
			pv[i].value = val_min + i;
			try {
				pv[i].text = new TCHAR[MAX_VALUE];
			} catch (...) {
				return Error();
			}
			if (_stprintf(pv[i].text, text_fmt, text_min + i * text_step) == -1) return Error();
			i++;
		}

		succ = Add(addr, name, descname, desc, pv, i, def_idx);
		while (--i >= 0) delete[] pv[i].text;
		return succ;
	}

	/*
	* パラメータを設定できるウィンドウ群を表示テキスト設定済み範囲指定で追加します。
	* この関数の失敗後に処理を継続しないでください。
	* @param addr システムエクスクルーシブアドレス。
	* @param name パラメータ名。
	* @param descname 説明パラメータ名。
	* @param desc 説明。
	* @param ptext 表示テキストリスト。
	* @param range パラメータ範囲。
	* @param def_idx デフォルト値。
	* @param val_min 値の最小値。
	* @return bool 成功:true、失敗:false。
	*/
	bool Add(char addr, LPCTSTR name, LPCTSTR descname, LPTSTR desc, LPTSTR *ptext,
	short range, char def_idx, char val_min = 0) {
		short i;
		ParamValue pv[VALUE_RANGE];

		for (i = 0; i < range; i++) {
			pv[i].value = val_min + i;
			pv[i].text = ptext[i];
		}
		return Add(addr, name, descname, desc, pv, range, def_idx);
	}

	/*
	* パラメータを設定できるウィンドウ群を全て破棄します。
	* この関数の失敗後に処理を継続しないでください。
	* @return bool 成功:true、失敗:false。
	*/
	bool Delete() {
		while (m_param_cnt > 0) {
			if (!m_param[m_param_cnt - 1].Delete()) return Error();
			m_param_cnt--;
		}
		return true;
	}

	/*
	* 親ウィンドウからのWM_COMMANDを処理します。
	* @param code コントロールコード。
	* @param id コントロールID。
	* @param hwnd ウィンドウハンドル。
	*/
	void Command(WORD code, WORD id, HWND hwnd) {
		switch (id & MASK_CTRLID) {
		case IDMASK_CB_VAL:
			switch (code) {
			case CBN_SELCHANGE:
				m_param[id & MASK_INDEX].setSysEx();
				break;
			}
			break;
		case IDMASK_BTN_COPY:
			switch (code) {
			case BN_CLICKED:
				m_param[id & MASK_INDEX].copySysEx();
				break;
			}
			break;
		}
	}
};

LPTSTR g_efxpv1[] = {
	TEXT("0.0"), TEXT("0.1"), TEXT("0.2"), TEXT("0.3"), TEXT("0.4"), TEXT("0.5"), TEXT("0.6"), TEXT("0.7"),
	TEXT("0.8"), TEXT("0.9"), TEXT("1.0"), TEXT("1.1"), TEXT("1.2"), TEXT("1.3"), TEXT("1.4"), TEXT("1.5"),
	TEXT("1.6"), TEXT("1.7"), TEXT("1.8"), TEXT("1.9"), TEXT("2.0"), TEXT("2.1"), TEXT("2.2"), TEXT("2.3"),
	TEXT("2.4"), TEXT("2.5"), TEXT("2.6"), TEXT("2.7"), TEXT("2.8"), TEXT("2.9"), TEXT("3.0"), TEXT("3.1"),
	TEXT("3.2"), TEXT("3.3"), TEXT("3.4"), TEXT("3.5"), TEXT("3.6"), TEXT("3.7"), TEXT("3.8"), TEXT("3.9"),
	TEXT("4.0"), TEXT("4.1"), TEXT("4.2"), TEXT("4.3"), TEXT("4.4"), TEXT("4.5"), TEXT("4.6"), TEXT("4.7"),
	TEXT("4.8"), TEXT("4.9"), TEXT("5.0"), TEXT("5.5"), TEXT("6.0"), TEXT("6.5"), TEXT("7.0"), TEXT("7.5"),
	TEXT("8.0"), TEXT("8.5"), TEXT("9.0"), TEXT("9.5"), TEXT("10"), TEXT("11"), TEXT("12"), TEXT("13"),
	TEXT("14"), TEXT("15"), TEXT("16"), TEXT("17"), TEXT("18"), TEXT("19"), TEXT("20"), TEXT("21"),
	TEXT("22"), TEXT("23"), TEXT("24"), TEXT("25"), TEXT("26"), TEXT("27"), TEXT("28"), TEXT("29"),
	TEXT("30"), TEXT("31"), TEXT("32"), TEXT("33"), TEXT("34"), TEXT("35"), TEXT("36"), TEXT("37"),
	TEXT("38"), TEXT("39"), TEXT("40"), TEXT("41"), TEXT("42"), TEXT("43"), TEXT("44"), TEXT("45"),
	TEXT("46"), TEXT("47"), TEXT("48"), TEXT("49"), TEXT("50"), TEXT("52"), TEXT("54"), TEXT("56"),
	TEXT("58"), TEXT("60"), TEXT("62"), TEXT("64"), TEXT("66"), TEXT("68"), TEXT("70"), TEXT("72"),
	TEXT("74"), TEXT("76"), TEXT("78"), TEXT("80"), TEXT("82"), TEXT("84"), TEXT("86"), TEXT("88"),
	TEXT("90"), TEXT("92"), TEXT("94"), TEXT("96"), TEXT("98"), TEXT("100")
}, g_efxpv2[] = {
	TEXT("200"), TEXT("205"), TEXT("210"), TEXT("215"), TEXT("220"), TEXT("225"), TEXT("230"), TEXT("235"),
	TEXT("240"), TEXT("245"), TEXT("250"), TEXT("255"), TEXT("260"), TEXT("265"), TEXT("270"), TEXT("275"),
	TEXT("280"), TEXT("285"), TEXT("290"), TEXT("295"), TEXT("300"), TEXT("305"), TEXT("310"), TEXT("315"),
	TEXT("320"), TEXT("325"), TEXT("330"), TEXT("335"), TEXT("340"), TEXT("345"), TEXT("350"), TEXT("355"),
	TEXT("360"), TEXT("365"), TEXT("370"), TEXT("375"), TEXT("380"), TEXT("385"), TEXT("390"), TEXT("395"),
	TEXT("400"), TEXT("405"), TEXT("410"), TEXT("415"), TEXT("420"), TEXT("425"), TEXT("430"), TEXT("435"),
	TEXT("440"), TEXT("445"), TEXT("450"), TEXT("455"), TEXT("460"), TEXT("465"), TEXT("470"), TEXT("475"),
	TEXT("480"), TEXT("485"), TEXT("490"), TEXT("495"), TEXT("500"), TEXT("505"), TEXT("510"), TEXT("515"),
	TEXT("520"), TEXT("525"), TEXT("530"), TEXT("535"), TEXT("540"), TEXT("545"), TEXT("550"), TEXT("560"),
	TEXT("570"), TEXT("580"), TEXT("590"), TEXT("600"), TEXT("610"), TEXT("620"), TEXT("630"), TEXT("640"),
	TEXT("650"), TEXT("660"), TEXT("670"), TEXT("680"), TEXT("690"), TEXT("700"), TEXT("710"), TEXT("720"),
	TEXT("730"), TEXT("740"), TEXT("750"), TEXT("760"), TEXT("770"), TEXT("780"), TEXT("790"), TEXT("800"),
	TEXT("810"), TEXT("820"), TEXT("830"), TEXT("840"), TEXT("850"), TEXT("860"), TEXT("870"), TEXT("880"),
	TEXT("890"), TEXT("900"), TEXT("910"), TEXT("920"), TEXT("930"), TEXT("940"), TEXT("950"), TEXT("960"),
	TEXT("970"), TEXT("980"), TEXT("990"), TEXT("1000")
}, g_efxpv3[] = {
	TEXT("200"), TEXT("205"), TEXT("210"), TEXT("215"), TEXT("220"), TEXT("225"), TEXT("230"), TEXT("235"),
	TEXT("240"), TEXT("245"), TEXT("250"), TEXT("255"), TEXT("260"), TEXT("265"), TEXT("270"), TEXT("275"),
	TEXT("280"), TEXT("285"), TEXT("290"), TEXT("295"), TEXT("300"), TEXT("305"), TEXT("310"), TEXT("315"),
	TEXT("320"), TEXT("325"), TEXT("330"), TEXT("335"), TEXT("340"), TEXT("345"), TEXT("350"), TEXT("355"),
	TEXT("360"), TEXT("365"), TEXT("370"), TEXT("375"), TEXT("380"), TEXT("385"), TEXT("390"), TEXT("395"),
	TEXT("400"), TEXT("405"), TEXT("410"), TEXT("415"), TEXT("420"), TEXT("425"), TEXT("430"), TEXT("435"),
	TEXT("440"), TEXT("445"), TEXT("450"), TEXT("455"), TEXT("460"), TEXT("465"), TEXT("470"), TEXT("475"),
	TEXT("480"), TEXT("485"), TEXT("490"), TEXT("495"), TEXT("500"), TEXT("505"), TEXT("510"), TEXT("515"),
	TEXT("520"), TEXT("525"), TEXT("530"), TEXT("535"), TEXT("540"), TEXT("545"), TEXT("550"), TEXT("555"),
	TEXT("560"), TEXT("565"), TEXT("570"), TEXT("575"), TEXT("580"), TEXT("585"), TEXT("590"), TEXT("595"),
	TEXT("600"), TEXT("610"), TEXT("620"), TEXT("630"), TEXT("640"), TEXT("650"), TEXT("660"), TEXT("670"),
	TEXT("680"), TEXT("690"), TEXT("700"), TEXT("710"), TEXT("720"), TEXT("730"), TEXT("740"), TEXT("750"),
	TEXT("760"), TEXT("770"), TEXT("780"), TEXT("790"), TEXT("800"), TEXT("810"), TEXT("820"), TEXT("830"),
	TEXT("840"), TEXT("850"), TEXT("860"), TEXT("870"), TEXT("880"), TEXT("890"), TEXT("900"), TEXT("910"),
	TEXT("920"), TEXT("930"), TEXT("940"), TEXT("950"), TEXT("960"), TEXT("970"), TEXT("980"), TEXT("990"),
	TEXT("1000")
}, g_efxpv4[] = {
	TEXT("0.0"), TEXT("0.1"), TEXT("0.2"), TEXT("0.3"), TEXT("0.4"), TEXT("0.5"), TEXT("0.6"), TEXT("0.7"),
	TEXT("0.8"), TEXT("0.9"), TEXT("1.0"), TEXT("1.1"), TEXT("1.2"), TEXT("1.3"), TEXT("1.4"), TEXT("1.5"),
	TEXT("1.6"), TEXT("1.7"), TEXT("1.8"), TEXT("1.9"), TEXT("2.0"), TEXT("2.1"), TEXT("2.2"), TEXT("2.3"),
	TEXT("2.4"), TEXT("2.5"), TEXT("2.6"), TEXT("2.7"), TEXT("2.8"), TEXT("2.9"), TEXT("3.0"), TEXT("3.1"),
	TEXT("3.2"), TEXT("3.3"), TEXT("3.4"), TEXT("3.5"), TEXT("3.6"), TEXT("3.7"), TEXT("3.8"), TEXT("3.9"),
	TEXT("4.0"), TEXT("4.1"), TEXT("4.2"), TEXT("4.3"), TEXT("4.4"), TEXT("4.5"), TEXT("4.6"), TEXT("4.7"),
	TEXT("4.8"), TEXT("4.9"), TEXT("5.0"), TEXT("5.5"), TEXT("6.0"), TEXT("6.5"), TEXT("7.0"), TEXT("7.5"),
	TEXT("8.0"), TEXT("8.5"), TEXT("9.0"), TEXT("9.5"), TEXT("10"), TEXT("11"), TEXT("12"), TEXT("13"),
	TEXT("14"), TEXT("15"), TEXT("16"), TEXT("17"), TEXT("18"), TEXT("19"), TEXT("20"), TEXT("21"),
	TEXT("22"), TEXT("23"), TEXT("24"), TEXT("25"), TEXT("26"), TEXT("27"), TEXT("28"), TEXT("29"),
	TEXT("30"), TEXT("31"), TEXT("32"), TEXT("33"), TEXT("34"), TEXT("35"), TEXT("36"), TEXT("37"),
	TEXT("38"), TEXT("39"), TEXT("40"), TEXT("50"), TEXT("60"), TEXT("70"), TEXT("80"), TEXT("90"),
	TEXT("100"), TEXT("110"), TEXT("120"), TEXT("130"), TEXT("140"), TEXT("150"), TEXT("160"), TEXT("170"),
	TEXT("180"), TEXT("190"), TEXT("200"), TEXT("210"), TEXT("220"), TEXT("230"), TEXT("240"), TEXT("250"),
	TEXT("260"), TEXT("270"), TEXT("280"), TEXT("290"), TEXT("300"), TEXT("320"), TEXT("340"), TEXT("360"),
	TEXT("380"), TEXT("400"), TEXT("420"), TEXT("440"), TEXT("460"), TEXT("480"), TEXT("500")
}, g_efxpv5[] = {
	TEXT("0"), TEXT("5"), TEXT("10"), TEXT("15"), TEXT("20"), TEXT("25"), TEXT("30"), TEXT("35"),
	TEXT("40"), TEXT("45"), TEXT("50"), TEXT("55"), TEXT("60"), TEXT("65"), TEXT("70"), TEXT("75"),
	TEXT("80"), TEXT("85"), TEXT("90"), TEXT("95"), TEXT("100"), TEXT("105"), TEXT("110"), TEXT("115"),
	TEXT("120"), TEXT("125"), TEXT("130"), TEXT("135"), TEXT("140"), TEXT("145"), TEXT("150"), TEXT("155"),
	TEXT("160"), TEXT("165"), TEXT("170"), TEXT("175"), TEXT("180"), TEXT("185"), TEXT("190"), TEXT("195"),
	TEXT("200"), TEXT("205"), TEXT("210"), TEXT("215"), TEXT("220"), TEXT("225"), TEXT("230"), TEXT("235"),
	TEXT("240"), TEXT("245"), TEXT("250"), TEXT("255"), TEXT("260"), TEXT("265"), TEXT("270"), TEXT("275"),
	TEXT("280"), TEXT("285"), TEXT("290"), TEXT("295"), TEXT("300"), TEXT("305"), TEXT("310"), TEXT("315"),
	TEXT("320"), TEXT("325"), TEXT("330"), TEXT("335"), TEXT("340"), TEXT("345"), TEXT("350"), TEXT("355"),
	TEXT("360"), TEXT("365"), TEXT("370"), TEXT("375"), TEXT("380"), TEXT("385"), TEXT("390"), TEXT("395"),
	TEXT("400"), TEXT("405"), TEXT("410"), TEXT("415"), TEXT("420"), TEXT("425"), TEXT("430"), TEXT("435"),
	TEXT("440"), TEXT("445"), TEXT("450"), TEXT("455"), TEXT("460"), TEXT("465"), TEXT("470"), TEXT("475"),
	TEXT("480"), TEXT("485"), TEXT("490"), TEXT("495"), TEXT("500"), TEXT("505"), TEXT("510"), TEXT("515"),
	TEXT("520"), TEXT("525"), TEXT("530"), TEXT("535"), TEXT("540"), TEXT("545"), TEXT("550"), TEXT("555"),
	TEXT("560"), TEXT("565"), TEXT("570"), TEXT("575"), TEXT("580"), TEXT("585"), TEXT("590"), TEXT("595"),
	TEXT("600"), TEXT("605"), TEXT("610"), TEXT("615"), TEXT("620"), TEXT("625"), TEXT("630"), TEXT("635")
}, g_efxpv6[] = {
	TEXT("0.05"), TEXT("0.10"), TEXT("0.15"), TEXT("0.20"), TEXT("0.25"), TEXT("0.30"), TEXT("0.35"),
	TEXT("0.40"), TEXT("0.45"), TEXT("0.50"), TEXT("0.55"), TEXT("0.60"), TEXT("0.65"), TEXT("0.70"),
	TEXT("0.75"), TEXT("0.80"), TEXT("0.85"), TEXT("0.90"), TEXT("0.95"), TEXT("1.00"), TEXT("1.05"),
	TEXT("1.10"), TEXT("1.15"), TEXT("1.20"), TEXT("1.25"), TEXT("1.30"), TEXT("1.35"), TEXT("1.40"),
	TEXT("1.45"), TEXT("1.50"), TEXT("1.55"), TEXT("1.60"), TEXT("1.65"), TEXT("1.70"), TEXT("1.75"),
	TEXT("1.80"), TEXT("1.85"), TEXT("1.90"), TEXT("1.95"), TEXT("2.00"), TEXT("2.05"), TEXT("2.10"),
	TEXT("2.15"), TEXT("2.20"), TEXT("2.25"), TEXT("2.30"), TEXT("2.35"), TEXT("2.40"), TEXT("2.45"),
	TEXT("2.50"), TEXT("2.55"), TEXT("2.60"), TEXT("2.65"), TEXT("2.70"), TEXT("2.75"), TEXT("2.80"),
	TEXT("2.85"), TEXT("2.90"), TEXT("2.95"), TEXT("3.00"), TEXT("3.05"), TEXT("3.10"), TEXT("3.15"),
	TEXT("3.20"), TEXT("3.25"), TEXT("3.30"), TEXT("3.35"), TEXT("3.40"), TEXT("3.45"), TEXT("3.50"),
	TEXT("3.55"), TEXT("3.60"), TEXT("3.65"), TEXT("3.70"), TEXT("3.75"), TEXT("3.80"), TEXT("3.85"),
	TEXT("3.90"), TEXT("3.95"), TEXT("4.00"), TEXT("4.05"), TEXT("4.10"), TEXT("4.15"), TEXT("4.20"),
	TEXT("4.25"), TEXT("4.30"), TEXT("4.35"), TEXT("4.40"), TEXT("4.45"), TEXT("4.50"), TEXT("4.55"),
	TEXT("4.60"), TEXT("4.65"), TEXT("4.70"), TEXT("4.75"), TEXT("4.80"), TEXT("4.85"), TEXT("4.90"),
	TEXT("4.95"), TEXT("5.00"), TEXT("5.10"), TEXT("5.20"), TEXT("5.30"), TEXT("5.40"), TEXT("5.50"),
	TEXT("5.60"), TEXT("5.70"), TEXT("5.80"), TEXT("5.90"), TEXT("6.00"), TEXT("6.10"), TEXT("6.20"),
	TEXT("6.30"), TEXT("6.40"), TEXT("6.50"), TEXT("6.60"), TEXT("6.70"), TEXT("6.80"), TEXT("6.90"),
	TEXT("7.00"), TEXT("7.50"), TEXT("8.00"), TEXT("8.50"), TEXT("9.00"), TEXT("9.50"), TEXT("10.00")
}, g_efxpv7[] = {
	TEXT("0.05"), TEXT("0.10"), TEXT("0.15"), TEXT("0.20"), TEXT("0.25"), TEXT("0.30"), TEXT("0.35"),
	TEXT("0.40"), TEXT("0.45"), TEXT("0.50"), TEXT("0.55"), TEXT("0.60"), TEXT("0.65"), TEXT("0.70"),
	TEXT("0.75"), TEXT("0.80"), TEXT("0.85"), TEXT("0.90"), TEXT("0.95"), TEXT("1.00"), TEXT("1.05"),
	TEXT("1.10"), TEXT("1.15"), TEXT("1.20"), TEXT("1.25"), TEXT("1.30"), TEXT("1.35"), TEXT("1.40"),
	TEXT("1.45"), TEXT("1.50"), TEXT("1.55"), TEXT("1.60"), TEXT("1.65"), TEXT("1.70"), TEXT("1.75"),
	TEXT("1.80"), TEXT("1.85"), TEXT("1.90"), TEXT("1.95"), TEXT("2.00"), TEXT("2.05"), TEXT("2.10"),
	TEXT("2.15"), TEXT("2.20"), TEXT("2.25"), TEXT("2.30"), TEXT("2.35"), TEXT("2.40"), TEXT("2.45"),
	TEXT("2.50"), TEXT("2.55"), TEXT("2.60"), TEXT("2.65"), TEXT("2.70"), TEXT("2.75"), TEXT("2.80"),
	TEXT("2.85"), TEXT("2.90"), TEXT("2.95"), TEXT("3.00"), TEXT("3.05"), TEXT("3.10"), TEXT("3.15"),
	TEXT("3.20"), TEXT("3.25"), TEXT("3.30"), TEXT("3.35"), TEXT("3.40"), TEXT("3.45"), TEXT("3.50"),
	TEXT("3.55"), TEXT("3.60"), TEXT("3.65"), TEXT("3.70"), TEXT("3.75"), TEXT("3.80"), TEXT("3.85"),
	TEXT("3.90"), TEXT("3.95"), TEXT("4.00"), TEXT("4.05"), TEXT("4.10"), TEXT("4.15"), TEXT("4.20"),
	TEXT("4.25"), TEXT("4.30"), TEXT("4.35"), TEXT("4.40"), TEXT("4.45"), TEXT("4.50"), TEXT("4.55"),
	TEXT("4.60"), TEXT("4.65"), TEXT("4.70"), TEXT("4.75"), TEXT("4.80"), TEXT("4.85"), TEXT("4.90"),
	TEXT("4.95"), TEXT("5.00"), TEXT("5.05"), TEXT("5.10"), TEXT("5.15"), TEXT("5.20"), TEXT("5.25"),
	TEXT("5.30"), TEXT("5.35"), TEXT("5.40"), TEXT("5.45"), TEXT("5.50"), TEXT("5.55"), TEXT("5.60"),
	TEXT("5.65"), TEXT("5.70"), TEXT("5.75"), TEXT("5.80"), TEXT("5.85"), TEXT("5.90"), TEXT("5.95"),
	TEXT("6.00"), TEXT("6.05"), TEXT("6.10"), TEXT("6.15"), TEXT("6.20"), TEXT("6.25"), TEXT("6.30"),
	TEXT("6.35"), TEXT("6.40")
}, g_efxpv12[] = {
	TEXT("100"), TEXT("110"), TEXT("120"), TEXT("130"), TEXT("140"), TEXT("150"), TEXT("160"),
	TEXT("170"), TEXT("180"), TEXT("190"), TEXT("200"), TEXT("210"), TEXT("220"), TEXT("230"),
	TEXT("240"), TEXT("250"), TEXT("260"), TEXT("270"), TEXT("280"), TEXT("290"), TEXT("300"),
	TEXT("320"), TEXT("340"), TEXT("360"), TEXT("380"), TEXT("400"), TEXT("420"), TEXT("440"),
	TEXT("460"), TEXT("480"), TEXT("500"), TEXT("520"), TEXT("540"), TEXT("560"), TEXT("580"),
	TEXT("600"), TEXT("620"), TEXT("640"), TEXT("660"), TEXT("680"), TEXT("700"), TEXT("720"),
	TEXT("740"), TEXT("760"), TEXT("780"), TEXT("800"), TEXT("820"), TEXT("840"), TEXT("860"),
	TEXT("880"), TEXT("900"), TEXT("920"), TEXT("940"), TEXT("960"), TEXT("980"), TEXT("1000"),
	TEXT("1100"), TEXT("1200"), TEXT("1300"), TEXT("1400"), TEXT("1500"), TEXT("1600"), TEXT("1700"),
	TEXT("1800"), TEXT("1900"), TEXT("2000"), TEXT("2100"), TEXT("2200"), TEXT("2300"), TEXT("2400"),
	TEXT("2500"), TEXT("2600"), TEXT("2700"), TEXT("2800"), TEXT("2900"), TEXT("3000"), TEXT("3100"),
	TEXT("3200"), TEXT("3300"), TEXT("3400"), TEXT("3500"), TEXT("3600"), TEXT("3700"), TEXT("3800"),
	TEXT("3900"), TEXT("4000"), TEXT("4100"), TEXT("4200"), TEXT("4300"), TEXT("4400"), TEXT("4500"),
	TEXT("4600"), TEXT("4700"), TEXT("4800"), TEXT("4900"), TEXT("5000"), TEXT("5100"), TEXT("5200"),
	TEXT("5300"), TEXT("5400"), TEXT("5500"), TEXT("5600"), TEXT("5700"), TEXT("5800"), TEXT("5900"),
	TEXT("6000"), TEXT("6100"), TEXT("6200"), TEXT("6300"), TEXT("6400"), TEXT("6500"), TEXT("6600"),
	TEXT("6700"), TEXT("6800"), TEXT("6900"), TEXT("7000"), TEXT("7100"), TEXT("7200"), TEXT("7300"),
	TEXT("7400"), TEXT("7500"), TEXT("7600"), TEXT("7700"), TEXT("7800"), TEXT("7900"), TEXT("8000")
}, g_efxpv_o1[] = {
	TEXT("L63"), TEXT("L62"), TEXT("L61"), TEXT("L60"), TEXT("L59"), TEXT("L58"), TEXT("L57"), TEXT("L56"),
	TEXT("L55"), TEXT("L54"), TEXT("L53"), TEXT("L52"), TEXT("L51"), TEXT("L50"), TEXT("L49"), TEXT("L48"),
	TEXT("L47"), TEXT("L46"), TEXT("L45"), TEXT("L44"), TEXT("L43"), TEXT("L42"), TEXT("L41"), TEXT("L40"),
	TEXT("L39"), TEXT("L38"), TEXT("L37"), TEXT("L36"), TEXT("L35"), TEXT("L34"), TEXT("L33"), TEXT("L32"),
	TEXT("L31"), TEXT("L30"), TEXT("L29"), TEXT("L28"), TEXT("L27"), TEXT("L26"), TEXT("L25"), TEXT("L24"),
	TEXT("L23"), TEXT("L22"), TEXT("L21"), TEXT("L20"), TEXT("L19"), TEXT("L18"), TEXT("L17"), TEXT("L16"),
	TEXT("L15"), TEXT("L14"), TEXT("L13"), TEXT("L12"), TEXT("L11"), TEXT("L10"), TEXT("L9"), TEXT("L8"),
	TEXT("L7"), TEXT("L6"), TEXT("L5"), TEXT("L4"), TEXT("L3"), TEXT("L2"), TEXT("L1"), TEXT("0"),
	TEXT("0"), TEXT("R1"), TEXT("R2"), TEXT("R3"), TEXT("R4"), TEXT("R5"), TEXT("R6"), TEXT("R7"),
	TEXT("R8"), TEXT("R9"), TEXT("R10"), TEXT("R11"), TEXT("R12"), TEXT("R13"), TEXT("R14"), TEXT("R15"),
	TEXT("R16"), TEXT("R17"), TEXT("R18"), TEXT("R19"), TEXT("R20"), TEXT("R21"), TEXT("R22"), TEXT("R23"),
	TEXT("R24"), TEXT("R25"), TEXT("R26"), TEXT("R27"), TEXT("R28"), TEXT("R29"), TEXT("R30"), TEXT("R31"),
	TEXT("R32"), TEXT("R33"), TEXT("R34"), TEXT("R35"), TEXT("R36"), TEXT("R37"), TEXT("R38"), TEXT("R39"),
	TEXT("R40"), TEXT("R41"), TEXT("R42"), TEXT("R43"), TEXT("R44"), TEXT("R45"), TEXT("R46"), TEXT("R47"),
	TEXT("R48"), TEXT("R49"), TEXT("R50"), TEXT("R51"), TEXT("R52"), TEXT("R53"), TEXT("R54"), TEXT("R55"),
	TEXT("R56"), TEXT("R57"), TEXT("R58"), TEXT("R59"), TEXT("R60"), TEXT("R61"), TEXT("R62"), TEXT("R63")
}, g_efxpv_o2[] = {
	TEXT("D>0E"), TEXT("D>1E"), TEXT("D>2E"), TEXT("D>3E"), TEXT("D>4E"), TEXT("D>5E"), TEXT("D>6E"),
	TEXT("D>7E"), TEXT("D>8E"), TEXT("D>9E"), TEXT("D>10E"), TEXT("D>11E"), TEXT("D>12E"), TEXT("D>13E"),
	TEXT("D>14E"), TEXT("D>15E"), TEXT("D>16E"), TEXT("D>17E"), TEXT("D>18E"), TEXT("D>19E"), TEXT("D>20E"),
	TEXT("D>21E"), TEXT("D>22E"), TEXT("D>23E"), TEXT("D>24E"), TEXT("D>25E"), TEXT("D>26E"), TEXT("D>27E"),
	TEXT("D>28E"), TEXT("D>29E"), TEXT("D>30E"), TEXT("D>31E"), TEXT("D>32E"), TEXT("D>33E"), TEXT("D>34E"),
	TEXT("D>35E"), TEXT("D>36E"), TEXT("D>37E"), TEXT("D>38E"), TEXT("D>39E"), TEXT("D>40E"), TEXT("D>41E"),
	TEXT("D>42E"), TEXT("D>43E"), TEXT("D>44E"), TEXT("D>45E"), TEXT("D>46E"), TEXT("D>47E"), TEXT("D>48E"),
	TEXT("D>49E"), TEXT("D>50E"), TEXT("D>51E"), TEXT("D>52E"), TEXT("D>53E"), TEXT("D>54E"), TEXT("D>55E"),
	TEXT("D>56E"), TEXT("D>57E"), TEXT("D>58E"), TEXT("D>59E"), TEXT("D>60E"), TEXT("D>61E"), TEXT("D>62E"),
	TEXT("D>63E"), TEXT("D=E"), TEXT("D62<E"), TEXT("D61<E"), TEXT("D60<E"), TEXT("D59<E"), TEXT("D58<E"),
	TEXT("D57<E"), TEXT("D56<E"), TEXT("D55<E"), TEXT("D54<E"), TEXT("D53<E"), TEXT("D52<E"), TEXT("D51<E"),
	TEXT("D50<E"), TEXT("D49<E"), TEXT("D48<E"), TEXT("D47<E"), TEXT("D46<E"), TEXT("D45<E"), TEXT("D44<E"),
	TEXT("D43<E"), TEXT("D42<E"), TEXT("D41<E"), TEXT("D40<E"), TEXT("D39<E"), TEXT("D38<E"), TEXT("D37<E"),
	TEXT("D36<E"), TEXT("D35<E"), TEXT("D34<E"), TEXT("D33<E"), TEXT("D32<E"), TEXT("D31<E"), TEXT("D30<E"),
	TEXT("D29<E"), TEXT("D28<E"), TEXT("D27<E"), TEXT("D26<E"), TEXT("D25<E"), TEXT("D24<E"), TEXT("D23<E"),
	TEXT("D22<E"), TEXT("D21<E"), TEXT("D20<E"), TEXT("D19<E"), TEXT("D18<E"), TEXT("D17<E"), TEXT("D16<E"),
	TEXT("D15<E"), TEXT("D14<E"), TEXT("D13<E"), TEXT("D12<E"), TEXT("D11<E"), TEXT("D10<E"), TEXT("D9<E"),
	TEXT("D8<E"), TEXT("D7<E"), TEXT("D6<E"), TEXT("D5<E"), TEXT("D4<E"), TEXT("D3<E"), TEXT("D2<E"),
	TEXT("D1<E"), TEXT("D0<E")
}, g_efxpv_o3[] = {
	TEXT("A>0B"), TEXT("A>1B"), TEXT("A>2B"), TEXT("A>3B"), TEXT("A>4B"), TEXT("A>5B"), TEXT("A>6B"),
	TEXT("A>7B"), TEXT("A>8B"), TEXT("A>9B"), TEXT("A>10B"), TEXT("A>11B"), TEXT("A>12B"), TEXT("A>13B"),
	TEXT("A>14B"), TEXT("A>15B"), TEXT("A>16B"), TEXT("A>17B"), TEXT("A>18B"), TEXT("A>19B"), TEXT("A>20B"),
	TEXT("A>21B"), TEXT("A>22B"), TEXT("A>23B"), TEXT("A>24B"), TEXT("A>25B"), TEXT("A>26B"), TEXT("A>27B"),
	TEXT("A>28B"), TEXT("A>29B"), TEXT("A>30B"), TEXT("A>31B"), TEXT("A>32B"), TEXT("A>33B"), TEXT("A>34B"),
	TEXT("A>35B"), TEXT("A>36B"), TEXT("A>37B"), TEXT("A>38B"), TEXT("A>39B"), TEXT("A>40B"), TEXT("A>41B"),
	TEXT("A>42B"), TEXT("A>43B"), TEXT("A>44B"), TEXT("A>45B"), TEXT("A>46B"), TEXT("A>47B"), TEXT("A>48B"),
	TEXT("A>49B"), TEXT("A>50B"), TEXT("A>51B"), TEXT("A>52B"), TEXT("A>53B"), TEXT("A>54B"), TEXT("A>55B"),
	TEXT("A>56B"), TEXT("A>57B"), TEXT("A>58B"), TEXT("A>59B"), TEXT("A>60B"), TEXT("A>61B"), TEXT("A>62B"),
	TEXT("A>63B"), TEXT("A=B"), TEXT("A62<B"), TEXT("A61<B"), TEXT("A60<B"), TEXT("A59<B"), TEXT("A58<B"),
	TEXT("A57<B"), TEXT("A56<B"), TEXT("A55<B"), TEXT("A54<B"), TEXT("A53<B"), TEXT("A52<B"), TEXT("A51<B"),
	TEXT("A50<B"), TEXT("A49<B"), TEXT("A48<B"), TEXT("A47<B"), TEXT("A46<B"), TEXT("A45<B"), TEXT("A44<B"),
	TEXT("A43<B"), TEXT("A42<B"), TEXT("A41<B"), TEXT("A40<B"), TEXT("A39<B"), TEXT("A38<B"), TEXT("A37<B"),
	TEXT("A36<B"), TEXT("A35<B"), TEXT("A34<B"), TEXT("A33<B"), TEXT("A32<B"), TEXT("A31<B"), TEXT("A30<B"),
	TEXT("A29<B"), TEXT("A28<B"), TEXT("A27<B"), TEXT("A26<B"), TEXT("A25<B"), TEXT("A24<B"), TEXT("A23<B"),
	TEXT("A22<B"), TEXT("A21<B"), TEXT("A20<B"), TEXT("A19<B"), TEXT("A18<B"), TEXT("A17<B"), TEXT("A16<B"),
	TEXT("A15<B"), TEXT("A14<B"), TEXT("A13<B"), TEXT("A12<B"), TEXT("A11<B"), TEXT("A10<B"), TEXT("A9<B"),
	TEXT("A8<B"), TEXT("A7<B"), TEXT("A6<B"), TEXT("A5<B"), TEXT("A4<B"), TEXT("A3<B"), TEXT("A2<B"),
	TEXT("A1<B"), TEXT("A0<B")
};

const Efx::ParamValue g_efxpv8[] = {
	{0x00, TEXT("315")}, {0x08, TEXT("400")}, {0x10, TEXT("500")}, {0x18, TEXT("630")},
	{0x20, TEXT("800")}, {0x28, TEXT("1000")}, {0x30, TEXT("1250")}, {0x38, TEXT("1600")},
	{0x40, TEXT("2000")}, {0x48, TEXT("2500")}, {0x50, TEXT("3150")}, {0x58, TEXT("4000")},
	{0x60, TEXT("5000")}, {0x68, TEXT("6300")}, {0x70, TEXT("8000")}, {0x78, TEXT("Bypass")}
}, g_efxpv9[] = {
	{0x00, TEXT("250")}, {0x08, TEXT("315")}, {0x10, TEXT("400")}, {0x18, TEXT("500")},
	{0x20, TEXT("630")}, {0x28, TEXT("800")}, {0x30, TEXT("1000")}, {0x38, TEXT("1250")},
	{0x40, TEXT("1600")}, {0x48, TEXT("2000")}, {0x50, TEXT("2500")}, {0x58, TEXT("3150")},
	{0x60, TEXT("4000")}, {0x68, TEXT("5000")}, {0x70, TEXT("6300")}, {0x78, TEXT("8000")}
}, g_efxpv10[] = {
	{0x00, TEXT("200")}, {0x08, TEXT("250")}, {0x10, TEXT("315")}, {0x18, TEXT("400")},
	{0x20, TEXT("500")}, {0x28, TEXT("630")}, {0x30, TEXT("800")}, {0x38, TEXT("1000")},
	{0x40, TEXT("1250")}, {0x48, TEXT("1600")}, {0x50, TEXT("2000")}, {0x58, TEXT("2500")},
	{0x60, TEXT("3150")}, {0x68, TEXT("4000")}, {0x70, TEXT("5000")}, {0x78, TEXT("6300")}
}, g_efxpv11[] = {
	{0x00, TEXT("250")}, {0x08, TEXT("315")}, {0x10, TEXT("400")}, {0x18, TEXT("500")},
	{0x20, TEXT("630")}, {0x28, TEXT("800")}, {0x30, TEXT("1000")}, {0x38, TEXT("1250")},
	{0x40, TEXT("1600")}, {0x48, TEXT("2000")}, {0x50, TEXT("2500")}, {0x58, TEXT("3150")},
	{0x60, TEXT("4000")}, {0x68, TEXT("5000")}, {0x70, TEXT("6300")}, {0x78, TEXT("Bypass")}
}, g_efxpv13[] = {
	{0x00, TEXT("L180(=R180)")}, {0x06, TEXT("L168")}, {0x0a, TEXT("L156")}, {0x0e, TEXT("L144")},
	{0x12, TEXT("L132")}, {0x16, TEXT("L120")}, {0x1a, TEXT("L108")}, {0x1e, TEXT("L96")},
	{0x22, TEXT("L84")}, {0x26, TEXT("L72")}, {0x2a, TEXT("L60")}, {0x2e, TEXT("L48")},
	{0x32, TEXT("L36")}, {0x36, TEXT("L24")}, {0x3a, TEXT("L12")}, {0x3e, TEXT("0")},
	{0x42, TEXT("R12")}, {0x46, TEXT("R24")}, {0x4a, TEXT("R36")}, {0x4e, TEXT("R48")},
	{0x52, TEXT("R60")}, {0x56, TEXT("R72")}, {0x5a, TEXT("R84")}, {0x5e, TEXT("R96")},
	{0x62, TEXT("R108")}, {0x66, TEXT("R120")}, {0x6a, TEXT("R132")}, {0x6e, TEXT("R144")},
	{0x72, TEXT("R156")}, {0x76, TEXT("R168")}, {0x7a, TEXT("R180(=L180)")}
}, g_efxpv14[] = {
	{0x00, TEXT("0")}, {0x08, TEXT("1")}, {0x10, TEXT("2")}, {0x18, TEXT("3")},
	{0x20, TEXT("4")}, {0x28, TEXT("5")}, {0x30, TEXT("6")}, {0x38, TEXT("7")},
	{0x40, TEXT("8")}, {0x48, TEXT("9")}, {0x50, TEXT("10")}, {0x58, TEXT("11")},
	{0x60, TEXT("12")}, {0x68, TEXT("13")}, {0x70, TEXT("14")}, {0x78, TEXT("15")}
};

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	char i;
	LPNMTTDISPINFO lpnmtdi;
	TOOLINFO ti;
	static HWND hw_cb_efxname, hw_stc_efxname, hw_tt_efxname;
	static Efx *pefx;

	switch (uMsg) {
	case WM_CREATE:
		// EfxNameコンボボックス作成。
		if ((hw_cb_efxname = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("COMBOBOX"), NULL,
		WS_CHILD | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWNLIST, 10, 5, 550, CW_USEDEFAULT,
		hwnd, (HMENU)WNDID_EFXNAME, g_hinst, NULL)) == NULL) return -1;
		SendMessage(hw_cb_efxname, WM_SETFONT, (WPARAM)g_hfont, FALSE);
		for (i = 0; i < sizeof(g_efxname) / sizeof(g_efxname[0]); i++)
			if (SendMessage(hw_cb_efxname, CB_ADDSTRING, 0, (LPARAM)g_efxname[i].name) < 0) return -1;

		// EfxNameスタティック作成。
		if ((hw_stc_efxname = CreateWindowEx(WS_EX_STATICEDGE, TEXT("STATIC"), TEXT("ここに説明が表示されます。"),
		WS_CHILD | WS_VISIBLE, 10, 25, 550, 15, hwnd, NULL, g_hinst, NULL)) == NULL) return -1;
		SendMessage(hw_stc_efxname, WM_SETFONT, (WPARAM)g_hfont, FALSE);

		// EfxNameツールチップ作成。
		if ((hw_tt_efxname = CreateWindow(TOOLTIPS_CLASS, NULL, TTS_ALWAYSTIP | TTS_BALLOON | TTS_NOPREFIX,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, hwnd, NULL, g_hinst, NULL)) == NULL)
			return -1;
		ti.cbSize = sizeof(TOOLINFO);
		ti.uFlags = TTF_SUBCLASS | TTF_TRANSPARENT;
		ti.hwnd = hwnd;
		ti.uId = 0;
		ti.rect.left = 10;
		ti.rect.top = 25;
		ti.rect.right = 560;
		ti.rect.bottom = 40;
		ti.hinst = g_hinst;
		ti.lpszText = NULL;
		ti.lParam = 0;
		SendMessage(hw_tt_efxname, TTM_SETDELAYTIME, TTDT_AUTOPOP, MAKELPARAM(32767, 0));
		SendMessage(hw_tt_efxname, TTM_SETDELAYTIME, TTDT_INITIAL, MAKELPARAM(50, 0));
		SendMessage(hw_tt_efxname, TTM_SETMAXTIPWIDTH, 0, 300);
		if (!SendMessage(hw_tt_efxname, TTM_ADDTOOL, 0, (LPARAM)&ti)) return -1;

		try {
			pefx = new Efx(hwnd);
		} catch (...) {
			return -1;
		}
		return 0;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case WNDID_EFXNAME:
			switch (HIWORD(wParam)) {
			case CBN_SELCHANGE:
				if ((i = SendMessage(hw_cb_efxname, CB_GETCURSEL, 0, 0)) == CB_ERR) return 0;

				ti.cbSize = sizeof(TOOLINFO);
				ti.hwnd = hwnd;
				ti.uId = 0;
				ti.hinst = g_hinst;
				ti.lpszText = g_efxname[i].desc;
				SendMessage(hw_tt_efxname, TTM_UPDATETIPTEXT, 0, (LPARAM)&ti);

				if (!pefx->Delete()) return 0;

static LPTSTR pv1[] = {TEXT("200"), TEXT("400")},
pv2[] = {TEXT("4k"), TEXT("8k")},
pv3[] = {TEXT("0.5"), TEXT("1.0"), TEXT("2.0"), TEXT("4.0"), TEXT("9.0")},
pv4[] = {TEXT("Off"), TEXT("On")},
pv5[] = {TEXT("a"), TEXT("i"), TEXT("u"), TEXT("e"), TEXT("o")},
pv6[] = {TEXT("Small"), TEXT("BltIn"), TEXT("2-Stk"), TEXT("3-Stk")},
pv7[] = {TEXT("LPF"), TEXT("BPF")},
pv8[] = {TEXT("Down"), TEXT("Up")},
pv10[] = {TEXT("Off"), TEXT("LPF"), TEXT("HPF")},
pv11[] = {TEXT("Tri"), TEXT("Sqr"), TEXT("Sin"), TEXT("Saw1"), TEXT("Saw2")},
pv12[] = {TEXT("0"), TEXT("+6"), TEXT("+12"), TEXT("+18")},
pv13[] = {TEXT("1/1.5"), TEXT("1/2"), TEXT("1/4"), TEXT("1/100")},
pv14[] = {TEXT("Speaker"), TEXT("Phones")},
pv15[] = {TEXT("Norm"), TEXT("Cross")},
pv16[] = {TEXT("Norm"), TEXT("Invert")},
pv17[] = {TEXT("Room1"), TEXT("Room2"), TEXT("Stage1"), TEXT("Stage2"), TEXT("Hall1"), TEXT("Hall2")},
pv18[] = {TEXT("Norm"), TEXT("Reverse"), TEXT("Sweep1"), TEXT("Sweep2")},
pv19[] = {TEXT("-"), TEXT("+")},
pv20[] = {TEXT("White"), TEXT("Pink")},
pv21[] = {TEXT("LP"), TEXT("EP"), TEXT("SP"), TEXT("RND")},
pv22[] = {TEXT("Mono"), TEXT("Stereo")};
static const Efx::ParamValue pv9[] = {{0x00, TEXT("Slow")}, {0x7f, TEXT("Fast")}};
switch (i) {
case 3:
	if (!pefx->Add(0x03, TEXT("Low Freq"), TEXT("ロー・フリケンシー"), TEXT("低域の基準周波数(200Hz/400Hz)を選びます。"), pv1, sizeof(pv1) / sizeof(pv1[0]), 1)) return 0;
	if (!pefx->Add(0x04, TEXT("Low Gain"), TEXT("ロー・ゲイン"), TEXT("Low Freqパラメーターのゲインを調節します。"), 17, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x05, TEXT("Hi Freq"), TEXT("ハイ・フリケンシー"), TEXT("高域の基準周波数(4kHz/8kHz)を選びます。"), pv2, sizeof(pv2) / sizeof(pv2[0]), 1)) return 0;
	if (!pefx->Add(0x06, TEXT("Hi Gain"), TEXT("ハイ・ゲイン"), TEXT("High Freqパラメーターのゲインを調節します。"), 0, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x07, TEXT("M1 Freq"), TEXT("ミッド1・フリケンシー"), TEXT("ミッド1(中域1)の基準周波数を選びます。"), g_efxpv10, sizeof(g_efxpv10) / sizeof(g_efxpv10[0]), 9)) return 0;
	if (!pefx->Add(0x08, TEXT("M1 Q"), TEXT("ミッド1・Q"), TEXT("M1 Freqパラメーターを中心にしたゲインの変化範囲を調節します。値を大きくするほど変化範囲が狭くなりす。"), pv3, sizeof(pv3) / sizeof(pv3[0]), 0)) return 0;
	if (!pefx->Add(0x09, TEXT("M1 Gain"), TEXT("ミッド1・ゲイン"), TEXT("M1 FreqパラメーターとM1 Qパラメーターで設定した範囲のゲインを調節します。"), 20, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x0a, TEXT("M2 Freq"), TEXT("ミッド2・フリケンシー"), TEXT("ミッド2(中域2)の基準周波数を選びます。"), g_efxpv10, sizeof(g_efxpv10) / sizeof(g_efxpv10[0]), 7)) return 0;
	if (!pefx->Add(0x0b, TEXT("M2 Q"), TEXT("ミッド2・Q"), TEXT("M2 Freqパラメーターを中心にしたゲインの変化範囲を調節します。値を大きくするほど変化範囲が狭くなります。"), pv3, sizeof(pv3) / sizeof(pv3[0]), 0)) return 0;
	if (!pefx->Add(0x0c, TEXT("M2 Gain"), TEXT("ミッド2・ゲイン"), TEXT("M2 FreqパラメーターとM2 Qパラメーターで設定した範囲のゲインを調節します。"), 4, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x16, TEXT("+ Level"), TEXT("アウトプット・レベル"), TEXT("出力音量を調節します。"), 127)) return 0;
	break;
case 4:
	if (!pefx->Add(0x03, TEXT("Band 1"), TEXT("バンド1・ゲイン"), TEXT("250Hzのゲインを調節します。"), 8, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x04, TEXT("Band 2"), TEXT("バンド2・ゲイン"), TEXT("500Hzのゲインを調節します。"), 13, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x05, TEXT("Band 3"), TEXT("バンド3・ゲイン"), TEXT("1000Hzのゲインを調節します。"), 15, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x06, TEXT("Band 4"), TEXT("バンド4・ゲイン"), TEXT("1250Hzのゲインを調節します。"), 18, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x07, TEXT("Band 5"), TEXT("バンド5・ゲイン"), TEXT("2000Hzのゲインを調節します。"), 14, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x08, TEXT("Band 6"), TEXT("バンド6・ゲイン"), TEXT("3150Hzのゲインを調節します。"), 11, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x09, TEXT("Band 7"), TEXT("バンド7・ゲイン"), TEXT("4000Hzのゲインを調節します。"), 8, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x0a, TEXT("Band 8"), TEXT("バンド8・ゲイン"), TEXT("8000Hzのゲインを調節します。"), 7, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x0b, TEXT("Width"), TEXT("バンド・ウィズ"), TEXT("ゲインを変化させる帯域幅を各バンド共通で調節します。値を大きくするほど帯域幅が狭くなります。"), pv3, sizeof(pv3) / sizeof(pv3[0]), 2)) return 0;
	if (!pefx->Add(0x15, TEXT("+ Pan"), TEXT("アウトプット・パン"), TEXT("出力される音の定位を調節します。L63で最も左、0で中央、R63で最も右に定位します。"), g_efxpv_o1, sizeof(g_efxpv_o1) / sizeof(g_efxpv_o1[0]), 63)) return 0;
	if (!pefx->Add(0x16, TEXT("# Level"), TEXT("アウトプット・レベル"), TEXT("出力音量を調節します。"), 127)) return 0;
	break;
case 5:
	if (!pefx->Add(0x03, TEXT("+ Sens"), TEXT("センス"), TEXT("エンハンサーのかかり具合を調節します。"), 64)) return 0;
	if (!pefx->Add(0x04, TEXT("# Mix"), TEXT("ミックス・レベル"), TEXT("エンハンサーで生成された倍音を原音に混ぜ合わせる割合について調節します。"), 127)) return 0;
	if (!pefx->Add(0x13, TEXT("Low Gain"), TEXT("ロー・ゲイン"), TEXT("低域のゲインを調節します。"), 15, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x14, TEXT("Hi Gain"), TEXT("ハイ・ゲイン"), TEXT("高域のゲインを調節します。"), 12, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x16, TEXT("Level"), TEXT("アウトプット・レベル"), TEXT("出力音量を調節します。"), 127)) return 0;
	break;
case 6:
	if (!pefx->Add(0x03, TEXT("Drive"), TEXT("ドライブ"), TEXT("歪み具合を調節します。"), 48)) return 0;
	if (!pefx->Add(0x04, TEXT("Drive Sw"), TEXT("ドライブ・スイッチ"), TEXT("Driveをオン/オフします。"), pv4, sizeof(pv4) / sizeof(pv4[0]), 1)) return 0;
	if (!pefx->Add(0x05, TEXT("+ Vowel"), TEXT("ヴォウエル"), TEXT("母音を選びます。"), pv5, sizeof(pv5) / sizeof(pv5[0]), 0)) return 0;
	if (!pefx->Add(0x06, TEXT("Accel"), TEXT("アクセラレーション"), TEXT("Vowelで設定した音へ移り変わる時間を調節します。値を小さくするほど時間がかかります。"), g_efxpv14, sizeof(g_efxpv14) / sizeof(g_efxpv14[0]), 15)) return 0;
	if (!pefx->Add(0x13, TEXT("Low Gain"), TEXT("ロー・ゲイン"), TEXT("低域のゲインを調節します。"), 12, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x14, TEXT("Hi Gain"), TEXT("ハイ・ゲイン"), TEXT("高域のゲインを調節します。"), 12, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x15, TEXT("Pan"), TEXT("アウトプット・パン"), TEXT("出力される音の定位を調節します。L63で最も左、0で中央、R63で最も右に定位します。"), g_efxpv_o1, sizeof(g_efxpv_o1) / sizeof(g_efxpv_o1[0]), 63)) return 0;
	if (!pefx->Add(0x16, TEXT("# Level"), TEXT("アウトプット・レベル"), TEXT("出力音量を調節します。"), 127)) return 0;
	break;
case 7:
	if (!pefx->Add(0x03, TEXT("+ Drive"), TEXT("ドライブ"), TEXT("歪み具合を調節します。"), 48)) return 0;
	if (!pefx->Add(0x04, TEXT("Amp Type"), TEXT("アンプ・シミュレーター・タイプ"), TEXT("ギター・アンプの種類を選びます。\n Small ：小型アンプ\n BltIn ：ビルト・イン・タイプのアンプ\n 2-Stk ：大型2段積みアンプ\n 3-Stk ：大型3段積みアンプ"), pv6, sizeof(pv6) / sizeof(pv6[0]), 1)) return 0;
	if (!pefx->Add(0x05, TEXT("Amp Sw"), TEXT("アンプ・スイッチ"), TEXT("Amp Typeをオン/オフします。"), pv4, sizeof(pv4) / sizeof(pv4[0]), 1)) return 0;
	if (!pefx->Add(0x13, TEXT("Low Gain"), TEXT("ロー・ゲイン"), TEXT("低域のゲインを調節します。"), 12, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x14, TEXT("Hi Gain"), TEXT("ハイ・ゲイン"), TEXT("高域のゲインを調節します。"), 12, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x15, TEXT("Pan"), TEXT("アウトプット・パン"), TEXT("出力される音の定位を調節します。L63で最も左、0で中央、R63で最も右に定位します。"), g_efxpv_o1, sizeof(g_efxpv_o1) / sizeof(g_efxpv_o1[0]), 63)) return 0;
	if (!pefx->Add(0x16, TEXT("# Level"), TEXT("アウトプット・レベル"), TEXT("出力音量を調節します。"), 96)) return 0;
	break;
case 8:
	if (!pefx->Add(0x03, TEXT("+ Drive"), TEXT("ドライブ"), TEXT("歪み具合を調節します。"), 76)) return 0;
	if (!pefx->Add(0x04, TEXT("Amp Type"), TEXT("アンプ・シミュレーター・タイプ"), TEXT("ギター・アンプの種類を選びます。\n Small ：小型アンプ\n BltIn ：ビルト・イン・タイプのアンプ\n 2-Stk ：大型2段積みアンプ\n 3-Stk ：大型3段積みアンプ"), pv6, sizeof(pv6) / sizeof(pv6[0]), 3)) return 0;
	if (!pefx->Add(0x05, TEXT("Amp Sw"), TEXT("アンプ・スイッチ"), TEXT("Amp Typeをオン/オフします。"), pv4, sizeof(pv4) / sizeof(pv4[0]), 1)) return 0;
	if (!pefx->Add(0x13, TEXT("Low Gain"), TEXT("ロー・ゲイン"), TEXT("低域のゲインを調節します。"), 12, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x14, TEXT("Hi Gain"), TEXT("ハイ・ゲイン"), TEXT("高域のゲインを調節します。"), 4, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x15, TEXT("Pan"), TEXT("アウトプット・パン"), TEXT("出力される音の定位を調節します。L63で最も左、0で中央、R63で最も右に定位します。"), g_efxpv_o1, sizeof(g_efxpv_o1) / sizeof(g_efxpv_o1[0]), 63)) return 0;
	if (!pefx->Add(0x16, TEXT("# Level"), TEXT("アウトプット・レベル"), TEXT("出力音量を調節します。"), 84)) return 0;
	break;
case 9:
	if (!pefx->Add(0x03, TEXT("+ Manual"), TEXT("マニュアル"), TEXT("音をうねらせる基準の周波数を調節します。"), g_efxpv12, sizeof(g_efxpv12) / sizeof(g_efxpv12[0]), 36)) return 0;
	if (!pefx->Add(0x04, TEXT("# Rate"), TEXT("レイト"), TEXT("うねりの周期を調節します。"), g_efxpv6, sizeof(g_efxpv6) / sizeof(g_efxpv6[0]), 16)) return 0;
	if (!pefx->Add(0x05, TEXT("Depth"), TEXT("デプス"), TEXT("うねりの深さを調節します。"), 64)) return 0;
	if (!pefx->Add(0x06, TEXT("Reso"), TEXT("レゾナンス"), TEXT("Manualパラメーターで設定した基準周波数付近の周波数成分を強調します。"), 16)) return 0;
	if (!pefx->Add(0x07, TEXT("Mix"), TEXT("ミックス・レベル"), TEXT("位相をずらせた音を原音に混ぜ合わせる割合を調節します。"), 127)) return 0;
	if (!pefx->Add(0x13, TEXT("Low Gain"), TEXT("ロー・ゲイン"), TEXT("低域のゲインを調節します。"), 12, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x14, TEXT("Hi Gain"), TEXT("ハイ・ゲイン"), TEXT("高域のゲインを調節します。"), 12, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x16, TEXT("Level"), TEXT("アウトプット・レベル"), TEXT("出力音量を調節します。"), 104)) return 0;
	break;
case 10:
	if (!pefx->Add(0x03, TEXT("Fil Type"), TEXT("フィルター・タイプ"), TEXT("フィルターの種類を選びます。\n LPF：広い周波数範囲でワウ効果が得られます。\n BPF：狭い周波数範囲でワウ効果が得られます。"), pv7, sizeof(pv7) / sizeof(pv7[0]), 1)) return 0;
	if (!pefx->Add(0x04, TEXT("Sens"), TEXT("センス"), TEXT("フィルターを変化させる感度を調節します。値を大きくすると入力レベルに応じてフィルターの周波数が変わりやすくなります。"), 0)) return 0;
	if (!pefx->Add(0x05, TEXT("+ Manual"), TEXT("マニュアル"), TEXT("効果を与える基準周波数を調節します。"), 68)) return 0;
	if (!pefx->Add(0x06, TEXT("Peak"), TEXT("ピーク"), TEXT("基準周波数付近のワウ効果のかかり具合を調節します。値を小さくすると基準周波数周辺の広い範囲で、値を大きくすると狭い範囲でワウ効果が得られます。LPFの場合には、値を小さくするとワウ効果の変化が小さく聴こえます。"), 62)) return 0;
	if (!pefx->Add(0x07, TEXT("# Rate"), TEXT("レイト"), TEXT("揺れの周期を調節します。"), g_efxpv6, sizeof(g_efxpv6) / sizeof(g_efxpv6[0]), 40)) return 0;
	if (!pefx->Add(0x08, TEXT("Depth"), TEXT("デプス"), TEXT("揺れの深さを調節します。"), 72)) return 0;
	if (!pefx->Add(0x09, TEXT("Polarity"), TEXT("ポラリティー"), TEXT("フィルターを変化させる場合に、フィルターがどちらの周波数方向に動くかを設定します。Upではフィルターが高い周波数方向に動き、Downでは低い周波数方向に動きます。"), pv8, sizeof(pv8) / sizeof(pv8[0]), 1)) return 0;
	if (!pefx->Add(0x13, TEXT("Low Gain"), TEXT("ロー・ゲイン"), TEXT("低域のゲインを調節します。"), 12, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x14, TEXT("Hi Gain"), TEXT("ハイ・ゲイン"), TEXT("高域のゲインを調節します。"), 12, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x15, TEXT("Pan"), TEXT("アウトプット・パン"), TEXT("出力される音の定位を調節します。L63で最も左、0で中央、R63で最も右に定位します。"), g_efxpv_o1, sizeof(g_efxpv_o1) / sizeof(g_efxpv_o1[0]), 63)) return 0;
	if (!pefx->Add(0x16, TEXT("Level"), TEXT("アウトプット・レベル"), TEXT("出力音量を調節します。"), 96)) return 0;
	break;
case 11:
	if (!pefx->Add(0x03, TEXT("Low Slow"), TEXT("ロー・フリケンシー・スロー・レイト"), TEXT("低域ローターの低速回転時(Slow)の周期を調節します。"), g_efxpv6, sizeof(g_efxpv6) / sizeof(g_efxpv6[0]), 6)) return 0;
	if (!pefx->Add(0x04, TEXT("Low Fast"), TEXT("ロー・フリケンシー・ファースト・レイト"), TEXT("低域ローターの高速回転時(Fast)の周期を調節します。"), g_efxpv6, sizeof(g_efxpv6) / sizeof(g_efxpv6[0]), 113)) return 0;
	if (!pefx->Add(0x05, TEXT("Low Accl"), TEXT("ロー・フリケンシー・アクセラレーション"), TEXT("低速回転から高速回転(または高速回転から低速回転)に切り替えたときの、低域ローターの回転周期が落ちつくまでの時間を調節します。値を小さくするほど時間がかかります。"), g_efxpv14, sizeof(g_efxpv14) / sizeof(g_efxpv14[0]), 3)) return 0;
	if (!pefx->Add(0x06, TEXT("Low Level"), TEXT("ロー・フリケンシー・レベル"), TEXT("低域ローターの音量を調節します。"), 127)) return 0;
	if (!pefx->Add(0x07, TEXT("Hi Slow"), TEXT("ハイ・フリケンシー・スロー・レイト"), TEXT("高域ローターの低速回転時(Slow)の周期を調節します。"), g_efxpv6, sizeof(g_efxpv6) / sizeof(g_efxpv6[0]), 17)) return 0;
	if (!pefx->Add(0x08, TEXT("Hi Fast"), TEXT("ハイ・フリーケンシー・ファースト・レイト"), TEXT("高域ローターの高速回転時(Fast)の周期を調節します。"), g_efxpv6, sizeof(g_efxpv6) / sizeof(g_efxpv6[0]), 120)) return 0;
	if (!pefx->Add(0x09, TEXT("Hi Accl"), TEXT("ハイ・フリケンシー・アクセラレーション"), TEXT("低速回転から高速回転(または高速回転から低速回転)に切り替えたときの、高域ローターの回転周期が落ちつくまでの時間を調節します。値を小さくするほど時間がかかります。"), g_efxpv14, sizeof(g_efxpv14) / sizeof(g_efxpv14[0]), 11)) return 0;
	if (!pefx->Add(0x0a, TEXT("Hi Level"), TEXT("ハイ・フリケンシー・レベル"), TEXT("高域ローターの音量を調節します。"), 64)) return 0;
	if (!pefx->Add(0x0b, TEXT("Separate"), TEXT("セパレーション"), TEXT("音の広がり具合を調節します。"), 96)) return 0;
	if (!pefx->Add(0x0d, TEXT("+ Speed"), TEXT("スピード"), TEXT("低域ローターと高域ローターの回転速度を同時に切り替えます。\n Slow ：指定の回転周期(Low Slowパラメーター/Hi Slowパラメーターの値)に下げます。\n Fast ：指定の回転周期(Low Fastパラメーター/Hi Fastパラメーターの値)に上げます。"), pv9, sizeof(pv9) / sizeof(pv9[0]), 0)) return 0;
	if (!pefx->Add(0x13, TEXT("Low Gain"), TEXT("ロー・ゲイン"), TEXT("低域のゲインを調節します。"), 12, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x14, TEXT("Hi Gain"), TEXT("ハイ・ゲイン"), TEXT("高域のゲインを調節します。"), 12, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x16, TEXT("# Level"), TEXT("アウトプット・レベル"), TEXT("出力音量を調節します。"), 127)) return 0;
	break;
case 12:
	if (!pefx->Add(0x03, TEXT("Pre Filter"), TEXT("プリ・フィルター・タイプ"), TEXT("フィルターの種類を選びます。\n Off ：フィルターを使いません。\n LPF ：Cutoffパラメーターより上の帯域をカットします。\n HPF ：Cutoffパラメーターより下の帯域をカットします。"), pv10, sizeof(pv10) / sizeof(pv10[0]), 0)) return 0;
	if (!pefx->Add(0x04, TEXT("Cutoff"), TEXT("カットオフ・フリケンシー"), TEXT("フィルターの基準周波数を調節します。"), g_efxpv9, sizeof(g_efxpv9) / sizeof(g_efxpv9[0]), 0)) return 0;
	if (!pefx->Add(0x05, TEXT("Pre Dly"), TEXT("プリ・ディレイ・タイム"), TEXT("原音が鳴ってからフランジャー音が鳴るまでの時間を調節します。"), g_efxpv1, sizeof(g_efxpv1) / sizeof(g_efxpv1[0]), 16)) return 0;
	if (!pefx->Add(0x06, TEXT("+ Rate"), TEXT("レイト"), TEXT("揺れの周期を調節します。"), g_efxpv6, sizeof(g_efxpv6) / sizeof(g_efxpv6[0]), 11)) return 0;
	if (!pefx->Add(0x07, TEXT("Depth"), TEXT("デプス"), TEXT("揺れの深さを調節します。"), 24)) return 0;
	if (!pefx->Add(0x08, TEXT("# Feedback"), TEXT("フィードバック・レベル"), TEXT("エフェクト音を再び入力に戻すときの割合(%)を調節します。マイナスの値にしたときは位相が反転します。"), 89, 0x0f, 0x71, -98, TEXT("%+d%%"), 2)) return 0;
	if (!pefx->Add(0x09, TEXT("Phase"), TEXT("フェイズ"), TEXT("音の広がり具合を調節します。"), 90, 0x00, 0x5a, 0, TEXT("%d"), 2)) return 0;
	if (!pefx->Add(0x12, TEXT("Balance"), TEXT("エフェクト・バランス"), TEXT("原音とエフェクト音の音量バランスを調節します。表示の“D”、“E”は、D(ドライ音)、E(エフェクト音)の値がそれぞれ100であることを表します。"), g_efxpv_o2, sizeof(g_efxpv_o2) / sizeof(g_efxpv_o2[0]), 64)) return 0;
	if (!pefx->Add(0x13, TEXT("Low Gain"), TEXT("ロー・ゲイン"), TEXT("低域のゲインを調節します。"), 12, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x14, TEXT("Hi Gain"), TEXT("ハイ・ゲイン"), TEXT("高域のゲインを調節します。"), 12, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x16, TEXT("Level"), TEXT("アウトプット・レベル"), TEXT("出力音量を調節します。"), 104)) return 0;
	break;
case 13:
	if (!pefx->Add(0x03, TEXT("Pre Dly"), TEXT("プリ・ディレイ・タイム"), TEXT("原音が鳴ってからフランジャー音が鳴るまでの時間を調節します。"), g_efxpv1, sizeof(g_efxpv1) / sizeof(g_efxpv1[0]), 10)) return 0;
	if (!pefx->Add(0x04, TEXT("Rate"), TEXT("レイト"), TEXT("揺れの周期を調節します。"), g_efxpv6, sizeof(g_efxpv6) / sizeof(g_efxpv6[0]), 5)) return 0;
	if (!pefx->Add(0x05, TEXT("Depth"), TEXT("デプス"), TEXT("揺れの深さを調節します。"), 95)) return 0;
	if (!pefx->Add(0x06, TEXT("+ Feedback"), TEXT("フィードバック・レベル"), TEXT("エフェクト音を再び入力に戻すときの割合(%)を調節します。マイナスの値にしたときは位相が反転します。"), 64, 0x0f, 0x71, -98, TEXT("%+d%%"), 2)) return 0;
	if (!pefx->Add(0x07, TEXT("Phase"), TEXT("フェイズ"), TEXT("音の広がり具合を調節します。"), 90, 0x00, 0x5a, 0, TEXT("%d"), 2)) return 0;
	if (!pefx->Add(0x08, TEXT("# Step Rate"), TEXT("ステップ・レイト"), TEXT("ピッチ変化の周期を調節します。"), g_efxpv6, sizeof(g_efxpv6) / sizeof(g_efxpv6[0]), 54)) return 0;
	if (!pefx->Add(0x12, TEXT("Balance"), TEXT("エフェクト・バランス"), TEXT("原音とエフェクト音の音量バランスを調節します。表示の“D”、“E”は、D(ドライ音)、E(エフェクト音)の値がそれぞれ100であることを表します。"), g_efxpv_o2, sizeof(g_efxpv_o2) / sizeof(g_efxpv_o2[0]), 64)) return 0;
	if (!pefx->Add(0x13, TEXT("Low Gain"), TEXT("ロー・ゲイン"), TEXT("低域のゲインを調節します。"), 12, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x14, TEXT("Hi Gain"), TEXT("ハイ・ゲイン"), TEXT("高域のゲインを調節します。"), 12, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x16, TEXT("Level"), TEXT("アウトプット・レベル"), TEXT("出力音量を調節します。"), 104)) return 0;
	break;
case 14: case 15:
	if (!pefx->Add(0x03, TEXT("Mod Wave"), TEXT("モジュレーション・ウェーブ"), TEXT("揺れの変化のしかたを選びます。\n Tri ：三角波のように変化します。\n Sqr ：矩形波のように変化します。\n Sin ：正弦波のように変化します。\n Saw1,2 ：のこぎり波のように変化します。Saw1とSaw2では、のこぎりの刃の向きが逆になっています。(Saw1：／|／|、Saw2：＼|＼|)"), pv11, sizeof(pv11) / sizeof(pv11[0]), 1)) return 0;
	if (!pefx->Add(0x04, TEXT("+ Mod Rate"), TEXT("モジュレーション・レイト"), TEXT("揺れの周期を調節します。"), g_efxpv6, sizeof(g_efxpv6) / sizeof(g_efxpv6[0]), 60)) return 0;
	if (!pefx->Add(0x05, TEXT("# Mod Depth"), TEXT("モジュレーション・デプス"), TEXT("揺れの深さを調節します。"), 96)) return 0;
	if (!pefx->Add(0x13, TEXT("Low Gain"), TEXT("ロー・ゲイン"), TEXT("低域のゲインを調節します。"), 12, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x14, TEXT("Hi Gain"), TEXT("ハイ・ゲイン"), TEXT("高域のゲインを調節します。"), 12, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x16, TEXT("Level"), TEXT("アウトプット・レベル"), TEXT("出力音量を調節します。"), 127)) return 0;
	break;
case 16:
	if (!pefx->Add(0x03, TEXT("Attack"), TEXT("アタック"), TEXT("音を入力したときの音の立ち上がり時間を調節します。"), 72)) return 0;
	if (!pefx->Add(0x04, TEXT("Sustain"), TEXT("サスティン"), TEXT("小さなレベルの音を持ち上げて一定の音量に達するまでの時間を調節します。値を大きくすると時間が短くなります。値を変えるとレベルも変化します。"), 100)) return 0;
	if (!pefx->Add(0x05, TEXT("Post Gain"), TEXT("ポスト・ゲイン"), TEXT("出力ゲインを調節します。"), pv12, sizeof(pv12) / sizeof(pv12[0]), 0)) return 0;
	if (!pefx->Add(0x13, TEXT("Low Gain"), TEXT("ロー・ゲイン"), TEXT("低域のゲインを調節します。"), 12, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x14, TEXT("Hi Gain"), TEXT("ハイ・ゲイン"), TEXT("高域のゲインを調節します。"), 12, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x15, TEXT("+ Pan"), TEXT("アウトプット・パン"), TEXT("出力される音の定位を調節します。L63で最も左、0で中央、R63で最も右に定位します。"), g_efxpv_o1, sizeof(g_efxpv_o1) / sizeof(g_efxpv_o1[0]), 63)) return 0;
	if (!pefx->Add(0x16, TEXT("# Level"), TEXT("アウトプット・レベル"), TEXT("出力音量を調節します。"), 104)) return 0;
	break;
case 17:
	if (!pefx->Add(0x03, TEXT("Threshold"), TEXT("スレッショルド・レベル"), TEXT("圧縮を始める音量レベルを調節します。"), 85)) return 0;
	if (!pefx->Add(0x04, TEXT("Ratio"), TEXT("コンプレッション・レシオ"), TEXT("スレッショルド・レベル以上の信号の圧縮率を調節します。1/100が最も圧縮率が高く、出力レベルは小さくなります。"), pv13, sizeof(pv13) / sizeof(pv13[0]), 3)) return 0;
	if (!pefx->Add(0x05, TEXT("Release"), TEXT("リリース・タイム"), TEXT("音量がスレッショルド・レベル以下になってから効果がなくなるまでの時間を調節します。"), 16)) return 0;
	if (!pefx->Add(0x06, TEXT("Post Gain"), TEXT("ポスト・ゲイン"), TEXT("出力ゲインを調節します。"), pv12, sizeof(pv12) / sizeof(pv12[0]), 0)) return 0;
	if (!pefx->Add(0x13, TEXT("Low Gain"), TEXT("ロー・ゲイン"), TEXT("低域のゲインを調節します。"), 12, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x14, TEXT("Hi Gain"), TEXT("ハイ・ゲイン"), TEXT("高域のゲインを調節します。"), 12, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x15, TEXT("+ Pan"), TEXT("アウトプット・パン"), TEXT("出力される音の定位を調節します。L63で最も左、0で中央、R63で最も右に定位します。"), g_efxpv_o1, sizeof(g_efxpv_o1) / sizeof(g_efxpv_o1[0]), 63)) return 0;
	if (!pefx->Add(0x16, TEXT("# Level"), TEXT("アウトプット・レベル"), TEXT("出力音量を調節します。"), 127)) return 0;
	break;
case 18:
	if (!pefx->Add(0x03, TEXT("Pre Dly"), TEXT("プリ・ディレイ・タイム"), TEXT("原音が鳴ってからエフェクト音が鳴るまでの時間を調節します。"), g_efxpv1, sizeof(g_efxpv1) / sizeof(g_efxpv1[0]), 24)) return 0;
	if (!pefx->Add(0x04, TEXT("+ Rate"), TEXT("レイト"), TEXT("揺れの周期を調節します。"), g_efxpv6, sizeof(g_efxpv6) / sizeof(g_efxpv6[0]), 8)) return 0;
	if (!pefx->Add(0x05, TEXT("Depth"), TEXT("デプス"), TEXT("揺れの深さを調節します。"), 127)) return 0;
	if (!pefx->Add(0x06, TEXT("Pre Dly Dev"), TEXT("プリ・ディレイ・デビエーション"), TEXT("プリ・ディレイは原音が鳴ってからコーラス音が鳴るまでの時間です。ここでは、6相の各コーラス音のプリ・ディレイの差を調節します。"), 5, 0x00, 0x14)) return 0;
	if (!pefx->Add(0x07, TEXT("Depth Dev"), TEXT("デプス・デビエーション"), TEXT("6相の各コーラス音の揺れの深さの差を調節します。"), 22, 0x2C, 0x54, -20, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x08, TEXT("Pan Dev"), TEXT("パン・デビエーション"), TEXT("6相の各コーラス音の定位の差を調節します。0ですべてのコーラス音の定位が中央になります。20にすると、中央の定位を基準に各コーラス音が30度間隔で定位します。"), 16, 0x00, 0x14)) return 0;
	if (!pefx->Add(0x12, TEXT("# Balance"), TEXT("エフェクト・バランス"), TEXT("原音とエフェクト音の音量バランスを調節します。表示の“D”、“E”は、D(ドライ音)、E(エフェクト音)の値がそれぞれ100であることを表します。"), g_efxpv_o2, sizeof(g_efxpv_o2) / sizeof(g_efxpv_o2[0]), 64)) return 0;
	if (!pefx->Add(0x13, TEXT("Low Gain"), TEXT("ロー・ゲイン"), TEXT("低域のゲインを調節します。"), 12, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x14, TEXT("Hi Gain"), TEXT("ハイ・ゲイン"), TEXT("高域のゲインを調節します。"), 12, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x16, TEXT("Level"), TEXT("アウトプット・レベル"), TEXT("出力音量を調節します。"), 112)) return 0;
	break;
case 19:
	if (!pefx->Add(0x03, TEXT("Pre Dly"), TEXT("プリ・ディレイ・タイム"), TEXT("原音が鳴ってからコーラス音が鳴るまでの時間を調節します。"), g_efxpv1, sizeof(g_efxpv1) / sizeof(g_efxpv1[0]), 16)) return 0;
	if (!pefx->Add(0x04, TEXT("Cho Rate"), TEXT("コーラス・レイト"), TEXT("コーラス効果の揺れの周期を調節します。"), g_efxpv6, sizeof(g_efxpv6) / sizeof(g_efxpv6[0]), 8)) return 0;
	if (!pefx->Add(0x05, TEXT("Cho Depth"), TEXT("コーラス・デプス"), TEXT("コーラス効果の揺れの深さを調節します。"), 40)) return 0;
	if (!pefx->Add(0x06, TEXT("Trem Phase"), TEXT("トレモロ・フェイズ"), TEXT("トレモロ音の広がり具合を調節します。"), 40, 0x00, 0x5a, 0, TEXT("%d"), 2)) return 0;
	if (!pefx->Add(0x07, TEXT("+ Trem Rate"), TEXT("トレモロ・レイト"), TEXT("トレモロ効果の揺れの周期を調節します。"), g_efxpv6, sizeof(g_efxpv6) / sizeof(g_efxpv6[0]), 60)) return 0;
	if (!pefx->Add(0x08, TEXT("Trem Sep"), TEXT("トレモロ・セパレーション"), TEXT("トレモロ効果の広がり具合を調節します。"), 96)) return 0;
	if (!pefx->Add(0x12, TEXT("# Balance"), TEXT("エフェクト・バランス"), TEXT("原音とエフェクト音の音量バランスを調節します。表示の“D”、“E”は、D(ドライ音)、E(エフェクト音)の値がそれぞれ100であることを表します。"), g_efxpv_o2, sizeof(g_efxpv_o2) / sizeof(g_efxpv_o2[0]), 127)) return 0;
	if (!pefx->Add(0x13, TEXT("Low Gain"), TEXT("ロー・ゲイン"), TEXT("低域のゲインを調節します。"), 12, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x14, TEXT("Hi Gain"), TEXT("ハイ・ゲイン"), TEXT("高域のゲインを調節します。"), 12, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x16, TEXT("Level"), TEXT("アウトプット・レベル"), TEXT("出力音量を調節します。"), 127)) return 0;
	break;
case 20:
	if (!pefx->Add(0x03, TEXT("Pre Filter"), TEXT("プリ・フィルター・タイプ"), TEXT("フィルターの種類を選びます。\n Off ：フィルターを使いません。\n LPF ：Cutoffパラメーターより上の帯域をカットします。\n HPF ：Cutoffパラメーターより下の帯域をカットします。"), pv10, sizeof(pv10) / sizeof(pv10[0]), 0)) return 0;
	if (!pefx->Add(0x04, TEXT("Cutoff"), TEXT("カットオフ・フリケンシー"), TEXT("コーラス音のフィルターの基準周波数を調節します。"), g_efxpv9, sizeof(g_efxpv9) / sizeof(g_efxpv9[0]), 0)) return 0;
	if (!pefx->Add(0x05, TEXT("Pre Dly"), TEXT("プリ・ディレイ・タイム"), TEXT("原音が鳴ってからコーラス音が鳴るまでの時間を調節します。"), g_efxpv1, sizeof(g_efxpv1) / sizeof(g_efxpv1[0]), 10)) return 0;
	if (!pefx->Add(0x06, TEXT("+ Rate"), TEXT("レイト"), TEXT("揺れの周期を調節します。"), g_efxpv6, sizeof(g_efxpv6) / sizeof(g_efxpv6[0]), 8)) return 0;
	if (!pefx->Add(0x07, TEXT("Depth"), TEXT("デプス"), TEXT("揺れの深さを調節します。"), 111)) return 0;
	if (!pefx->Add(0x08, TEXT("Phase"), TEXT("フェイズ"), TEXT("音の広がり具合を調節します。"), 90, 0x00, 0x5a, 0, TEXT("%d"), 2)) return 0;
	if (!pefx->Add(0x12, TEXT("# Balance"), TEXT("エフェクト・バランス"), TEXT("原音とエフェクト音の音量バランスを調節します。表示の“D”、“E”は、D(ドライ音)、E(エフェクト音)の値がそれぞれ100であることを表します。"), g_efxpv_o2, sizeof(g_efxpv_o2) / sizeof(g_efxpv_o2[0]), 64)) return 0;
	if (!pefx->Add(0x13, TEXT("Low Gain"), TEXT("ロー・ゲイン"), TEXT("低域のゲインを調節します。"), 12, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x14, TEXT("Hi Gain"), TEXT("ハイ・ゲイン"), TEXT("高域のゲインを調節します。"), 12, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x16, TEXT("Level"), TEXT("アウトプット・レベル"), TEXT("出力音量を調節します。"), 104)) return 0;
	break;
case 21:
	if (!pefx->Add(0x03, TEXT("Pre Dly"), TEXT("プリ・ディレイ・タイム"), TEXT("原音が鳴ってからコーラス音が鳴るまでの時間を調節します。"), g_efxpv1, sizeof(g_efxpv1) / sizeof(g_efxpv1[0]), 32)) return 0;
	if (!pefx->Add(0x04, TEXT("+ Rate"), TEXT("レイト"), TEXT("揺れの周期を調節します。"), g_efxpv6, sizeof(g_efxpv6) / sizeof(g_efxpv6[0]), 8)) return 0;
	if (!pefx->Add(0x05, TEXT("Depth"), TEXT("デプス"), TEXT("揺れの深さを調節します。"), 127)) return 0;
	if (!pefx->Add(0x06, TEXT("Phase"), TEXT("フェイズ"), TEXT("音の広がり具合を調節します。"), 90, 0x00, 0x5a, 0, TEXT("%d"), 2)) return 0;
	if (!pefx->Add(0x12, TEXT("# Balance"), TEXT("エフェクト・バランス"), TEXT("原音とエフェクト音の音量バランスを調節します。表示の“D”、“E”は、D(ドライ音)、E(エフェクト音)の値がそれぞれ100であることを表します。"), g_efxpv_o2, sizeof(g_efxpv_o2) / sizeof(g_efxpv_o2[0]), 64)) return 0;
	if (!pefx->Add(0x13, TEXT("Low Gain"), TEXT("ロー・ゲイン"), TEXT("低域のゲインを調節します。"), 12, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x14, TEXT("Hi Gain"), TEXT("ハイ・ゲイン"), TEXT("高域のゲインを調節します。"), 12, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x16, TEXT("Level"), TEXT("アウトプット・レベル"), TEXT("出力音量を調節します。"), 96)) return 0;
	break;
case 22:
	if (!pefx->Add(0x03, TEXT("Pre Dly"), TEXT("プリ・ディレイ・タイム"), TEXT("原音が鳴ってからコーラス音が鳴るまでの時間を調節します。"), g_efxpv1, sizeof(g_efxpv1) / sizeof(g_efxpv1[0]), 10)) return 0;
	if (!pefx->Add(0x04, TEXT("+ Cho Rate"), TEXT("コーラス・レイト"), TEXT("コーラス音の揺れの周期を調節します。"), g_efxpv6, sizeof(g_efxpv6) / sizeof(g_efxpv6[0]), 8)) return 0;
	if (!pefx->Add(0x05, TEXT("Cho Depth"), TEXT("コーラス・デプス"), TEXT("コーラス音の揺れの深さを調節します。"), 72)) return 0;
	if (!pefx->Add(0x11, TEXT("Out"), TEXT("アウトプット・モード"), TEXT("OUTPUTジャックから出力させる音を再生する方法を設定します。スピーカーで鳴らすときはSpeakerを、ヘッドホンで鳴らすときはPhonesを選ぶと、最適な3D効果が得られます。"), pv14, sizeof(pv14) / sizeof(pv14[0]), 0)) return 0;
	if (!pefx->Add(0x12, TEXT("# Balance"), TEXT("エフェクト・バランス"), TEXT("原音とエフェクト音の音量バランスを調節します。表示の“D”、“E”は、D(ドライ音)、E(エフェクト音)の値がそれぞれ100であることを表します。"), g_efxpv_o2, sizeof(g_efxpv_o2) / sizeof(g_efxpv_o2[0]), 64)) return 0;
	if (!pefx->Add(0x13, TEXT("Low Gain"), TEXT("ロー・ゲイン"), TEXT("低域のゲインを調節します。"), 12, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x14, TEXT("Hi Gain"), TEXT("ハイ・ゲイン"), TEXT("高域のゲインを調節します。"), 12, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x16, TEXT("Level"), TEXT("アウトプット・レベル"), TEXT("出力音量を調節します。"), 80)) return 0;
	break;
case 23:
	if (!pefx->Add(0x03, TEXT("Dly Tm L"), TEXT("ディレイ・タイム・レフト"), TEXT("原音が鳴ってから左のディレイ音が鳴るまでの時間を調節します。"), g_efxpv4, sizeof(g_efxpv4) / sizeof(g_efxpv4[0]), 101)) return 0;
	if (!pefx->Add(0x04, TEXT("Dly Tm R"), TEXT("ディレイ・タイム・ライト"), TEXT("原音が鳴ってから右のディレイ音が鳴るまでの時間を調節します。"), g_efxpv4, sizeof(g_efxpv4) / sizeof(g_efxpv4[0]), 116)) return 0;
	if (!pefx->Add(0x05, TEXT("+ Feedback"), TEXT("フィードバック・レベル"), TEXT("エフェクト音を再び入力に戻すときの割合(%)を調節します。マイナスの値にしたときは位相が反転します。"), 73, 0x0f, 0x71, -98, TEXT("%+d%%"), 2)) return 0;
	if (!pefx->Add(0x06, TEXT("Fb Mode"), TEXT("フィードバック・モード"), TEXT("エフェクト音を入力に戻すときのつなぎかたを選びます。\n Norm ：左のディレイ音は左に、右のディレイ音は右に戻します。\n Cross ：左のディレイ音は右に、右のディレイ音は左に戻します。"), pv15, sizeof(pv15) / sizeof(pv15[0]), 1)) return 0;
	if (!pefx->Add(0x07, TEXT("Phase L"), TEXT("フェイズ・レフト"), TEXT("左のディレイ音の位相を選びます。\n Norm ：位相は変わりません。\n Invert ：位相が反転します。"), pv16, sizeof(pv16) / sizeof(pv16[0]), 0)) return 0;
	if (!pefx->Add(0x08, TEXT("Phase R"), TEXT("フェイズ・ライト"), TEXT("右のディレイ音の位相を選びます。\n Norm ：位相は変わりません。\n Invert ：位相が反転します。"), pv16, sizeof(pv16) / sizeof(pv16[0]), 0)) return 0;
	if (!pefx->Add(0x0a, TEXT("HF Damp"), TEXT("HFダンプ"), TEXT("入力に戻したエフェクト音について、高域成分をカットする周波数を調節します。カットしないときはBypassに設定します。"), g_efxpv8, sizeof(g_efxpv8) / sizeof(g_efxpv8[0]), 15)) return 0;
	if (!pefx->Add(0x12, TEXT("# Balance"), TEXT("エフェクト・バランス"), TEXT("原音とエフェクト音の音量バランスを調節します。表示の“D”、“E”は、D(ドライ音)、E(エフェクト音)の値がそれぞれ100であることを表します。"), g_efxpv_o2, sizeof(g_efxpv_o2) / sizeof(g_efxpv_o2[0]), 63)) return 0;
	if (!pefx->Add(0x13, TEXT("Low Gain"), TEXT("ロー・ゲイン"), TEXT("低域のゲインを調節します。"), 12, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x14, TEXT("Hi Gain"), TEXT("ハイ・ゲイン"), TEXT("高域のゲインを調節します。"), 12, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x16, TEXT("Level"), TEXT("アウトプット・レベル"), TEXT("出力音量を調節します。"), 127)) return 0;
	break;
case 24:
	if (!pefx->Add(0x03, TEXT("Dly Tm L"), TEXT("ディレイ・タイム・レフト"), TEXT("原音が鳴ってから左のディレイ音が鳴るまでの時間を調節します。"), g_efxpv4, sizeof(g_efxpv4) / sizeof(g_efxpv4[0]), 90)) return 0;
	if (!pefx->Add(0x04, TEXT("Dly Tm R"), TEXT("ディレイ・タイム・ライト"), TEXT("原音が鳴ってから右のディレイ音が鳴るまでの時間を調節します。"), g_efxpv4, sizeof(g_efxpv4) / sizeof(g_efxpv4[0]), 108)) return 0;
	if (!pefx->Add(0x05, TEXT("Feedback"), TEXT("フィードバック・レベル"), TEXT("エフェクト音を再び入力に戻すときの割合(%)を調節します。マイナスの値にしたときは位相が反転します。"), 73, 0x0f, 0x71, -98, TEXT("%+d%%"), 2)) return 0;
	if (!pefx->Add(0x06, TEXT("Fb Mode"), TEXT("フィードバック・モード"), TEXT("エフェクト音を入力に戻すときのつなぎかたを選びます。\n Norm ：左のディレイ音は左に、右のディレイ音は右に戻します。\n Cross ：左のディレイ音は右に、右のディレイ音は左に戻します。"), pv15, sizeof(pv15) / sizeof(pv15[0]), 1)) return 0;
	if (!pefx->Add(0x07, TEXT("+ Mod Rate"), TEXT("モジュレーション・レイト"), TEXT("揺れの周期を調節します。"), g_efxpv6, sizeof(g_efxpv6) / sizeof(g_efxpv6[0]), 12)) return 0;
	if (!pefx->Add(0x08, TEXT("Mod Depth"), TEXT("モジュレーション・デプス"), TEXT("揺れの深さを調節します。"), 21)) return 0;
	if (!pefx->Add(0x09, TEXT("Mod Phase"), TEXT("モジュレーション・フェイズ"), TEXT("音の広がり具合を調節します。"), 90, 0x00, 0x5a, 0, TEXT("%d"), 2)) return 0;
	if (!pefx->Add(0x0a, TEXT("HF Damp"), TEXT("HFダンプ"), TEXT("入力に戻したエフェクト音について、高域成分をカットする周波数を調節します。カットしないときはBypassに設定します。"), g_efxpv8, sizeof(g_efxpv8) / sizeof(g_efxpv8[0]), 15)) return 0;
	if (!pefx->Add(0x12, TEXT("# Balance"), TEXT("エフェクト・バランス"), TEXT("原音とエフェクト音の音量バランスを調節します。表示の“D”、“E”は、D(ドライ音)、E(エフェクト音)の値がそれぞれ100であることを表します。"), g_efxpv_o2, sizeof(g_efxpv_o2) / sizeof(g_efxpv_o2[0]), 61)) return 0;
	if (!pefx->Add(0x13, TEXT("Low Gain"), TEXT("ロー・ゲイン"), TEXT("低域のゲインを調節します。"), 12, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x14, TEXT("Hi Gain"), TEXT("ハイ・ゲイン"), TEXT("高域のゲインを調節します。"), 12, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x16, TEXT("Level"), TEXT("アウトプット・レベル"), TEXT("出力音量を調節します。"), 127)) return 0;
	break;
case 25:
	if (!pefx->Add(0x03, TEXT("Dly Tm C"), TEXT("ディレイ・タイム・センター"), TEXT("原音が鳴ってから中央のディレイ音が鳴るまでの時間をそれぞれ調節します。"), g_efxpv2, sizeof(g_efxpv2) / sizeof(g_efxpv2[0]), 20)) return 0;
	if (!pefx->Add(0x04, TEXT("Dly Tm L"), TEXT("ディレイ・タイム・レフト"), TEXT("原音が鳴ってから左のディレイ音が鳴るまでの時間を調節します。"), g_efxpv2, sizeof(g_efxpv2) / sizeof(g_efxpv2[0]), 0)) return 0;
	if (!pefx->Add(0x05, TEXT("Dly Tm R"), TEXT("ディレイ・タイム・ライト"), TEXT("原音が鳴ってから右のディレイ音が鳴るまでの時間を調節します。"), g_efxpv2, sizeof(g_efxpv2) / sizeof(g_efxpv2[0]), 7)) return 0;
	if (!pefx->Add(0x06, TEXT("+ Feedback"), TEXT("フィードバック・レベル"), TEXT("エフェクト・センター音を再び入力に戻すときの割合(%)を調節します。マイナスの値にしたときは位相が反転します。"), 65, 0x0f, 0x71, -98, TEXT("%+d%%"), 2)) return 0;
	if (!pefx->Add(0x07, TEXT("Dly Lev C"), TEXT("ディレイ・レベル・センター"), TEXT("中央のディレイ音の音量を調節します。"), 127)) return 0;
	if (!pefx->Add(0x08, TEXT("Dly Lev L"), TEXT("ディレイ・レベル・レフト"), TEXT("左のディレイ音の音量を調節します。"), 127)) return 0;
	if (!pefx->Add(0x09, TEXT("Dly Lev R"), TEXT("ディレイ・レベル・ライト"), TEXT("右のディレイ音の音量を調節します。"), 127)) return 0;
	if (!pefx->Add(0x0a, TEXT("HF Damp"), TEXT("HFダンプ"), TEXT("ディレイ・センター音を入力に戻したときの、高域成分をカットする周波数を調節します。カットしないときはBypassに設定します。"), g_efxpv8, sizeof(g_efxpv8) / sizeof(g_efxpv8[0]), 15)) return 0;
	if (!pefx->Add(0x12, TEXT("# Balance"), TEXT("エフェクト・バランス"), TEXT("原音とエフェクト音の音量バランスを調節します。表示の“D”、“E”は、D(ドライ音)、E(エフェクト音)の値がそれぞれ100であることを表します。"), g_efxpv_o2, sizeof(g_efxpv_o2) / sizeof(g_efxpv_o2[0]), 63)) return 0;
	if (!pefx->Add(0x13, TEXT("Low Gain"), TEXT("ロー・ゲイン"), TEXT("低域のゲインを調節します。"), 12, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x14, TEXT("Hi Gain"), TEXT("ハイ・ゲイン"), TEXT("高域のゲインを調節します。"), 12, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x16, TEXT("Level"), TEXT("アウトプット・レベル"), TEXT("出力音量を調節します。"), 127)) return 0;
	break;
case 26:
	if (!pefx->Add(0x03, TEXT("Dly Tm 1"), TEXT("ディレイ・タイム1"), TEXT("原音が鳴ってから、ディレイ1の音が鳴るまでの時間を調節します。"), g_efxpv2, sizeof(g_efxpv2) / sizeof(g_efxpv2[0]), 60)) return 0;
	if (!pefx->Add(0x04, TEXT("Dly Tm 2"), TEXT("ディレイ・タイム2"), TEXT("原音が鳴ってから、ディレイ2の音が鳴るまでの時間を調節します。"), g_efxpv2, sizeof(g_efxpv2) / sizeof(g_efxpv2[0]), 20)) return 0;
	if (!pefx->Add(0x05, TEXT("Dly Tm 3"), TEXT("ディレイ・タイム3"), TEXT("原音が鳴ってから、ディレイ3の音が鳴るまでの時間を調節します。"), g_efxpv2, sizeof(g_efxpv2) / sizeof(g_efxpv2[0]), 40)) return 0;
	if (!pefx->Add(0x06, TEXT("Dly Tm 4"), TEXT("ディレイ・タイム4"), TEXT("原音が鳴ってから、ディレイ4の音が鳴るまでの時間を調節します。"), g_efxpv2, sizeof(g_efxpv2) / sizeof(g_efxpv2[0]), 0)) return 0;
	if (!pefx->Add(0x07, TEXT("Dly Lev 1"), TEXT("ディレイ・レベル1"), TEXT("ディレイ1の音量を調節します。"), 127)) return 0;
	if (!pefx->Add(0x08, TEXT("Dly Lev 2"), TEXT("ディレイ・レベル2"), TEXT("ディレイ2の音量を調節します。"), 127)) return 0;
	if (!pefx->Add(0x09, TEXT("Dly Lev 3"), TEXT("ディレイ・レベル3"), TEXT("ディレイ3の音量を調節します。"), 127)) return 0;
	if (!pefx->Add(0x0a, TEXT("Dly Lev 4"), TEXT("ディレイ・レベル4"), TEXT("ディレイ4の音量を調節します。"), 127)) return 0;
	if (!pefx->Add(0x0b, TEXT("+ Feedback"), TEXT("フィードバック・レベル"), TEXT("ディレイ1を再び入力に戻すときの割合(%)を調節します。マイナスの値にしたときは位相が反転します。"), 65, 0x0f, 0x71, -98, TEXT("%+d%%"), 2)) return 0;
	if (!pefx->Add(0x0c, TEXT("HF Damp"), TEXT("HFダンプ"), TEXT("ディレイ1音を入力に戻したときの、高域成分をカットする周波数を調節します。カットしないときはBypassに設定します。"), g_efxpv8, sizeof(g_efxpv8) / sizeof(g_efxpv8[0]), 15)) return 0;
	if (!pefx->Add(0x12, TEXT("# Balance"), TEXT("エフェクト・バランス"), TEXT("原音とエフェクト音の音量バランスを調節します。表示の“D”、“E”は、D(ドライ音)、E(エフェクト音)の値がそれぞれ100であることを表します。"), g_efxpv_o2, sizeof(g_efxpv_o2) / sizeof(g_efxpv_o2[0]), 63)) return 0;
	if (!pefx->Add(0x13, TEXT("Low Gain"), TEXT("ロー・ゲイン"), TEXT("低域のゲインを調節します。"), 12, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x14, TEXT("Hi Gain"), TEXT("ハイ・ゲイン"), TEXT("高域のゲインを調節します。"), 12, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x16, TEXT("Level"), TEXT("アウトプット・レベル"), TEXT("出力音量を調節します。"), 127)) return 0;
	break;
case 27:
	if (!pefx->Add(0x03, TEXT("+ Dly Time"), TEXT("ディレイ・タイム"), TEXT("原音が鳴ってからディレイ音が鳴るまでの時間を調節します。"), g_efxpv3, sizeof(g_efxpv3) / sizeof(g_efxpv3[0]), 60)) return 0;
	if (!pefx->Add(0x04, TEXT("Accel"), TEXT("アクセラレーション"), TEXT("ディレイ・タイムを変化させた場合、現在のディレイ・タイムから指定のディレイ・タイムに達するまでの時間を調節します。ディレイ・タイムと同時にピッチ変化の速さも変わります。"), g_efxpv14, sizeof(g_efxpv14) / sizeof(g_efxpv14[0]), 10)) return 0;
	if (!pefx->Add(0x05, TEXT("# Feedback"), TEXT("フィードバック・レベル"), TEXT("エフェクト音を再び入力に戻すときの割合(%)を調節します。マイナスの値にしたときは位相が反転します。"), 65, 0x0f, 0x71, -98, TEXT("%+d%%"), 2)) return 0;
	if (!pefx->Add(0x06, TEXT("HF Damp"), TEXT("HFダンプ"), TEXT("入力に戻したエフェクト音について、高域成分をカットする周波数を調節します。カットしないときはBypassに設定します。"), g_efxpv8, sizeof(g_efxpv8) / sizeof(g_efxpv8[0]), 15)) return 0;
	if (!pefx->Add(0x07, TEXT("EFX Pan"), TEXT("エフェクト・アウトプット・パン"), TEXT("エフェクト音の定位を調節します。L63で最も左、0で中央、R63で最も右に定位します。"), g_efxpv_o1, sizeof(g_efxpv_o1) / sizeof(g_efxpv_o1[0]), 63)) return 0;
	if (!pefx->Add(0x12, TEXT("Balance"), TEXT("エフェクト・バランス"), TEXT("原音とエフェクト音の音量バランスを調節します。表示の“D”、“E”は、D(ドライ音)、E(エフェクト音)の値がそれぞれ100であることを表します。"), g_efxpv_o2, sizeof(g_efxpv_o2) / sizeof(g_efxpv_o2[0]), 63)) return 0;
	if (!pefx->Add(0x13, TEXT("Low Gain"), TEXT("ロー・ゲイン"), TEXT("低域のゲインを調節します。"), 12, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x14, TEXT("Hi Gain"), TEXT("ハイ・ゲイン"), TEXT("高域のゲインを調節します。"), 12, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x16, TEXT("Level"), TEXT("アウトプット・レベル"), TEXT("出力音量を調節します。"), 127)) return 0;
	break;
case 28:
	if (!pefx->Add(0x03, TEXT("Type"), TEXT("リバーブ・タイプ"), TEXT("リバーブの種類を選びます。\n Room1 ：残響が短く、密度の濃いリバーブ\n Room2 ：残響が短く、密度の薄いリバーブ\n Stage1 ：後部残響音の多いリバーブ\n Stage2 ：初期反射の強いリバーブ\n Hall1 ：澄んだ響きのリバーブ\n Hall2 ：豊かな響きのリバーブ"), pv17, sizeof(pv17) / sizeof(pv17[0]), 3)) return 0;
	if (!pefx->Add(0x04, TEXT("Pre Dly"), TEXT("プリ・ディレイ・タイム"), TEXT("原音が鳴ってからリバーブ音が鳴るまでの時間を調節します。"), g_efxpv1, sizeof(g_efxpv1) / sizeof(g_efxpv1[0]), 112)) return 0;
	if (!pefx->Add(0x05, TEXT("+ Time"), TEXT("リバーブ・タイム"), TEXT("リバーブ音の余韻の長さを調節します。"), 120)) return 0;
	if (!pefx->Add(0x06, TEXT("HF Damp"), TEXT("HFダンプ"), TEXT("残響音の高域成分をカットする周波数を調節します。周波数を低くするほど高域成分がカットされ、やわらかな残響音になります。高域成分をカットしないときはBypassにします。"), g_efxpv8, sizeof(g_efxpv8) / sizeof(g_efxpv8[0]), 13)) return 0;
	if (!pefx->Add(0x12, TEXT("# Balance"), TEXT("エフェクト・バランス"), TEXT("原音とエフェクト音の音量バランスを調節します。表示の“D”、“E”は、D(ドライ音)、E(エフェクト音)の値がそれぞれ100であることを表します。"), g_efxpv_o2, sizeof(g_efxpv_o2) / sizeof(g_efxpv_o2[0]), 64)) return 0;
	if (!pefx->Add(0x13, TEXT("Low Gain"), TEXT("ロー・ゲイン"), TEXT("低域のゲインを調節します。"), 12, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x14, TEXT("Hi Gain"), TEXT("ハイ・ゲイン"), TEXT("高域のゲインを調節します。"), 12, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x16, TEXT("Level"), TEXT("アウトプット・レベル"), TEXT("出力音量を調節します。"), 127)) return 0;
	break;
case 29:
	if (!pefx->Add(0x03, TEXT("Type"), TEXT("ゲート・リバーブ・タイプ"), TEXT("リバーブの種類を選びます。\n Norm ：通常のゲート・リバーブ。\n Reverse ：逆回転のリバーブ。\n Sweep1 ：残響音が右から左へ移動します。\n Sweep2 ：残響音が左から右へ移動します。"), pv18, sizeof(pv18) / sizeof(pv18[0]), 0)) return 0;
	if (!pefx->Add(0x04, TEXT("Pre Dly"), TEXT("プリ・ディレイ・タイム"), TEXT("原音が鳴ってからリバーブ音が鳴るまでの時間を調節します。"), g_efxpv1, sizeof(g_efxpv1) / sizeof(g_efxpv1[0]), 5)) return 0;
	if (!pefx->Add(0x06, TEXT("Gate Time"), TEXT("ゲート・タイム"), TEXT("リバーブ音が鳴りはじめてから消えるまでの時間を調節します。"), 13, 0x00, 0x63, 0, TEXT("%d"), 5)) return 0;
	if (!pefx->Add(0x12, TEXT("+ Balance"), TEXT("エフェクト・バランス"), TEXT("原音とエフェクト音の音量バランスを調節します。表示の“D”、“E”は、D(ドライ音)、E(エフェクト音)の値がそれぞれ100であることを表します。"), g_efxpv_o2, sizeof(g_efxpv_o2) / sizeof(g_efxpv_o2[0]), 63)) return 0;
	if (!pefx->Add(0x13, TEXT("Low Gain"), TEXT("ロー・ゲイン"), TEXT("低域のゲインを調節します。"), 12, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x14, TEXT("Hi Gain"), TEXT("ハイ・ゲイン"), TEXT("高域のゲインを調節します。"), 9, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x16, TEXT("# Level"), TEXT("アウトプット・レベル"), TEXT("出力音量を調節します。"), 112)) return 0;
	break;
case 30:
	if (!pefx->Add(0x03, TEXT("Dly Tm C"), TEXT("ディレイ・タイム・センター"), TEXT("原音が鳴ってから中央のディレイ音が鳴るまでの時間をそれぞれ調節します。"), g_efxpv2, sizeof(g_efxpv2) / sizeof(g_efxpv2[0]), 20)) return 0;
	if (!pefx->Add(0x04, TEXT("Dly Tm L"), TEXT("ディレイ・タイム・レフト"), TEXT("原音が鳴ってから左のディレイ音が鳴るまでの時間をそれぞれ調節します。"), g_efxpv2, sizeof(g_efxpv2) / sizeof(g_efxpv2[0]), 0)) return 0;
	if (!pefx->Add(0x05, TEXT("Dly Tm R"), TEXT("ディレイ・タイム・ライト"), TEXT("原音が鳴ってから右のディレイ音が鳴るまでの時間をそれぞれ調節します。"), g_efxpv2, sizeof(g_efxpv2) / sizeof(g_efxpv2[0]), 8)) return 0;
	if (!pefx->Add(0x06, TEXT("+ Feedback"), TEXT("フィードバック・レベル"), TEXT("ディレイ・センター音を再び入力に戻すときの割合(%)を調節します。マイナスの値にしたときは位相が反転します。"), 65, 0x0f, 0x71, -98, TEXT("%+d%%"), 2)) return 0;
	if (!pefx->Add(0x07, TEXT("Dly Lev C"), TEXT("ディレイ・レベル・センター"), TEXT("中央のディレイ音の音量を調節します。"), 40)) return 0;
	if (!pefx->Add(0x08, TEXT("Dly Lev L"), TEXT("ディレイ・レベル・レフト"), TEXT("左のディレイ音の音量を調節します。"), 64)) return 0;
	if (!pefx->Add(0x09, TEXT("Dly Lev R"), TEXT("ディレイ・レベル・ライト"), TEXT("右のディレイ音の音量を調節します。"), 64)) return 0;
	if (!pefx->Add(0x0a, TEXT("HF Damp"), TEXT("HFダンプ"), TEXT("ディレイ・センター音を入力に戻したときの、高域成分をカットする周波数を調節します。カットしないときはBypassに設定します。"), g_efxpv8, sizeof(g_efxpv8) / sizeof(g_efxpv8[0]), 15)) return 0;
	if (!pefx->Add(0x11, TEXT("Out"), TEXT("アウトプット・モード"), TEXT("OUTPUTジャックから出力させる音を再生する方法を設定します。スピーカーで鳴らすときはSpeakerを、ヘッドホンで鳴らすときはPhonesを選ぶと、最適な3D効果が得られます。"), pv14, sizeof(pv14) / sizeof(pv14[0]), 0)) return 0;
	if (!pefx->Add(0x12, TEXT("# Balance"), TEXT("エフェクト・バランス"), TEXT("原音とエフェクト音の音量バランスを調節します。表示の“D”、“E”は、D(ドライ音)、E(エフェクト音)の値がそれぞれ100であることを表します。"), g_efxpv_o2, sizeof(g_efxpv_o2) / sizeof(g_efxpv_o2[0]), 63)) return 0;
	if (!pefx->Add(0x13, TEXT("Low Gain"), TEXT("ロー・ゲイン"), TEXT("低域のゲインを調節します。"), 12, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x14, TEXT("Hi Gain"), TEXT("ハイ・ゲイン"), TEXT("高域のゲインを調節します。"), 12, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x16, TEXT("Level"), TEXT("アウトプット・レベル"), TEXT("出力音量を調節します。"), 127)) return 0;
	break;
case 31:
	if (!pefx->Add(0x03, TEXT("+ Coarse 1"), TEXT("コース・ピッチ1"), TEXT("ピッチ・シフト1のピッチを半音単位で調節します(-2～+1オクターブ)。"), 31, 0x28, 0x4c, -24, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x04, TEXT("Fine 1"), TEXT("ファイン・ピッチ1"), TEXT("ピッチ・シフト1のピッチを2セント単位で微調節します(-100～+100セント)。"), 48, 0x0e, 0x72, -100, TEXT("%+d"), 2)) return 0;
	if (!pefx->Add(0x05, TEXT("Pre Dly 1"), TEXT("プリ・ディレイ・タイム1"), TEXT("原音が鳴ってからピッチ・シフト1の音が鳴るまでの時間を調節します。"), g_efxpv1, sizeof(g_efxpv1) / sizeof(g_efxpv1[0]), 0)) return 0;
	if (!pefx->Add(0x06, TEXT("EFX Pan 1"), TEXT("エフェクト・アウトプット・パン1"), TEXT("ピッチ・シフト1の音の定位を調節します。L63で最も左、0で中央、R63で最も右に定位します。"), g_efxpv_o1, sizeof(g_efxpv_o1) / sizeof(g_efxpv_o1[0]), 127)) return 0;
	if (!pefx->Add(0x07, TEXT("# Coarse 2"), TEXT("コース・ピッチ2"), TEXT("ピッチ・シフト2のピッチを半音単位で調節します(-2～+1オクターブ)。"), 19, 0x28, 0x4c, -24, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x08, TEXT("Fine 2"), TEXT("ファイン・ピッチ2"), TEXT("ピッチ・シフト2のピッチを2セント単位で微調節します(-100～+100セント)。"), 52, 0x0e, 0x72, -100, TEXT("%+d"), 2)) return 0;
	if (!pefx->Add(0x09, TEXT("Pre Dly 2"), TEXT("プリ・ディレイ・タイム2"), TEXT("原音が鳴ってからピッチ・シフト2の音が鳴るまでの時間を調節します。"), g_efxpv1, sizeof(g_efxpv1) / sizeof(g_efxpv1[0]), 0)) return 0;
	if (!pefx->Add(0x0a, TEXT("EFX Pan 2"), TEXT("エフェクト・アウトプット・パン2"), TEXT("ピッチ・シフト2の音の定位を調節します。L63で最も左、0で中央、R63で最も右に定位します。"), g_efxpv_o1, sizeof(g_efxpv_o1) / sizeof(g_efxpv_o1[0]), 0)) return 0;
	if (!pefx->Add(0x0b, TEXT("Shift Mode"), TEXT("ピッチ・シフター・モード"), TEXT("数値を大きくするほど反応が遅くなりますが、音揺れが少なくなります。"), 3, 0x00, 0x04)) return 0;
	if (!pefx->Add(0x0c, TEXT("L.Bal"), TEXT("レベル・バランス"), TEXT("ピッチ・シフト1とピッチ・シフト2の音量バランスを調節します。"), g_efxpv_o3, sizeof(g_efxpv_o3) / sizeof(g_efxpv_o3[0]), 64)) return 0;
	if (!pefx->Add(0x12, TEXT("Balance"), TEXT("エフェクト・バランス"), TEXT("原音とエフェクト音の音量バランスを調節します。表示の“D”、“E”は、D(ドライ音)、E(エフェクト音)の値がそれぞれ100であることを表します。"), g_efxpv_o2, sizeof(g_efxpv_o2) / sizeof(g_efxpv_o2[0]), 63)) return 0;
	if (!pefx->Add(0x13, TEXT("Low Gain"), TEXT("ロー・ゲイン"), TEXT("低域のゲインを調節します。"), 12, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x14, TEXT("Hi Gain"), TEXT("ハイ・ゲイン"), TEXT("高域のゲインを調節します。"), 12, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x16, TEXT("Level"), TEXT("アウトプット・レベル"), TEXT("出力音量を調節します。"), 95)) return 0;
	break;
case 32:
	if (!pefx->Add(0x03, TEXT("P.Coarse"), TEXT("コース・ピッチ"), TEXT("ピッチ・シフトのピッチを半音単位で調節します(-2～+1オクターブ)。"), 31, 0x28, 0x4c, -24, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x04, TEXT("P.Fine"), TEXT("ファイン・ピッチ"), TEXT("ピッチ・シフトのピッチを2セント単位で微調節します(-100～+100セント)。"), 50, 0x0e, 0x72, -100, TEXT("%+d"), 2)) return 0;
	if (!pefx->Add(0x05, TEXT("# Feedback"), TEXT("フィードバック・レベル"), TEXT("ピッチ・シフト音を再び入力に戻すときの割合(%)を調節します。マイナスの値にしたときは位相が反転します。"), 69, 0x0f, 0x71, -98, TEXT("%+d%%"), 2)) return 0;
	if (!pefx->Add(0x06, TEXT("Pre Dly"), TEXT("プリ・ディレイ・タイム"), TEXT("原音が鳴ってからピッチ・シフト音が鳴るまでの時間を調節します。"), g_efxpv1, sizeof(g_efxpv1) / sizeof(g_efxpv1[0]), 95)) return 0;
	if (!pefx->Add(0x07, TEXT("Mode"), TEXT("ピッチ・シフター・モード"), TEXT("数値を大きくするほど反応が遅くなりますが、音揺れが少なくなります。"), 3, 0x00, 0x04)) return 0;
	if (!pefx->Add(0x08, TEXT("EFX Pan"), TEXT("エフェクト・アウトプット・パン"), TEXT("ピッチ・シフト音の定位を調節します。L63で最も左、0で中央、R63で最も右に定位します。"), g_efxpv_o1, sizeof(g_efxpv_o1) / sizeof(g_efxpv_o1[0]), 63)) return 0;
	if (!pefx->Add(0x12, TEXT("Balance"), TEXT("エフェクト・バランス"), TEXT("原音とエフェクト音の音量バランスを調節します。表示の“D”、“E”は、D(ドライ音)、E(エフェクト音)の値がそれぞれ100であることを表します。"), g_efxpv_o2, sizeof(g_efxpv_o2) / sizeof(g_efxpv_o2[0]), 64)) return 0;
	if (!pefx->Add(0x13, TEXT("Low Gain"), TEXT("ロー・ゲイン"), TEXT("低域のゲインを調節します。"), 12, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x14, TEXT("Hi Gain"), TEXT("ハイ・ゲイン"), TEXT("高域のゲインを調節します。"), 6, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x16, TEXT("Level"), TEXT("アウトプット・レベル"), TEXT("出力音量を調節します。"), 127)) return 0;
	break;
case 33:
	if (!pefx->Add(0x03, TEXT("Azimuth"), TEXT("アジマス"), TEXT("回転を止めたときに止まる位置を設定します。0で中央に定位します。"), g_efxpv13, sizeof(g_efxpv13) / sizeof(g_efxpv13[0]), 15)) return 0;
	if (!pefx->Add(0x04, TEXT("+ Speed"), TEXT("スピード"), TEXT("回転する速さを設定します。"), g_efxpv6, sizeof(g_efxpv6) / sizeof(g_efxpv6[0]), 25)) return 0;
	if (!pefx->Add(0x05, TEXT("Clockwise"), TEXT("クロックワイズ"), TEXT("回転方向を設定します。-で反時計周りに、+で時計周りに回転します。"), pv19, sizeof(pv19) / sizeof(pv19[0]), 1)) return 0;
	if (!pefx->Add(0x06, TEXT("# Turn"), TEXT("ターン"), TEXT("回転を止めたり、動かしたりします。Onにすると回転します。Offにすると回転がAzimuthで設定した位置で止まります。"), pv4, sizeof(pv4) / sizeof(pv4[0]), 1)) return 0;
	if (!pefx->Add(0x11, TEXT("Out"), TEXT("アウトプット・モード"), TEXT("OUTPUTジャックから出力させる音を再生する方法を設定します。スピーカーで鳴らすときはSpeakerを、ヘッドホンで鳴らすときはPhonesを選ぶと、最適な3D効果が得られます。"), pv14, sizeof(pv14) / sizeof(pv14[0]), 0)) return 0;
	if (!pefx->Add(0x16, TEXT("Level"), TEXT("アウトプット・レベル"), TEXT("出力音量を調節します。"), 127)) return 0;
	break;
case 34:
	if (!pefx->Add(0x03, TEXT("+ Azimuth"), TEXT("アジマス"), TEXT("定位を設定します。0で中央に定位します。"), g_efxpv13, sizeof(g_efxpv13) / sizeof(g_efxpv13[0]), 15)) return 0;
	if (!pefx->Add(0x11, TEXT("Out"), TEXT("アウトプット・モード"), TEXT("OUTPUTジャックから出力させる音を再生する方法を設定します。スピーカーで鳴らすときはSpeakerを、ヘッドホンで鳴らすときはPhonesを選ぶと、最適な3D効果が得られます。"), pv14, sizeof(pv14) / sizeof(pv14[0]), 0)) return 0;
	if (!pefx->Add(0x16, TEXT("# Level"), TEXT("アウトプット・レベル"), TEXT("出力音量を調節します。"), 127)) return 0;
	break;
case 35:
	if (!pefx->Add(0x03, TEXT("Pre Filter"), TEXT("プリ・フィルター・タイプ"), TEXT("Lo-Fiを通る前のフィルターのタイプを設定します。"), 1, 0x00, 0x05, 1)) return 0;
	if (!pefx->Add(0x04, TEXT("Lo-Fi Type"), TEXT("ロー・ファイ・タイプ"), TEXT("音質を粗くします。値が大きいほど音質が粗くなります。"), 5, 0x00, 0x08, 1)) return 0;
	if (!pefx->Add(0x05, TEXT("Post Filter"), TEXT("ポスト・フィルター・タイプ"), TEXT("Lo-Fiを通った後のフィルターのタイプを設定します。"), 1, 0x00, 0x05, 1)) return 0;
	if (!pefx->Add(0x12, TEXT("+ Balance"), TEXT("バランス"), TEXT("原音とエフェクト音の音量バランスを調節します。表示の“D”、“E”は、D(ドライ音)、E(エフェクト音)の値がそれぞれ100であることを表します。"), g_efxpv_o2, sizeof(g_efxpv_o2) / sizeof(g_efxpv_o2[0]), 127)) return 0;
	if (!pefx->Add(0x13, TEXT("Low Gain"), TEXT("ロー・ゲイン"), TEXT("低域のゲインを調節します。"), 12, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x14, TEXT("Hi Gain"), TEXT("ハイ・ゲイン"), TEXT("高域のゲインを調節します。"), 12, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x15, TEXT("# Pan"), TEXT("アウトプット・パン"), TEXT("出力される音の定位を調節します。L63で最も左、0で中央、R63で最も右に定位します。"), g_efxpv_o1, sizeof(g_efxpv_o1) / sizeof(g_efxpv_o1[0]), 63)) return 0;
	if (!pefx->Add(0x16, TEXT("Level"), TEXT("アウトプット・レベル"), TEXT("出力音量を調節します。"), 127)) return 0;
	break;
case 36:
	if (!pefx->Add(0x03, TEXT("Lo-Fi Type"), TEXT("ロー・ファイ・タイプ"), TEXT("音質を粗くします。値が大きいほど音質が粗くなります。"), 1, 0x00, 0x05, 1)) return 0;
	if (!pefx->Add(0x04, TEXT("Fil Type"), TEXT("フィルター・タイプ"), TEXT("Lo-Fiを通った後のフィルターの種類を選びます。"), pv10, sizeof(pv10) / sizeof(pv10[0]), 1)) return 0;
	if (!pefx->Add(0x05, TEXT("Cutoff"), TEXT("カットオフ・フリケンシー"), TEXT("Lo-Fiを通った後のフィルターの基準周波数を調節します。"), g_efxpv9, sizeof(g_efxpv9) / sizeof(g_efxpv9[0]), 4)) return 0;
	if (!pefx->Add(0x06, TEXT("+ R.Detune"), TEXT("ラジオ・デチューン"), TEXT("ラジオのチューニングのノイズをシミュレートします。値が大きくなるほどチューニングがずれます。"), 0)) return 0;
	if (!pefx->Add(0x07, TEXT("R.Nz Lev"), TEXT("ラジオ・ノイズ・レベル"), TEXT("ラジオ・ノイズの音量を調節します。"), 64)) return 0;
	if (!pefx->Add(0x08, TEXT("W/P Sel"), TEXT("ホワイト/ピンク・ノイズ・セレクト"), TEXT("ホワイト・ノイズまたはピンク・ノイズを選択します。"), pv20, sizeof(pv20) / sizeof(pv20[0]), 1)) return 0;
	if (!pefx->Add(0x09, TEXT("W/P LPF"), TEXT("ホワイト/ピンク・ノイズLPF"), TEXT("ホワイト・ノイズまたはピンク・ノイズにかけるロー・パス・フィルターの基準周波数を調節します。"), g_efxpv11, sizeof(g_efxpv11) / sizeof(g_efxpv11[0]), 15)) return 0;
	if (!pefx->Add(0x0a, TEXT("W/P Level"), TEXT("ホワイト/ピンク・ノイズ・レベル"), TEXT("ホワイト・ノイズまたはピンク・ノイズの音量を調節します。"), 0)) return 0;
	if (!pefx->Add(0x0b, TEXT("Disc Type"), TEXT("ディスク・ノイズ・タイプ"), TEXT("レコード・ノイズの種類を選びます。タイプによってノイズの出る頻度が変わります。"), pv21, sizeof(pv21) / sizeof(pv21[0]), 0)) return 0;
	if (!pefx->Add(0x0c, TEXT("Disc LPF"), TEXT("ディスク・ノイズLPF"), TEXT("レコード・ノイズにかけるロー・パス・フィルターの基準周波数を調節します。"), g_efxpv11, sizeof(g_efxpv11) / sizeof(g_efxpv11[0]), 15)) return 0;
	if (!pefx->Add(0x0d, TEXT("Disc Nz Lev"), TEXT("ディスク・ノイズ・レベル"), TEXT("レコード・ノイズの音量を調節します。"), 0)) return 0;
	if (!pefx->Add(0x0e, TEXT("Hum Type"), TEXT("ハム・ノイズ・タイプ"), TEXT("ハム・ノイズの種類を選びます。"), 0, 0x00, 0x01, 50, TEXT("%dHz"), 10)) return 0;
	if (!pefx->Add(0x0f, TEXT("Hum LPF"), TEXT("ハム・ノイズLPF"), TEXT("ハム・ノイズにかけるロー・パス・フィルターの基準周波数を調節します。"), g_efxpv11, sizeof(g_efxpv11) / sizeof(g_efxpv11[0]), 15)) return 0;
	if (!pefx->Add(0x10, TEXT("Hum Level"), TEXT("ハム・ノイズ・レベル"), TEXT("ハム・ノイズの音量を調節します。"), 0)) return 0;
	if (!pefx->Add(0x11, TEXT("M/S"), TEXT("モノラル/ステレオ・スイッチ"), TEXT("エフェクト音をモノラルにするかステレオにするかを選びます。"), pv22, sizeof(pv22) / sizeof(pv22[0]), 1)) return 0;
	if (!pefx->Add(0x12, TEXT("# Balance"), TEXT("エフェクト・バランス"), TEXT("原音とエフェクト音の音量バランスを調節します。表示の“D”、“E”は、D(ドライ音)、E(エフェクト音)の値がそれぞれ100であることを表します。"), g_efxpv_o2, sizeof(g_efxpv_o2) / sizeof(g_efxpv_o2[0]), 127)) return 0;
	if (!pefx->Add(0x13, TEXT("Low Gain"), TEXT("ロー・ゲイン"), TEXT("低域のゲインを調節します。"), 12, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x14, TEXT("Hi Gain"), TEXT("ハイ・ゲイン"), TEXT("高域のゲインを調節します。"), 12, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x15, TEXT("Pan(Mono)"), TEXT("アウトプット・パン・モノラル"), TEXT("モノラル・モードのときに出力されるエフェクト音の定位を調節します。L63で最も左、0で中央、R63で最も右に定位します。"), g_efxpv_o1, sizeof(g_efxpv_o1) / sizeof(g_efxpv_o1[0]), 63)) return 0;
	if (!pefx->Add(0x16, TEXT("Level"), TEXT("アウトプット・レベル"), TEXT("出力音量を調節します。"), 127)) return 0;
	break;
}
				break;
			}
			break;
		}
		pefx->Command(HIWORD(wParam), LOWORD(wParam), (HWND)lParam);
		return 0;
	case WM_DESTROY:
		delete pefx;
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	WNDCLASS wc;
	MSG msg;
	INITCOMMONCONTROLSEX iccex;

	// 多重起動防止。
	CreateMutex(NULL, FALSE, TEXT("sc88pefx"));
	if (GetLastError() == ERROR_ALREADY_EXISTS) return 0;

	// ウィンドウクラス登録。
	wc.style = 0;
	wc.lpfnWndProc = WindowProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = TEXT("sc88pefx");
	if (RegisterClass(&wc) == 0) return 0;

	g_hinst = hInstance;

	// フォント作成。
	if ((g_hfont = CreateFont(12, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
	OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, NULL)) == NULL) return 0;

	// コモンコントロールの使用準備。
	iccex.dwSize = sizeof(INITCOMMONCONTROLSEX);
	iccex.dwICC = ICC_BAR_CLASSES;
	if (!InitCommonControlsEx(&iccex)) return 0;

	// トップウィンドウ作成。
	if (CreateWindow(TEXT("sc88pefx"), TEXT("SC-88ProEfxEdit"), WS_MINIMIZEBOX | WS_SYSMENU | WS_VISIBLE,
	100, 100, 580, 530, NULL, NULL, hInstance, NULL) == NULL) return 0;

	// メッセージループ。
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	DeleteObject(g_hfont);
	return msg.wParam;
}
