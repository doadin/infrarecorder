/*
 * Copyright (C) 2006-2007 Christian Kindahl, christian dot kindahl at gmail dot com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "stdafx.h"
#include "DevicesDlg.h"
#include "DeviceManager.h"
#include "DeviceDlg.h"
#include "WaitDlg.h"
#include "../../Common/StringUtil.h"
#include "StringTable.h"
#include "Settings.h"
#include "LangUtil.h"

CDevicesDlg::CDevicesDlg()
{
	m_hListImageList = NULL;
}

CDevicesDlg::~CDevicesDlg()
{
	if (m_hListImageList)
		ImageList_Destroy(m_hListImageList);
}

bool CDevicesDlg::Translate()
{
	if (g_LanguageSettings.m_pLNGProcessor == NULL)
		return false;

	CLNGProcessor *pLNG = g_LanguageSettings.m_pLNGProcessor;
	
	// Make sure that there is a devices translation section.
	if (!pLNG->EnterSection(_T("devices")))
		return false;

	// Translate.
	TCHAR *szStrValue;

	if (pLNG->GetValuePtr(IDD_DEVICESDLG,szStrValue))			// Title.
		SetWindowText(szStrValue);
	if (pLNG->GetValuePtr(IDOK,szStrValue))
		SetDlgItemText(IDOK,szStrValue);
	if (pLNG->GetValuePtr(IDC_HELPBUTTON,szStrValue))
		SetDlgItemText(IDC_HELPBUTTON,szStrValue);
	if (pLNG->GetValuePtr(IDC_RESCANBUTTON,szStrValue))
		SetDlgItemText(IDC_RESCANBUTTON,szStrValue);
	if (pLNG->GetValuePtr(IDC_INFOSTATIC,szStrValue))
		SetDlgItemText(IDC_INFOSTATIC,szStrValue);
	if (pLNG->GetValuePtr(IDC_AUTOSCANCHECK,szStrValue))
		SetDlgItemText(IDC_AUTOSCANCHECK,szStrValue);

	return true;
}

void CDevicesDlg::FillListView()
{
	int iItemCount = 0;

	for (unsigned int i = 0; i < g_DeviceManager.GetDeviceCount(); i++)
	{
		if (!g_DeviceManager.IsDeviceReader(i))
			continue;

		tDeviceInfo *pDeviceInfo = g_DeviceManager.GetDeviceInfo(i);

		TCHAR szBuffer[64];
		lsprintf(szBuffer,_T("[%d,%d,%d]"),pDeviceInfo->Address.m_iBus,
			pDeviceInfo->Address.m_iTarget,pDeviceInfo->Address.m_iLun);
		m_ListView.AddItem(iItemCount,0,szBuffer,0);

		m_ListView.AddItem(iItemCount,1,pDeviceInfo->szVendor,0);
		m_ListView.AddItem(iItemCount,2,pDeviceInfo->szIdentification,0);
		m_ListView.AddItem(iItemCount,3,pDeviceInfo->szRevision,0);
		m_ListView.SetItemData(iItemCount,i);

		iItemCount++;
	}
}

void CDevicesDlg::InitializeListView()
{
	// Create the image list.
	HINSTANCE hInstance = LoadLibrary(_T("shell32.dll"));
		HICON hIcon = (HICON)LoadImage(hInstance,MAKEINTRESOURCE(12),IMAGE_ICON,16,16,/*LR_DEFAULTCOLOR*/LR_LOADTRANSPARENT);
	FreeLibrary(hInstance);

	m_hListImageList = ImageList_Create(16,16,ILC_COLOR32,0,1);
	ImageList_AddIcon(m_hListImageList,hIcon);

	DestroyIcon(hIcon);

	// Setup the list view.
	m_ListView.SubclassWindow(GetDlgItem(IDC_DEVICELIST));
	m_ListView.SetImageList(m_hListImageList,LVSIL_NORMAL);
	m_ListView.SetImageList(m_hListImageList,LVSIL_SMALL);
	m_ListView.SetExtendedListViewStyle(LVS_EX_FULLROWSELECT);

	// Add the columns.
	m_ListView.AddColumn(lngGetString(COLUMN_ID),0);
	m_ListView.SetColumnWidth(0,60);
	m_ListView.AddColumn(lngGetString(COLUMN_VENDOR),1);
	m_ListView.SetColumnWidth(1,80);
	m_ListView.AddColumn(lngGetString(COLUMN_IDENTIFICATION),2);
	m_ListView.SetColumnWidth(2,120);
	m_ListView.AddColumn(lngGetString(COLUMN_REVISION),3);
	m_ListView.SetColumnWidth(3,60);
}

LRESULT CDevicesDlg::OnInitDialog(UINT uMsg,WPARAM wParam,LPARAM lParam,BOOL &bHandled)
{
	CenterWindow(GetParent());

	// Initialize the list view.
	InitializeListView();

	// Fill the list view.
	FillListView();

	CheckDlgButton(IDC_AUTOSCANCHECK,g_GlobalSettings.m_bAutoCheckBus);

	// Translate the window.
	Translate();

	return TRUE;
}

LRESULT CDevicesDlg::OnDestroy(UINT uMsg,WPARAM wParam,LPARAM lParam,BOOL &bHandled)
{
	// Detach the internal list view control.
	m_ListView.UnsubclassWindow();

	bHandled = false;
	return 0;
}

LRESULT CDevicesDlg::OnListDblClick(int iCtrlID,LPNMHDR pNMH,BOOL &bHandled)
{
	if (m_ListView.GetSelectedCount() > 0)
	{
		UINT_PTR uiDeviceIndex = m_ListView.GetItemData(m_ListView.GetSelectedIndex());
		tDeviceInfo *pDeviceInfo = g_DeviceManager.GetDeviceInfo(uiDeviceIndex);

		TCHAR szDeviceName[128];
		g_DeviceManager.GetDeviceName(pDeviceInfo,szDeviceName);
		
		TCHAR szTitle[128];
		lstrcpy(szTitle,lngGetString(PROPERTIES_TITLE));
		lstrcat(szTitle,szDeviceName);

		CDeviceDlg DeviceDlg(uiDeviceIndex,szTitle);
		DeviceDlg.DoModal();
	}

	bHandled = false;
	return 0;
}

LRESULT CDevicesDlg::OnOK(WORD wNotifyCode,WORD wID,HWND hWndCtl,BOOL &bHandled)
{
	g_GlobalSettings.m_bAutoCheckBus = IsDlgButtonChecked(IDC_AUTOSCANCHECK) == TRUE;

	EndDialog(wID);
	return FALSE;
}

LRESULT CDevicesDlg::OnRescan(WORD wNotifyCode,WORD wID,HWND hWndCtl,BOOL &bHandled)
{
	// Empty the list view.
	m_ListView.DeleteAllItems();

	// Rescan the bus.
	CWaitDlg WaitDlg;
	WaitDlg.Create(m_hWnd);
	WaitDlg.ShowWindow(SW_SHOW);
		g_DeviceManager.Reset();

		WaitDlg.SetMessage(lngGetString(INIT_SCANBUS));
		g_DeviceManager.ScanBus();

		WaitDlg.SetMessage(lngGetString(INIT_LOADCAPABILITIES));
		g_DeviceManager.LoadCapabilities();

		WaitDlg.SetMessage(lngGetString(INIT_LOADINFOEX));
		g_DeviceManager.LoadExInfo();
	WaitDlg.DestroyWindow();

	// Fill the list view.
	FillListView();

	return FALSE;
}

LRESULT CDevicesDlg::OnHelp(WORD wNotifyCode,WORD wID,HWND hWndCtl,BOOL &bHandled)
{
	TCHAR szFileName[MAX_PATH];
	GetModuleFileName(NULL,szFileName,MAX_PATH - 1);

	ExtractFilePath(szFileName);
	lstrcat(szFileName,lngGetManual());
	lstrcat(szFileName,_T("::/how_to_use/device_configuration.html"));

	HtmlHelp(m_hWnd,szFileName,HH_DISPLAY_TOC,NULL);
	return 0;
}