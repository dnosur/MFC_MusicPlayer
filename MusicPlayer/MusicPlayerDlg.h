
// MusicPlayerDlg.h: файл заголовка
//

#pragma once

#include <vector>
#include <fstream>

#include "bass.h"
#include "Shlwapi.h"

// Диалоговое окно CMusicPlayerDlg
class CMusicPlayerDlg : public CDialogEx
{
	const char* db_name = "db";
	std::vector<CString> filePaths;

	CListBox listFiles;
	CSliderCtrl progressSlider;

	CButton prevButton;
	CButton nextButton;
	CButton stateButton;

	HSTREAM stream;
	float spectr[512];

	int currCell;
	bool pause;

	void NextCurrCell();
	void PrevCurrCell();

	void UpdateSlider();

	void SetControlButtonsEnableStatuses(bool next, bool prev, bool state);

	void Save();
	void Load();

	void Play();
// Создание
public:
	CMusicPlayerDlg(CWnd* pParent = nullptr);	// стандартный конструктор
	~CMusicPlayerDlg();

// Данные диалогового окна
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MUSICPLAYER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// поддержка DDX/DDV


// Реализация
protected:
	HICON m_hIcon;

	// Созданные функции схемы сообщений
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedLoadButton();
	afx_msg void OnLbnSelchangeMusicList();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnNMCustomdrawMusicPosSlider(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBnClickedMusicStateButton();
	afx_msg void OnBnClickedNextButton();
	afx_msg void OnBnClickedPrevButton();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnNMReleasedcaptureMusicPosSlider(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBnClickedDeleteButton();
};
