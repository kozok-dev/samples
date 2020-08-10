#include <tchar.h>
#include <windows.h>
#include <commctrl.h>
#include <shlobj.h>
#define MAX_PATH2 MAX_PATH * 2

HWND hsta;

class DirList {
	HWND hwnd;
	typedef struct {
		LPSHELLFOLDER lpsf;
		LPITEMIDLIST lpiidl;
		LPITEMIDLIST lpiidldl;
	} DLDATA;

	// �A�C�e���ǉ�
	BOOL insertItem(LPSHELLFOLDER lpsf, LPITEMIDLIST lpiidl, HTREEITEM hParent, DLDATA *pdldata, LPMALLOC lpmalloc) {
		TVINSERTSTRUCT tvi;
		SFGAOF attr;
		STRRET strret;
		TCHAR fnbuf[MAX_PATH2];
		LPSTR filename;
		SHFILEINFO shfi;
		DLDATA *dldata;
		LPITEMIDLIST lptiidl;
		UINT t1 = 0, t2 = 0;

		// �擾����A�C�e���̑����^�C�v���擾
		attr = SFGAO_FOLDER | SFGAO_SHARE | SFGAO_HIDDEN | SFGAO_FILESYSTEM | SFGAO_HASSUBFOLDER;
		lpsf->GetAttributesOf(1, (LPCITEMIDLIST *)&lpiidl, &attr);

		// �t�H���_�łȂ����H
		if (!(attr & SFGAO_FOLDER)) return FALSE;

		// ���ڂ̎��
		tvi.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM;

		if (attr & SFGAO_HASSUBFOLDER) {	// �T�u�t�H���_�����邩�H
			tvi.item.cChildren = 1;
			tvi.item.mask |= TVIF_CHILDREN;
		}

		// �t�@�C�����擾
		if (lpsf->GetDisplayNameOf(lpiidl, SHGDN_NORMAL, &strret) != S_OK) return FALSE;
		switch (strret.uType) {
		case STRRET_CSTR:
			filename = strret.cStr;
			break;
		case STRRET_OFFSET:
			filename =(LPSTR)lpiidl + strret.uOffset;
			break;
		case STRRET_WSTR:
			WideCharToMultiByte(CP_ACP, 0, strret.pOleStr, -1, fnbuf, sizeof(fnbuf), NULL, NULL);
			filename = fnbuf;
			break;
		default:
			return FALSE;
		}

		// �A�C�e�����̈�m��
		if (!(dldata = (DLDATA *)lpmalloc->Alloc(sizeof(DLDATA)))) return FALSE;

		// �A�C�e�����ݒ�
		if (hParent == TVI_ROOT) {
			dldata->lpsf = lpsf;
		} else {
			if (FAILED(lpsf->BindToObject(lpiidl, NULL, IID_IShellFolder, (LPVOID *)&dldata->lpsf))) {
				lpmalloc->Free(dldata);
				return FALSE;
			}
		}
		dldata->lpiidl = lpiidl;

		// �A�C�e�����i�[
		tvi.item.lParam = (LPARAM)dldata;

		// TreeView�̍��ڃe�L�X�g�ݒ�
		tvi.item.pszText = filename;
		tvi.item.cchTextMax = MAX_PATH2;

		tvi.item.stateMask = tvi.item.state = 0;

		if (attr & SFGAO_SHARE) {	// ���L�t�H���_��?
			tvi.item.mask |= TVIF_STATE;
			tvi.item.stateMask |= TVIS_OVERLAYMASK;
			tvi.item.state |= INDEXTOOVERLAYMASK(1);
		}

		if (attr & SFGAO_HIDDEN) {	// �B���t�H���_��?
			tvi.item.mask |= TVIF_STATE;
			tvi.item.stateMask |= TVIS_CUT;
			tvi.item.state |= TVIS_CUT;
		}

		// �e����ItemIDList�̒������擾
		if (pdldata) {
			lptiidl = pdldata->lpiidldl;
			if (lptiidl) {
				while (lptiidl->mkid.cb > 0) {
					t1 += lptiidl->mkid.cb;
					lptiidl = (LPITEMIDLIST)((LPBYTE)lptiidl + lptiidl->mkid.cb);
				}
			}
		}
		// �q����ItemIDList�̒������擾
		lptiidl = lpiidl;
		if (lptiidl) {
			t2 += sizeof(lptiidl->mkid.cb);
			while (lptiidl->mkid.cb > 0) {
				t2 += lptiidl->mkid.cb;
				lptiidl = (LPITEMIDLIST)((LPBYTE)lptiidl + lptiidl->mkid.cb);
			}
		}
		// ItemIDList�Z�b�g
		dldata->lpiidldl = (LPITEMIDLIST)lpmalloc->Alloc(t1 + t2);
		if (dldata->lpiidldl) {
			if (pdldata && pdldata->lpiidldl) CopyMemory(dldata->lpiidldl, pdldata->lpiidldl, t1);
			CopyMemory((LPSTR)dldata->lpiidldl + t1, lpiidl, t2);
		}

		// TreeView�̍��ڃA�C�R���ݒ�
		SHGetFileInfo((LPCTSTR)dldata->lpiidldl, 0, &shfi, sizeof(SHFILEINFO), SHGFI_SYSICONINDEX | SHGFI_SMALLICON | SHGFI_PIDL);
		tvi.item.iImage = shfi.iIcon;
		SHGetFileInfo((LPCTSTR)dldata->lpiidldl, 0, &shfi, sizeof(SHFILEINFO), SHGFI_SYSICONINDEX | SHGFI_SMALLICON | SHGFI_OPENICON | SHGFI_PIDL);
		tvi.item.iSelectedImage = shfi.iIcon;

		// TreeView�ɒǉ�
		tvi.hParent = hParent;
		tvi.hInsertAfter = TVI_LAST;
		TreeView_InsertItem(this->hwnd, &tvi);

		return TRUE;
	}

	// ���בւ���r�֐�
	friend int CALLBACK CompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort) {
		HRESULT hr = ((LPSHELLFOLDER)lParamSort)->CompareIDs(0,((DLDATA *)lParam1)->lpiidl,((DLDATA *)lParam2)->lpiidl);
		return SUCCEEDED(hr) ? (short)HRESULT_CODE(hr) : 0;
	}

public:
	TCHAR path[MAX_PATH2];

	// �쐬
	void Create(HWND hwnd, HINSTANCE hInstance, int x, int y, int width, int height) {
		LPMALLOC lpmalloc;
		LPSHELLFOLDER lpsf;
		LPITEMIDLIST lpiidl;
		SHFILEINFO shfi;

		// TreeView�쐬
		if ((this->hwnd = CreateWindowEx(
			WS_EX_CLIENTEDGE,
			WC_TREEVIEW, NULL,
			WS_CHILD | WS_VISIBLE | TVS_HASBUTTONS | TVS_HASLINES,
			x, y, width, height,
			hwnd, NULL, hInstance, NULL
		)) == NULL) return;

		// �V�X�e���C���[�W���X�g�ݒ�
		TreeView_SetImageList(this->hwnd, (HIMAGELIST)SHGetFileInfo(NULL, 0, &shfi, sizeof(SHFILEINFO), SHGFI_SYSICONINDEX | SHGFI_SMALLICON), TVSIL_NORMAL);

		// Desktop���擾
		SHGetDesktopFolder(&lpsf);
		SHGetSpecialFolderLocation(GetAncestor(hwnd, GA_ROOTOWNER), CSIDL_DESKTOP, &lpiidl);

		if (FAILED(CoGetMalloc(1, &lpmalloc))) return;
		insertItem(lpsf, lpiidl, TVI_ROOT, NULL, lpmalloc);
		lpmalloc->Free(lpiidl);
		lpmalloc->Release();

		lpsf->Release();
	}

	// ���b�Z�[�W����
	void Notify(HWND hwnd, WPARAM wParam, LPARAM lParam) {
		DLDATA *dldata;
		LPMALLOC lpmalloc;
		LPITEMIDLIST lpiidl;
		LPENUMIDLIST lpeidl;
		TVITEM tvi;
		TVSORTCB tvscb;
		BOOL subfolder = FALSE;

		if (((LPNMHDR)lParam)->hwndFrom != this->hwnd) return;

		switch (((LPNMTREEVIEW)lParam)->hdr.code) {
		case TVN_SELCHANGED:	// �I����
			dldata = (DLDATA *)(((LPNMTREEVIEW)lParam)->itemNew.lParam);

			SHGetPathFromIDList(dldata->lpiidldl, path);
			SetWindowText(hsta, path);
			break;
		case TVN_ITEMEXPANDING:	// �W�J��
			if (((LPNMTREEVIEW)lParam)->itemNew.state & TVIS_EXPANDEDONCE) break;
			if (FAILED(CoGetMalloc(1, &lpmalloc))) break;

			dldata = (DLDATA *)(((LPNMTREEVIEW)lParam)->itemNew.lParam);

			// �}�E�X�|�C���^��҂���Ԃɂ���
			SetCapture(GetAncestor(hwnd, GA_ROOTOWNER));
			SetCursor(LoadCursor(NULL, IDC_WAIT));

			// �t�@�C����
			if (SUCCEEDED(dldata->lpsf->EnumObjects(GetAncestor(hwnd, GA_ROOTOWNER), SHCONTF_FOLDERS | SHCONTF_NONFOLDERS | SHCONTF_INCLUDEHIDDEN, &lpeidl))) {
				while (lpeidl->Next(1, &lpiidl, NULL) == NOERROR) {
					if (insertItem(dldata->lpsf, lpiidl, ((LPNMTREEVIEW)lParam)->itemNew.hItem, dldata, lpmalloc)) subfolder = TRUE;
				}
				if (!subfolder) {
					// �T�u�t�H���_���Ȃ���Έ������
					tvi.hItem = ((LPNMTREEVIEW)lParam)->itemNew.hItem;
					tvi.mask = TVIF_CHILDREN;
					tvi.cChildren = 0;
					TreeView_SetItem(this->hwnd, &tvi);
				}
				lpeidl->Release();

				// ���ڂ���בւ���
				tvscb.hParent = ((LPNMTREEVIEW)lParam)->itemNew.hItem;
				tvscb.lpfnCompare = CompareFunc;
				tvscb.lParam = (LPARAM)dldata->lpsf;
				TreeView_SortChildrenCB(this->hwnd, &tvscb, 0);
			}

			// �}�E�X�|�C���^��߂�
			ReleaseCapture();
			SetCursor(LoadCursor(NULL, IDC_ARROW));

			lpmalloc->Release();

			break;
		case TVN_DELETEITEM:	// �폜��
			if (FAILED(CoGetMalloc(1, &lpmalloc))) break;

			dldata = (DLDATA *)(((LPNMTREEVIEW)lParam)->itemOld.lParam);
			dldata->lpsf->Release();
			lpmalloc->Free(dldata->lpiidl);
			lpmalloc->Free(dldata->lpiidldl);
			lpmalloc->Free(dldata);

			lpmalloc->Release();

			break;
		}
	}

	// �j��
	void Destroy() {
		DestroyWindow(hwnd);
	}
};

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static DirList dirlist;

	switch (uMsg) {
	case WM_CREATE:
		dirlist.Create(hwnd, ((LPCREATESTRUCT)lParam)->hInstance, 0, 0, 250, 350);
		return 0;
	case WM_NOTIFY:
		dirlist.Notify(hwnd, wParam, lParam);
		return 0;
	case WM_DESTROY:
		dirlist.Destroy();
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	HWND hwnd;
	HANDLE hmutex;
	MSG msg;
	WNDCLASS wc;
	HFONT hfont;

	// 2�d�N���h�~
	hmutex = CreateMutex(NULL, FALSE, TEXT("dirlist"));
	if (GetLastError() == ERROR_ALREADY_EXISTS) {
		hwnd = FindWindow(TEXT("dirlist"), TEXT("DirList"));
		SetForegroundWindow(hwnd);
		ShowWindow(hwnd, SW_SHOWNORMAL);
		CloseHandle(hmutex);
		return 0;
	}

	// �E�B���h�E�N���X�o�^
	wc.style         = CS_DBLCLKS;
	wc.lpfnWndProc   = WindowProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = hInstance;
	wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = TEXT("dirlist");
	if (!RegisterClass(&wc)) return 0;

	if ((hwnd = CreateWindow(
		TEXT("dirlist"), TEXT("DirList"),
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		200, 200, 350, 500,
		NULL, NULL, hInstance, NULL
	)) == NULL) return 0;

	if ((hsta = CreateWindow(
		TEXT("static"), TEXT(""),
		WS_CHILD | WS_VISIBLE,
		0, 360, 300, 100,
		hwnd, NULL, hInstance, NULL
	)) == NULL) return 0;
	hfont = CreateFont(
		12, 0, 0, 0, FW_NORMAL,
		FALSE, FALSE, FALSE,
		SHIFTJIS_CHARSET,
		OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH, TEXT("�l�r �o�S�V�b�N")
	);
	SendMessage(hsta, WM_SETFONT, (WPARAM)hfont, 0);

	InitCommonControls();	// �R�����R���g���[��������
	CoInitialize(NULL);	// COM������

	// ���b�Z�[�W���[�v
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	DeleteObject(hfont);
	CloseHandle(hmutex);
	CoUninitialize();
	return msg.wParam;
}
