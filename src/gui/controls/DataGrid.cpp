/*
Copyright (c) 2004, 2005, 2006 The FlameRobin Development Team

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

#ifdef __BORLANDC__
  #pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWindows headers
#ifndef WX_PRECOMP
  #include "wx/wx.h"
#endif

#include <wx/clipbrd.h>
#include <wx/fontdlg.h>
#include <wx/grid.h>
#include <wx/textbuf.h>
#include <wx/txtstrm.h>
#include <wx/wfstream.h>

#include "config/Config.h"
#include "core/FRError.h"
#include "gui/controls/DataGrid.h"
#include "gui/controls/DataGridTable.h"
#include "ugly.h"

//-----------------------------------------------------------------------------
DataGrid::DataGrid(wxWindow* parent, wxWindowID id)
    : wxGrid(parent, id)
{
    EnableEditing(false);
    SetColLabelValue(0, wxT(""));
    SetRowLabelSize(50);
    DisableDragRowSize();
    SetGridLineColour(*wxLIGHT_GREY);
    SetRowLabelAlignment(wxALIGN_RIGHT, wxALIGN_CENTRE);

    wxString s;
    wxFont f;
    if (config().getValue(wxT("DataGridFont"), s) && !s.empty())
    {
        f.SetNativeFontInfo(s);
        if (f.Ok())
            SetDefaultCellFont(f);
    }
    if (config().getValue(wxT("DataGridHeaderFont"), s) && !s.empty())
    {
        f.SetNativeFontInfo(s);
        if (f.Ok())
            SetLabelFont(f);
    }
    updateRowHeights();
}
//-----------------------------------------------------------------------------
DataGrid::~DataGrid()
{
}
//-----------------------------------------------------------------------------
void DataGrid::copyToClipboard(const wxString cbText)
{
    if (!wxTheClipboard->Open())
    {
        wxMessageBox(_("Cannot open clipboard"), _("Error"), wxOK|wxICON_EXCLAMATION);
        return;
    }

    if (!wxTheClipboard->SetData(new wxTextDataObject(cbText)))
        wxMessageBox(_("Cannot write to clipboard"), _("Error"), wxOK|wxICON_EXCLAMATION);
    wxTheClipboard->Close();
}
//-----------------------------------------------------------------------------
void DataGrid::fill(const wxString& connectionCharset)
{
    GridTable* table = dynamic_cast<GridTable*>(GetTable());
    if (!table)
        return;

    wxBusyCursor bc;
    BeginBatch();
    table->initialFetch(connectionCharset);

    for (int i = 1; i <= table->GetNumberCols(); i++)
    {
        wxGridCellAttr *ca = new wxGridCellAttr;
        ca->SetAlignment(
            (table->isNumericColumn(i)) ? wxALIGN_RIGHT : wxALIGN_LEFT,
            wxALIGN_CENTRE);
        SetColAttr(i-1, ca);
    }
    AutoSizeColumns();
    EndBatch();

    // event handler is only needed if not all rows have already been
    // fetched
    if (table->canFetchMoreRows())
        Connect(wxID_ANY, wxEVT_IDLE, wxIdleEventHandler(DataGrid::OnIdle));

#ifdef __WXGTK__
    // needed to make scrollbars show on large datasets
    Layout();
#endif
}
//-----------------------------------------------------------------------------
void DataGrid::showPopMenu(wxPoint cursorPos)
{
    wxMenu m(0);
    m.Append(ID_MENU_COPYTOCLIPBOARD, _("Copy"));
    m.Append(ID_MENU_COPYTOCLIPBOARDASINSERT, _("Copy as INSERT statements"));
    m.Append(ID_MENU_SAVEASHTML, _("Save as HTML file..."));
    m.AppendSeparator();

    m.Append(ID_MENU_LABELFONT, _("Set header font"));
    m.Append(ID_MENU_CELLFONT, _("Set cell font"));
    PopupMenu(&m, cursorPos);
}
//-----------------------------------------------------------------------------
void DataGrid::updateRowHeights()
{
    // HACK alert: this is taken straight from wxWidgets grid.cpp...
#if defined(__WXMOTIF__) || defined(__WXGTK__)
    int extraHeight = 8;
#else
    int extraHeight = 4;
#endif

    wxScreenDC dc;
    // adjust height of grid header row (extra 2 pixels for border)
    dc.SetFont(GetLabelFont());
    SetColLabelSize(dc.GetCharHeight() + extraHeight + 2);

    // adjust height of rows, and make grid scroll by that amount
    dc.SetFont(GetDefaultCellFont());
    int h = dc.GetCharHeight() + extraHeight;
    SetRowMinimalAcceptableHeight(h);
    SetDefaultRowSize(h, true);
    SetScrollLineY(h);
}
//-----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(DataGrid, wxGrid)
    EVT_CONTEXT_MENU(DataGrid::OnContextMenu)

#ifdef __WXGTK__
    EVT_MOUSEWHEEL(DataGrid::OnMouseWheel)
    EVT_SCROLLWIN_THUMBRELEASE(DataGrid::OnThumbRelease)
#endif

    EVT_GRID_CELL_RIGHT_CLICK(DataGrid::OnGridCellRightClick)
    EVT_GRID_LABEL_RIGHT_CLICK(DataGrid::OnGridLabelRightClick)
    EVT_MENU(DataGrid::ID_MENU_CELLFONT, DataGrid::OnMenuCellFont)
    EVT_MENU(DataGrid::ID_MENU_COPYTOCLIPBOARD, DataGrid::OnMenuCopyToCB)
    EVT_UPDATE_UI(DataGrid::ID_MENU_COPYTOCLIPBOARD, DataGrid::OnMenuUpdateIfHasSelection)
    EVT_MENU(DataGrid::ID_MENU_COPYTOCLIPBOARDASINSERT, DataGrid::OnMenuCopyToCBAsInsert)
    EVT_UPDATE_UI(DataGrid::ID_MENU_COPYTOCLIPBOARDASINSERT, DataGrid::OnMenuUpdateIfHasSelection)
    EVT_MENU(DataGrid::ID_MENU_LABELFONT, DataGrid::OnMenuLabelFont)
    EVT_MENU(DataGrid::ID_MENU_SAVEASHTML, DataGrid::OnMenuSaveAsHTML)
    EVT_UPDATE_UI(DataGrid::ID_MENU_SAVEASHTML, DataGrid::OnMenuUpdateIfHasSelection)
END_EVENT_TABLE()
//-----------------------------------------------------------------------------
void DataGrid::OnContextMenu(wxContextMenuEvent& event)
{
    showPopMenu(event.GetPosition());
}
//-----------------------------------------------------------------------------
void DataGrid::OnThumbRelease(wxScrollWinEvent& event)
{
    wxIdleEvent dummy;
    OnIdle(dummy);
    event.Skip();
}
//-----------------------------------------------------------------------------
void DataGrid::OnMouseWheel(wxMouseEvent& event)
{
    int wheelrotation = event.GetWheelRotation();
    int x, y;
    GetViewStart(&x, &y);
    if (wheelrotation < 0)
        y += 5;
    else
        y -= 5;
    Scroll(x,y);
    AdjustScrollbars();
}
//-----------------------------------------------------------------------------
void DataGrid::OnGridCellRightClick(wxGridEvent& event)
{
    showPopMenu(event.GetPosition());
}
//-----------------------------------------------------------------------------
void DataGrid::OnGridLabelRightClick(wxGridEvent& WXUNUSED(event))
{
    showPopMenu(ScreenToClient(::wxGetMousePosition()));
}
//-----------------------------------------------------------------------------
void DataGrid::OnIdle(wxIdleEvent& event)
{
    FR_TRY

    GridTable* table = dynamic_cast<GridTable*>(GetTable());
    // disconnect event handler if nothing more to be done, will be
    // re-registered on next successfull execution of select statement
    if (!table || !table->canFetchMoreRows())
    {
        Disconnect(wxID_ANY, wxEVT_IDLE);
        return;
    }
    // fetch more rows until row cache is filled or timeslice is spent, and
    // request another wxEVT_IDLE event if row cache has not been filled
    if (table->needsMoreRowsFetched())
    {
        table->fetch();
        if (table->needsMoreRowsFetched())
            event.RequestMore();
        AdjustScrollbars();
    }

    FR_CATCH
}
//-----------------------------------------------------------------------------
void DataGrid::OnMenuCellFont(wxCommandEvent& WXUNUSED(event))
{
    wxFont f = ::wxGetFontFromUser(this, GetDefaultCellFont());
    if (f.Ok())
    {
        SetDefaultCellFont(f);
        config().setValue(wxT("DataGridFont"), f.GetNativeFontInfoDesc());
        updateRowHeights();
        ForceRefresh();
    }
}
//-----------------------------------------------------------------------------
void DataGrid::OnMenuCopyToCB(wxCommandEvent& WXUNUSED(event))
{
    wxBusyCursor cr;
    wxString sRows;
    bool all = true;
    for (int i = 0; i < GetNumberRows(); i++)
    {
        wxString sRow;
        for (int j = 0; j < GetNumberCols(); j++)
        {
            if (IsInSelection(i, j))
            {
                // TODO: - align fields in columns ?
                //       - fields with multiline strings don't really work...
                if (!sRow.IsEmpty())
                    sRow += wxT("\t");
                sRow += GetCellValue(i, j);
            }
            else
                all = false;
        }
        if (!sRow.IsEmpty())
            sRows += sRow + wxTextBuffer::GetEOL();
    }
    if (!sRows.IsEmpty())
        copyToClipboard(sRows);

    GridTable* table = dynamic_cast<GridTable*>(GetTable());
    if (all && table && table->canFetchMoreRows())
        wxMessageBox(_("There are more rows to fetch from database"),
        _("Only the rows in the grid are copied"), wxOK|wxICON_INFORMATION);
}
//-----------------------------------------------------------------------------
void DataGrid::OnMenuCopyToCBAsInsert(wxCommandEvent& WXUNUSED(event))
{
    wxBusyCursor cr;

    GridTable* table = dynamic_cast<GridTable*>(GetTable());
    if (!table)
        return;
    // TODO: - using one table is not correct for JOINs or sub-SELECTs
    //       -> should probably refuse to work if not from one table
    //       - should probably refuse to create INSERT for "[...]"
    wxString tableName = table->getTableName();
    // NOTE: this has been reworked (compared to myDataGrid), because
    //       not all rows have necessarily the same fields selected
    wxString sRows;
    bool all = true;
    for (int i = 0; i < GetNumberRows(); i++)
    {
        wxString sCols;
        wxString sValues;
        for (int j = 0; j < GetNumberCols(); j++)
        {
            if (IsInSelection(i, j))
            {
                if (!sCols.IsEmpty())
                    sCols += wxT(", ");
                // NOTE: preloading the column names into a local wxString
                //       array might be a worthy optimization
                sCols += GetColLabelValue(j);
                if (!sValues.IsEmpty())
                    sValues += wxT(", ");
                sValues += table->getCellValueForInsert(i, j);
            }
            else
                all = false;
        }
        if (!sCols.IsEmpty())
        {
            sRows += wxT("INSERT INTO ") + tableName + wxT(" (");
            sRows += sCols;
            sRows += wxT(") VALUES (");
            sRows += sValues;
            sRows += wxT(");");
            sRows += wxTextBuffer::GetEOL();
        }
    }
    if (!sRows.IsEmpty())
        copyToClipboard(sRows);

    if (all && table->canFetchMoreRows())
        wxMessageBox(_("There are more rows to fetch from database"),
        _("Only the rows in the grid are copied"), wxOK|wxICON_INFORMATION);
}
//-----------------------------------------------------------------------------
void DataGrid::OnMenuLabelFont(wxCommandEvent& WXUNUSED(event))
{
    wxFont f = ::wxGetFontFromUser(this, GetLabelFont());
    if (f.Ok())
    {
        SetLabelFont(f);
        config().setValue(wxT("DataGridHeaderFont"),
            f.GetNativeFontInfoDesc());
        updateRowHeights();
        AutoSizeColumns();
        ForceRefresh();
    }
}
//-----------------------------------------------------------------------------
void DataGrid::OnMenuSaveAsHTML(wxCommandEvent& WXUNUSED(event))
{
    wxString fname = ::wxFileSelector(_("Save data in selected cells as"),
        wxEmptyString, wxEmptyString, wxT("*.html"),
        _("HTML files (*.html)|*.html|All files (*.*)|*.*"),
        wxSAVE|wxCHANGE_DIR|wxOVERWRITE_PROMPT, this);
    if (fname.empty())
        return;

    // find all columns that have at least one cell selected
    std::vector<bool> selCols;
    int cols = GetNumberCols();
    selCols.reserve(cols);
    for (int i = 0; i < cols; i++)
        selCols.push_back(false);

    int rows = GetNumberRows();
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            if (IsInSelection(i, j))
                selCols[j] = true;
        }
    }

    // write HTML file
    wxFileOutputStream fos(fname);
    if (!fos.Ok()) // TODO: report error
        return;
    wxTextOutputStream outStr(fos);

    outStr.WriteString(wxT("<html><body><table bgcolor=black cellspacing=1 cellpadding=3 border=0><tr>\n"));
    // write table header
    outStr.WriteString(wxT("<tr>"));
    for (int i = 0; i < cols; i++)
    {
        if (selCols[i])
        {
            outStr.WriteString(wxT("<td nowrap><font color=white><b>"));
            outStr.WriteString(GetColLabelValue(i));
            outStr.WriteString(wxT("</b></font></td>"));
        }
    }
    outStr.WriteString(wxT("</tr>\n"));

    GridTable* table = dynamic_cast<GridTable*>(GetTable());
    // write table data
    for (int i = 0; i < rows; i++)
    {
        // check if at least one cell in this row selected
        int selcnt = 0;
        std::vector<bool> selCells(selCols);
        for (int j = 0; j < cols; j++)
        {
            if (IsInSelection(i, j))
                selcnt++;
            else // skip cell even if selection contains this column
                selCells[j] = false;
        }
        if (!selcnt)
            continue;

        outStr.WriteString(wxT("<tr bgcolor=white>"));
        // write data for selected grid cells only
        for (int j = 0; j < cols; j++)
        {
            if (!selCols[j])
                continue;
            if (!selCells[j])
                outStr.WriteString(wxT("<td bgcolor=silver>"));
            else if (table->isNullCell(i, j))
                outStr.WriteString(wxT("<td><font color=red>NULL</font>"));
            else
            {
                outStr.WriteString(wxT("<td"));
                int halign, valign;
                GetCellAlignment(i, j, &halign, &valign);
                if (halign == wxALIGN_RIGHT)
                    outStr.WriteString(wxT(" align=right"));
                outStr.WriteString(wxT(" nowrap>") + GetCellValue(i, j));
            }
            outStr.WriteString(wxT("</td>"));
        }
        outStr.WriteString(wxT("</tr>\n"));
    }
    outStr.WriteString(wxT("</table></body></html>\n"));
}
//-----------------------------------------------------------------------------
void DataGrid::OnMenuUpdateIfHasSelection(wxUpdateUIEvent& event)
{
    event.Enable(IsSelection());
}
//-----------------------------------------------------------------------------