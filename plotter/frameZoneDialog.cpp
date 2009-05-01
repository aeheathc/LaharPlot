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

#include <wx/progdlg.h>
#include <wx/filedlg.h>
#include <wx/filename.h>
#include <wx/msgdlg.h>
#include <wx/process.h>

#include "frameZoneDialog.h"

laharPlotFrame* zonePar;

frameZoneDialog::frameZoneDialog( laharPlotFrame* parent )
:
ZoneDialog( parent )
{
	zonePar = parent;
}

void frameZoneDialog::OnBrowseSimple( wxCommandEvent& event )
{
	wxFileDialog openDem (this, _("Choose a File"), _(""), _(""), _("TSV Files (*.tsv)|*.tsv"), wxOPEN, wxDefaultPosition);
	if ( openDem.ShowModal() == wxID_OK )
	{
		wxString path;
		path.append( openDem.GetFilename() );

		int i;
		for (i = path.length(); path.substr(i,1) != _("-"); i--);
		path = path.substr(0, i);

		SimpleBox->SetValue(path);

		OutputBox->SetValue(openDem.GetDirectory());
	}
}

void frameZoneDialog::OnBrowseOutput( wxCommandEvent& event )
{
	wxFileDialog outputName (this, _("Choose a File"), _(""), _(""), _("All Files (*.*)|*.*"), wxCHANGE_DIR, wxDefaultPosition);
	if ( outputName.ShowModal() == wxID_OK )
	{
		// get path and filename
		wxString path;
		path.append( outputName.GetDirectory() );
		path.append( wxFileName::GetPathSeparator() );

		OutputBox->SetValue(path);
	}
}

void frameZoneDialog::OnCancel( wxCommandEvent& event )
{
	EndDialog(1);
}

void frameZoneDialog::OnRun( wxCommandEvent& event )
{
	wxString sname = SimpleBox->GetValue();
	wxString odir = OutputBox->GetValue();
	wxString vols = VolumeBox->GetValue();
	long sx, sy;
	if (!sname.IsNull() && !odir.IsNull() && !vols.IsNull()
			&& startx->GetValue().ToLong(&sx) && starty->GetValue().ToLong(&sy))
	{
		bool doCont = true;
		int temp = 0;
		long tem;
		for (int i = 0; i < vols.length() && doCont; i++)
		{
			if (vols.substr(i, 1) == _(" "))
			{
				doCont = wxString(vols.substr(temp, i - temp)).ToLong(&tem);
				i++;
				temp = i;
			}
			if (i == vols.length() -  1)
			{
				doCont = wxString(vols.substr(temp, vols.length() - temp)).ToLong(&tem);
			}

		}
		if (doCont)
		{
			wxString opts;
			opts.append(_("zone"));
			opts.append(_(" -n "));
			opts.append(sname);
			opts.append(_(" -p "));
			opts.append(odir);
			opts.append(_(" -x "));
			opts << sx;
			opts.append(_(" -y "));
			opts << sy;
			opts.append(_(" -v "));
			opts.append(vols);

			wxProgressDialog zonePBar (_("Running 'zone'"), _(""), 100, this, wxPD_APP_MODAL | wxPD_AUTO_HIDE);
			zonePBar.SetSize(300, 120);
			wxProcess* process = wxProcess::Open(opts);
			process->Redirect();
			wxInputStream* streamIn = process->GetInputStream();

			while (process->IsInputOpened()) {
				streamIn->GetC();
			}

			streamIn->~wxInputStream();
			delete process;
			zonePBar.Update(100);

			EndDialog(1);
		}
	}
}

void frameZoneDialog::OnShowDEM( wxCommandEvent& event )
{
	Hide();
}

void frameZoneDialog::SetStartX(int x)
{
	wxString val (_(""));
	val << x;
	startx->SetValue(val);
}

void frameZoneDialog::SetStartY(int y)
{
	wxString val (_(""));
	val << y;
	starty->SetValue(val);
}
