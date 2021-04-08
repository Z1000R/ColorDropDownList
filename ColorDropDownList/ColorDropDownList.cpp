#include <Windows.h>
#include <commctrl.h>
#include "resource.h"

#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif

#pragma comment(lib, "comctl32.lib")

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
			LRESULT lResult = SendMessage(hCombo, (UINT)CB_GETCURSEL, 0, 0);
			wchar_t szColor[16];
			// DropDownList ����ł͂Ȃ��A�J���[�z�񂩂�ݒ�l���擾����
			wsprintf(szColor, L"%02X%02X%02X",
				GetRValue(rgbColoList[lResult]),
				GetGValue(rgbColoList[lResult]),
				GetBValue(rgbColoList[lResult]));
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

		for (int i = 0; i < sizeof(rgbColoList) / sizeof(rgbColoList[0]); ++i)
		{
			// �F���ڂ̒ǉ�
			// COLORREF�l�́ArgbColoList�ŊǗ�����̂ŁA�����ł͋󕶎����ݒ肵�A
			// �F�`�掞�ɃJ���[�R�[�h����������
			SendMessageW(hCombo, CB_ADDSTRING, 0, (LPARAM)L"");
		}
		// �����I������
		// wParam
		//  Specifies the zero-based index of the string to select.
		//  If this parameter is -1, any current selection in the list is removed and the edit control is cleared.
		// lParam
		//  This parameter is not used.
		SendMessage(hCombo, CB_SETCURSEL, 0, 0);

		return (INT_PTR)TRUE;
	}
	case WM_DRAWITEM:
	{
		LPDRAWITEMSTRUCT lpDis = (LPDRAWITEMSTRUCT)lParam;

		if (lpDis->CtlID != IDC_COMBO1)
			return (INT_PTR)FALSE;

		// �w�i�h��ׂ�
		DWORD dwBackColor = GetSysColor(COLOR_WINDOW);
		HBRUSH hbrBack = CreateSolidBrush(dwBackColor);
		FillRect(lpDis->hDC, &lpDis->rcItem, hbrBack);
		DeleteObject(hbrBack);

		// ���ڂ��ݒ肳��Ă��Ȃ���΁A�����𔲂���
		if (lpDis->itemID == -1)
			return (INT_PTR)TRUE;

		// �F�h��ׂ��̈�ݒ�
		// �E���ɃJ���[�R�[�h��\�������邽�߂̗]�������
		RECT rcColor{ lpDis->rcItem };
		rcColor.top += 2;
		rcColor.bottom -= 2;
		rcColor.left += 2;
		rcColor.right = (rcColor.bottom - rcColor.top) * 3;
		HBRUSH hbrColor = CreateSolidBrush(rgbColoList[lpDis->itemID]);
		FillRect(lpDis->hDC, &rcColor, hbrColor);
		DeleteObject(hbrColor);

		// �t�H�[�J�X����
		// ODS_SELECTED    0x0001
		// ODS_FOCUS       0x0010
		UINT uiState = ODS_FOCUS | ODS_SELECTED;

		// �h���b�v�_�E�������
		//	ODS_SELECTED	��ON
		//	ODS_FOCUS		��OFF
		//	�ƂȂ��Ă��邽�߁A�����ꂩ�̃t���O��ON�ł���΁A�t�H�[�J�X�\��������
		DWORD dwTextColor;
		if ((lpDis->itemState & uiState))
		{
			// �h��ׂ����F�̕���������āA�E���݂̂��t�H�[�J�X�\���i���]�j�����邽�߁Aleft ���I�t�Z�b�g
			RECT rcFocus{ lpDis->rcItem };
			rcFocus.left = (lpDis->rcItem.bottom - lpDis->rcItem.top) * 3 - 5;
			HBRUSH hbrFocus = GetSysColorBrush(COLOR_HIGHLIGHT);
			FillRect(lpDis->hDC, &rcFocus, hbrFocus);

			dwTextColor = GetSysColor(COLOR_HIGHLIGHTTEXT);
		}
		else
			dwTextColor = GetSysColor(COLOR_WINDOWTEXT);

		// �J���[�R�[�h
		wchar_t szColor[16];
		wsprintf(szColor, L"%02X%02X%02X",
			GetRValue(rgbColoList[lpDis->itemID]),
			GetGValue(rgbColoList[lpDis->itemID]),
			GetBValue(rgbColoList[lpDis->itemID]));
		SetTextColor(lpDis->hDC, dwTextColor);
		SetBkMode(lpDis->hDC, TRANSPARENT);
		// �h��ׂ����F�̉E���ɕ\�����邽�߁Aleft ���I�t�Z�b�g�����ʒu�ɏ�������
		RECT rcText{ lpDis->rcItem };
		rcText.left = (lpDis->rcItem.bottom - lpDis->rcItem.top) * 3;
		DrawText(lpDis->hDC, szColor, -1, &rcText, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
		return (INT_PTR)TRUE;
	}
	default:
		break;
	}

	return (INT_PTR)FALSE;
}
