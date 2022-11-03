#pragma once
#include <stdio.h>
#include <io.h>
#include "cryptblowfish.cpp"
#define PACKFILE "warun.pak"
#define PACKFILE_TYPE 0	// 0:�Í����ς݃p�b�N�t�@�C���A1:���p�b�N�t�@�C���A2:�X�̃t�@�C��

// �p�b�N�t�@�C���ǂݍ��݃N���X�B
class PackFile {
public:
	// �R���X�g���N�^�B
	// �����Fcache_size �������ς݃f�[�^�̃L���b�V���T�C�Y�B
	PackFile(UINT cache_size) {
#if PACKFILE_TYPE < 2
		m_pfile = fopen(PACKFILE, "rb");	// �Í������ꂽ8�̔{���̃T�C�Y�̃p�b�N�t�@�C���B

		m_start_pos = m_pos = 0;
		m_end_pos = m_pfile == NULL ? 0 : _filelength(_fileno(m_pfile));

		m_cache_size = cache_size < 256 ? 256 : cache_size;
		m_pcache = new char[m_cache_size + 1];	// ������\0��ǉ�����̂ŁA+1���Ă����B
		m_cache_start_pos = m_cache_end_pos = 0;
#else
		m_pfile = NULL;
#endif
	}

	// �f�X�g���N�^�B
	~PackFile() {
#if PACKFILE_TYPE < 2
		if (m_pcache != NULL) delete[] m_pcache;
#endif
		if (m_pfile != NULL) fclose(m_pfile);
	}

	// �t�@�C�����J���B
	// �����Fpfliename �t�@�C�����B
	//       pmode fopen�`���̃t�@�C�����[�h�B
	// �Ԓl�F�����Ȃ�true�A����ȊO��false�B
	bool Open(LPCSTR pfliename, LPCSTR pmode = "rb") {
#if PACKFILE_TYPE < 2
		char buf[256];

		if (m_pfile == NULL || m_pcache == NULL) return false;
		if (fseek(m_pfile, 0, SEEK_SET) != 0) return false;
		m_start_pos = m_pos = 0;
		m_end_pos = _filelength(_fileno(m_pfile));
		m_cache_start_pos = m_cache_end_pos = 0;

		// �V�O�j�`���m�F(����Afseek��SEEK_CUR�łȂ��ꍇ�́A�V�O�j�`���̃T�C�Y���l���ɓ����悤�ɂ���)�B
		if (fread(buf, 10, 1, m_pfile) != 1) return false;
		if (memcmp(buf, "WARUNPACK\0", 10) != 0) return false;

		// �w�b�_����t�@�C�����������B
		for (;;) {
			if (!readLine(buf, sizeof(buf))) return false;
			if (buf[0] == '\0') return false;	// �Ō�܂Ō�����Ȃ������B
			if (strcmp(buf, pfliename) != 0) {
				if (!Seek(8, SEEK_CUR)) return false;	// �s�v�����΂��B
				continue;
			}

			if (Read(&m_start_pos, 4) != 4) return false;	// �t�@�C���̈ʒu�B
			if (Read(&m_end_pos, 4) != 4) {	// �t�@�C���̃T�C�Y�B
				m_start_pos = 0;
				return false;
			}

			m_pos = m_start_pos;
			m_end_pos += m_start_pos;
			return Seek(0, SEEK_SET);
		}
		return false;
#else
		if (m_pfile != NULL) fclose(m_pfile);
		m_pfile = fopen(pfliename, pmode);
		return m_pfile != NULL;
#endif
	}

	// �t�@�C���̓��e��ǂݍ��ށB
	// �����Fpbuf �ǂݍ��ݐ�B
	//       len �ǂݍ��ރo�C�g���B
	// �Ԓl�F�ǂݍ��񂾃o�C�g���B
	UINT Read(void *pbuf_t, UINT len) {
#if PACKFILE_TYPE < 2
#if PACKFILE_TYPE == 0
		char buf[16], offset, *pbuf, *plen1, *plen2;
		UINT len8, tmp;
		HANDLE thread;
		DECRYPT_INFO di;
		CryptBlowfish *pcb = CryptBlowfish::getObject();
#endif
		UINT pos;

		if (m_pos > m_cache_start_pos && m_pos <= m_cache_end_pos) {
			// �������̂��߁AreadLine()�̃L���b�V�����g�p���Ă���ꍇ��fseek�͂����A
			// m_pos�������ω����Ă���̂Ŏ��ۂ̃t�@�C���ʒu������ɍ��킹��B
			if (fseek(m_pfile, m_pos + 10, SEEK_SET) != 0) return 0;
		}
#endif
#if PACKFILE_TYPE == 0
		//--------------------------------------------------
		// 8�o�C�g�u���b�N�Í��f�[�^���ǂ̈ʒu����ł�8�̔{���̈ʒu�Ɉړ����A���������ēǂݍ��߂鏈����
		// �O�u���b�N�A����f�[�^�A���ԃf�[�^�A�ŏI�f�[�^�̒i�K�ɕ����铙�A������x�̏������x��������}��B
		// �܂��ASIMD���g�p����(mm0,xmm0:�f�[�^�Amm1,xmm1:�O�u���b�N�Amm2,xmm2:�O�u���b�N�ꎞ�ۑ��p)�B
		//   �O�u���b�N�FCBC�Í��̂��߂̏���f�[�^�̑O��8�o�C�g
		//   ����f�[�^�F�ǂݍ��ރo�C�g���̍ŏ���8�o�C�g
		//   ���ԃf�[�^�F����ƍŏI�̊Ԃ�8�ȏ��8�̔{���̃o�C�g
		//   �ŏI�f�[�^�F�ǂݍ��ރo�C�g���̍Ō��8�o�C�g
		//--------------------------------------------------

		// �t�@�C����ǂݍ��߂�c��̃T�C�Y���A�w�肵���ǂݍ��ރo�C�g����菭�Ȃ��ꍇ�́A
		// �ǂݍ��ރo�C�g�����t�@�C����ǂݍ��߂�c��̃T�C�Y�ɐݒ肷��B
		// ����œǂݍ��݊J�n�ʒu���t�@�C���̍ŏI�ʒu�𒴂��Ȃ��悤�ɂł���B
		pos = m_end_pos - m_pos;
		if (pos < len) len = pos;
		if (len == 0) return 0;

		// �����ݒ�B
		pbuf = (char *)pbuf_t;
		pos = m_pos;
		offset = tmp = m_pos & 7;
		if (m_pos >= 8) tmp += 8;	// CBC�Í��Ȃ̂őO�u���b�N����ǂݍ��ނ悤�ɂ���B

		// �ǂݍ��݊J�n�ʒu��8�̔{���̈ʒu�ɐݒ肷��B
		if (fseek(m_pfile, -tmp, SEEK_CUR) != 0) return 0;

		// ���ԃf�[�^�����œǂݍ��ރo�C�g����8�̔{���ɂȂ�悤�ɐݒ肷��B
		len8 = len + offset;
		len8 -= 8 - ((8 - (len8 & 7)) & 7);
		if (len8 >= 8) len8 -= 8;	// �ŏI�f�[�^�����̓ǂݍ��݃o�C�g�����܂܂�Ă���̂ň����Ă����B


		//--- �O�u���b�N�����B

		if (m_pos < 8) {	// �t�@�C���̐擪�H
			// �擪�͑O�u���b�N�������̂ŁA����ɏ����x�N�g����ݒ肷��B
			CopyMemory(buf, pcb->m_vec, 8);
		} else {
			if (fread(buf, 4, 2, m_pfile) != 2) goto procend;
		}
		_asm movq mm1, buf


		//--- ����f�[�^�����B

		if (fread(buf, 8, 1, m_pfile) != 1) goto procend;

		_asm movq mm2, buf

		// �������B
		pcb->Decrypt((DWORD *)buf, (DWORD *)(buf + 4));
		_asm {
			movq mm0, buf
			pxor mm0, mm1
			movq buf, mm0

			movq mm1, mm2
		}

		// �ǂݍ��ݐ�ɐݒ肷��B
		tmp = 8 - offset;
		if (tmp > len) tmp = len;
		m_pos += tmp;
		CopyMemory(pbuf, buf + offset, tmp);
		pbuf += tmp;


		//--- ���ԃf�[�^�����B
		// �o�C�g������萔��葽���ꍇ�A�ʃX���b�h�ɂē�������������������(���񏈗�)�B�{�X���b�h�ł�
		// �f�[�^�̑O������(8�̔{���Ő؂�グ)�𕜍������A�ʃX���b�h�Ō㔼����(8�̔{���Ő؂�̂�)�𕜍�������B

		if (len8 > 0) {
			if (fread(pbuf, len8, 1, m_pfile) != 1) goto procend;

			m_pos += len8;
			plen1 = pbuf + len8;

			tmp = 16 - (len8 & 15);	// len8��8�ȏ��8�̔{���Ȃ̂ŁA���ʂ�8��16�̂ǂ��炩�̂͂��B
			len8 -= tmp;
			plen2 = pbuf + len8;
			if (pbuf < plen2) {	// �ǂݍ��ރo�C�g����16��葽�����H
				if (len > 8192) {
					// �ʃX���b�h�p�̃p�����[�^��ݒ�B
					di.plen = (DWORD *)plen2;
					len8 >>= 1;
					plen2 = pbuf + len8 + (len8 & 15);
					CopyMemory(di.vec, plen2 - 8, 16);
					di.pdata = (DWORD *)plen2;

					thread = CreateThread(NULL, 0, procDecrypt, &di, 0, NULL);
					if (thread == NULL) goto procend;
				}

				_asm movq buf, mm1
				CopyMemory(buf + 8, pbuf, 8);
				_asm movdqu xmm1, buf
				for (; pbuf < plen2; pbuf += 16) {
					_asm {
						mov edx, pbuf
						movdqu xmm2, [edx + 8]
					}

					// �������B
					pcb->Decrypt((DWORD *)pbuf, (DWORD *)(pbuf + 4));
					pcb->Decrypt((DWORD *)(pbuf + 8), (DWORD *)(pbuf + 12));
					_asm {
						mov edx, pbuf
						movdqu xmm0, [edx]
						pxor xmm0, xmm1
						movdqu [edx], xmm0

						movdqa xmm1, xmm2
					}
				}

				if (len > 8192) {
					WaitForSingleObject(thread, INFINITE);	// �ʃX���b�h�̏����I���܂ő҂B
					CloseHandle(thread);

					// �ʃX���b�h�ōŌ�ɏ������ꂽ�l�̂�������ݒ�B
					pbuf = (char *)di.pdata;
					CopyMemory(buf, di.vec, 8);
					_asm movq mm1, buf
				} else {
					_asm movdq2q mm1, xmm1
				}
			}

			// �c��8�o�C�g���̏����B
			for (; pbuf < plen1; pbuf += 8) {
				_asm {
					mov edx, pbuf
					movq mm2, [edx]
				}

				// �������B
				pcb->Decrypt((DWORD *)pbuf, (DWORD *)(pbuf + 4));
				_asm {
					mov edx, pbuf
					movq mm0, [edx]
					pxor mm0, mm1
					movq [edx], mm0

					movq mm1, mm2
				}
			}
		}


		//--- �ŏI�f�[�^�����B
		// �ǂݍ��ރo�C�g����8�����̏ꍇ�ł��Afread���̏��������s����邪�A�Ō��fseek�Ō��ɖ߂�A
		// �܂��ACopyMemory��tmp��0�̂��߁A�������Ȃ��B�������A�ǂݍ��ރo�C�g����8�ȏ�̏ꍇ�͓K�؂ɏ��������B

		if (fread(buf, 8, 1, m_pfile) != 1) goto procend;

		_asm movq mm2, buf

		// �������B
		pcb->Decrypt((DWORD *)buf, (DWORD *)(buf + 4));
		_asm {
			movq mm0, buf
			pxor mm0, mm1
			movq buf, mm0
		}

		// �ǂݍ��ݐ�ɐݒ肷��B
		tmp = len - (m_pos - pos);
		m_pos += tmp;
		CopyMemory(pbuf, buf, tmp);

procend:
		_asm emms
		fseek(m_pfile, m_pos + 10, SEEK_SET);
		return m_pos - pos;

#elif PACKFILE_TYPE == 1
		pos = m_end_pos - m_pos;
		if (pos < len) len = pos;
		pos = fread(pbuf_t, 1, len, m_pfile);
		m_pos += pos;
		return pos;
#else
		return fread(pbuf_t, 1, len, m_pfile);
#endif
	}

	// �t�@�C���̓��e��1�s�ǂݍ��ށB�܂�Alen�̒����܂œǂ񂾂��A�I�[�ɒB�������A
	// \0�������������A�܂��͉��s��������Ƃ����œǂݍ��݂��I���܂��B�܂��A�Ō��\0��������B
	// �����Fpbuf �ǂݍ��ݐ�B
	//       len �ǂݍ��ރo�C�g���B
	// �Ԓl�F�������ǂݍ��߂��Ȃ�true�A����ȊO��false�B
	bool readLine(LPSTR pbuf, UINT len) {
#if PACKFILE_TYPE < 2
		char tmp, *pcache, *pcache_len, *plen;
		bool cache_flag;
		UINT cache_size, pos;

		//--------------------------------------------------
		// �����������f�[�^���ė��p�ł���悤�ɃL���b�V�����A������x�̏������x��������}��B
		//--------------------------------------------------

		// ��1(���̏��������Ă��闝�R�ɂ��Ă�Read()���̃R�����g���Q�ƁB)
		pos = m_end_pos - m_pos;
		if (pos < len) len = pos;
		if (len == 0) return false;

		cache_size = len < m_cache_size ? len : m_cache_size;	// ����������f�[�^�̃L���b�V���T�C�Y��ݒ肷��B
		plen = pbuf + len - 1;	// ������\0��ǉ�����̂ŁA-1���Ă����B
		if (pbuf >= plen) return false;

		for (; pbuf < plen; pbuf += pos) {
			//--- ��1�Ɠ��l�̗��R�̏����B
			pos = plen - pbuf;
			if (pos < cache_size) cache_size = pos;

			//--- �L���b�V���ݒ�B
			cache_flag = m_pos >= m_cache_start_pos && m_pos < m_cache_end_pos;
			if (!cache_flag) {	// �ǂݍ��ވʒu���L���b�V���͈̔͊O�Ȃ̂ŁA�V�����L���b�V���ɂ���K�v�L��H
				// �L���b�V���Ƃ��̈ʒu����ݒ�B
				m_cache_start_pos = m_pos;
				if (m_pcache == NULL || Read(m_pcache, cache_size) != cache_size) return false;
				m_cache_end_pos = m_cache_start_pos + cache_size;

				pcache = m_pcache;
				pcache_len = m_pcache + cache_size;
				*pcache_len = '\0';
			} else {
				pcache = m_pcache + (m_pos - m_cache_start_pos);	// Read()���œǂݍ��݈ʒu���ς���Ă��A����ɍ��킹��B
				pcache_len = m_pcache + (m_cache_end_pos - m_cache_start_pos);
			}

			//--- �ǂݍ��ݐ�ɐݒ肷��B
			pos = strcspn(pcache, "\n\r");	// \n��\r�A�܂���\0�̈ʒu���擾�B
			CopyMemory(pbuf, pcache, pos);
			pcache += pos;

			//--- �L���b�V���̍Ō�ɒB�����ꍇ�̏����B
			if (pcache == pcache_len) {
				if (cache_flag) {
					if (fseek(m_pfile, m_cache_end_pos + 10, SEEK_SET) != 0) return false;
					m_pos += pos;	// ���̏�����Am_pos��m_cache_end_pos�Ɠ������Ȃ�͂��B
				}
				pbuf[pos] = '\0';
				continue;
			}

			//--- �ǂݍ��ݐ�̖�����\0��ǉ����Am_pos��ǂݍ��݂���������߂�����A�ǂݍ��񂾕����Z�����肷��B
			if (*pcache == '\n') {
				pbuf[pos] = '\n';
				pbuf[pos + 1] = '\0';
			} else if (*pcache == '\r') {
				pbuf[pos] = '\n';
				pbuf[pos + 1] = '\0';

				// \r�Ȃ̂ŁA���̕�����\n�Ȃ玟��̓ǂݍ��ވʒu�͂��̎��̕�������ƂȂ�悤�ɐݒ肷��B
				if (pcache < pcache_len - 1) {
					if (pcache[1] == '\n') {
						if (cache_flag) {
							m_pos += pos + 2;
							return true;
						}
						m_pos -= pcache_len - pcache - 2;
						return true;
					}
					// ������return�͂��Ȃ����Ƃɒ��ӁB
				} else {
					if (cache_flag) m_pos += pos + 1;
					// ���̎��_��m_pos�Ǝ��ۂ̃t�@�C���ʒu�͈�v���Ă���͂��B
					if (Read(&tmp, 1) == 1 && tmp != '\n') return Seek(-1, SEEK_CUR);
					return true;
				}
			} else {
				pbuf[pos] = '\0';
			}
			if (cache_flag) {
				m_pos += pos + 1;
				return true;
			}
			m_pos -= pcache_len - pcache - 1;
			return true;
		}

		return true;
#else
		return fgets(pbuf, len, m_pfile) != NULL;
#endif
	}

	// �t�@�C���̓ǂݍ��݈ʒu���ړ�����B
	// �����Foffset �ړ��I�t�Z�b�g�B
	//       offset_type �I�t�Z�b�g���(fseek�Ɠ���)�B
	// �Ԓl�F�����Ȃ�true�A����ȊO��false�B
	bool Seek(int offset, BYTE offset_type) {
#if PACKFILE_TYPE < 2
		UINT pos;

		// (���̏��������Ă��闝�R�ɂ��Ă�Read()���̃R�����g���Q�ƁB)
		if (m_pos > m_cache_start_pos && m_pos <= m_cache_end_pos) {
			if (fseek(m_pfile, m_pos + 10, SEEK_SET) != 0) return false;
		}

		switch (offset_type) {
		case SEEK_CUR:
			pos = m_pos + offset;
			if (pos < m_start_pos || pos > m_end_pos) return false;
			if (fseek(m_pfile, offset, SEEK_CUR) != 0) return false;

			m_pos += offset;
			break;
		case SEEK_END:
			pos = m_end_pos + offset;
			if (pos < m_start_pos || pos > m_end_pos) return false;
			if (fseek(m_pfile, pos + 10, SEEK_SET) != 0) return false;	// SEEK_END�ł͂Ȃ����Ƃɒ��ӁB

			m_pos = pos;
			break;
		default:
			pos = m_start_pos + offset;
			if (pos < m_start_pos || pos > m_end_pos) return false;
			if (fseek(m_pfile, pos + 10, SEEK_SET) != 0) return false;

			m_pos = pos;
			break;
		}
		return true;

#else
		return fseek(m_pfile, offset, offset_type) == 0;
#endif
	}

#if PACKFILE_TYPE == 0
	// ���������񏈗��p�B
	static DWORD WINAPI procDecrypt(LPVOID lpParameter) {
		DECRYPT_INFO *pdi = (DECRYPT_INFO *)lpParameter;
		DWORD *pdata = pdi->pdata, *plen = pdi->plen;
		CryptBlowfish *pcb = CryptBlowfish::getObject();

		_asm {
			mov edx, pdi
			movdqu xmm1, [edx]
		}
		for (; pdata < plen; pdata += 4) {
			_asm {
				mov edx, pdata
				movdqu xmm2, [edx + 8]
			}

			// �������B
			pcb->Decrypt(pdata, pdata + 1);
			pcb->Decrypt(pdata + 2, pdata + 3);
			_asm {
				mov edx, pdata
				movdqu xmm0, [edx]
				pxor xmm0, xmm1
				movdqu [edx], xmm0

				movdqa xmm1, xmm2
			}
		}

		_asm {
			// pdi->vec[0]�`pdi->vec[3]��xmm1�̒l��ݒ�B
			mov edx, pdi
			movdqu [edx], xmm1
		}
		pdi->pdata = pdata;
		return 0;
	}
#endif

protected:
	FILE *m_pfile;

#if PACKFILE_TYPE < 2
	UINT m_start_pos;	// �w�肵���t�@�C���̊J�n�ʒu�B
	UINT m_end_pos;	// �w�肵���t�@�C���̏I���ʒu�B
	UINT m_pos;	// ���݈ʒu(�t�@�C���̑��Έʒu�ł͂Ȃ��A�p�b�N�t�@�C���S�̂̈ʒu�B�܂�A��Έʒu)�B

	// readLine()�̃L���b�V���p�B
	UINT m_cache_size;	// �������ς݃f�[�^�̊i�[�T�C�Y�B
	char *m_pcache;	// �������ς݃f�[�^�i�[��B
	UINT m_cache_start_pos, m_cache_end_pos;	// �������ς݃f�[�^�̈ʒu�B

#if PACKFILE_TYPE == 0
	// �������p�̏��B
	struct DECRYPT_INFO {
		DWORD vec[4], *pdata, *plen;
	};
#endif
#endif
};
