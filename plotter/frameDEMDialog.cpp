#include <wx/filedlg.h>
#include <wx/filename.h>
#include <wx/platinfo.h>
#include <wx/msgdlg.h>

#include <string>

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
	// create option string
	wxString opts;
	opts = _(" -f ");
	opts.append(DEMTextBox->GetValue());
	opts.append(_(" -o "));
	opts.append(OutputTextBox->GetValue());

	// get OS and set proper binary
	wxPlatformInfo temp;
	wxString bina;
	wxString os = temp.GetOperatingSystemFamilyName();
	if (os == _("Windows"))
		bina = _("stream.exe");
	else if (os == _("Unix"))
		bina = _("stream");

	// append opts to binary string
	bina.append(opts);

	// execute command
	system(bina.mb_str(wxConvUTF8));
}
