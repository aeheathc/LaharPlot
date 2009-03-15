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

#include <wx/dcclient.h>
#include <wx/dcmemory.h>
#include <wx/msgdlg.h>
#include <wx/filedlg.h>
#include <wx/filename.h>

#include <string>
using namespace std;

sDEM* curSDEM = new sDEM(_(""));
wxBitmap* image;
wxBitmap* display;
float curZoom = 1;

laharPlotFrame::laharPlotFrame(wxFrame *frame)
    : GUIFrame(frame)
{
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

	// if the image is not blank, display it
	if (display != NULL)
		dc.DrawBitmap(*display, 0, 0, false);
}

void laharPlotFrame::OnLoadSdem(wxCommandEvent &event)
{
	// Open file dialog box
	wxFileDialog* openSdem = new wxFileDialog(this, _("Choose a File"), _(""), _(""), _("SDEM files (*-sdem.tsv)|*-sdem.tsv|All Files (*.*)|*.*"), wxOPEN, wxDefaultPosition);

	// Show dialog box
	if ( openSdem->ShowModal() == wxID_OK )
	{
		// get path and filename
		wxString path;
		path.append( openSdem->GetDirectory() );
		path.append( wxFileName::GetPathSeparator() );
		path.append( openSdem->GetFilename() );

		// set current file
		curSDEM->setFileName(path);

		int sw = curSDEM->at(0).size();
		int sh = curSDEM->size();

		// create dc, and bitmap
		wxMemoryDC memdc;
		image = new wxBitmap( sw,sh );
		memdc.SelectObject(*image);

		// display SDEM on bitmap
		displaySDEM(&memdc);
		display = image;

		// set and refresh scrollwindow
		SDEMScroll->SetScrollbars(20, 20, sw, sh, 0, 0);
		SDEMScroll->Refresh();
	}

	delete openSdem;
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

void laharPlotFrame::Zoom(float zChange, wxCoord x, wxCoord y)
{
	if (image != NULL)
	{
		// change current zoom level
		curZoom *= zChange;

		// zoom image
		wxImage wxi = image->ConvertToImage();
		int zwidth = int (wxi.GetWidth() * curZoom);
		int zheight = int (wxi.GetHeight() * curZoom);
		display = new wxBitmap(wxi.Scale(zwidth, zheight));

		// get information for centering
		int panel_width, panel_height, cur_x, cur_y;
		SDEMScroll->GetViewStart(&cur_x, &cur_y);
		SDEMScroll->GetSize(&panel_width, &panel_height);

		// calculate position for proper centering
		x = int ((cur_x + x) * zChange) - int (.5 * panel_width);
		y = int ((cur_y + y) * zChange) - int (.5 * panel_height);

		// set scrollbars and refresh
		SDEMScroll->SetScrollbars(1, 1, zwidth, zheight, x, y);
		SDEMScroll->Refresh();
	}
}

void laharPlotFrame::displaySDEM(wxDC *dc)
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

		// go through 2 dimensional vector
		for (unsigned int i = 0; i < curSDEM->size(); i++)
		{
			vector<float> demLine = curSDEM->at(i);
			for (unsigned int j = 0; j < demLine.size(); j++)
			{
				// calculate the shade of point
				int color = 255 - int ((sdemMax - demLine.at(j)) * incr);

				// set color and draw point
				wxColor pixcolor (color,color,color);
				dc->SetPen(pixcolor);
				dc->DrawPoint(j, i);
			}
		}
	}
}
