// WaruNパックファイル展開ツール

#include <direct.h>
#include <conio.h>
#include "packfile.cpp"

#if PACKFILE_TYPE < 2
class PackFileExtract : public PackFile {
public:
	PackFileExtract() : PackFile(0) {
	}

	bool Extract() {
		char buf[10240], *tmp;
		FILE *f;
		UINT m_pos_tmp, r;

		if (m_pfile == NULL || m_pcache == NULL) return false;
		if (fseek(m_pfile, 0, SEEK_SET) != 0) return false;
		if (fread(buf, 10, 1, m_pfile) != 1) return false;
		if (memcmp(buf, "WARUNPACK\0", 10) != 0) return false;

		m_pos_tmp = m_cache_start_pos = m_cache_end_pos = 0;
		for (;;) {
			m_start_pos = 0;
			m_end_pos = _filelength(_fileno(m_pfile));
			if (!Seek(m_pos_tmp, SEEK_SET)) return false;

			if (!readLine(buf, sizeof(buf))) return false;
			if (buf[0] == '\0') return true;

			if (Read(&m_start_pos, 4) != 4) return false;
			if (Read(&m_end_pos, 4) != 4) return false;
			m_pos_tmp = m_pos;
			printf("%s %u %u\n", buf, m_start_pos, m_end_pos);

			m_pos = m_start_pos;
			m_end_pos += m_start_pos;
			if (!Seek(0, SEEK_SET)) return false;

			tmp = strchr(buf, '/');
			if (tmp != NULL) {
				*tmp = '\0';
				mkdir(buf);
				*tmp = '/';
			}

			f = fopen(buf, "wb");
			if (f == NULL) return false;
			for (;;) {
				r = Read(buf, sizeof(buf));
				if (r == 0) break;

				if (fwrite(buf, r, 1, f) != 1) {
					fclose(f);
					return false;
				}
			}
			fclose(f);
		}
		return true;
	}
};
#endif

void main()
{
#if PACKFILE_TYPE < 2
	PackFileExtract pfe;
	if (!pfe.Extract()) {
		printf("error\n");
		getch();
	}
#endif
}
