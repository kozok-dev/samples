#pragma once
#include <stdio.h>
#include <io.h>
#include "cryptblowfish.cpp"
#define PACKFILE "warun.pak"
#define PACKFILE_TYPE 0	// 0:暗号化済みパックファイル、1:生パックファイル、2:個々のファイル

// パックファイル読み込みクラス。
class PackFile {
public:
	// コンストラクタ。
	// 引数：cache_size 復号化済みデータのキャッシュサイズ。
	PackFile(UINT cache_size) {
#if PACKFILE_TYPE < 2
		m_pfile = fopen(PACKFILE, "rb");	// 暗号化された8の倍数のサイズのパックファイル。

		m_start_pos = m_pos = 0;
		m_end_pos = m_pfile == NULL ? 0 : _filelength(_fileno(m_pfile));

		m_cache_size = cache_size < 256 ? 256 : cache_size;
		m_pcache = new char[m_cache_size + 1];	// 末尾に\0を追加するので、+1しておく。
		m_cache_start_pos = m_cache_end_pos = 0;
#else
		m_pfile = NULL;
#endif
	}

	// デストラクタ。
	~PackFile() {
#if PACKFILE_TYPE < 2
		if (m_pcache != NULL) delete[] m_pcache;
#endif
		if (m_pfile != NULL) fclose(m_pfile);
	}

	// ファイルを開く。
	// 引数：pfliename ファイル名。
	//       pmode fopen形式のファイルモード。
	// 返値：成功ならtrue、それ以外はfalse。
	bool Open(LPCSTR pfliename, LPCSTR pmode = "rb") {
#if PACKFILE_TYPE < 2
		char buf[256];

		if (m_pfile == NULL || m_pcache == NULL) return false;
		if (fseek(m_pfile, 0, SEEK_SET) != 0) return false;
		m_start_pos = m_pos = 0;
		m_end_pos = _filelength(_fileno(m_pfile));
		m_cache_start_pos = m_cache_end_pos = 0;

		// シグニチャ確認(今後、fseekでSEEK_CURでない場合は、シグニチャのサイズを考慮に入れるようにする)。
		if (fread(buf, 10, 1, m_pfile) != 1) return false;
		if (memcmp(buf, "WARUNPACK\0", 10) != 0) return false;

		// ヘッダからファイル情報を検索。
		for (;;) {
			if (!readLine(buf, sizeof(buf))) return false;
			if (buf[0] == '\0') return false;	// 最後まで見つからなかった。
			if (strcmp(buf, pfliename) != 0) {
				if (!Seek(8, SEEK_CUR)) return false;	// 不要情報を飛ばす。
				continue;
			}

			if (Read(&m_start_pos, 4) != 4) return false;	// ファイルの位置。
			if (Read(&m_end_pos, 4) != 4) {	// ファイルのサイズ。
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

	// ファイルの内容を読み込む。
	// 引数：pbuf 読み込み先。
	//       len 読み込むバイト数。
	// 返値：読み込んだバイト数。
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
			// 高速化のため、readLine()のキャッシュを使用している場合はfseekはせず、
			// m_posだけが変化しているので実際のファイル位置もそれに合わせる。
			if (fseek(m_pfile, m_pos + 10, SEEK_SET) != 0) return 0;
		}
#endif
#if PACKFILE_TYPE == 0
		//--------------------------------------------------
		// 8バイトブロック暗号データをどの位置からでも8の倍数の位置に移動し、復号化して読み込める処理を
		// 前ブロック、初回データ、中間データ、最終データの段階に分ける等、ある程度の処理速度高速化を図る。
		// また、SIMDも使用する(mm0,xmm0:データ、mm1,xmm1:前ブロック、mm2,xmm2:前ブロック一時保存用)。
		//   前ブロック：CBC暗号のための初回データの前の8バイト
		//   初回データ：読み込むバイト数の最初の8バイト
		//   中間データ：初回と最終の間の8以上の8の倍数のバイト
		//   最終データ：読み込むバイト数の最後の8バイト
		//--------------------------------------------------

		// ファイルを読み込める残りのサイズが、指定した読み込むバイト数より少ない場合は、
		// 読み込むバイト数をファイルを読み込める残りのサイズに設定する。
		// これで読み込み開始位置がファイルの最終位置を超えないようにできる。
		pos = m_end_pos - m_pos;
		if (pos < len) len = pos;
		if (len == 0) return 0;

		// 初期設定。
		pbuf = (char *)pbuf_t;
		pos = m_pos;
		offset = tmp = m_pos & 7;
		if (m_pos >= 8) tmp += 8;	// CBC暗号なので前ブロックから読み込むようにする。

		// 読み込み開始位置を8の倍数の位置に設定する。
		if (fseek(m_pfile, -tmp, SEEK_CUR) != 0) return 0;

		// 中間データ処理で読み込むバイト数を8の倍数になるように設定する。
		len8 = len + offset;
		len8 -= 8 - ((8 - (len8 & 7)) & 7);
		if (len8 >= 8) len8 -= 8;	// 最終データ処理の読み込みバイト数が含まれているので引いておく。


		//--- 前ブロック処理。

		if (m_pos < 8) {	// ファイルの先頭？
			// 先頭は前ブロックが無いので、代わりに初期ベクトルを設定する。
			CopyMemory(buf, pcb->m_vec, 8);
		} else {
			if (fread(buf, 4, 2, m_pfile) != 2) goto procend;
		}
		_asm movq mm1, buf


		//--- 初回データ処理。

		if (fread(buf, 8, 1, m_pfile) != 1) goto procend;

		_asm movq mm2, buf

		// 復号化。
		pcb->Decrypt((DWORD *)buf, (DWORD *)(buf + 4));
		_asm {
			movq mm0, buf
			pxor mm0, mm1
			movq buf, mm0

			movq mm1, mm2
		}

		// 読み込み先に設定する。
		tmp = 8 - offset;
		if (tmp > len) tmp = len;
		m_pos += tmp;
		CopyMemory(pbuf, buf + offset, tmp);
		pbuf += tmp;


		//--- 中間データ処理。
		// バイト数が一定数より多い場合、別スレッドにて同じ復号化処理をする(並列処理)。本スレッドでは
		// データの前半部分(8の倍数で切り上げ)を復号化し、別スレッドで後半部分(8の倍数で切り捨て)を復号化する。

		if (len8 > 0) {
			if (fread(pbuf, len8, 1, m_pfile) != 1) goto procend;

			m_pos += len8;
			plen1 = pbuf + len8;

			tmp = 16 - (len8 & 15);	// len8は8以上の8の倍数なので、結果は8か16のどちらかのはず。
			len8 -= tmp;
			plen2 = pbuf + len8;
			if (pbuf < plen2) {	// 読み込むバイト数が16より多いか？
				if (len > 8192) {
					// 別スレッド用のパラメータを設定。
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

					// 復号化。
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
					WaitForSingleObject(thread, INFINITE);	// 別スレッドの処理終了まで待つ。
					CloseHandle(thread);

					// 別スレッドで最後に処理された値のいくつかを設定。
					pbuf = (char *)di.pdata;
					CopyMemory(buf, di.vec, 8);
					_asm movq mm1, buf
				} else {
					_asm movdq2q mm1, xmm1
				}
			}

			// 残り8バイト分の処理。
			for (; pbuf < plen1; pbuf += 8) {
				_asm {
					mov edx, pbuf
					movq mm2, [edx]
				}

				// 復号化。
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


		//--- 最終データ処理。
		// 読み込むバイト数が8未満の場合でも、fread等の処理が実行されるが、最後のfseekで元に戻り、
		// また、CopyMemoryもtmpが0のため、何もしない。もちろん、読み込むバイト数が8以上の場合は適切に処理される。

		if (fread(buf, 8, 1, m_pfile) != 1) goto procend;

		_asm movq mm2, buf

		// 復号化。
		pcb->Decrypt((DWORD *)buf, (DWORD *)(buf + 4));
		_asm {
			movq mm0, buf
			pxor mm0, mm1
			movq buf, mm0
		}

		// 読み込み先に設定する。
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

	// ファイルの内容を1行読み込む。つまり、lenの長さまで読んだか、終端に達したか、
	// \0が見つかったか、または改行が見つかるとそこで読み込みを終えます。また、最後に\0を加える。
	// 引数：pbuf 読み込み先。
	//       len 読み込むバイト数。
	// 返値：正しく読み込めたならtrue、それ以外はfalse。
	bool readLine(LPSTR pbuf, UINT len) {
#if PACKFILE_TYPE < 2
		char tmp, *pcache, *pcache_len, *plen;
		bool cache_flag;
		UINT cache_size, pos;

		//--------------------------------------------------
		// 復号化したデータを再利用できるようにキャッシュし、ある程度の処理速度高速化を図る。
		//--------------------------------------------------

		// ※1(この処理をしている理由についてはRead()内のコメントを参照。)
		pos = m_end_pos - m_pos;
		if (pos < len) len = pos;
		if (len == 0) return false;

		cache_size = len < m_cache_size ? len : m_cache_size;	// 復号化するデータのキャッシュサイズを設定する。
		plen = pbuf + len - 1;	// 末尾に\0を追加するので、-1しておく。
		if (pbuf >= plen) return false;

		for (; pbuf < plen; pbuf += pos) {
			//--- ※1と同様の理由の処理。
			pos = plen - pbuf;
			if (pos < cache_size) cache_size = pos;

			//--- キャッシュ設定。
			cache_flag = m_pos >= m_cache_start_pos && m_pos < m_cache_end_pos;
			if (!cache_flag) {	// 読み込む位置がキャッシュの範囲外なので、新しいキャッシュにする必要有り？
				// キャッシュとその位置情報を設定。
				m_cache_start_pos = m_pos;
				if (m_pcache == NULL || Read(m_pcache, cache_size) != cache_size) return false;
				m_cache_end_pos = m_cache_start_pos + cache_size;

				pcache = m_pcache;
				pcache_len = m_pcache + cache_size;
				*pcache_len = '\0';
			} else {
				pcache = m_pcache + (m_pos - m_cache_start_pos);	// Read()等で読み込み位置が変わっても、それに合わせる。
				pcache_len = m_pcache + (m_cache_end_pos - m_cache_start_pos);
			}

			//--- 読み込み先に設定する。
			pos = strcspn(pcache, "\n\r");	// \nか\r、または\0の位置を取得。
			CopyMemory(pbuf, pcache, pos);
			pcache += pos;

			//--- キャッシュの最後に達した場合の処理。
			if (pcache == pcache_len) {
				if (cache_flag) {
					if (fseek(m_pfile, m_cache_end_pos + 10, SEEK_SET) != 0) return false;
					m_pos += pos;	// この処理後、m_posはm_cache_end_posと等しくなるはず。
				}
				pbuf[pos] = '\0';
				continue;
			}

			//--- 読み込み先の末尾に\0を追加し、m_posを読み込みすぎた分を戻したり、読み込んだ分加算したりする。
			if (*pcache == '\n') {
				pbuf[pos] = '\n';
				pbuf[pos + 1] = '\0';
			} else if (*pcache == '\r') {
				pbuf[pos] = '\n';
				pbuf[pos + 1] = '\0';

				// \rなので、次の文字が\nなら次回の読み込む位置はその次の文字からとなるように設定する。
				if (pcache < pcache_len - 1) {
					if (pcache[1] == '\n') {
						if (cache_flag) {
							m_pos += pos + 2;
							return true;
						}
						m_pos -= pcache_len - pcache - 2;
						return true;
					}
					// ここでreturnはしないことに注意。
				} else {
					if (cache_flag) m_pos += pos + 1;
					// この時点のm_posと実際のファイル位置は一致しているはず。
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

	// ファイルの読み込み位置を移動する。
	// 引数：offset 移動オフセット。
	//       offset_type オフセット種別(fseekと同じ)。
	// 返値：成功ならtrue、それ以外はfalse。
	bool Seek(int offset, BYTE offset_type) {
#if PACKFILE_TYPE < 2
		UINT pos;

		// (この処理をしている理由についてはRead()内のコメントを参照。)
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
			if (fseek(m_pfile, pos + 10, SEEK_SET) != 0) return false;	// SEEK_ENDではないことに注意。

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
	// 復号化並列処理用。
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

			// 復号化。
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
			// pdi->vec[0]～pdi->vec[3]にxmm1の値を設定。
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
	UINT m_start_pos;	// 指定したファイルの開始位置。
	UINT m_end_pos;	// 指定したファイルの終了位置。
	UINT m_pos;	// 現在位置(ファイルの相対位置ではなく、パックファイル全体の位置。つまり、絶対位置)。

	// readLine()のキャッシュ用。
	UINT m_cache_size;	// 復号化済みデータの格納サイズ。
	char *m_pcache;	// 復号化済みデータ格納先。
	UINT m_cache_start_pos, m_cache_end_pos;	// 復号化済みデータの位置。

#if PACKFILE_TYPE == 0
	// 復号化用の情報。
	struct DECRYPT_INFO {
		DWORD vec[4], *pdata, *plen;
	};
#endif
#endif
};
