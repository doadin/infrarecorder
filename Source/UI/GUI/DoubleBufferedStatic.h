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

#pragma once
#include <vector>
#include "../../Common/StringUtil.h"

#define DOUBLEBUFFEREDSTATIC_RESERVE_LENGTH				256

class CDoubleBufferedStatic : public CWindowImpl<CDoubleBufferedStatic,CStatic>
{
private:
	tstring m_Text;

public:
	CDoubleBufferedStatic();
	~CDoubleBufferedStatic();

	void SetWindowText(const TCHAR *szWindowText);

	BEGIN_MSG_MAP(CDoubleBufferedStatic)
		MESSAGE_HANDLER(WM_PAINT,OnPaint)
		MESSAGE_HANDLER(WM_ERASEBKGND,OnEraseBkGnd)
	END_MSG_MAP()

	LRESULT OnPaint(UINT uMsg,WPARAM wParam,LPARAM lParam,BOOL &bHandled);
	LRESULT OnEraseBkGnd(UINT uMsg,WPARAM wParam,LPARAM lParam,BOOL &bHandled);
};