///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __GUIFrame__
#define __GUIFrame__

#include <wx/string.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/menu.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/statusbr.h>
#include <wx/scrolwin.h>
#include <wx/sizer.h>
#include <wx/frame.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/button.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////

#define idLoadSdem 1000
#define idMenuQuit 1001
#define idMenuAbout 1002
#define idSDEMScroll 1003
#define idDEMDialog 1004
#define idConvert 1005

///////////////////////////////////////////////////////////////////////////////
/// Class GUIFrame
///////////////////////////////////////////////////////////////////////////////
class GUIFrame : public wxFrame 
{
	private:
	
	protected:
		wxMenuBar* mbar;
		wxMenu* fileMenu;
		wxMenu* helpMenu;
		wxStatusBar* statusBar;
		wxScrolledWindow* SDEMScroll;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnClose( wxCloseEvent& event ){ event.Skip(); }
		virtual void OnConvertDEM( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnLoadSdem( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnQuit( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnAbout( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnScrollwheel( wxMouseEvent& event ){ event.Skip(); }
		virtual void OnPaint( wxPaintEvent& event ){ event.Skip(); }
		
	
	public:
		GUIFrame( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("LaharPlot"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,500 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );
		~GUIFrame();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class DEMDialog
///////////////////////////////////////////////////////////////////////////////
class DEMDialog : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* m_staticText3;
		wxTextCtrl* DEMTextBox;
		wxButton* m_button3;
		wxStaticText* m_staticText4;
		wxTextCtrl* OutputTextBox;
		wxButton* m_button4;
		
		
		wxButton* m_button6;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnBrowseDEM( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnOutputBrowse( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnConvert( wxCommandEvent& event ){ event.Skip(); }
		
	
	public:
		DEMDialog( wxWindow* parent, wxWindowID id = idDEMDialog, const wxString& title = wxT("Convert DEM"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 486,135 ), long style = wxDEFAULT_DIALOG_STYLE );
		~DEMDialog();
	
};

#endif //__GUIFrame__
