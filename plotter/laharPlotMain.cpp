/***************************************************************
 * Name:      laharPlotMain.cpp
 * Purpose:   Code for Application Frame
 * Author:     ()
 * Created:   2009-02-19
 * Copyright:  ()
 * License:
 **************************************************************/

#ifdef WX_PRECOMP
#include "wx_pch.h"
#endif

#ifdef __BORLANDC__
#pragma hdrstop
#endif //__BORLANDC__

#include "laharPlotMain.h"
#include "frameDEMDialog.h"

#include <wx/dcclient.h>
#include <wx/dcmemory.h>
#include <wx/msgdlg.h>
#include <wx/filedlg.h>
#include <wx/filename.h>

#include <string>
using namespace std;

static float MINZOOM = .125;
static float MAXZOOM = 8;

float curZoom;
tsv* curSDEM;
wxBitmap* image;
wxBitmap* display;
wxBitmap* streams[100];

laharPlotFrame::laharPlotFrame(wxFrame *frame)
    : GUIFrame(frame)
{
	curSDEM = new tsv(_(""));
	curZoom = 1;
#if wxUSE_STATUSBAR
    statusBar->SetStatusText(_("LaharPlot"), 0);
    statusBar->SetStatusText(_(""), 1);
#endif
}

laharPlotFrame::~laharPlotFrame()
{
	delete curSDEM;
	delete image;
	delete display;
}

void laharPlotFrame::OnClose(wxCloseEvent &event)
{
    Destroy();
}

void laharPlotFrame::OnQuit(wxCommandEvent &event)
{
    Destroy();
}

void laharPlotFrame::OnAbout(wxCommandEvent &event)
{
    wxMessageBox(_("LaharPlot"), _("Welcome to..."));
}

void laharPlotFrame::OnPaint(wxPaintEvent &event)
{
	// create and prepare the dc
	wxPaintDC dc(SDEMScroll);
	SDEMScroll->DoPrepareDC(dc);

	if (curZoom >= 1)
	{
		dc.SetUserScale(curZoom,curZoom);
		if (image != NULL)
			dc.DrawBitmap(*image, 0, 0, false);
	} else {
		dc.SetUserScale(1,1);
		if (display != NULL)
			dc.DrawBitmap(*display, 0, 0, false);
	}
}

void laharPlotFrame::OnConvertDEM(wxCommandEvent& event)
{
	frameDEMDialog demDlg (this);
	demDlg.ShowModal();
}

void laharPlotFrame::OnLoadSdem(wxCommandEvent &event)
{
	// Open file dialog box
	wxFileDialog openSdem (this, _("Choose a File"), _(""), _(""), _("SDEM files (*-sdem.tsv)|*-sdem.tsv|All Files (*.*)|*.*"), wxOPEN, wxDefaultPosition);

	// Show dialog box
	if ( openSdem.ShowModal() == wxID_OK )
	{
		// get path and filename
		wxString path;
		path.append( openSdem.GetDirectory() );
		path.append( wxFileName::GetPathSeparator() );
		path.append( openSdem.GetFilename() );

		SetFile(path);
	}
}

void laharPlotFrame::OnShowStreams(wxCommandEvent &event)
{
	if (showStreams->IsChecked()) {
		// display image
	} else {
		// remove image
	}
}

void laharPlotFrame::OnScrollwheel(wxMouseEvent &event)
{
	// check if control button is down
	if (event.m_controlDown)
	{
		// get direction of scroll
		int wheel_amount = event.m_wheelRotation;
		if (wheel_amount < 0)
			Zoom(.5, event.GetX(), event.GetY());
		else
			Zoom(2, event.GetX(), event.GetY());
	}
}

void laharPlotFrame::SetFile(wxString file)
{
	// progress bar
	wxProgressDialog sdemLoadBar (_("Loading SDEM"), _(""), 100, this, wxPD_APP_MODAL | wxPD_AUTO_HIDE);
	sdemLoadBar.SetSize(300, 120);

	// clear old image
	delete image;

	// set current file
	curSDEM->setFileName(file, &sdemLoadBar);

	// update progress
	sdemLoadBar.Update(50,_("Drawing SDEM"));

	int sw = curSDEM->front().size();
	int sh = curSDEM->size();

	// create dc, and bitmap
	wxMemoryDC memdc;
	image = new wxBitmap( sw,sh );
	memdc.SelectObject(*image);

	// display SDEM on bitmap
	displaySDEM(&memdc, &sdemLoadBar);
	wxImage wxi = image->ConvertToImage();
	display = new wxBitmap(wxi.Scale(image->GetWidth(), image->GetHeight()));

	// unselect bitmap, for safe destruction
	memdc.SelectObject(wxNullBitmap);

	// set and refresh scrollwindow
	SDEMScroll->SetScrollbars(1, 1, sw, sh, 0, 0);
	SDEMScroll->Refresh();

	// update progress
	sdemLoadBar.Update(100);
}

void laharPlotFrame::Zoom(float zChange, wxCoord x, wxCoord y)
{
	if (image != NULL && ((curZoom < MAXZOOM && zChange > 1) || (curZoom > MINZOOM && zChange < 1)))
	{
		// calculate new curzoom
		float tempPreZoom = curZoom;
		curZoom *= zChange;
		float tempPostZoom = curZoom;
		if (curZoom > MAXZOOM)
			curZoom = MAXZOOM;
		if (curZoom < MINZOOM)
			curZoom = MINZOOM;

		// correct for over/under zooms
		if (tempPostZoom != curZoom) {
			zChange = curZoom / tempPreZoom;
		}

		// calculate zoom
		int zwidth = 0,
			zheight = 0;
		if (curZoom >= 1)
		{
			zwidth = int (image->GetWidth() * curZoom);
			zheight = int (image->GetHeight() * curZoom);
		} else {
			// create zoomed out image
			delete display;
			wxImage wxi = image->ConvertToImage();
			zwidth = int (wxi.GetWidth() * curZoom);
			zheight = int (wxi.GetHeight() * curZoom);
			display = new wxBitmap(wxi.Scale(zwidth, zheight));
		}

		// get information for centering
		int panel_width, panel_height, cur_x, cur_y;
		SDEMScroll->GetViewStart(&cur_x, &cur_y);
		SDEMScroll->GetSize(&panel_width, &panel_height);

		// calculate position for proper centering
		x = int ((cur_x + x) * zChange) - int (.5 * panel_width);
		y = int ((cur_y + y) * zChange) - int (.5 * panel_height);
		if (x < 0) x = 0;
		if (y < 0) y = 0;

		// set scrollbars and refresh
		SDEMScroll->SetScrollbars(1, 1, zwidth, zheight, x, y);
		SDEMScroll->Refresh();
	}
}

void laharPlotFrame::displaySDEM(wxDC *dc, wxProgressDialog* progDlg)
{
    // check if sdem file has been loaded
    string file = curSDEM->getFileName();
    if (file != "")
    {
    	// get some variables needed for calculations for display
    	float sdemMax = curSDEM->getMax();
		float sdemMin = curSDEM->getMin();

		// calculate some values for display
		float diff = sdemMax - sdemMin;
		float incr = 0;
		if (diff > 0)
			incr = 255 / diff;

		list< list<float> >::iterator demLine = curSDEM->begin();

		// go through 2 dimensional list
		for (unsigned int i = 0; i < curSDEM->size(); i++)
		{
			list<float> dl = *demLine;
			list<float>::iterator elem = dl.begin();
			for (unsigned int j = 0; j < dl.size(); j++)
			{
				// figure out shade of  point
				int color;
				if (*elem <= -10000)
					color = 0;
				else
					color = 255 - int ((sdemMax - *elem) * incr);

				// set color and draw point
				wxColor pixcolor (color,color,color);
				dc->SetPen(pixcolor);
				dc->DrawPoint(j, i);

				// move iterator
				elem++;
			}

			// move iterator
			demLine++;
			progDlg->Update(50 + (i * (49.9 / curSDEM->size())));
		}
	}
}
