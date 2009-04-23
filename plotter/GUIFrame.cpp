///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif //__BORLANDC__

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif //WX_PRECOMP

#include "GUIFrame.h"

///////////////////////////////////////////////////////////////////////////

GUIFrame::GUIFrame( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	mbar = new wxMenuBar( 0 );
	fileMenu = new wxMenu();
	wxMenuItem* menuFileConvertDEM;
	menuFileConvertDEM = new wxMenuItem( fileMenu, wxID_ANY, wxString( wxT("Convert DEM File") ) , wxT("Convert DEM to SDEM and Flow Direction Grid"), wxITEM_NORMAL );
	fileMenu->Append( menuFileConvertDEM );
	
	wxMenuItem* menuFileLoadSdem;
	menuFileLoadSdem = new wxMenuItem( fileMenu, idLoadSdem, wxString( wxT("Load SDEM / Flow Totals") ) , wxT("Load simple DEM file"), wxITEM_NORMAL );
	fileMenu->Append( menuFileLoadSdem );
	
	wxMenuItem* menuFileRunZone;
	menuFileRunZone = new wxMenuItem( fileMenu, wxID_ANY, wxString( wxT("Inundation Zone Mapper") ) , wxEmptyString, wxITEM_NORMAL );
	fileMenu->Append( menuFileRunZone );
	
	wxMenuItem* menuLoadIZone;
	menuLoadIZone = new wxMenuItem( fileMenu, wxID_ANY, wxString( wxT("Load Inundation Zone") ) , wxEmptyString, wxITEM_NORMAL );
	fileMenu->Append( menuLoadIZone );
	
	wxMenuItem* menuFileQuit;
	menuFileQuit = new wxMenuItem( fileMenu, idMenuQuit, wxString( wxT("&Quit") ) + wxT('\t') + wxT("Alt+F4"), wxT("Quit the application"), wxITEM_NORMAL );
	fileMenu->Append( menuFileQuit );
	
	mbar->Append( fileMenu, wxT("&File") );
	
	helpMenu = new wxMenu();
	wxMenuItem* menuHelpAbout;
	menuHelpAbout = new wxMenuItem( helpMenu, idMenuAbout, wxString( wxT("&About") ) + wxT('\t') + wxT("F1"), wxT("Show info about this application"), wxITEM_NORMAL );
	helpMenu->Append( menuHelpAbout );
	
	mbar->Append( helpMenu, wxT("&Help") );
	
	this->SetMenuBar( mbar );
	
	statusBar = this->CreateStatusBar( 2, wxST_SIZEGRIP, wxID_ANY );
	wxFlexGridSizer* fgSizer2;
	fgSizer2 = new wxFlexGridSizer( 2, 1, 0, 0 );
	fgSizer2->AddGrowableCol( 0 );
	fgSizer2->AddGrowableRow( 0 );
	fgSizer2->SetFlexibleDirection( wxBOTH );
	fgSizer2->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	SDEMScroll = new wxScrolledWindow( this, idSDEMScroll, wxDefaultPosition, wxDefaultSize, wxHSCROLL|wxVSCROLL );
	SDEMScroll->SetScrollRate( 5, 5 );
	fgSizer2->Add( SDEMScroll, 1, wxEXPAND | wxALL, 3 );
	
	wxGridBagSizer* gbSizer2;
	gbSizer2 = new wxGridBagSizer( 0, 0 );
	gbSizer2->SetFlexibleDirection( wxBOTH );
	gbSizer2->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	showStreams = new wxCheckBox( this, wxID_ANY, wxT("Show Streams"), wxDefaultPosition, wxDefaultSize, 0 );
	showStreams->SetValue(true);
	
	gbSizer2->Add( showStreams, wxGBPosition( 0, 0 ), wxGBSpan( 1, 1 ), wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	streamSliderRadio = new wxRadioButton( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
	gbSizer2->Add( streamSliderRadio, wxGBPosition( 0, 1 ), wxGBSpan( 1, 1 ), wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	streamSlider = new wxSlider( this, wxID_ANY, 500, 0, 1000, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL );
	gbSizer2->Add( streamSlider, wxGBPosition( 0, 2 ), wxGBSpan( 1, 1 ), wxALIGN_CENTER_VERTICAL|wxALL, 0 );
	
	streamBoxRadio = new wxRadioButton( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	streamBoxRadio->SetValue( true ); 
	gbSizer2->Add( streamBoxRadio, wxGBPosition( 0, 3 ), wxGBSpan( 1, 1 ), wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	streamBox = new wxTextCtrl( this, wxID_ANY, wxT("1000"), wxDefaultPosition, wxSize( 48,-1 ), wxTE_PROCESS_ENTER );
	streamBox->SetMaxLength( 6 ); 
	gbSizer2->Add( streamBox, wxGBPosition( 0, 4 ), wxGBSpan( 1, 1 ), wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	fgSizer2->Add( gbSizer2, 1, wxEXPAND, 5 );
	
	this->SetSizer( fgSizer2 );
	this->Layout();
	
	// Connect Events
	this->Connect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( GUIFrame::OnClose ) );
	this->Connect( menuFileConvertDEM->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( GUIFrame::OnConvertDEM ) );
	this->Connect( menuFileLoadSdem->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( GUIFrame::OnLoadSdem ) );
	this->Connect( menuFileRunZone->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( GUIFrame::OnRunZone ) );
	this->Connect( menuLoadIZone->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( GUIFrame::OnLoadIZone ) );
	this->Connect( menuFileQuit->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( GUIFrame::OnQuit ) );
	this->Connect( menuHelpAbout->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( GUIFrame::OnAbout ) );
	SDEMScroll->Connect( wxEVT_LEFT_UP, wxMouseEventHandler( GUIFrame::OnLeftUp ), NULL, this );
	SDEMScroll->Connect( wxEVT_MOUSEWHEEL, wxMouseEventHandler( GUIFrame::OnScrollwheel ), NULL, this );
	SDEMScroll->Connect( wxEVT_PAINT, wxPaintEventHandler( GUIFrame::OnPaint ), NULL, this );
	showStreams->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( GUIFrame::OnShowStreams ), NULL, this );
	streamBox->Connect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( GUIFrame::OnTextEnter ), NULL, this );
}

GUIFrame::~GUIFrame()
{
	// Disconnect Events
	this->Disconnect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( GUIFrame::OnClose ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( GUIFrame::OnConvertDEM ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( GUIFrame::OnLoadSdem ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( GUIFrame::OnRunZone ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( GUIFrame::OnLoadIZone ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( GUIFrame::OnQuit ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( GUIFrame::OnAbout ) );
	SDEMScroll->Disconnect( wxEVT_LEFT_UP, wxMouseEventHandler( GUIFrame::OnLeftUp ), NULL, this );
	SDEMScroll->Disconnect( wxEVT_MOUSEWHEEL, wxMouseEventHandler( GUIFrame::OnScrollwheel ), NULL, this );
	SDEMScroll->Disconnect( wxEVT_PAINT, wxPaintEventHandler( GUIFrame::OnPaint ), NULL, this );
	showStreams->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( GUIFrame::OnShowStreams ), NULL, this );
	streamBox->Disconnect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( GUIFrame::OnTextEnter ), NULL, this );
}

DEMDialog::DEMDialog( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxFlexGridSizer* fgSizer3;
	fgSizer3 = new wxFlexGridSizer( 3, 3, 0, 0 );
	fgSizer3->SetFlexibleDirection( wxBOTH );
	fgSizer3->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText3 = new wxStaticText( this, wxID_ANY, wxT("DEM File"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText3->Wrap( -1 );
	fgSizer3->Add( m_staticText3, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	DEMTextBox = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 300,-1 ), 0 );
	fgSizer3->Add( DEMTextBox, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	m_button3 = new wxButton( this, wxID_ANY, wxT("Browse..."), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer3->Add( m_button3, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	m_staticText4 = new wxStaticText( this, wxID_ANY, wxT("Output Files"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText4->Wrap( -1 );
	fgSizer3->Add( m_staticText4, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	OutputTextBox = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 300,-1 ), 0 );
	fgSizer3->Add( OutputTextBox, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	m_button4 = new wxButton( this, wxID_ANY, wxT("Browse..."), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer3->Add( m_button4, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	
	fgSizer3->Add( 0, 0, 1, wxEXPAND, 5 );
	
	
	fgSizer3->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_button6 = new wxButton( this, idConvert, wxT("Convert"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer3->Add( m_button6, 0, wxALL, 5 );
	
	this->SetSizer( fgSizer3 );
	this->Layout();
	
	// Connect Events
	m_button3->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DEMDialog::OnBrowseDEM ), NULL, this );
	m_button4->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DEMDialog::OnOutputBrowse ), NULL, this );
	m_button6->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DEMDialog::OnConvert ), NULL, this );
}

DEMDialog::~DEMDialog()
{
	// Disconnect Events
	m_button3->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DEMDialog::OnBrowseDEM ), NULL, this );
	m_button4->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DEMDialog::OnOutputBrowse ), NULL, this );
	m_button6->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DEMDialog::OnConvert ), NULL, this );
}

ZoneDialog::ZoneDialog( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxVERTICAL );
	
	wxGridBagSizer* gbSizer2;
	gbSizer2 = new wxGridBagSizer( 0, 0 );
	gbSizer2->SetFlexibleDirection( wxBOTH );
	gbSizer2->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	basename = new wxStaticText( this, wxID_ANY, wxT("Simple Name"), wxDefaultPosition, wxDefaultSize, 0 );
	basename->Wrap( -1 );
	gbSizer2->Add( basename, wxGBPosition( 0, 0 ), wxGBSpan( 1, 1 ), wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	SimpleBox = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 300,-1 ), 0 );
	gbSizer2->Add( SimpleBox, wxGBPosition( 0, 1 ), wxGBSpan( 1, 1 ), wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	BrowseSimple = new wxButton( this, wxID_ANY, wxT("Browse"), wxDefaultPosition, wxDefaultSize, 0 );
	gbSizer2->Add( BrowseSimple, wxGBPosition( 0, 2 ), wxGBSpan( 1, 1 ), wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	Output = new wxStaticText( this, wxID_ANY, wxT("Output Directory"), wxDefaultPosition, wxDefaultSize, 0 );
	Output->Wrap( -1 );
	gbSizer2->Add( Output, wxGBPosition( 1, 0 ), wxGBSpan( 1, 1 ), wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	OutputBox = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 300,-1 ), 0 );
	gbSizer2->Add( OutputBox, wxGBPosition( 1, 1 ), wxGBSpan( 1, 1 ), wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	BrowseOutput = new wxButton( this, wxID_ANY, wxT("Browse"), wxDefaultPosition, wxDefaultSize, 0 );
	gbSizer2->Add( BrowseOutput, wxGBPosition( 1, 2 ), wxGBSpan( 1, 1 ), wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	StreamStart = new wxStaticText( this, wxID_ANY, wxT("Stream Start"), wxDefaultPosition, wxDefaultSize, 0 );
	StreamStart->Wrap( -1 );
	gbSizer2->Add( StreamStart, wxGBPosition( 2, 0 ), wxGBSpan( 1, 1 ), wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	volumelabel = new wxStaticText( this, wxID_ANY, wxT("Volumes: Separated by spaces, eg \"4000 5000\""), wxDefaultPosition, wxDefaultSize, 0 );
	volumelabel->Wrap( -1 );
	gbSizer2->Add( volumelabel, wxGBPosition( 3, 0 ), wxGBSpan( 1, 3 ), wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	VolumeBox = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 400,-1 ), 0 );
	gbSizer2->Add( VolumeBox, wxGBPosition( 4, 0 ), wxGBSpan( 1, 5 ), wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	cancel = new wxButton( this, wxID_ANY, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	gbSizer2->Add( cancel, wxGBPosition( 5, 1 ), wxGBSpan( 1, 1 ), wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxALL, 5 );
	
	run = new wxButton( this, wxID_ANY, wxT("Run"), wxDefaultPosition, wxDefaultSize, 0 );
	gbSizer2->Add( run, wxGBPosition( 5, 2 ), wxGBSpan( 1, 1 ), wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT|wxALL, 5 );
	
	wxGridBagSizer* gbSizer3;
	gbSizer3 = new wxGridBagSizer( 0, 0 );
	gbSizer3->SetFlexibleDirection( wxBOTH );
	gbSizer3->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	startxlabel = new wxStaticText( this, wxID_ANY, wxT("x"), wxDefaultPosition, wxDefaultSize, 0 );
	startxlabel->Wrap( -1 );
	gbSizer3->Add( startxlabel, wxGBPosition( 0, 0 ), wxGBSpan( 1, 1 ), wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	startx = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 48,-1 ), 0 );
	startx->SetMaxLength( 6 ); 
	gbSizer3->Add( startx, wxGBPosition( 0, 1 ), wxGBSpan( 1, 1 ), wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	startylabel = new wxStaticText( this, wxID_ANY, wxT("y"), wxDefaultPosition, wxDefaultSize, 0 );
	startylabel->Wrap( -1 );
	gbSizer3->Add( startylabel, wxGBPosition( 0, 2 ), wxGBSpan( 1, 1 ), wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	starty = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 48,-1 ), 0 );
	starty->SetMaxLength( 6 ); 
	gbSizer3->Add( starty, wxGBPosition( 0, 3 ), wxGBSpan( 1, 1 ), wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	clickstart = new wxButton( this, wxID_ANY, wxT("Show DEM"), wxDefaultPosition, wxDefaultSize, 0 );
	gbSizer3->Add( clickstart, wxGBPosition( 0, 4 ), wxGBSpan( 1, 1 ), wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	gbSizer2->Add( gbSizer3, wxGBPosition( 2, 1 ), wxGBSpan( 1, 3 ), 0, 5 );
	
	bSizer2->Add( gbSizer2, 1, wxEXPAND, 5 );
	
	this->SetSizer( bSizer2 );
	this->Layout();
	
	// Connect Events
	BrowseSimple->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ZoneDialog::OnBrowseSimple ), NULL, this );
	BrowseOutput->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ZoneDialog::OnBrowseOutput ), NULL, this );
	cancel->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ZoneDialog::OnCancel ), NULL, this );
	run->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ZoneDialog::OnRun ), NULL, this );
	clickstart->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ZoneDialog::OnShowDEM ), NULL, this );
}

ZoneDialog::~ZoneDialog()
{
	// Disconnect Events
	BrowseSimple->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ZoneDialog::OnBrowseSimple ), NULL, this );
	BrowseOutput->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ZoneDialog::OnBrowseOutput ), NULL, this );
	cancel->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ZoneDialog::OnCancel ), NULL, this );
	run->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ZoneDialog::OnRun ), NULL, this );
	clickstart->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ZoneDialog::OnShowDEM ), NULL, this );
}
