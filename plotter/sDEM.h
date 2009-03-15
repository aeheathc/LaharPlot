/***************************************************************
 * Name:      sDEM.h
 * Purpose:   Handles SDEM files, and functions
 * Author:     ()
 * Created:   2009-02-19
 * Copyright:  ()
 * License:
 **************************************************************/

#ifndef SDEM_H
#define SDEM_H

#include <vector>
#include <wx/string.h>

class sDEM: public std::vector< std::vector<float> >
{
    public:
        sDEM(wxString filename);
        ~sDEM();
        void setFileName(wxString filename);
        const char* getFileName();
        float getMax();
        float getMin();
    private:
        void loadSDEM();
};

#endif
