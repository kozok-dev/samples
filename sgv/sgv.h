#pragma once
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0400
#endif
#include <stdio.h>
#include <tchar.h>
#include <windows.h>

/**
 * 最低限の機能を持ったGridView。
 */
class SimpleGridView {
public:
	/**
	 * 項目タイプ。
	 */
	enum ITEMTYPE {
		ITEMTYPE_INTEGER = 1,	///< 整数。
		ITEMTYPE_DECIMAL,	///< 小数。
		ITEMTYPE_STRING	///< 文字列。
	};

	/**
	 * コンストラクタ。各メンバ変数を初期化します。
	 */
	SimpleGridView();

	/**
	 * デストラクタ。各動的確保していた格納領域を開放します。
	 * 項目タイプが文字列ならそれも開放します。
	 */
	virtual ~SimpleGridView();

	/**
	 * ウィンドウを作成して表示します。
	 * @param hwnd 親ウィンドウハンドル。
	 * @param hinst インスタンスハンドル。
	 * @param posx X座標。
	 * @param posy Y座標。
	 * @param width 幅。
	 * @param height 高さ。
	 * @return 成功:true、失敗:false。
	 */
	bool Create(HWND hwnd, HINSTANCE hinst, int posx, int posy, int width, int height);

	/**
	 * 項目を追加します。
	 * @param name 項目名。
	 * @param type 項目タイプ。
	 * @param format 項目形式。
	 * @param width 項目幅。
	 * @param setscrflag スクロールバーを設定する:true、しない:false。
	 * @return 成功:true、失敗:false。
	 */
	bool addItem(TCHAR *name, ITEMTYPE type, TCHAR *format, BYTE width, bool setscrflag = false);

	/**
	 * データ格納領域を追加します。追加した格納領域のポインタを返します。失敗するとNULLを返します。
	 * データ設定時、項目タイプが小数ならfloatValueメソッドを使用できます。
	 * データ設定時、項目タイプが文字列ならstringValueメソッドを使用してください。
	 * @param setscrflag スクロールバーを設定する:true、しない:false。
	 * @return 成功:追加した格納領域のポインタ、失敗:NULL。
	 */
	void **addData(bool setscrflag = false);

	/**
	 * ウィンドウのサイズを変更します。ウィンドウが作成されていることが前提です。
	 * @param width 幅。
	 * @param height 高さ。
	 */
	void setWndSize(int width, int height);

	/**
	 * floatをデータ構造を変更することなくvoid *に変換して返します。
	 * @param f 変換前の値。
	 * @return 変換後の値。
	 */
	static void *floatValue(float f);

	/**
	 * 指定した文字列をコピーしてそのポインタを返します。コピーに失敗するとNULLを返します。
	 * @param str 文字列ポインタ。
	 * @return 成功:コピーした文字列のポインタ、失敗:NULL。
	 */
	static void *stringValue(TCHAR *str);

private:
	static const char ITEM_MAX = 100;	///< 項目の最大の数。
	static const char ITEMNAME_MAX = 20;	///< 項目名の最大の長さ。
	static const char ITEMFORMAT_MAX = 10;	///< 項目形式の最大の長さ。
	static const short DATA_MAX = 10000;	///< データの最大の数。
	static const char STRING_MAX = 20;	///< 文字列の最大の長さ。

	char m_colcnt;	///< 列数。
	short m_rowcnt;	///< 行数。
	void ***m_pppdata;	///< データ。
	HWND m_hwnd;
	HFONT m_hfont;

	/**
	 * 項目。
	 */
	struct Item {
		TCHAR name[ITEMNAME_MAX];	///< 項目名。
		ITEMTYPE type;	///< 項目タイプ。
		TCHAR format[ITEMFORMAT_MAX];	///< 項目形式。
		BYTE width;	///< 項目幅。
	} *m_pitem;

	/**
	 * ウィンドウプロシージャ。
	 */
	static LRESULT CALLBACK procWnd(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	/**
	 * 描画。
	 * @param hdc デバイスコンテキスト。
	 */
	void Paint(HDC hdc);

	/**
	 * マウスホイール操作。
	 * @param dir 方向。
	 */
	void mouseWheel(short dir);

	/**
	 * 横スクロールバー操作。
	 * @param pos 位置。
	 * @param type 操作の種類。
	 */
	void hScroll(WORD pos, WORD type);

	/**
	 * 縦スクロールバー操作。
	 * @param pos 位置。
	 * @param type 操作の種類。
	 */
	void vScroll(WORD pos, WORD type);

	/**
	 * 横スクロールバー設定。
	 */
	void setHScrollInfo();

	/**
	 * 縦スクロールバー設定。
	 */
	void setVScrollInfo();
};
