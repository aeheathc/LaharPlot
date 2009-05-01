#include <wx/progdlg.h>
#include <wx/filedlg.h>
#include <wx/filename.h>
#include <wx/msgdlg.h>
#include <wx/process.h>
#include <wx/utils.h>
#include <wx/stream.h>

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
		if (process != NULL)
		{
			process->Redirect();
			wxInputStream* streamIn = process->GetInputStream();
			wxStreamBuffer streamBuf(*streamIn, wxStreamBuffer::read);

			wxProgressDialog streamPBar (_("Running 'stream'"), _("Filling Sinkholes... (May take a while.)"), 100, this, wxPD_APP_MODAL | wxPD_AUTO_HIDE);
			streamPBar.SetSize(300, 120);
			wxChar charIn;
			long xsize, ysize, ft;

			if (streamIn != NULL) {
				int i = 0, j = 0, k = 0;
				while (process->IsInputOpened())
				{
					charIn = streamBuf.GetChar();
					if (charIn == '-') i++;
					else if (charIn == '.') j++;
					else if (charIn == '#') k++;

					if (i > 0 && j == 0) streamPBar.Update(0, _("Building DEM..."));
					else if (i > 0 && j < 0) streamPBar.Update(j * (50.0 / i), _("Finding Flow Direction..."));
					else if (i > 0 && j == i) streamPBar.Update(j * (50.0 / i), _("Finding Flow Totals..."));
				}
			}
			delete process;
			streamPBar.Update(100);

			//par->SetFile(otbValue);

			EndDialog(1);
		}
	}
}
