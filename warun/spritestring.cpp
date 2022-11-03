#pragma once
#include "sprite.cpp"

// ������X�v���C�g�N���X�B
class SpriteString : public Sprite {
public:
	// �摜�X�v���C�g�R���X�g���N�^�B
	// �����Fpd3dd Direct3D�f�o�C�X�I�u�W�F�N�g�B
	//       pos_x X�ʒu�B
	//       pos_y Y�ʒu�B
	//       width ���B
	//       height �����B
	//       size �t�H���g�T�C�Y�B
	//       color �t�H���g�̐F�B
	//       color_mo �J�[�\�������킹�����̃t�H���g�̐F�B�ς������Ȃ��Ȃ�0�A���ɂ������Ȃ�0xff000000�B
	//       id �X�v���C�gID�B
	SpriteString(LPDIRECT3DDEVICE9 pd3dd, short pos_x, short pos_y, WORD width, WORD height
		, BYTE size, D3DCOLOR color, D3DCOLOR color_mo, BYTE id) : Sprite(pd3dd) {

		m_id = id;
		m_color = color;
		m_color_mo = color_mo;
		m_color_flag = false;

		// �t�H���g�쐬�B
		m_hfont = CreateFont(size, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET
			, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, TEXT("�l�r �o����"));
		if (m_hfont == NULL) return;

		// ��X�v���C�g�쐬�B
		for (m_scr_width = 1; m_scr_width < width; m_scr_width <<= 1);
		for (m_scr_height = 1; m_scr_height < height; m_scr_height <<= 1);
		if (FAILED(m_pd3dd->CreateTexture(m_scr_width, m_scr_height, 1, 0
			, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &m_pd3dt, NULL))) return;

		m_pos_x = pos_x;
		m_pos_y = pos_y;
		m_width = width;
		m_height = height;
	}

	~SpriteString() {
		if (m_hfont != NULL) DeleteObject(m_hfont);
	}

	// �����ɐݒ��A�������`��̈�ɏ�������(���s��͓����ɂȂ����܂܂Ȃ̂ŕ\�����������s���邱��)�B
	// �����Fptext ���́B
	//       pname ���O�B
	//       bracket ���͂��͂ފ��ʂ̎�ށB
	//       noclr_flag true�ɂ���ƕ`��̈�����������Ȃ��B
	//       pos_x ��������X�ʒu�B
	//       pos_y ��������Y�ʒu�B
	// �Ԓl�F�����Ȃ�true�A����ȊO��false�B
	bool writeString(LPCWSTR ptext, LPCWSTR pname, BYTE bracket, bool noclr_flag, WORD pos_x, WORD pos_y) {
		static LPCWSTR pbracket_start[] = {L"�u", L"�i", L"�w", L" "};
		static LPCWSTR pbracket_end[] = {L"�v", L"�j", L"�x", L""};
		bool succ_flag = false, lock_flag = false;
		LPWSTR pstr = NULL;
		HDC hdc = CreateCompatibleDC(NULL);
		TEXTMETRIC tm;
		D3DLOCKED_RECT d3dlr;

		m_alpha = 0;
		m_color_flag = false;
		if (hdc == NULL) return false;

		SelectObject(hdc, m_hfont);
		if (!GetTextMetrics(hdc, &tm)) goto procend;

		if (FAILED(m_pd3dt->LockRect(0, &d3dlr, NULL, 0))) goto procend;
		lock_flag = true;
		if (!noclr_flag) ZeroMemory(d3dlr.pBits, m_scr_width * m_height * 4);

		if (pname == NULL) {
			if (!writeStringDetail(hdc, &tm, &d3dlr, ptext, &pos_x, &pos_y, 0)) goto procend;
		} else {
			if (bracket >= sizeof(pbracket_start) / sizeof(pbracket_start[0])) bracket = 0;

			if (!writeStringDetail(hdc, &tm, &d3dlr, pname, &pos_x, &pos_y, 0)) goto procend;
			if (!writeStringDetail(hdc, &tm, &d3dlr, pbracket_start[bracket], &pos_x, &pos_y, 0)) goto procend;

			pstr = new WCHAR[lstrlenW(ptext) + 2];
			if (pstr == NULL) goto procend;
			if (lstrcpyW(pstr, ptext) == NULL) goto procend;
			if (lstrcatW(pstr, pbracket_end[bracket]) == NULL) goto procend;

			writeStringDetail(hdc, &tm, &d3dlr, pstr, &pos_x, &pos_y, pos_x);
		}

		succ_flag = true;
procend:
		if (lock_flag) m_pd3dt->UnlockRect(0);
		if (pstr != NULL) delete[] pstr;
		DeleteDC(hdc);
		return succ_flag;
	}

	void onLButtonDown() {
		if (!m_color_flag && m_color_mo > 0) changeColor(m_color_mo);
	}

	void onMouseOver() {
		if (m_color_mo > 0) changeColor(m_color_mo);
		m_color_flag = true;
	}

	void onMouseOut() {
		if (m_color_mo > 0) changeColor(m_color);
		m_color_flag = false;
	}

protected:
	HFONT m_hfont;
	D3DCOLOR m_color, m_color_mo;
	bool m_color_flag;

	// �������ݑΏۗ̈�ɕ��������擾���Ȃ���1�������K�؂Ȉʒu�ɏ�������ł����B
	// �����Fhdc �f�o�C�X�R���e�L�X�g�B
	//       ptm �t�H���g���g���b�N�B
	//       pd3dlr �������ݑΏۗ̈�B
	//       pstr �������ޕ�����B���s�����ŉ��s����B���ɂ͂ݏo��ꍇ��s���ƍs���֑̋������Ŏ������s����B
	//       ppos_x �������݂��J�n����X�ʒu�B������A�Ō�ɏ������񂾕����̈ʒu�ɕύX�����B
	//       ppos_y �������݂��J�n����Y�ʒu�B������A�Ō�ɏ������񂾕����̈ʒu�ɕύX�����B
	//       nl_pos ���s��ɏ������݂��J�n����X�ʒu�B
	// �Ԓl�F������𐳂����������߂��ꍇ��true�A����ȊO��false�B
	bool writeStringDetail(HDC hdc, const LPTEXTMETRIC ptm, const D3DLOCKED_RECT *pd3dlr
		, LPCWSTR pstr, WORD *ppos_x, WORD *ppos_y, WORD nl_pos) {

		static MAT2 mat = {{0, 1}, {0, 0}, {0, 0}, {0, 1}};
		LPCWSTR plen = pstr + lstrlenW(pstr);
		BYTE nl_type, *pfont_buf, *pfb;
		WORD i, j;
		DWORD pos, font_buf_size;
		GLYPHMETRICS gm, gm_next;

		for (; pstr < plen; pstr++) {
			// ���������i�[����̂ɕK�v�ȃo�b�t�@�T�C�Y���擾����B
			font_buf_size = GetGlyphOutlineW(hdc, *pstr, GGO_GRAY8_BITMAP, &gm, 0, NULL, &mat);
			if (font_buf_size == GDI_ERROR) break;

			// ���s���ׂ����ǂ�������B
			nl_type = 0;
			if (*pstr == L'\n') {
				nl_type = 2;	// �������s�B
			} else if (*ppos_x > nl_pos) {	// �ʒu���s�̎n�߂̏ꍇ�͎������s���Ȃ��B
				if (*ppos_x + gm.gmCellIncX >= m_width) {
					nl_type = 1;	// ���ɂ͂ݏo��̂ŉ��s�B
				} else if (pstr + 1 < plen) {
					if (getNormalStrPos(pstr, false) == 0) {	// ���̕������s���֑������ł͂Ȃ��H
						pos = getNormalStrPos(pstr + 1, false);

						for (i = j = 0; i < pos; j += gm_next.gmCellIncX, i++) {
							if (GetGlyphOutlineW(hdc, pstr[1 + i], GGO_GRAY8_BITMAP, &gm_next, 0, NULL, &mat) == GDI_ERROR) break;
							if (*ppos_x + gm.gmCellIncX + j + gm_next.gmCellIncX < m_width) continue;
							nl_type = 1;	// �s���֑������̂��߂̉��s�B
							break;
						}

						if (i < pos && nl_type == 0) break;	// ��L�����̕������擾�֐������s�����ꍇ�B
					}
					if (nl_type == 0 && getNormalStrPos(pstr, true) > 0) {	// ���̕������s���֑��������H
						if (GetGlyphOutlineW(hdc, pstr[1], GGO_GRAY8_BITMAP, &gm_next, 0, NULL, &mat) == GDI_ERROR) continue;
						if (*ppos_x + gm.gmCellIncX + gm_next.gmCellIncX >= m_width) nl_type = 1;	// �s���֑������̂��߂̉��s�B
					}
				}
			}

			if (nl_type > 0) {	// ���s�H
				*ppos_x = nl_pos;
				*ppos_y += ptm->tmHeight;
				if (nl_type == 2 || *pstr == L' ' || *pstr == L'�@') continue;	// ���s�������A�s�����󔒂Ȃ珑�����܂Ȃ��B
			}

			if (*ppos_y + ptm->tmHeight > m_height) return true;	// �c�ɂ͂ݏo��ꍇ�͂���ȏ�̕����̏������݂͕s�Ƃ���B

			// �������i�[�o�b�t�@���m�ۂ���B
			pfont_buf = new BYTE[font_buf_size];
			if (pfont_buf == NULL) break;

			font_buf_size = GetGlyphOutlineW(hdc, *pstr, GGO_GRAY8_BITMAP, &gm, font_buf_size, pfont_buf, &mat);
			if (font_buf_size != GDI_ERROR) {
				// �o�b�t�@��4�̔{���̃T�C�Y�ɒ������ꂽ������񂪊i�[���ꂽ�B

				if (*pstr != L' ' && *pstr != L'�@') {	// �󔒂�`��̈�ɏ������ޕK�v�͂Ȃ��B
					pfb = pfont_buf;
					for (i = 0; i < gm.gmBlackBoxY; i++) {
						for (j = 0; j < gm.gmBlackBoxX; pfb++, j++) {
							if (*pfb <= 0) continue;
							pos = (i + (ptm->tmAscent - gm.gmptGlyphOrigin.y) + *ppos_y) * m_scr_width
								+ (j + gm.gmptGlyphOrigin.x + *ppos_x);
							if (pos >= m_scr_width * m_height) continue;

							// �`��̈�ɏ������߂Ό�͕`�揈���ŕ`�悳���B
							((DWORD *)pd3dlr->pBits)[pos] = ((DWORD)(255 * ((float)*pfb / 64) + 0.5) << 24) + (m_color & 0x00ffffff);
						}
						pfb += (4 - (j & 3)) & 3;	// 4�̔{���ɂȂ�悤�ɒ����B
					}
				}

				*ppos_x += gm.gmCellIncX;
			}

			delete[] pfont_buf;
			if (font_buf_size == GDI_ERROR) break;
		}

		return pstr == plen;
	}

	// �����񂪋֑������łȂ��Ȃ����ʒu��Ԃ��B
	// �����Fpstr ���ׂ镶����B
	//       chchr_end_flag false�Ȃ�s���֑������A����ȊO�͍s���֑��������`�F�b�N����B
	// �Ԓl�F�����񂪋֑������łȂ��Ȃ����ʒu�B
	UINT getNormalStrPos(LPCWSTR pstr, bool chchr_end_flag) {
		static LPCWSTR pchchr_start = L"�A�B�C�D�E�I�H���v�x�j�p�l�n�z�[�`�c����������������@�B�D�F�H�������b��,.�!?%�)}]����������";
		static LPCWSTR pchchr_end = L"�u�w�i�o�k�m�y�({[";

		return wcsspn(pstr, chchr_end_flag ? pchchr_end : pchchr_start);
	}

	// �F��ύX����B
	// �����Fcolor �F�B
	void changeColor(D3DCOLOR color) {
		DWORD i, j, *pbuf;
		D3DLOCKED_RECT d3dlr;

		if (FAILED(m_pd3dt->LockRect(0, &d3dlr, NULL, 0))) return;

		pbuf = (DWORD *)d3dlr.pBits;
		for (i = 0; i < m_height; i++) {
			for (j = 0; j < m_width; j++) {
				*pbuf = (*pbuf & 0xff000000) + (color & 0x00ffffff);
				pbuf++;
			}
			pbuf += m_scr_width - m_width;
		}

		m_pd3dt->UnlockRect(0);
	}
};
