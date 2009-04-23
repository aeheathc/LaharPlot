#ifndef __frameZoneDialog__
#define __frameZoneDialog__

/**
@file
Subclass of ZoneDialog, which is generated by wxFormBuilder.
*/

#include "GUIFrame.h"
#include "laharPlotMain.h"

/** Implementing ZoneDialog */
class frameZoneDialog : public ZoneDialog
{
protected:
	// Handlers for ZoneDialog events.
	void OnBrowseSimple( wxCommandEvent& event );
	void OnBrowseOutput( wxCommandEvent& event );
	void OnCancel( wxCommandEvent& event );
	void OnRun( wxCommandEvent& event );
	void OnShowDEM( wxCommandEvent& event );

public:
	/** Constructor */
	frameZoneDialog( laharPlotFrame* parent );
	void SetStartX(int x);
	void SetStartY(int y);
};

#endif // __frameZoneDialog__
