/***************************************************************
 * Name:      tsv.h
 * Purpose:   Handles tsv files, and functions
 * Author:     ()
 * Created:   2009-03-16
 * Copyright:  ()
 * License:
 **************************************************************/

#ifndef TSV_H
#define TSV_H

#include <list>
#include <wx/string.h>
#include <wx/progdlg.h>

class tsv: public std::list< std::list<float> >
{
    public:
        tsv(wxString filename);
        float getMin();
        float getMax();
        virtual ~tsv();
        virtual void setFileName(wxString filename, wxProgressDialog *progDlg);
        virtual const char* getFileName();
    private:
        void loadtsv(wxProgressDialog *progDlg);
        void findLength();
};

#endif
