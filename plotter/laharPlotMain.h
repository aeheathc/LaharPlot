/***************************************************************
 * Name:      laharPlotMain.h
 * Purpose:   Defines Application Frame
 * Author:     ()
 * Created:   2009-02-19
 * Copyright:  ()
 * License:
 **************************************************************/

#ifndef LAHARPLOTMAIN_H
#define LAHARPLOTMAIN_H

#include <wx/progdlg.h>
#include <wx/string.h>

#include "laharPlotApp.h"
#include "GUIFrame.h"
#include "tsv.h"

class laharPlotFrame: public GUIFrame
{
    public:
        void SetFile(wxString file);
        laharPlotFrame(wxFrame *frame);
        ~laharPlotFrame();
    private:
        virtual void OnClose(wxCloseEvent& event);
        virtual void OnQuit(wxCommandEvent& event);
        virtual void OnAbout(wxCommandEvent& event);
        virtual void OnConvertDEM(wxCommandEvent& event);
        virtual void OnLoadSdem(wxCommandEvent& event);
        virtual void OnRunZone(wxCommandEvent& event);
        virtual void OnLoadIZone(wxCommandEvent& event);
        virtual void OnPaint(wxPaintEvent& event);
        virtual void OnScrollwheel(wxMouseEvent& event);
        virtual void OnShowStreams(wxCommandEvent& event);
        virtual void OnTextEnter(wxCommandEvent& event);
        virtual void OnLeftUp( wxMouseEvent& event );
		void Zoom(float zLevel, wxCoord x, wxCoord y);
        void displaySDEM(wxDC *dc, wxProgressDialog *progDlg, float start, float end);
        void dispSingleStream(long thresh, wxDC *dc, wxProgressDialog *progDlg, float start, float end);
        void dispIZone(wxString filename);
};

#endif // LAHARPLOTMAIN_H
