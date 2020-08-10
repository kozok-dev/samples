#pragma once
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0400
#endif
#include <stdio.h>
#include <tchar.h>
#include <windows.h>

/**
 * �Œ���̋@�\��������GridView�B
 */
class SimpleGridView {
public:
	/**
	 * ���ڃ^�C�v�B
	 */
	enum ITEMTYPE {
		ITEMTYPE_INTEGER = 1,	///< �����B
		ITEMTYPE_DECIMAL,	///< �����B
		ITEMTYPE_STRING	///< ������B
	};

	/**
	 * �R���X�g���N�^�B�e�����o�ϐ������������܂��B
	 */
	SimpleGridView();

	/**
	 * �f�X�g���N�^�B�e���I�m�ۂ��Ă����i�[�̈���J�����܂��B
	 * ���ڃ^�C�v��������Ȃ炻����J�����܂��B
	 */
	virtual ~SimpleGridView();

	/**
	 * �E�B���h�E���쐬���ĕ\�����܂��B
	 * @param hwnd �e�E�B���h�E�n���h���B
	 * @param hinst �C���X�^���X�n���h���B
	 * @param posx X���W�B
	 * @param posy Y���W�B
	 * @param width ���B
	 * @param height �����B
	 * @return ����:true�A���s:false�B
	 */
	bool Create(HWND hwnd, HINSTANCE hinst, int posx, int posy, int width, int height);

	/**
	 * ���ڂ�ǉ����܂��B
	 * @param name ���ږ��B
	 * @param type ���ڃ^�C�v�B
	 * @param format ���ڌ`���B
	 * @param width ���ڕ��B
	 * @param setscrflag �X�N���[���o�[��ݒ肷��:true�A���Ȃ�:false�B
	 * @return ����:true�A���s:false�B
	 */
	bool addItem(TCHAR *name, ITEMTYPE type, TCHAR *format, BYTE width, bool setscrflag = false);

	/**
	 * �f�[�^�i�[�̈��ǉ����܂��B�ǉ������i�[�̈�̃|�C���^��Ԃ��܂��B���s�����NULL��Ԃ��܂��B
	 * �f�[�^�ݒ莞�A���ڃ^�C�v�������Ȃ�floatValue���\�b�h���g�p�ł��܂��B
	 * �f�[�^�ݒ莞�A���ڃ^�C�v��������Ȃ�stringValue���\�b�h���g�p���Ă��������B
	 * @param setscrflag �X�N���[���o�[��ݒ肷��:true�A���Ȃ�:false�B
	 * @return ����:�ǉ������i�[�̈�̃|�C���^�A���s:NULL�B
	 */
	void **addData(bool setscrflag = false);

	/**
	 * �E�B���h�E�̃T�C�Y��ύX���܂��B�E�B���h�E���쐬����Ă��邱�Ƃ��O��ł��B
	 * @param width ���B
	 * @param height �����B
	 */
	void setWndSize(int width, int height);

	/**
	 * float���f�[�^�\����ύX���邱�ƂȂ�void *�ɕϊ����ĕԂ��܂��B
	 * @param f �ϊ��O�̒l�B
	 * @return �ϊ���̒l�B
	 */
	static void *floatValue(float f);

	/**
	 * �w�肵����������R�s�[���Ă��̃|�C���^��Ԃ��܂��B�R�s�[�Ɏ��s�����NULL��Ԃ��܂��B
	 * @param str ������|�C���^�B
	 * @return ����:�R�s�[����������̃|�C���^�A���s:NULL�B
	 */
	static void *stringValue(TCHAR *str);

private:
	static const char ITEM_MAX = 100;	///< ���ڂ̍ő�̐��B
	static const char ITEMNAME_MAX = 20;	///< ���ږ��̍ő�̒����B
	static const char ITEMFORMAT_MAX = 10;	///< ���ڌ`���̍ő�̒����B
	static const short DATA_MAX = 10000;	///< �f�[�^�̍ő�̐��B
	static const char STRING_MAX = 20;	///< ������̍ő�̒����B

	char m_colcnt;	///< �񐔁B
	short m_rowcnt;	///< �s���B
	void ***m_pppdata;	///< �f�[�^�B
	HWND m_hwnd;
	HFONT m_hfont;

	/**
	 * ���ځB
	 */
	struct Item {
		TCHAR name[ITEMNAME_MAX];	///< ���ږ��B
		ITEMTYPE type;	///< ���ڃ^�C�v�B
		TCHAR format[ITEMFORMAT_MAX];	///< ���ڌ`���B
		BYTE width;	///< ���ڕ��B
	} *m_pitem;

	/**
	 * �E�B���h�E�v���V�[�W���B
	 */
	static LRESULT CALLBACK procWnd(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	/**
	 * �`��B
	 * @param hdc �f�o�C�X�R���e�L�X�g�B
	 */
	void Paint(HDC hdc);

	/**
	 * �}�E�X�z�C�[������B
	 * @param dir �����B
	 */
	void mouseWheel(short dir);

	/**
	 * ���X�N���[���o�[����B
	 * @param pos �ʒu�B
	 * @param type ����̎�ށB
	 */
	void hScroll(WORD pos, WORD type);

	/**
	 * �c�X�N���[���o�[����B
	 * @param pos �ʒu�B
	 * @param type ����̎�ށB
	 */
	void vScroll(WORD pos, WORD type);

	/**
	 * ���X�N���[���o�[�ݒ�B
	 */
	void setHScrollInfo();

	/**
	 * �c�X�N���[���o�[�ݒ�B
	 */
	void setVScrollInfo();
};
