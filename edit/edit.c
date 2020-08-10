/*
�u���v�}�[�N�R�����g�ɂ��̃T���v���̉ۑ肪������Ă���B
�܂��A�^�u�\���A2�o�C�g�����\���ɂ����ƑΉ����铙�̉ۑ���c���Ă���B
*/

#define _WIN32_WINNT 0x0400
#include <windows.h>
#include <windowsx.h>
#include <imm.h>

/* �L�����b�g�̈ʒu������x�Ay�Ɋ�Â��Ď擾����B
�߂�l��x��y�s�̕�������葽�����Ause_x���U�̏ꍇ��y�s�̕��������Ԃ�A����ȊO��x�����̂܂ܕԂ�B
�������Ay��str�̉��s��(�܂�A���ۂ̍s��)��葽���ꍇ��-2147483648��Ԃ��B
y�s�̕��������X�N���[���ʒu��菬�����ꍇ��0��Ԃ��B*/
int get_edit_pos(LPTSTR str, int x, int y, HWND hwnd, BOOL use_x)
{
	int size = GlobalSize(str), i, j;

	if (size == 0) return -2147483648;

	y += GetScrollPos(hwnd, SB_VERT);
	for (i = 0; i < size && y > 0; i++)
		if (str[i] == '\n') y--;
	if (y > 0) return -2147483648;	/* �w�肵���s�����ۂ̍s�����Ȃ��ꍇ�B*/
	if (use_x) {
		x += GetScrollPos(hwnd, SB_HORZ);
		for (j = i; j < size && x > j - i; j++)
			if (str[j] == '\n') break;
	} else
		for (j = i; j < size && str[j] != '\n'; j++);

	return j - i - GetScrollPos(hwnd, SB_HORZ);
}

/* ���������̕�����̈ʒu������x�Ay�Ɋ�Â��Ď擾����B
����x�Ay�����������̕�����̈ʒu�ɑΉ������A���̃|�C���^��Ԃ��B
y��str�̉��s��(�܂�A���ۂ̍s��)��葽���ꍇ��NULL��Ԃ��B
x��y�s�̕�������葽���ꍇ��y�s�̕������Ƃ��ď�������B*/
LPTSTR get_edit_str(LPTSTR str, int x, int y, HWND hwnd)
{
	int size = GlobalSize(str), i, j;

	y += GetScrollPos(hwnd, SB_VERT);
	for (i = 0; i < size && y > 0; i++)
		if (str[i] == '\n') y--;
	if (y > 0) return NULL;	/* �w�肵���s�����ۂ̍s�����Ȃ��ꍇ�B*/
	x += GetScrollPos(hwnd, SB_HORZ);
	for (j = i; j < size && x > j - i; j++)
		if (str[j] == '\n') break;

	return str + j;
}

/* �I�𕶎����`��/�����`�悷��B*/
void draw_selected_text(HWND hwnd, HDC hdc, LPTSTR str, HFONT hfont, LPTEXTMETRIC ptm,
int x, int y, int from_x, int from_y, BOOL clear)
{
	int i, j, k, l, m;

	HideCaret(hwnd);
	SelectObject(hdc, hfont);
	if (clear) {
		SetTextColor(hdc, GetSysColor(COLOR_WINDOWTEXT));
		SetBkColor(hdc, GetSysColor(COLOR_WINDOW));
	} else {
		SetTextColor(hdc, GetSysColor(COLOR_HIGHLIGHTTEXT));
		SetBkColor(hdc, GetSysColor(COLOR_HIGHLIGHT));
	}
	if (y == from_y) {	/* �I�𕶎���1�s���H */
		j = y / ptm->tmHeight;
		k = from_y / ptm->tmHeight;
		/* �ʒux���傫������l�A�������ق���m�Ƃ���B*/
		if (x < from_x) {
			l = x / ptm->tmAveCharWidth;
			m = from_x / ptm->tmAveCharWidth;
		} else {
			l = from_x / ptm->tmAveCharWidth;
			m = x / ptm->tmAveCharWidth;
		}
		/* ���������̎w��ʒu����傫�����̎w��ʒu�܂ŕ`�悷��B*/
		TextOut(hdc, l * ptm->tmAveCharWidth, j * ptm->tmHeight, get_edit_str(str, l, j, hwnd), m - l);
	} else {	/* �I�𕶎��񂪕����s�B*/
		/* �ʒuy���傫������l�A�������ق���m�Ƃ���B�ʒux�̑召�֌W�͖����ł���B*/
		if (y < from_y) {
			j = y / ptm->tmHeight;
			k = from_y / ptm->tmHeight;
			l = x / ptm->tmAveCharWidth;
			m = from_x / ptm->tmAveCharWidth;
		} else {
			j = from_y / ptm->tmHeight;
			k = y / ptm->tmHeight;
			l = from_x / ptm->tmAveCharWidth;
			m = x / ptm->tmAveCharWidth;
		}
		for (i = j; i <= k; i++) {
			/* �w��ʒu����s�̖����܂ŕ`�悷��B*/
			if (i == j) TextOut(hdc, l * ptm->tmAveCharWidth, j * ptm->tmHeight, get_edit_str(str, l, j, hwnd), get_edit_pos(str, -1, j, hwnd, FALSE) - l);
			/* �s�̎n�߂���w��ʒu�܂ŕ`�悷��B*/
			else if (i == k) TextOut(hdc, 0, k * ptm->tmHeight, get_edit_str(str, 0, k, hwnd), m);
			/* �s���ƕ`�悷��B*/
			else TextOut(hdc, 0, i * ptm->tmHeight, get_edit_str(str, 0, i, hwnd), get_edit_pos(str, -1, i, hwnd, FALSE));
		}
	}
	ShowCaret(hwnd);
}

typedef struct {
	int min_x;	/* �I���ʒu�̏���������x�B*/
	int min_y;	/* �I���ʒu�̏���������y�B*/
	int max_x;	/* �I���ʒu�̑傫������x�B*/
	int max_y;	/* �I���ʒu�̑傫������y�B*/
	int sel_cnt;	/* �I�𕶎����B*/
	int sel_cnt2;	/* �w��ʒu���������ꍇ�ɑI�𕶎�����ݒ肵�܂��B����ȊO��0�ł��B*/
} EDITSELPOS;

/* �I�𕶎�������擾(pesp�ɐݒ�)����B
�I������Ă��Ȃ��ꍇ�͏��������Ƒ傫�����̈ʒu�͓����ɂȂ�A�I�𕶎�����0�ɂȂ�܂��B*/
void get_edit_selection_pos(LPTSTR str, HWND hwnd, LPTEXTMETRIC ptm, BOOL *pselected,
int x, int y, int from_x, int from_y, EDITSELPOS *pesp)
{
	if (*pselected) {	/* �I������Ă��邩�H */
		if (y == from_y) {	/* �I�𕶎���1�s���H */
			/* min_y��max_y�����l�ɂȂ�B*/
			pesp->min_y = y / ptm->tmHeight;
			pesp->max_y = from_y / ptm->tmHeight;

			if (x < from_x) {
				pesp->min_x = x / ptm->tmAveCharWidth;
				pesp->max_x = from_x / ptm->tmAveCharWidth;
				pesp->sel_cnt2 = 1;
			} else {
				pesp->min_x = from_x / ptm->tmAveCharWidth;
				pesp->max_x = x / ptm->tmAveCharWidth;
				pesp->sel_cnt2 = 0;
			}
		} else {
			if (y < from_y) {
				pesp->min_y = y / ptm->tmHeight;
				pesp->max_y = from_y / ptm->tmHeight;
				pesp->min_x = x / ptm->tmAveCharWidth;
				pesp->max_x = from_x / ptm->tmAveCharWidth;
				pesp->sel_cnt2 = 1;
			} else {
				pesp->min_y = from_y / ptm->tmHeight;
				pesp->max_y = y / ptm->tmHeight;
				pesp->min_x = from_x / ptm->tmAveCharWidth;
				pesp->max_x = x / ptm->tmAveCharWidth;
				pesp->sel_cnt2 = 0;
			}
		}
		pesp->sel_cnt = get_edit_str(str, pesp->max_x, pesp->max_y, hwnd) - get_edit_str(str, pesp->min_x, pesp->min_y, hwnd);
		if (pesp->sel_cnt2) pesp->sel_cnt2 = pesp->sel_cnt;

		*pselected = FALSE;
	} else {
		pesp->sel_cnt = pesp->sel_cnt2 = 0;
		pesp->min_y = pesp->max_y = y / ptm->tmHeight;
		pesp->min_x = pesp->max_x = x / ptm->tmAveCharWidth;
	}
}

/* �X�N���[���o�[�̐ݒ�Bv�͑����ʁB-2147483648���w�肷��ƍĕ`����s���B*/
void set_max_line(HWND hwnd, LPTEXTMETRIC ptm, int h, int v)
{
	int i;
	RECT rect;
	SCROLLINFO si;
	static int hline, vline;

	/* �� */
	if (h != -2147483648) h += GetScrollPos(hwnd, SB_HORZ);
	if (hline < h || h == -2147483648) {
		if (h != -2147483648) hline = h;
		GetClientRect(hwnd, &rect);
		if ((i = hline - (rect.right - rect.left) / ptm->tmAveCharWidth + 1) > 0 || h == -2147483648) {
			si.cbSize = sizeof(SCROLLINFO);
			si.fMask = SIF_PAGE | SIF_RANGE;
			si.nPage = (rect.right - rect.left) / ptm->tmAveCharWidth;
			si.nMin = 0;
			si.nMax = i + si.nPage - 1;
			SetScrollInfo(hwnd, SB_HORZ, &si, TRUE);
		}
	}
	/* �c */
	if (v != 0 || v == -2147483648) {
		if (v != -2147483648) vline += v;
		GetClientRect(hwnd, &rect);
		si.cbSize = sizeof(SCROLLINFO);
		si.fMask = SIF_PAGE | SIF_RANGE;
		si.nPage = (rect.bottom - rect.top) / ptm->tmHeight;
		si.nMin = 0;
		si.nMax = vline + si.nPage - 1;
		SetScrollInfo(hwnd, SB_VERT, &si, TRUE);
	}
}

/* �ʒu����ʊO�ɂ���Ƃ��A�X�N���[���o�[���ړ�������B*/
void set_pos(HWND hwnd, LPTEXTMETRIC ptm, int *x, int *y, int *from_x, int *from_y)
{
	int i, j, k, l;
	RECT rect;

	i = GetScrollPos(hwnd, SB_HORZ);
	if (*x < 0)	/* �ʒu����ʂ�荶�ɂ��邩�H */
		SetScrollPos(hwnd, SB_HORZ, i + *x / ptm->tmAveCharWidth, TRUE);
	else {
		GetClientRect(hwnd, &rect);
		if (*x + ptm->tmAveCharWidth > rect.right - rect.left) /* �ʒu����ʂ��E�ɂ��邩�H */
			SetScrollPos(hwnd, SB_HORZ, i + *x / ptm->tmAveCharWidth - (rect.right - rect.left) / ptm->tmAveCharWidth + 1, TRUE);
	}
	j = GetScrollPos(hwnd, SB_HORZ);
	if (i != j) {
		/* ���֐i�ނƐ��A���֐i�ނƕ��ɂ���B*/
		*x += (i - j) * ptm->tmAveCharWidth;
		*from_x += (i - j) * ptm->tmAveCharWidth;
	}

	k = GetScrollPos(hwnd, SB_VERT);
	if (*y < 0)	/* �ʒu����ʂ���ɂ��邩�H */
		SetScrollPos(hwnd, SB_VERT, k + *y / ptm->tmHeight, TRUE);
	else {
		GetClientRect(hwnd, &rect);
		if (*y + ptm->tmHeight > rect.bottom - rect.top) /* �ʒu����ʂ�艺�ɂ��邩�H */
			SetScrollPos(hwnd, SB_VERT, k + *y / ptm->tmHeight - (rect.bottom - rect.top) / ptm->tmHeight + 1, TRUE);
	}
	l = GetScrollPos(hwnd, SB_VERT);
	if (k != l) {
		/* ��֐i�ނƐ��A���֐i�ނƕ��ɂ���B*/
		*y += (k - l) * ptm->tmHeight;
		*from_y += (k - l) * ptm->tmHeight;
	}

	if (i != j || k != l) InvalidateRect(hwnd, NULL, TRUE);
}

/* �L�����b�g�̈ʒu���ʒux�Ay�ɖO�a�ݒ肷��B*/
void set_edit_pos(LPTSTR str, int *x, int *y, int *from_x, int *from_y, HWND hwnd, LPTEXTMETRIC ptm)
{
	int size = GlobalSize(str), i, j, k;

	k = *y;
	k += GetScrollPos(hwnd, SB_VERT);
	for (i = j = 0; i < size && k > 0; i++)
		if (str[i] == '\n') {
			j = i + 1;
			k--;
		}

	*x += GetScrollPos(hwnd, SB_HORZ);
	for (i = j; j < size && *x > j - i; j++)
		if (str[j] == '\n') break;

	*x = (j - i - GetScrollPos(hwnd, SB_HORZ)) * ptm->tmAveCharWidth;
	*y = (*y - k) * ptm->tmHeight;
	set_pos(hwnd, ptm, x, y, from_x, from_y);
	SetCaretPos(*x, *y);
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int i, j, k;
	float l;
	LPTSTR pstr;
	POINT point;
	RECT rect;
	PAINTSTRUCT ps;
	HDC hdc;
	HGLOBAL halc;
	HMENU hmenu;
	HIMC himc;
	COMPOSITIONFORM composform;
	EDITSELPOS esp;
	static int caret_pos_x, caret_pos_y, caret_from_pos_x, caret_from_pos_y;
	static BOOL selected = FALSE;
	static LPTSTR str = NULL;
	static LOGFONT logfont = {13, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
	CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, "�l�r �S�V�b�N"};
	static TEXTMETRIC tm;
	static HFONT hfont;

	switch (uMsg) {
	case WM_CREATE:
		hdc = GetDC(hwnd);
		hfont = CreateFontIndirect(&logfont);
		SelectObject(hdc, hfont);
		GetTextMetrics(hdc, &tm);
		ReleaseDC(hwnd, hdc);
		caret_pos_x = caret_pos_y = 0;
		return 0;
	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
		SetTextColor(hdc, GetSysColor(COLOR_WINDOWTEXT));
		SetBkColor(hdc, GetSysColor(COLOR_WINDOW));
		SelectObject(hdc, hfont);
		/* �s���ɕ`��B*/
		for (i = 0; (j = get_edit_pos(str, -1, i, hwnd, FALSE)) != -2147483648; i++) TextOut(hdc, 0, i * tm.tmHeight, get_edit_str(str, 0, i, hwnd), j);
		if (selected) draw_selected_text(hwnd, hdc, str, hfont, &tm, caret_pos_x, caret_pos_y, caret_from_pos_x, caret_from_pos_y, FALSE);
		EndPaint(hwnd, &ps);
		return 0;
	case WM_SIZE:
		i = GetScrollPos(hwnd, SB_HORZ);

		set_max_line(hwnd, &tm, -2147483648, -2147483648);

		if (caret_pos_x < 0)	/* �ʒu����ʂ�荶�ɂ��邩�H */
			SetScrollPos(hwnd, SB_HORZ, i + caret_pos_x / tm.tmAveCharWidth, TRUE);
		else {
			GetClientRect(hwnd, &rect);
			if (caret_pos_x + tm.tmAveCharWidth > rect.right - rect.left) /* �ʒu����ʂ��E�ɂ��邩�H */
				SetScrollPos(hwnd, SB_HORZ, i + caret_pos_x / tm.tmAveCharWidth - (rect.right - rect.left) / tm.tmAveCharWidth + 1, TRUE);
		}
		j = GetScrollPos(hwnd, SB_HORZ);
		if (i != j) {
			/* ���֐i�ނƐ��A���֐i�ނƕ��ɂ���B*/
			caret_pos_x += (i - j) * tm.tmAveCharWidth;
			caret_from_pos_x += (i - j) * tm.tmAveCharWidth;
			SetCaretPos(caret_pos_x, caret_pos_y);
			InvalidateRect(hwnd, NULL, TRUE);
		}
		return 0;
	case WM_SETFOCUS:
		CreateCaret(hwnd, NULL, 1, tm.tmHeight);
		ShowCaret(hwnd);
		return 0;
	case WM_KILLFOCUS:
		DestroyCaret();
		return 0;
	case WM_LBUTTONDOWN:
		if (selected) {
			hdc = GetDC(hwnd);
			draw_selected_text(hwnd, hdc, str, hfont, &tm, caret_pos_x, caret_pos_y, caret_from_pos_x, caret_from_pos_y, TRUE);
			ReleaseDC(hwnd, hdc);
			selected = FALSE;
		}

		/* �}�E�X�̈ʒu���e�L�X�g�̈ʒu�ɍ����悤�ɂ���B*/
		l = (float)GET_X_LPARAM(lParam) / tm.tmAveCharWidth;
		caret_pos_x = l > 0 ? l + .5 : l - 1;
		l = (float)GET_Y_LPARAM(lParam) / tm.tmHeight;
		caret_pos_y = l > 0 ? l : l - 1;
		set_edit_pos(str, &caret_pos_x, &caret_pos_y, &caret_from_pos_x, &caret_from_pos_y, hwnd, &tm);
		/* �I���J�n�ʒu���ݒ肵�Ă����B*/
		caret_from_pos_x = caret_pos_x;
		caret_from_pos_y = caret_pos_y;

		SetCapture(hwnd);
		return 0;
	case WM_LBUTTONUP:
		ReleaseCapture();
		return 0;
	case WM_MOUSEMOVE:
		if (!(wParam & MK_LBUTTON)) break;

		hdc = GetDC(hwnd);
		/* ��x�A�����`����s���B���ǖ@���������ׂ��ł���B*/
		draw_selected_text(hwnd, hdc, str, hfont, &tm, caret_pos_x, caret_pos_y, caret_from_pos_x, caret_from_pos_y, TRUE);

		/* �}�E�X�̈ʒu���e�L�X�g�̈ʒu�ɍ����悤�ɂ���B*/
		l = (float)GET_X_LPARAM(lParam) / tm.tmAveCharWidth;
		caret_pos_x = l > 0 ? l + .5 : l - 1;
		l = (float)GET_Y_LPARAM(lParam) / tm.tmHeight;
		caret_pos_y = l > 0 ? l : l - 1;
		set_edit_pos(str, &caret_pos_x, &caret_pos_y, &caret_from_pos_x, &caret_from_pos_y, hwnd, &tm);

		selected = caret_pos_x != caret_from_pos_x || caret_pos_y != caret_from_pos_y;
		/* �`��B*/
		draw_selected_text(hwnd, hdc, str, hfont, &tm, caret_pos_x, caret_pos_y, caret_from_pos_x, caret_from_pos_y, FALSE);

		ReleaseDC(hwnd, hdc);
		return 0;
	case WM_RBUTTONDOWN:
		hmenu = CreatePopupMenu();
		AppendMenu(hmenu, MF_STRING | (selected > 0 ? 0 : MF_GRAYED), 100, "�؂���(&T)");
		AppendMenu(hmenu, MF_STRING | (selected > 0 ? 0 : MF_GRAYED), 101, "�R�s�[(&C)");

		j = IsClipboardFormatAvailable(CF_TEXT) ? 0 : MF_GRAYED;
		AppendMenu(hmenu, MF_STRING, 102, "�\��t��(&P)");

		AppendMenu(hmenu, MF_STRING | (selected > 0 ? 0 : MF_GRAYED), 103, "�폜(&D)");
		AppendMenu(hmenu, MF_SEPARATOR, 0, NULL);

		for (i = 0; get_edit_pos(str, 0, i, hwnd, TRUE) != -2147483648; i++);
		i--;
		/* �����񂪂���A���ׂđI������Ă��Ȃ���ΗL���ɂ���B*/
		if (i >= 0 && (!selected || caret_from_pos_x != -GetScrollPos(hwnd, SB_HORZ) * tm.tmAveCharWidth ||
			caret_from_pos_y != -GetScrollPos(hwnd, SB_VERT) * tm.tmHeight ||
			caret_pos_x != get_edit_pos(str, -1, i, hwnd, FALSE) * tm.tmAveCharWidth ||
			caret_pos_y != i * tm.tmHeight)) j = 0;
		else
			j = MF_GRAYED;
		AppendMenu(hmenu, MF_STRING | j, 104, "���ׂđI��(&A)");

		GetCursorPos(&point);
		TrackPopupMenu(hmenu, TPM_LEFTALIGN | TPM_TOPALIGN, point.x, point.y, 0, hwnd, NULL);
		DestroyMenu(hmenu);
		return 0;
	case WM_MOUSEWHEEL:
		i = GetScrollPos(hwnd, SB_VERT);
		SetScrollPos(hwnd, SB_VERT, GetScrollPos(hwnd, SB_VERT) + (GET_WHEEL_DELTA_WPARAM(wParam) < 0 ? 3 : -3), TRUE);
		j = GetScrollPos(hwnd, SB_VERT);
		if (i != j) {
			/* ��֐i�ނƐ��A���֐i�ނƕ��ɂ���B*/
			caret_pos_y += (i - j) * tm.tmHeight;
			caret_from_pos_y += (i - j) * tm.tmHeight;
			SetCaretPos(caret_pos_x, caret_pos_y);
			InvalidateRect(hwnd, NULL, TRUE);
		}
		return 0;
	case WM_KEYDOWN:
		if (GetCapture() != NULL) break;

		switch (wParam) {
		case VK_UP:
			if (GetKeyState(VK_SHIFT) < 0) {
				if (caret_pos_y <= -GetScrollPos(hwnd, SB_VERT) * tm.tmHeight) break;

				/* �I���J�n�ʒu��ۑ�����B*/
				if (!selected) {
					caret_from_pos_x = caret_pos_x;
					caret_from_pos_y = caret_pos_y;
					selected = TRUE;
				}

				HideCaret(hwnd);
				hdc = GetDC(hwnd);
				SelectObject(hdc, hfont);

				/* �I��/�ʏ핶����\���̐ݒ�B*/
				if (caret_pos_y <= caret_from_pos_y) {
					SetTextColor(hdc, GetSysColor(COLOR_HIGHLIGHTTEXT));
					SetBkColor(hdc, GetSysColor(COLOR_HIGHLIGHT));
				} else {
					SetTextColor(hdc, GetSysColor(COLOR_WINDOWTEXT));
					SetBkColor(hdc, GetSysColor(COLOR_WINDOW));
				}

				/* �I��/�ʏ핶�����`�悷��B*/
				i = get_edit_pos(str, caret_pos_x / tm.tmAveCharWidth, caret_pos_y / tm.tmHeight, hwnd, TRUE);
				TextOut(hdc, 0, caret_pos_y, get_edit_str(str, 0, caret_pos_y / tm.tmHeight, hwnd), i);
				i = get_edit_pos(str, -1, caret_pos_y / tm.tmHeight - 1, hwnd, FALSE) - caret_pos_x / tm.tmAveCharWidth;
				TextOut(hdc, caret_pos_x, caret_pos_y - tm.tmHeight, get_edit_str(str, caret_pos_x / tm.tmAveCharWidth, caret_pos_y / tm.tmHeight - 1, hwnd), i);

				/* �ʒu�ɂ���Ă͕ʂ̈ʒu�̑I��/�ʏ핶������`�悷��B*/
				if (caret_pos_y - tm.tmHeight == caret_from_pos_y && caret_pos_x < caret_from_pos_x) {
					SetTextColor(hdc, GetSysColor(COLOR_HIGHLIGHTTEXT));
					SetBkColor(hdc, GetSysColor(COLOR_HIGHLIGHT));
					i = caret_from_pos_x / tm.tmAveCharWidth - caret_pos_x / tm.tmAveCharWidth;
					TextOut(hdc, caret_pos_x, caret_pos_y - tm.tmHeight, get_edit_str(str, caret_pos_x / tm.tmAveCharWidth, caret_pos_y / tm.tmHeight - 1, hwnd), i);
				} else if (caret_pos_y == caret_from_pos_y && caret_pos_x > caret_from_pos_x) {
					SetTextColor(hdc, GetSysColor(COLOR_WINDOWTEXT));
					SetBkColor(hdc, GetSysColor(COLOR_WINDOW));
					i = caret_pos_x / tm.tmAveCharWidth - caret_from_pos_x / tm.tmAveCharWidth;
					TextOut(hdc, caret_from_pos_x, caret_from_pos_y, get_edit_str(str, caret_from_pos_x / tm.tmAveCharWidth, caret_from_pos_y / tm.tmHeight, hwnd), i);
				}

				ReleaseDC(hwnd, hdc);
				ShowCaret(hwnd);
			} else if (selected) {
				hdc = GetDC(hwnd);
				draw_selected_text(hwnd, hdc, str, hfont, &tm, caret_pos_x, caret_pos_y, caret_from_pos_x, caret_from_pos_y, TRUE);
				ReleaseDC(hwnd, hdc);
				selected = FALSE;
			}

			if (caret_pos_y > -GetScrollPos(hwnd, SB_VERT) * tm.tmHeight) {	/* ��Ɉړ��ł��邩�H */
				caret_pos_y -= tm.tmHeight;
				caret_pos_x = get_edit_pos(str, caret_pos_x / tm.tmAveCharWidth, caret_pos_y / tm.tmHeight, hwnd, TRUE) * tm.tmAveCharWidth;
			}
			set_pos(hwnd, &tm, &caret_pos_x, &caret_pos_y, &caret_from_pos_x, &caret_from_pos_y);
			break;
		case VK_DOWN:
			if (GetKeyState(VK_SHIFT) < 0) {
				if (get_edit_pos(str, 0, caret_pos_y / tm.tmHeight + 1, hwnd, TRUE) == -2147483648) break;

				/* �I���J�n�ʒu��ۑ�����B*/
				if (!selected) {
					caret_from_pos_x = caret_pos_x;
					caret_from_pos_y = caret_pos_y;
					selected = TRUE;
				}

				HideCaret(hwnd);
				hdc = GetDC(hwnd);
				SelectObject(hdc, hfont);

				/* �I��/�ʏ핶����\���̐ݒ�B*/
				if (caret_pos_y >= caret_from_pos_y) {
					SetTextColor(hdc, GetSysColor(COLOR_HIGHLIGHTTEXT));
					SetBkColor(hdc, GetSysColor(COLOR_HIGHLIGHT));
				} else {
					SetTextColor(hdc, GetSysColor(COLOR_WINDOWTEXT));
					SetBkColor(hdc, GetSysColor(COLOR_WINDOW));
				}

				/* �I��/�ʏ핶�����`�悷��B*/
				i = get_edit_pos(str, -1, caret_pos_y / tm.tmHeight, hwnd, FALSE) - caret_pos_x / tm.tmAveCharWidth;
				TextOut(hdc, caret_pos_x, caret_pos_y, get_edit_str(str, caret_pos_x / tm.tmAveCharWidth, caret_pos_y / tm.tmHeight, hwnd), i);
				i = get_edit_pos(str, caret_pos_x / tm.tmAveCharWidth, caret_pos_y / tm.tmHeight + 1, hwnd, TRUE);
				TextOut(hdc, 0, caret_pos_y + tm.tmHeight, get_edit_str(str, 0, caret_pos_y / tm.tmHeight + 1, hwnd), i);

				/* �ʒu�ɂ���Ă͕ʂ̈ʒu�̑I��/�ʏ핶������`�悷��B*/
				if (caret_pos_y == caret_from_pos_y && caret_pos_x < caret_from_pos_x) {
					SetTextColor(hdc, GetSysColor(COLOR_WINDOWTEXT));
					SetBkColor(hdc, GetSysColor(COLOR_WINDOW));
					i = caret_from_pos_x / tm.tmAveCharWidth - caret_pos_x / tm.tmAveCharWidth;
					TextOut(hdc, caret_pos_x, caret_pos_y, get_edit_str(str, caret_pos_x / tm.tmAveCharWidth, caret_pos_y / tm.tmHeight, hwnd), i);
				} else if (caret_pos_y + tm.tmHeight == caret_from_pos_y && caret_pos_x > caret_from_pos_x) {
					SetTextColor(hdc, GetSysColor(COLOR_HIGHLIGHTTEXT));
					SetBkColor(hdc, GetSysColor(COLOR_HIGHLIGHT));
					i = caret_pos_x / tm.tmAveCharWidth - caret_from_pos_x / tm.tmAveCharWidth;
					TextOut(hdc, caret_from_pos_x, caret_from_pos_y, get_edit_str(str, caret_from_pos_x / tm.tmAveCharWidth, caret_from_pos_y / tm.tmHeight, hwnd), i);
				}

				ReleaseDC(hwnd, hdc);
				ShowCaret(hwnd);
			} else if (selected) {
				hdc = GetDC(hwnd);
				draw_selected_text(hwnd, hdc, str, hfont, &tm, caret_pos_x, caret_pos_y, caret_from_pos_x, caret_from_pos_y, TRUE);
				ReleaseDC(hwnd, hdc);
				selected = FALSE;
			}

			if ((i = get_edit_pos(str, caret_pos_x / tm.tmAveCharWidth, caret_pos_y / tm.tmHeight + 1, hwnd, TRUE)) != -2147483648) {	/* ���Ɉړ��ł��邩�H */
				caret_pos_y += tm.tmHeight;
				caret_pos_x = i * tm.tmAveCharWidth;
			}
			set_pos(hwnd, &tm, &caret_pos_x, &caret_pos_y, &caret_from_pos_x, &caret_from_pos_y);
			break;
		case VK_LEFT:
			if (caret_pos_x > -GetScrollPos(hwnd, SB_HORZ) * tm.tmAveCharWidth) {
				if (GetKeyState(VK_SHIFT) < 0) {
					/* �I���J�n�ʒu��ۑ�����B*/
					if (!selected) {
						caret_from_pos_x = caret_pos_x;
						caret_from_pos_y = caret_pos_y;
						selected = TRUE;
					}

					HideCaret(hwnd);
					hdc = GetDC(hwnd);
					SelectObject(hdc, hfont);

					/* �I��/�ʏ핶����\���̐ݒ�B*/
					if (caret_pos_y == caret_from_pos_y && caret_pos_x <= caret_from_pos_x ||
					caret_pos_y < caret_from_pos_y) {
						SetTextColor(hdc, GetSysColor(COLOR_HIGHLIGHTTEXT));
						SetBkColor(hdc, GetSysColor(COLOR_HIGHLIGHT));
					} else {
						SetTextColor(hdc, GetSysColor(COLOR_WINDOWTEXT));
						SetBkColor(hdc, GetSysColor(COLOR_WINDOW));
					}

					/* �I��/�ʏ핶�����`�悷��B*/
					TextOut(hdc, caret_pos_x - tm.tmAveCharWidth, caret_pos_y, get_edit_str(str, caret_pos_x / tm.tmAveCharWidth - 1, caret_pos_y / tm.tmHeight, hwnd), 1);

					ReleaseDC(hwnd, hdc);
					ShowCaret(hwnd);
				} else if (selected) {
					hdc = GetDC(hwnd);
					draw_selected_text(hwnd, hdc, str, hfont, &tm, caret_pos_x, caret_pos_y, caret_from_pos_x, caret_from_pos_y, TRUE);
					ReleaseDC(hwnd, hdc);
					selected = FALSE;
				}

				caret_pos_x -= tm.tmAveCharWidth;
			} else if (caret_pos_y > -GetScrollPos(hwnd, SB_VERT) * tm.tmHeight) {
				if (GetKeyState(VK_SHIFT) < 0) {
					/* �I���J�n�ʒu��ۑ�����B*/
					if (!selected) {
						caret_from_pos_x = caret_pos_x;
						caret_from_pos_y = caret_pos_y;
						selected = TRUE;
					}
				} else if (selected) {
					hdc = GetDC(hwnd);
					draw_selected_text(hwnd, hdc, str, hfont, &tm, caret_pos_x, caret_pos_y, caret_from_pos_x, caret_from_pos_y, TRUE);
					ReleaseDC(hwnd, hdc);
					selected = FALSE;
				}

				caret_pos_y -= tm.tmHeight;
				caret_pos_x = get_edit_pos(str, -1, caret_pos_y / tm.tmHeight, hwnd, FALSE) * tm.tmAveCharWidth;
			} else if (selected && GetKeyState(VK_SHIFT) >= 0) {
				hdc = GetDC(hwnd);
				draw_selected_text(hwnd, hdc, str, hfont, &tm, caret_pos_x, caret_pos_y, caret_from_pos_x, caret_from_pos_y, TRUE);
				ReleaseDC(hwnd, hdc);
				selected = FALSE;
			}
			set_pos(hwnd, &tm, &caret_pos_x, &caret_pos_y, &caret_from_pos_x, &caret_from_pos_y);
			break;
		case VK_RIGHT:
			if (caret_pos_x / tm.tmAveCharWidth < get_edit_pos(str, -1, caret_pos_y / tm.tmHeight, hwnd, FALSE)) {
				if (GetKeyState(VK_SHIFT) < 0) {
					/* �I���J�n�ʒu��ۑ�����B*/
					if (!selected) {
						caret_from_pos_x = caret_pos_x;
						caret_from_pos_y = caret_pos_y;
						selected = TRUE;
					}

					HideCaret(hwnd);
					hdc = GetDC(hwnd);
					SelectObject(hdc, hfont);

					/* �I��/�ʏ핶����\���̐ݒ�B*/
					if (caret_pos_y == caret_from_pos_y && caret_pos_x >= caret_from_pos_x ||
					caret_pos_y > caret_from_pos_y) {
						SetTextColor(hdc, GetSysColor(COLOR_HIGHLIGHTTEXT));
						SetBkColor(hdc, GetSysColor(COLOR_HIGHLIGHT));
					} else {
						SetTextColor(hdc, GetSysColor(COLOR_WINDOWTEXT));
						SetBkColor(hdc, GetSysColor(COLOR_WINDOW));
					}

					/* �I��/�ʏ핶�����`�悷��B*/
					TextOut(hdc, caret_pos_x, caret_pos_y, get_edit_str(str, caret_pos_x / tm.tmAveCharWidth, caret_pos_y / tm.tmHeight, hwnd), 1);

					ReleaseDC(hwnd, hdc);
					ShowCaret(hwnd);
				} else if (selected) {
					hdc = GetDC(hwnd);
					draw_selected_text(hwnd, hdc, str, hfont, &tm, caret_pos_x, caret_pos_y, caret_from_pos_x, caret_from_pos_y, TRUE);
					ReleaseDC(hwnd, hdc);
					selected = FALSE;
				}

				caret_pos_x += tm.tmAveCharWidth;
			} else if ((i = get_edit_pos(str, 0, caret_pos_y / tm.tmHeight + 1, hwnd, TRUE)) != -2147483648) {
				if (GetKeyState(VK_SHIFT) < 0) {
					/* �I���J�n�ʒu��ۑ�����B*/
					if (!selected) {
						caret_from_pos_x = caret_pos_x;
						caret_from_pos_y = caret_pos_y;
						selected = TRUE;
					}
				} else if (selected) {
					hdc = GetDC(hwnd);
					draw_selected_text(hwnd, hdc, str, hfont, &tm, caret_pos_x, caret_pos_y, caret_from_pos_x, caret_from_pos_y, TRUE);
					ReleaseDC(hwnd, hdc);
					selected = FALSE;
				}

				caret_pos_x = -GetScrollPos(hwnd, SB_HORZ) * tm.tmAveCharWidth;
				caret_pos_y += tm.tmHeight;
			} else if (selected && GetKeyState(VK_SHIFT) >= 0) {
				hdc = GetDC(hwnd);
				draw_selected_text(hwnd, hdc, str, hfont, &tm, caret_pos_x, caret_pos_y, caret_from_pos_x, caret_from_pos_y, TRUE);
				ReleaseDC(hwnd, hdc);
				selected = FALSE;
			}
			set_pos(hwnd, &tm, &caret_pos_x, &caret_pos_y, &caret_from_pos_x, &caret_from_pos_y);
			break;
		case VK_HOME:
			caret_pos_x = -GetScrollPos(hwnd, SB_HORZ) * tm.tmAveCharWidth;
			set_pos(hwnd, &tm, &caret_pos_x, &caret_pos_y, &caret_from_pos_x, &caret_from_pos_y);
			break;
		case VK_END:
			caret_pos_x = get_edit_pos(str, -1, caret_pos_y / tm.tmHeight, hwnd, FALSE) * tm.tmAveCharWidth;
			set_pos(hwnd, &tm, &caret_pos_x, &caret_pos_y, &caret_from_pos_x, &caret_from_pos_y);
			break;
		case VK_DELETE:
			/* �����񂪑I������Ă��Ȃ��Ȃ�ʒu��1��Ɉړ������Ă���Back Space�Ɠ����������s���B
			�����łȂ��ꍇ��Back Space�Ɠ��������Ƃ���B*/
			if (caret_pos_x / tm.tmAveCharWidth < get_edit_pos(str, -1, caret_pos_y / tm.tmHeight, hwnd, FALSE)) {
				if (!selected) caret_pos_x += tm.tmAveCharWidth;
				goto backspace;
			} else if ((i = get_edit_pos(str, 0, caret_pos_y / tm.tmHeight + 1, hwnd, TRUE)) != -2147483648) {
				if (!selected) {
					caret_pos_x = -GetScrollPos(hwnd, SB_HORZ) * tm.tmAveCharWidth;
					caret_pos_y += tm.tmHeight;
				}
				goto backspace;
			} else if (selected)
				goto backspace;
			MessageBeep(MB_OK);
			break;
		}
		SetCaretPos(caret_pos_x, caret_pos_y);
		return 0;
	case WM_CHAR:
		if (GetCapture() != NULL) break;

		/* �����������當������폜����O�̍s���̏�񂪎擾�ł����Rectangle��rect.right������ł��邾�낤�B*/
		switch (wParam) {
		case 0x08:	/* Back Space */
			if (caret_pos_x <= -GetScrollPos(hwnd, SB_HORZ) * tm.tmAveCharWidth &&
			caret_pos_y <= -GetScrollPos(hwnd, SB_VERT) * tm.tmHeight && !selected) {
				MessageBeep(MB_OK);
				return 0;
			}
backspace:
			get_edit_selection_pos(str, hwnd, &tm, &selected, caret_pos_x, caret_pos_y, caret_from_pos_x, caret_from_pos_y, &esp);

			/* �폜�O�̑傫�����̈ʒu�̍s�̖����ƍ폜�O�̑傫�����̎w��ʒu�̍��̈ʒu��ۑ�����B�폜���čs���ς��ꍇ�Ɏg�p����B*/
			j = (get_edit_pos(str, -1, esp.max_y, hwnd, FALSE) - esp.max_x) * tm.tmAveCharWidth;

			/* ���������̎w��ʒu��1����/�I�𕶎�����폜����B
			�����񂪑I������Ă��Ȃ��ꍇ��1�����폜�Ƃ��A�����łȂ��ꍇ�͑I�𕶎���̂ݍ폜�Ƃ���B*/
			pstr = get_edit_str(str, caret_pos_x / tm.tmAveCharWidth, caret_pos_y / tm.tmHeight, hwnd);
			MoveMemory(pstr - (esp.sel_cnt > 0 ? (esp.sel_cnt2 ? 0 : esp.sel_cnt) : 1), pstr + esp.sel_cnt2, GlobalSize(str) - (pstr + esp.sel_cnt2 - str));
			str = (LPTSTR)GlobalReAlloc(str, GlobalSize(str) - (esp.sel_cnt > 0 ? esp.sel_cnt : 1), GMEM_MOVEABLE);

			/* �ʒu��I�𕶎���̏��������̈ʒu�ɐݒ肷��B*/
			caret_pos_x = esp.min_x * tm.tmAveCharWidth;
			caret_pos_y = esp.min_y * tm.tmHeight;

			hdc = GetDC(hwnd);
			SetTextColor(hdc, GetSysColor(COLOR_WINDOWTEXT));
			SetBkColor(hdc, GetSysColor(COLOR_WINDOW));
			SelectObject(hdc, hfont);
			HideCaret(hwnd);
			GetClientRect(hwnd, &rect);
			SelectObject(hdc, CreatePen(PS_SOLID, 1, GetSysColor(COLOR_WINDOW)));
			SelectObject(hdc, GetSysColorBrush(COLOR_WINDOW));

			if (caret_pos_x > -GetScrollPos(hwnd, SB_HORZ) * tm.tmAveCharWidth) {
				if (esp.sel_cnt == 0) caret_pos_x -= tm.tmAveCharWidth;

				k = 0;	/* �I�𕶎���p�B*/
				set_pos(hwnd, &tm, &caret_pos_x, &caret_pos_y, &caret_from_pos_x, &caret_from_pos_y);
				set_max_line(hwnd, &tm, get_edit_pos(str, -1, caret_pos_y / tm.tmHeight, hwnd, FALSE), esp.min_y - esp.max_y);
			} else {
				if (esp.sel_cnt == 0) {
					caret_pos_y -= tm.tmHeight;
					if (caret_pos_y < -GetScrollPos(hwnd, SB_VERT) * tm.tmHeight) caret_pos_y = -GetScrollPos(hwnd, SB_VERT) * tm.tmHeight;	/* �I�𕶎��񎞁A�ʒuy�����ɂȂ�ꍇ�����邽�߁B*/
					/* ��̍s�Ɉړ��̍ہA�ʒux���c��̕��������炷�B*/
					caret_pos_x = get_edit_pos(str, -1, caret_pos_y / tm.tmHeight, hwnd, FALSE) * tm.tmAveCharWidth - j;
				}

				/* �폜���ꂽ�s��艺�̍s��`�悷��B*/
				for (i = caret_pos_y / tm.tmHeight + 1; get_edit_pos(str, 0, i, hwnd, TRUE) != -2147483648; i++) {
					Rectangle(hdc, 0, i * tm.tmHeight, rect.right, i * tm.tmHeight + tm.tmHeight);
					TextOut(hdc, 0, i * tm.tmHeight, get_edit_str(str, 0, i, hwnd), get_edit_pos(str, -1, i, hwnd, FALSE));
				}
				Rectangle(hdc, 0, i * tm.tmHeight, rect.right, i * tm.tmHeight + tm.tmHeight);

				k = 1;	/* �I�𕶎���p�B*/
				set_pos(hwnd, &tm, &caret_pos_x, &caret_pos_y, &caret_from_pos_x, &caret_from_pos_y);
				set_max_line(hwnd, &tm, get_edit_pos(str, -1, caret_pos_y / tm.tmHeight, hwnd, FALSE), (esp.sel_cnt > 0 ? 0 : -1) - (esp.max_y - esp.min_y));
			}
			/* �폜���������̍s���폜���������̈ʒu����`�悷��B*/
			i = get_edit_pos(str, -1, caret_pos_y / tm.tmHeight, hwnd, FALSE) - caret_pos_x / tm.tmAveCharWidth;
			TextOut(hdc, caret_pos_x, caret_pos_y, get_edit_str(str, caret_pos_x / tm.tmAveCharWidth, caret_pos_y / tm.tmHeight, hwnd), i);
			/* �s�̖������炻�̍s�������`�悷��B*/
			Rectangle(hdc, caret_pos_x + i * tm.tmAveCharWidth, caret_pos_y, rect.right, caret_pos_y + tm.tmHeight);

			/* �I�𕶎��񂪕����s�̏ꍇ�A�폜���ꂽ�s��艺�̍s��`�悷��B*/
			if (esp.min_y < esp.max_y) {
				for (i = esp.min_y + 1; get_edit_pos(str, 0, i, hwnd, TRUE) != -2147483648; i++) {
					Rectangle(hdc, 0, i * tm.tmHeight, rect.right, i * tm.tmHeight + tm.tmHeight);
					TextOut(hdc, 0, i * tm.tmHeight, get_edit_str(str, 0, i, hwnd), get_edit_pos(str, -1, i, hwnd, FALSE));
				}
				/* �폜���ꂽ�����s�������`�悷��B*/
				Rectangle(hdc, 0, i * tm.tmHeight, rect.right, i * tm.tmHeight + tm.tmHeight * (esp.max_y - esp.min_y + k));
			}

			DeleteObject(SelectObject(hdc, GetStockObject(BLACK_PEN)));
			ReleaseDC(hwnd, hdc);
			break;
		case 0x0a:	/* Enter */
		case 0x0d:
			get_edit_selection_pos(str, hwnd, &tm, &selected, caret_pos_x, caret_pos_y, caret_from_pos_x, caret_from_pos_y, &esp);

			/* �����񂪑I������Ă���/���Ȃ���Ԃ�A������̓r���̈ʒu�ɓ��͂��Ă�
			���������̕�����̓K�؂Ȉʒu�ɉ��s������ݒ肷��B*/
			str = str == NULL ? (LPTSTR)GlobalAlloc(GMEM_FIXED, sizeof(TCHAR)) : (LPTSTR)GlobalReAlloc(str, GlobalSize(str) + sizeof(TCHAR), GMEM_MOVEABLE);
			pstr = get_edit_str(str, caret_pos_x / tm.tmAveCharWidth, caret_pos_y / tm.tmHeight, hwnd);
			MoveMemory(pstr + 1 - (esp.sel_cnt2 ? 0 : esp.sel_cnt), pstr + esp.sel_cnt2, GlobalSize(str) - (pstr + esp.sel_cnt2 - str) - 1);
			pstr[esp.sel_cnt2 ? 0 : -esp.sel_cnt] = '\n';
			if (esp.sel_cnt > 0) str = (LPTSTR)GlobalReAlloc(str, GlobalSize(str) - esp.sel_cnt, GMEM_MOVEABLE);

			/* �ʒu��I�𕶎���̏��������̈ʒu�ɐݒ肷��B*/
			caret_pos_x = esp.min_x * tm.tmAveCharWidth;
			caret_pos_y = esp.min_y * tm.tmHeight;

			hdc = GetDC(hwnd);
			SetTextColor(hdc, GetSysColor(COLOR_WINDOWTEXT));
			SetBkColor(hdc, GetSysColor(COLOR_WINDOW));
			SelectObject(hdc, hfont);
			HideCaret(hwnd);
			GetClientRect(hwnd, &rect);
			SelectObject(hdc, CreatePen(PS_SOLID, 1, GetSysColor(COLOR_WINDOW)));
			SelectObject(hdc, GetSysColorBrush(COLOR_WINDOW));
			/* �ǉ����ꂽ�s�ȍ~��`�悷��B*/
			for (i = esp.min_y; get_edit_pos(str, 0, i, hwnd, TRUE) != -2147483648; i++) {
				Rectangle(hdc, 0, i * tm.tmHeight, rect.right, i * tm.tmHeight + tm.tmHeight);
				TextOut(hdc, 0, i * tm.tmHeight, get_edit_str(str, 0, i, hwnd), get_edit_pos(str, -1, i, hwnd, FALSE));
			}

			/* �I�𕶎��񂪕����s�̏ꍇ�A�폜���ꂽ�s��艺�̍s��`�悷��B*/
			if (esp.min_y < esp.max_y) {
				for (i = esp.min_y + 1; get_edit_pos(str, 0, i, hwnd, TRUE) != -2147483648; i++) {
					Rectangle(hdc, 0, i * tm.tmHeight, rect.right, i * tm.tmHeight + tm.tmHeight);
					TextOut(hdc, 0, i * tm.tmHeight, get_edit_str(str, 0, i, hwnd), get_edit_pos(str, -1, i, hwnd, FALSE));
				}
				/* �폜���ꂽ�����s�������`�悷��B*/
				Rectangle(hdc, 0, i * tm.tmHeight, rect.right, i * tm.tmHeight + tm.tmHeight * (esp.max_y - esp.min_y));
			}

			DeleteObject(SelectObject(hdc, GetStockObject(BLACK_PEN)));
			ReleaseDC(hwnd, hdc);

			caret_pos_x = -GetScrollPos(hwnd, SB_HORZ) * tm.tmAveCharWidth;
			caret_pos_y += tm.tmHeight;
			set_pos(hwnd, &tm, &caret_pos_x, &caret_pos_y, &caret_from_pos_x, &caret_from_pos_y);
			set_max_line(hwnd, &tm, get_edit_pos(str, -1, caret_pos_y / tm.tmHeight, hwnd, FALSE), 1 - (esp.max_y - esp.min_y));
			break;
		case 0x1b:	/* Escape */
			if (!selected) break;
			hdc = GetDC(hwnd);
			draw_selected_text(hwnd, hdc, str, hfont, &tm, caret_pos_x, caret_pos_y, caret_from_pos_x, caret_from_pos_y, TRUE);
			ReleaseDC(hwnd, hdc);
			selected = FALSE;
			break;
		default:
			get_edit_selection_pos(str, hwnd, &tm, &selected, caret_pos_x, caret_pos_y, caret_from_pos_x, caret_from_pos_y, &esp);

			/* �����񂪑I������Ă���/���Ȃ���Ԃ�A������̓r���̈ʒu�ɓ��͂��Ă�
			���������̕�����̓K�؂Ȉʒu�ɓ��͂��ꂽ1������ݒ肷��B*/
			str = str == NULL ? (LPTSTR)GlobalAlloc(GMEM_FIXED, sizeof(TCHAR)) : (LPTSTR)GlobalReAlloc(str, GlobalSize(str) + sizeof(TCHAR), GMEM_MOVEABLE);
			pstr = get_edit_str(str, caret_pos_x / tm.tmAveCharWidth, caret_pos_y / tm.tmHeight, hwnd);
			MoveMemory(pstr + 1 - (esp.sel_cnt2 ? 0 : esp.sel_cnt), pstr + esp.sel_cnt2, GlobalSize(str) - (pstr + esp.sel_cnt2 - str) - 1);
			pstr[esp.sel_cnt2 ? 0 : -esp.sel_cnt] = (TCHAR)wParam;
			if (esp.sel_cnt > 0) str = (LPTSTR)GlobalReAlloc(str, GlobalSize(str) - esp.sel_cnt, GMEM_MOVEABLE);

			/* �ʒu��I�𕶎���̏��������̈ʒu�ɐݒ肷��B*/
			caret_pos_x = esp.min_x * tm.tmAveCharWidth;
			caret_pos_y = esp.min_y * tm.tmHeight;

			hdc = GetDC(hwnd);
			SelectObject(hdc, hfont);
			HideCaret(hwnd);
			GetClientRect(hwnd, &rect);

			SelectObject(hdc, CreatePen(PS_SOLID, 1, GetSysColor(COLOR_WINDOW)));
			SelectObject(hdc, GetSysColorBrush(COLOR_WINDOW));
			/* �܂��A�ǉ����ꂽ�s�ȍ~�������`�悷��B*/
			Rectangle(hdc, caret_pos_x, caret_pos_y, rect.right, caret_pos_y + tm.tmHeight);

			/* �}�����������̍s��}�����������̈ʒu����`�悷��B*/
			SetTextColor(hdc, GetSysColor(COLOR_WINDOWTEXT));
			SetBkColor(hdc, GetSysColor(COLOR_WINDOW));
			i = get_edit_pos(str, -1, esp.min_y, hwnd, FALSE) - esp.min_x;
			TextOut(hdc, caret_pos_x, caret_pos_y, get_edit_str(str, esp.min_x, esp.min_y, hwnd), i);

			/* �I�𕶎��񂪕����s�̏ꍇ�A�폜���ꂽ�s��艺�̍s��`�悷��B*/
			if (esp.min_y < esp.max_y) {
				for (i = esp.min_y + 1; get_edit_pos(str, 0, i, hwnd, TRUE) != -2147483648; i++) {
					Rectangle(hdc, 0, i * tm.tmHeight, rect.right, i * tm.tmHeight + tm.tmHeight);
					TextOut(hdc, 0, i * tm.tmHeight, get_edit_str(str, 0, i, hwnd), get_edit_pos(str, -1, i, hwnd, FALSE));
				}
				/* �폜���ꂽ�����s�������`�悷��B*/
				Rectangle(hdc, 0, i * tm.tmHeight, rect.right, i * tm.tmHeight + tm.tmHeight * (esp.max_y - esp.min_y));
			}

			DeleteObject(SelectObject(hdc, GetStockObject(BLACK_PEN)));
			ReleaseDC(hwnd, hdc);

			caret_pos_x += tm.tmAveCharWidth;
			set_pos(hwnd, &tm, &caret_pos_x, &caret_pos_y, &caret_from_pos_x, &caret_from_pos_y);
			set_max_line(hwnd, &tm, get_edit_pos(str, -1, caret_pos_y / tm.tmHeight, hwnd, FALSE), esp.min_y - esp.max_y);
		}
		SetCaretPos(caret_pos_x, caret_pos_y);
		ShowCaret(hwnd);
		return 0;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case 100:	/* �u�؂���v */
		case 101:	/* �u�R�s�[�v */
			if (!selected || !OpenClipboard(hwnd) || !EmptyClipboard()) break;

			i = selected;	/* �_�~�[���g�p����selected�͕ۂB*/
			get_edit_selection_pos(str, hwnd, &tm, &i, caret_pos_x, caret_pos_y, caret_from_pos_x, caret_from_pos_y, &esp);

			halc = GlobalAlloc(GMEM_MOVEABLE, esp.sel_cnt + 1);
			pstr = GlobalLock(halc);
			CopyMemory(pstr, get_edit_str(str, esp.min_x, esp.min_y, hwnd), esp.sel_cnt);
			pstr[esp.sel_cnt] = '\0';
			GlobalUnlock(halc);
			SetClipboardData(CF_TEXT, halc);
			CloseClipboard();

			if (LOWORD(wParam) == 100) goto backspace;
			break;
		case 102:	/* �u�\��t���v */
			if (!IsClipboardFormatAvailable(CF_TEXT) || !OpenClipboard(hwnd)) break;

			halc = GetClipboardData(CF_TEXT);
			pstr = GlobalLock(halc);
			/* ����x�ɓ\��t���ł���ׂ��ł���B*/
			for (i = 0; pstr[i] != '\0'; i++)
				if (pstr[i] != 0x0d) SendMessage(hwnd, WM_CHAR, pstr[i], 0);
			GlobalUnlock(halc);
			CloseClipboard();
			break;
		case 103:	/* �u�폜�v */
			if (selected) goto backspace;
			break;
		case 104:	/* �u���ׂđI���v */
			for (i = 0; get_edit_pos(str, 0, i, hwnd, TRUE) != -2147483648; i++);
			i--;
			if (i < 0) break;

			caret_from_pos_x = -GetScrollPos(hwnd, SB_HORZ) * tm.tmAveCharWidth;
			caret_from_pos_y = -GetScrollPos(hwnd, SB_VERT) * tm.tmHeight;
			caret_pos_x = get_edit_pos(str, -1, i, hwnd, FALSE) * tm.tmAveCharWidth;
			caret_pos_y = i * tm.tmHeight;
			selected = TRUE;

			hdc = GetDC(hwnd);
			draw_selected_text(hwnd, hdc, str, hfont, &tm, caret_from_pos_x, caret_from_pos_y, caret_pos_x, caret_pos_y, FALSE);
			ReleaseDC(hwnd, hdc);
			break;
		}
		return 0;
	case WM_HSCROLL:	/* ���X�N���[�� */
		i = GetScrollPos(hwnd, SB_HORZ);
		switch (LOWORD(wParam)) {
		case SB_LEFT:
			GetScrollRange(hwnd, SB_HORZ, &j, &k);
			SetScrollPos(hwnd, SB_HORZ, j, TRUE);
			break;
		case SB_RIGHT:
			GetScrollRange(hwnd, SB_HORZ, &j, &k);
			SetScrollPos(hwnd, SB_HORZ, k, TRUE);
			break;
		case SB_LINELEFT:
			SetScrollPos(hwnd, SB_HORZ, GetScrollPos(hwnd, SB_HORZ) - 1, TRUE);
			break;
		case SB_LINERIGHT:
			SetScrollPos(hwnd, SB_HORZ, GetScrollPos(hwnd, SB_HORZ) + 1, TRUE);
			break;
		case SB_PAGELEFT:
			GetClientRect(hwnd, &rect);
			SetScrollPos(hwnd, SB_HORZ, GetScrollPos(hwnd, SB_HORZ) - (rect.right - rect.left) / tm.tmAveCharWidth, TRUE);
			break;
		case SB_PAGERIGHT:
			GetClientRect(hwnd, &rect);
			SetScrollPos(hwnd, SB_HORZ, GetScrollPos(hwnd, SB_HORZ) + (rect.right - rect.left) / tm.tmAveCharWidth, TRUE);
			break;
		case SB_THUMBTRACK:
			SetScrollPos(hwnd, SB_HORZ, HIWORD(wParam), TRUE);
			break;
		}
		j = GetScrollPos(hwnd, SB_HORZ);
		if (i != j) {
			/* ���֐i�ނƐ��A�E�֐i�ނƕ��ɂ���B*/
			caret_pos_x += (i - j) * tm.tmAveCharWidth;
			caret_from_pos_x += (i - j) * tm.tmAveCharWidth;
			SetCaretPos(caret_pos_x, caret_pos_y);
			InvalidateRect(hwnd, NULL, TRUE);
		}
		return 0;
	case WM_VSCROLL:	/* �c�X�N���[�� */
		i = GetScrollPos(hwnd, SB_VERT);
		switch (LOWORD(wParam)) {
		case SB_BOTTOM:
			GetScrollRange(hwnd, SB_VERT, &j, &k);
			SetScrollPos(hwnd, SB_VERT, k, TRUE);
			break;
		case SB_LINEDOWN:
			SetScrollPos(hwnd, SB_VERT, GetScrollPos(hwnd, SB_VERT) + 1, TRUE);
			break;
		case SB_LINEUP:
			SetScrollPos(hwnd, SB_VERT, GetScrollPos(hwnd, SB_VERT) - 1, TRUE);
			break;
		case SB_PAGEDOWN:
			GetClientRect(hwnd, &rect);
			SetScrollPos(hwnd, SB_VERT, GetScrollPos(hwnd, SB_VERT) + (rect.bottom - rect.top) / tm.tmHeight, TRUE);
			break;
		case SB_PAGEUP:
			GetClientRect(hwnd, &rect);
			SetScrollPos(hwnd, SB_VERT, GetScrollPos(hwnd, SB_VERT) - (rect.bottom - rect.top) / tm.tmHeight, TRUE);
			break;
		case SB_THUMBTRACK:
			SetScrollPos(hwnd, SB_VERT, HIWORD(wParam), TRUE);
			break;
		case SB_TOP:
			GetScrollRange(hwnd, SB_VERT, &j, &k);
			SetScrollPos(hwnd, SB_VERT, j, TRUE);
			break;
		}
		j = GetScrollPos(hwnd, SB_VERT);
		if (i != j) {
			/* ��֐i�ނƐ��A���֐i�ނƕ��ɂ���B*/
			caret_pos_y += (i - j) * tm.tmHeight;
			caret_from_pos_y += (i - j) * tm.tmHeight;
			SetCaretPos(caret_pos_x, caret_pos_y);
			InvalidateRect(hwnd, NULL, TRUE);
		}
		return 0;
	case WM_IME_STARTCOMPOSITION:
		/* IME�̕\���ʒu���L�����b�g�̈ʒu�ɐݒ肷��B*/
		himc = ImmGetContext(hwnd);
		ImmGetCompositionWindow(himc, &composform);
		GetCaretPos(&composform.ptCurrentPos);
		ImmSetCompositionFont(himc, &logfont);
		ImmSetCompositionWindow(himc, &composform);
		ImmReleaseContext(hwnd, himc);
		return 0;
	case WM_DESTROY:
		GlobalFree(str);
		DeleteObject(hfont);
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	WNDCLASS wc;
	MSG msg;
	HWND hwnd;
	ACCEL accel[] = {
		{FCONTROL | FVIRTKEY, 0x58, 100},	/* �u�؂���v */
		{FCONTROL | FVIRTKEY, 0x43, 101},	/* �u�R�s�[�v */
		{FCONTROL | FVIRTKEY, 0x56, 102},	/* �u�\��t���v */
		{FCONTROL | FVIRTKEY, 0x41, 104}	/* �u���ׂđI���v */
	};
	HACCEL haccel;

	wc.style = 0;
	wc.lpfnWndProc = WindowProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_IBEAM);
	wc.hbrBackground = (HBRUSH)COLOR_WINDOWFRAME;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = "edittest";
	if (!RegisterClass(&wc)) return 0;

	if ((hwnd = CreateWindow(
		wc.lpszClassName, "EditTest",
		WS_HSCROLL | WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_VSCROLL,
		200, 200, 300, 250, NULL, NULL, hInstance, NULL
	)) == NULL) return 0;
	ShowScrollBar(hwnd, SB_HORZ, FALSE);
	ShowScrollBar(hwnd, SB_VERT, FALSE);

	haccel = CreateAcceleratorTable(accel, sizeof(accel) / sizeof(accel[0]));

	while (GetMessage(&msg, NULL, 0, 0)) {
		if (TranslateAccelerator(hwnd, haccel, &msg)) continue;
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	DestroyAcceleratorTable(haccel);
	return msg.wParam;
}
