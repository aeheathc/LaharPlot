#include <wx/progdlg.h>
#include <wx/filedlg.h>
#include <wx/filename.h>
#include <wx/msgdlg.h>
#include <wx/process.h>
#include <wx/utils.h>

#include "frameDEMDialog.h"

laharPlotFrame* par;

frameDEMDialog::frameDEMDialog( laharPlotFrame* parent )
:
DEMDialog( parent )
{
	par = parent;
}

void frameDEMDialog::OnBrowseDEM( wxCommandEvent& event )
{
	wxFileDialog openDem (this, _("Choose a File"), _(""), _(""), _("All Files (*.*)|*.*"), wxOPEN, wxDefaultPosition);
	if ( openDem.ShowModal() == wxID_OK )
	{
		// get path and filename
		wxString path;
		path.append( openDem.GetDirectory() );
		path.append( wxFileName::GetPathSeparator() );
		wxString pathTwo = path;
		pathTwo.append(_("new"));
		path.append( openDem.GetFilename() );

		if (OutputTextBox->GetValue() == _(""))
			OutputTextBox->SetValue(pathTwo);

		DEMTextBox->SetValue(path);
	}
}

void frameDEMDialog::OnOutputBrowse( wxCommandEvent& event )
{
	wxFileDialog outputName (this, _("Choose a File"), _(""), _(""), _("All Files (*.*)|*.*"), wxSAVE, wxDefaultPosition);
	if ( outputName.ShowModal() == wxID_OK )
	{
		// get path and filename
		wxString path;
		path.append( outputName.GetDirectory() );
		path.append( wxFileName::GetPathSeparator() );
		path.append( outputName.GetFilename() );

		OutputTextBox->SetValue(path);
	}
}

void frameDEMDialog::OnConvert( wxCommandEvent& event )
{
	wxString dtbValue = DEMTextBox->GetValue();
	wxString otbValue = OutputTextBox->GetValue();
	if (!dtbValue.IsNull() && !otbValue.IsNull())
	{
		// create option string
		wxString opts;
		opts = _("stream");
		opts.append(_(" -f "));
		opts.append(dtbValue);
		opts.append(_(" -o "));
		opts.append(otbValue);
		opts.append(_(" -l progress -e"));

		wxProcess* process = wxProcess::Open(opts);
		process->Redirect();
		wxInputStream* streamIn = process->GetInputStream();

		wxProgressDialog streamPBar (_("Running 'stream'"), _("Filling Sinkholes... (May take a while.)"), 100, this, wxPD_APP_MODAL | wxPD_AUTO_HIDE);
		streamPBar.SetSize(300, 120);
		wxChar charIn;
		long xsize, ysize, ft;

		if (streamIn != NULL) {
			int eq = 0, i = 0, j = 0, k = 0;
			while (!streamIn->Eof())
			{
				charIn = streamIn->GetC();
				/*if (charIn == '=')
				{
					eq++;
					if (eq == 1 || eq == 2)
					{
						wxString num = _("");
						charIn = streamIn->GetC();
						while (charIn != ',')
						{
							num.append(charIn);
							charIn = streamIn->GetC();
						}
						if (eq == 1) num.ToLong(&xsize);
						else
						{
							num.ToLong(&ysize);
							ft = 2 * (ysize + xsize - 2);
						}
					}
				}*/
				if (charIn == '-')
				{
					i++;
					if (i == 1) streamPBar.Update(0, _("Building DEM..."));
				}
				if (charIn == '.')
				{
					j++;
					if (j == 1) streamPBar.Update(20, _("Finding Flow Direction..."));
				}
				if (charIn == '#')
				{
					k++;
					if (k == 1) streamPBar.Update(50,_("Finding Flow Totals..."));
				}
			}
		}
		streamIn->~wxInputStream();
		delete process;
		streamPBar.Update(100);

		//par->SetFile(otbValue);

		EndDialog(1);
	}
}
