#include "framework.h"
#include "resource.h"

using namespace std;

#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif

// �ݒ肷��F
COLORREF rgbColoList[16] =
{
	0xFFFFFF,
	0xC0C0C0,
	0x808080,
	0x000000,
	0x0000FF,
	0x000080,
	0x00FFFF,
	0x008080,
	0x00FF00,
	0x008000,
	0xFFFF00,
	0x808000,
	0xFF0000,
	0x800000,
	0xFF00FF,
	0x800080
};

INT_PTR CALLBACK DialogProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), nullptr, DialogProc);

	return 0;
}

INT_PTR CALLBACK DialogProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);

	static HWND hCombo;

	switch (msg)
	{
	case WM_COMMAND:
	{
		switch (LOWORD(wParam))
		{
		case IDC_GETCOLOR:
		{
			// �I�𒆂̃C���f�b�N�X�擾
			LRESULT index = SendMessageW(hCombo, CB_GETCURSEL, 0, 0);
			wchar_t szColor[16];
			// �I�𒆂̍��ڂ̃e�L�X�g���擾
			SendMessageW(hCombo, CB_GETLBTEXT, index, (LPARAM)szColor);
			MessageBox(hDlg, szColor, L"Color", MB_OK | MB_ICONINFORMATION);
			return (INT_PTR)TRUE;
		}
		case IDCANCEL:
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	case WM_INITDIALOG:
	{
		INITCOMMONCONTROLSEX ic;
		ic.dwICC = ICC_COOL_CLASSES;
		ic.dwSize = sizeof(INITCOMMONCONTROLSEX);
		InitCommonControlsEx(&ic);

		hCombo = GetDlgItem(hDlg, IDC_COMBO1);

		// �J���[�R�[�h�ǉ�
		for (const auto& c : rgbColoList)
		{
			wstring wsColor = format(L"{:02X}{:02X}{:02X}", GetRValue(c), GetGValue(c), GetBValue(c));
			SendMessageW(hCombo, CB_ADDSTRING, 0, (LPARAM)wsColor.c_str());
		}

		// �����I������
		// wParam
		//  Specifies the zero-based index of the string to select.
		//  If this parameter is -1, any current selection in the list is removed and the edit control is cleared.
		// lParam
		//  This parameter is not used.
		SendMessageW(hCombo, CB_SETCURSEL, 0, 0);

		return (INT_PTR)TRUE;
	}
	case WM_DRAWITEM:
	{
		LPDRAWITEMSTRUCT lpDis = (LPDRAWITEMSTRUCT)lParam;

		if (lpDis->CtlID != IDC_COMBO1)
			return (INT_PTR)FALSE;

		// ���ڂ��ݒ肳��Ă��Ȃ���΁A�����𔲂���
		if (lpDis->itemID == -1)
			return (INT_PTR)TRUE;

		// �J���[�R�[�h�̕\���I�t�Z�b�g
		LONG lOffsetText = (lpDis->rcItem.bottom - lpDis->rcItem.top) * 3;

		// �F�h��ׂ��̈�ݒ�
		// �E���ɃJ���[�R�[�h��\�������邽�߂̗]�������
		RECT rcFill{ lpDis->rcItem };
		rcFill.top += 2;
		rcFill.bottom -= 2;
		rcFill.left += 2;
		rcFill.right = lOffsetText - 2;
		HBRUSH hbrColor = CreateSolidBrush(rgbColoList[lpDis->itemID]);
		FillRect(lpDis->hDC, &rcFill, hbrColor);
		DeleteObject(hbrColor);

		// �J���[�R�[�h�擾
		wchar_t wcColorCode[32]{};
		size_t cch = SendMessageW(lpDis->hwndItem, CB_GETLBTEXT, lpDis->itemID, (LPARAM)wcColorCode);
		if (cch == 0 || cch == CB_ERR)
			wcColorCode[0] = L'\0';

		// �I����Ԃɍ��킹�ĕ\������F��ݒ�
		COLORREF crForegroundPrev;
		COLORREF crBackgroundPrev;
		if (lpDis->itemState & ODS_SELECTED)
		{
			crForegroundPrev = SetTextColor(lpDis->hDC, GetSysColor(COLOR_HIGHLIGHTTEXT));
			crBackgroundPrev = SetBkColor(lpDis->hDC, GetSysColor(COLOR_HIGHLIGHT));
		}
		else
		{
			crForegroundPrev = SetTextColor(lpDis->hDC, GetSysColor(COLOR_WINDOWTEXT));
			crBackgroundPrev = SetBkColor(lpDis->hDC, GetSysColor(COLOR_WINDOW));
		}

		// �J���[�R�[�h��\��������W�����߂�
		TEXTMETRIC tm;
		GetTextMetrics(lpDis->hDC, &tm);
		int x = lOffsetText + 5;
		int y = (lpDis->rcItem.bottom + lpDis->rcItem.top - tm.tmHeight) / 2;

		// ETO_CLIPPED�AETO_OPAQUE���K�p�����̈�
		//   �I�����̔w�i�F���A�\������Ă���F�����Əd�Ȃ�Ȃ��悤�ɁA
		//   left ���I�t�Z�b�g������
		RECT rcText{ lpDis->rcItem };
		rcText.left = lOffsetText;

		// ���ݑI������Ă���t�H���g�A�w�i�F�A����уe�L�X�g�̐F���g�p���ăe�L�X�g��`�悵�܂��B
		// ETO_CLIPPED : �e�L�X�g���l�p�`�ɃN���b�v����܂��B
		// ETO_OPAQUE  : ���݂̔w�i�F���g�p���Ďl�p�`��h��Ԃ��K�v������܂��B
		ExtTextOutW(lpDis->hDC,
			x, y,
			ETO_CLIPPED | ETO_OPAQUE,
			&rcText,
			wcColorCode, (UINT)cch, nullptr);

		// �ݒ肵���F�����ɖ߂�
		SetTextColor(lpDis->hDC, crForegroundPrev);
		SetBkColor(lpDis->hDC, crBackgroundPrev);

		// �t�H�[�J�X������΁AFocusRect ��\������
		if (lpDis->itemState & ODS_FOCUS)
			DrawFocusRect(lpDis->hDC, &rcText);

		return (INT_PTR)TRUE;
	}
	default:
		break;
	}

	return (INT_PTR)FALSE;
}
