// Berkeley Open Infrastructure for Network Computing
// http://boinc.berkeley.edu
// Copyright (C) 2005 University of California
//
// This is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation;
// either version 2.1 of the License, or (at your option) any later version.
//
// This software is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU Lesser General Public License for more details.
//
// To view the GNU Lesser General Public License visit
// http://www.gnu.org/copyleft/lesser.html
// or write to the Free Software Foundation, Inc.,
// 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

#ifndef _VIEWPROJECTSGRID_H_
#define _VIEWPROJECTSGRID_H_

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "ViewProjectsGrid.cpp"
#endif


#include "BOINCBaseView.h"
#include "BOINCGridCtrl.h"
#include <wx/arrimpl.cpp>

class CProjectInfo {
public:
	CProjectInfo();
	wxString name;
	wxString accountname;
	wxString teamname;
	double totalcredit;
	double avgcredit;
	double resourceshare;
	double rspercent;
	wxString status;
	wxString hashKey;

	void makeHashKey();
};

WX_DEFINE_ARRAY(CProjectInfo*,ProjectCache);

class CViewProjectsGrid : public CBOINCBaseView
{
    DECLARE_DYNAMIC_CLASS( CViewProjectsGrid )
	CBOINCGridCtrl*	m_pGridPane;
	bool UpdateProjectCache();
	wxString GetReadableStatus(PROJECT* project);
	bool SortProjects();
	ProjectCache m_projectCache;
	
	void FillProjectInfo(PROJECT* p,CProjectInfo* info);
	bool IsProjectInfoOutOfDate(PROJECT* p,CProjectInfo* info);
	CProjectInfo* FindProjectInCache(wxString& name);
public:
    CViewProjectsGrid();
    CViewProjectsGrid(wxNotebook* pNotebook);

    ~CViewProjectsGrid();

    virtual wxString&       GetViewName();
    virtual const char**    GetViewIcon();

    void                    OnProjectUpdate( wxCommandEvent& event );
    void                    OnProjectSuspend( wxCommandEvent& event );
    void                    OnProjectNoNewWork( wxCommandEvent& event );
    void                    OnProjectReset( wxCommandEvent& event );
    void                    OnProjectDetach( wxCommandEvent& event );

    void                    OnProjectWebsiteClicked( wxEvent& event );
	void					UpdateWebsiteSelection(long lControlGroup, PROJECT* project);
protected:
    virtual wxInt32         GetDocCount();

    virtual void            UpdateSelection();

    virtual bool            OnSaveState( wxConfigBase* pConfig );
    virtual bool            OnRestoreState( wxConfigBase* pConfig );
	virtual void            OnListRender( wxTimerEvent& event );

    void	                FormatProjectName( wxInt32 item, wxString& strBuffer );
    void					FormatAccountName( wxInt32 item, wxString& strBuffer );
    void					FormatTeamName( wxInt32 item, wxString& strBuffer );
    void					FormatTotalCredit( wxInt32 item, wxString& strBuffer );
    void					FormatAVGCredit( wxInt32 item, wxString& strBuffer );
    void					FormatResourceShare( wxInt32 item, wxString& strBuffer );
    void					FormatStatus( wxInt32 item, wxString& strBuffer );

    bool                    IsWebsiteLink( const wxString& strLink );
    wxInt32                 ConvertWebsiteIndexToLink( wxInt32 iProjectIndex, wxInt32 iWebsiteIndex, wxString& strLink );
    wxInt32                 ConvertLinkToWebsiteIndex( const wxString& strLink, wxInt32& iProjectIndex, wxInt32& iWebsiteIndex );

	void					OnSelectCell( wxGridEvent& );
	void					OnSelectRange(wxGridRangeSelectEvent& ev);
    DECLARE_EVENT_TABLE()
private:
	bool					m_bIgnoreSelectionEvents;
};


#endif

