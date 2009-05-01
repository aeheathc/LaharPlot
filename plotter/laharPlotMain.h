/* LaharPlot is an application that can calculate and map out the inundation
   zone of a lahar (volcanic mudslide) given an elevation map and some starting
   parameters about the lahar itself.
   Copyright 2009: Anthony Heathcoat, Jason Anderson, Tim Root

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/***************************************************************
 * Name:      laharPlotMain.h
 * Purpose:   Defines Application Frame
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
