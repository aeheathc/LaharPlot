/***************************************************************
 * Name:      laharPlotApp.cpp
 * Purpose:   Code for Application Class
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

#include "laharPlotApp.h"
#include "laharPlotMain.h"

IMPLEMENT_APP(laharPlotApp)

bool laharPlotApp::OnInit()
{
    laharPlotFrame* frame = new laharPlotFrame(0L);

    frame->Show();

    return true;
}
