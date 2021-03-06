#ifndef __frameDEMDialog__
#define __frameDEMDialog__

/**
@file
Subclass of DEMDialog, which is generated by wxFormBuilder.
*/

#include "laharPlotMain.h"
#include "GUIFrame.h"

/** Implementing DEMDialog */
class frameDEMDialog : public DEMDialog
{
protected:
	// Handlers for DEMDialog events.
	void OnBrowseDEM( wxCommandEvent& event );
	void OnOutputBrowse( wxCommandEvent& event );
	void OnConvert( wxCommandEvent& event );
public:
	/** Constructor */
	frameDEMDialog( laharPlotFrame* parent );
};

#endif // __frameDEMDialog__
