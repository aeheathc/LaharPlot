#include <wx/progdlg.h>
#include <wx/filedlg.h>
#include <wx/filename.h>
#include <wx/msgdlg.h>
#include <wx/process.h>
#include <wx/utils.h>

#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <stdio.h>

#include "frameDEMDialog.h"

frameDEMDialog::frameDEMDialog( wxWindow* parent )
:
DEMDialog( parent )
{

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
		path.append( openDem.GetFilename() );

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
	if (!dtbValue.IsNull() && !otbValue.IsNull()) {
		// create option string
		wxString opts;
		opts = _("stream");
		opts.append(_(" -f "));
		opts.append(dtbValue);
		opts.append(_(" -o "));
		opts.append(otbValue);
		opts.append(_(" -v"));

		wxProcess process;
		//process.Redirect();

		//wxProgressDialog streamPBar (_("Running 'stream'"), _("doing something"), 100, this, wxPD_APP_MODAL | wxPD_AUTO_HIDE);
		wxExecute(opts, wxEXEC_SYNC, &process);
		/*wxInputStream* streamIn = process.GetInputStream();
		//wxChar wxCharIn(streamIn->GetC());
		while (process.Exists(process.GetPid())) {
			//wxCharIn = streamIn->GetC();
		}
		wxMessageBox(_("Get Here"), _("Does it"));
		delete streamIn;
		streamPBar.Update(100);*/
	}
}
