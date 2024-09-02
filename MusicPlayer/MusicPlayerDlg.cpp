
// MusicPlayerDlg.cpp: файл реализации
//

#include "pch.h"
#include "framework.h"
#include "MusicPlayer.h"
#include "MusicPlayerDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// Диалоговое окно CAboutDlg используется для описания сведений о приложении

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Данные диалогового окна
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // поддержка DDX/DDV

// Реализация
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// Диалоговое окно CMusicPlayerDlg


void CMusicPlayerDlg::NextCurrCell()
{
	currCell = currCell + 1 >= filePaths.size() ? 0 : currCell + 1;
}

void CMusicPlayerDlg::PrevCurrCell()
{
	currCell = currCell - 1 < 0 ? filePaths.size() - 1 : currCell - 1;
}

void CMusicPlayerDlg::UpdateSlider()
{
	QWORD pos = BASS_ChannelGetPosition(stream, BASS_POS_BYTE);
	QWORD len = BASS_ChannelGetLength(stream, BASS_POS_BYTE);
	double timePos = BASS_ChannelBytes2Seconds(stream, pos);
	double timeLen = BASS_ChannelBytes2Seconds(stream, len);

	int sliderPos = static_cast<int>((timePos / timeLen) * 100);
	progressSlider.SetPos(sliderPos);

	CString str;
	str.Format(_T("%02d:%02d / %02d:%02d"),
		static_cast<int>(timePos) / 60, static_cast<int>(timePos) % 60,
		static_cast<int>(timeLen) / 60, static_cast<int>(timeLen) % 60);
	SetDlgItemText(IDC_MUSIC_POS_STATIC, str);

	if (timePos != timeLen) {
		return;
	}

	NextCurrCell();
	Play();
}

void CMusicPlayerDlg::SetControlButtonsEnableStatuses(bool next, bool prev, bool state)
{
	nextButton.EnableWindow(next);
	prevButton.EnableWindow(prev);
	stateButton.EnableWindow(state);
}

void CMusicPlayerDlg::Save()
{
	std::ofstream file(db_name);
	if (file.is_open()) {
		for (CString& path : filePaths) {
			char filePath[1024];
			strcpy_s(filePath, (LPCSTR)(CStringA)path);

			file << filePath << std::endl;
		}
		file.close();
	}
}

void CMusicPlayerDlg::Load()
{
	std::ifstream file(db_name);
	if (!file.is_open()) {
		return;
	}

	char filePath[1024];
	while (file >> filePath) {
		CString path(filePath);
		filePaths.push_back(path);
		listFiles.AddString(PathFindFileName(path));
	}
	file.close();
}

void CMusicPlayerDlg::Play()
{
	if (stream) {
		BASS_StreamFree(stream);
	}

	CString filePath = filePaths[currCell];
	stream = BASS_StreamCreateFile(FALSE, filePath, 0, 0, 0);

	if (!stream) {
		AfxMessageBox(_T("Error playing file!"));
	}

	listFiles.SetCurSel(currCell);
	BASS_ChannelPlay(stream, FALSE);
	SetTimer(1, 1000, nullptr);
	SetTimer(2, 100, nullptr);
}

CMusicPlayerDlg::CMusicPlayerDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MUSICPLAYER_DIALOG, pParent), stream(0), currCell(0), pause(false)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CMusicPlayerDlg::~CMusicPlayerDlg()
{
	Save();
}

void CMusicPlayerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MUSIC_LIST, listFiles);
	DDX_Control(pDX, IDC_MUSIC_POS_SLIDER, progressSlider);

	DDX_Control(pDX, IDC_PREV_BUTTON, prevButton);
	DDX_Control(pDX, IDC_NEXT_BUTTON, nextButton);
	DDX_Control(pDX, IDC_MUSIC_STATE_BUTTON, stateButton);
}

BEGIN_MESSAGE_MAP(CMusicPlayerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_LOAD_BUTTON, &CMusicPlayerDlg::OnBnClickedLoadButton)
	ON_LBN_SELCHANGE(IDC_MUSIC_LIST, &CMusicPlayerDlg::OnLbnSelchangeMusicList)
	ON_WM_TIMER()
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_MUSIC_POS_SLIDER, &CMusicPlayerDlg::OnNMCustomdrawMusicPosSlider)
	ON_BN_CLICKED(IDC_MUSIC_STATE_BUTTON, &CMusicPlayerDlg::OnBnClickedMusicStateButton)
	ON_BN_CLICKED(IDC_NEXT_BUTTON, &CMusicPlayerDlg::OnBnClickedNextButton)
	ON_BN_CLICKED(IDC_PREV_BUTTON, &CMusicPlayerDlg::OnBnClickedPrevButton)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_MUSIC_POS_SLIDER, &CMusicPlayerDlg::OnNMReleasedcaptureMusicPosSlider)
	ON_BN_CLICKED(IDC_DELETE_BUTTON, &CMusicPlayerDlg::OnBnClickedDeleteButton)
END_MESSAGE_MAP()


// Обработчики сообщений CMusicPlayerDlg

BOOL CMusicPlayerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	if (!BASS_Init(-1, 44100, 0, m_hWnd, nullptr)) {
		AfxMessageBox(_T("BASS initialization error!"));
		return FALSE;
	}

	Load();
	listFiles.SetCurSel(currCell);

	if (filePaths.size() == 1) {
		SetControlButtonsEnableStatuses(false, false, true);
		return TRUE;
	}

	if (!filePaths.empty()) {
		return TRUE;
	}

	SetControlButtonsEnableStatuses(false, false, false);

	return TRUE; 
}

void CMusicPlayerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// При добавлении кнопки свертывания в диалоговое окно нужно воспользоваться приведенным ниже кодом,
//  чтобы нарисовать значок.  Для приложений MFC, использующих модель документов или представлений,
//  это автоматически выполняется рабочей областью.

void CMusicPlayerDlg::OnPaint()
{
	CPaintDC dc(this);

	CDialogEx::OnPaint();

	CRect clientRect;
	GetClientRect(&clientRect);
	int width = clientRect.Width();
	int height = clientRect.Height();

	CRect visualizationRect(clientRect.left + 160, clientRect.top + 45, clientRect.right - 10, clientRect.bottom - 110);
	dc.FillSolidRect(visualizationRect, RGB(0, 0, 0));

	int barWidth = (visualizationRect.Width() - 20) / 64;
	for (int i = 0; i < 73; i++) {
		if (spectr[i] == .0f) {
			continue;
		}

		spectr[i] *= 10;
		spectr[i] = spectr[i] > 1 ? .8f : spectr[i];

		int barHeight = static_cast<int>(spectr[i] * visualizationRect.Height());
		dc.FillSolidRect(visualizationRect.left + 10 + i * barWidth, visualizationRect.bottom - barHeight - 20, barWidth - 2, barHeight + 10, RGB(0, 255, 0));
	}
}

// Система вызывает эту функцию для получения отображения курсора при перемещении
//  свернутого окна.
HCURSOR CMusicPlayerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CMusicPlayerDlg::OnBnClickedLoadButton()
{
	CFileDialog fileDlg(TRUE, _T("mp3"), nullptr, OFN_ALLOWMULTISELECT, _T("MP3 Files (*.mp3)|*.mp3|All Files (*.*)|*.*||"));
	if (fileDlg.DoModal() != IDOK) {
		return;
	}

	POSITION pos = fileDlg.GetStartPosition();
	while (pos) {
		CString filePath = fileDlg.GetNextPathName(pos);
		filePaths.push_back(filePath);
		listFiles.AddString(PathFindFileName(filePath));
	}

	if (filePaths.size() == 1) {
		SetControlButtonsEnableStatuses(false, false, true);
		return;
	}

	SetControlButtonsEnableStatuses(true, true, true);
}


void CMusicPlayerDlg::OnLbnSelchangeMusicList()
{
	currCell = listFiles.GetCurSel();
	if (currCell == LB_ERR) {
		return;
	}

	Play();
}


void CMusicPlayerDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == 1 && stream) {
		UpdateSlider();
	}

	if (nIDEvent == 2 && stream) {
		BASS_ChannelGetData(stream, spectr, BASS_DATA_FFT1024);
		Invalidate();
	}

	CDialogEx::OnTimer(nIDEvent);
}


void CMusicPlayerDlg::OnNMCustomdrawMusicPosSlider(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	*pResult = 0;
}


void CMusicPlayerDlg::OnBnClickedMusicStateButton()
{
	if (!stream && filePaths.empty()) {
		return;
	}

	if (!stream && !filePaths.empty()) {
		stateButton.SetWindowTextW(L"|>");
		Play();
		return;
	}

	if (pause) {
		BASS_Start();
		pause = false;
		stateButton.SetWindowTextW(L"|>");
		return;
	}

	BASS_Pause();
	pause = true;
	stateButton.SetWindowTextW(L"| |");
}


void CMusicPlayerDlg::OnBnClickedNextButton()
{
	NextCurrCell();
	Play();
}


void CMusicPlayerDlg::OnBnClickedPrevButton()
{
	PrevCurrCell();
	Play();
}


void CMusicPlayerDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	CDialogEx::OnLButtonDown(nFlags, point);
}


void CMusicPlayerDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	CDialogEx::OnLButtonUp(nFlags, point);
}


void CMusicPlayerDlg::OnNMReleasedcaptureMusicPosSlider(NMHDR* pNMHDR, LRESULT* pResult)
{
	int pos = progressSlider.GetPos();

	QWORD len = BASS_ChannelGetLength(stream, BASS_POS_BYTE);
	double timeLen = BASS_ChannelBytes2Seconds(stream, len);

	QWORD newPos = BASS_ChannelSeconds2Bytes(stream, (timeLen * pos) / 100);
	BASS_ChannelSetPosition(stream, newPos, BASS_POS_BYTE);

	*pResult = 0;
}


void CMusicPlayerDlg::OnBnClickedDeleteButton()
{
	if (currCell < 0 || filePaths.empty()) {
		return;
	}

	listFiles.DeleteString(currCell);

	if (currCell < filePaths.size()) {
		filePaths.erase(filePaths.begin() + currCell);
	}

	BASS_StreamFree(stream);
	NextCurrCell();

	KillTimer(1);
	KillTimer(2);

	Invalidate();
}
