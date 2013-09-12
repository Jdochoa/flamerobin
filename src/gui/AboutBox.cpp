/*
  Copyright (c) 2004-2013 The FlameRobin Development Team

  Permission is hereby granted, free of charge, to any person obtaining
  a copy of this software and associated documentation files (the
  "Software"), to deal in the Software without restriction, including
  without limitation the rights to use, copy, modify, merge, publish,
  distribute, sublicense, and/or sell copies of the Software, and to
  permit persons to whom the Software is furnished to do so, subject to
  the following conditions:

  The above copyright notice and this permission notice shall be included
  in all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
  CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
  TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
  SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.


  $Id$

*/

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWindows headers
#ifndef WX_PRECOMP
  #include "wx/wx.h"
#endif

#ifdef wxUSE_ABOUTDLG
    #include <wx/aboutdlg.h>
#endif

#include <ibpp.h>

#include <boost/version.hpp>

#include "frversion.h"
#include "gui/AboutBox.h"
//-----------------------------------------------------------------------------
void showAboutBox(wxWindow* parent)
{
    wxString libs;
    libs.Printf(_("This tool uses IBPP library version %d.%d.%d.%d\nwxWidgets library version %d.%d.%d\nand Boost library version %d.%d.%d"),
        (IBPP::Version & 0xFF000000) >> 24,
        (IBPP::Version & 0x00FF0000) >> 16,
        (IBPP::Version & 0x0000FF00) >> 8,
        (IBPP::Version & 0x000000FF),
        wxMAJOR_VERSION,
        wxMINOR_VERSION,
        wxRELEASE_NUMBER,
        BOOST_VERSION / 100000,
        BOOST_VERSION / 100 % 1000,
        BOOST_VERSION % 100
    );

    wxString ver;
#if defined FR_GIT_HASH
    wxString githash(wxT(FR_GIT_HASH));
    ver.Printf(wxT("%d.%d.%d (git hash %s)"),
        FR_VERSION_MAJOR, FR_VERSION_MINOR, FR_VERSION_RLS, githash.c_str());
#else
    ver.Printf(wxT("%d.%d.%d"),
        FR_VERSION_MAJOR, FR_VERSION_MINOR, FR_VERSION_RLS);
#endif
#if wxUSE_UNICODE
    ver += wxT(" Unicode");
#endif

#if defined wxUSE_ABOUTDLG && (defined __WXMAC__ || defined __WXGTK__)

    wxUnusedVar(parent);

    wxAboutDialogInfo info;

    info.SetName(wxT("FlameRobin"));

    info.SetCopyright(_("Copyright (c) 2004-2013 FlameRobin Development Team"));

    info.SetVersion(ver);

    wxString msg(_("Database Administration Tool for Firebird RDBMS"));
    msg += wxT("\n\n");
    msg += libs;
    info.SetDescription(msg);

    // the following would prohibit the native dialog on Mac OS X
#if defined __WXGTK__
    info.SetWebSite(wxT("http://www.flamerobin.org"));
/* possible additions
    AddDeveloper(...);
    AddDocWriter(...);
    AddArtist(...);
    AddTranslator(...);
*/
#endif

    wxAboutBox(info);

#else

    wxString msg(wxT("FlameRobin ") + ver);

#if defined(_WIN64)
    msg += wxT(" (x64)");
#endif

    msg += wxT("\n");
    msg += _("Database administration tool for Firebird RDBMS");
    msg += wxT("\n\n");
    msg += libs;
    msg += wxT("\n\n");
    msg += _("Copyright (c) 2004-2013  FlameRobin Development Team");
    msg += wxT("\n");
    msg += _("http://www.flamerobin.org");

    wxMessageBox(msg, _("About FlameRobin"), wxOK | wxICON_INFORMATION, parent);

#endif
}
//-----------------------------------------------------------------------------
