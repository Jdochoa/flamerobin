/*
  Copyright (c) 2004-2022 The FlameRobin Development Team

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
*/

#pragma once

#ifndef FR_COLLATION_H
#define FR_COLLATION_H

#include "metadata/collection.h"
#include "metadata/CharacterSet.h"
//#include "metadata/privilege.h"

class Collations;

class Collation : public MetadataItem
{
private:
    int attributesM;
    wxString baseCollectionNameM;
    wxString specificAttibutesM;

    friend class Collations;
protected:
    static std::string getLoadStatement(bool list);
    void loadProperties(IBPP::Statement& statement, wxMBConv* converter);
    virtual void loadProperties();

public:
    Collation();
    Collation(DatabasePtr database, const wxString& name, int id = -1);
    Collation(MetadataItem* parent, const wxString& name, int id = -1);
    ~Collation();

    CharacterSet* getCharacterSet() const;

    int getAttributes();
    void setAttributes(int& attributes);
    wxString getBaseCollectionName();
    void setBaseCollectionName(wxString baseName);
    wxString getSpecificAttibutes();
    void setSpecificAttributes(wxString attibutes);

    virtual void acceptVisitor(MetadataItemVisitor* visitor);
};


class Collations : public MetadataCollection<Collation>
{
protected:
    virtual void loadChildren();
public:
    Collations(DatabasePtr database);

    virtual void acceptVisitor(MetadataItemVisitor* visitor);
    void load(ProgressIndicator* progressIndicator);
    virtual const wxString getTypeName() const;
};


class UserCollations : public MetadataCollection<Collation>
{
protected:
    virtual void loadChildren();
public:
    UserCollations(DatabasePtr database);

    virtual void acceptVisitor(MetadataItemVisitor* visitor);
    void load(ProgressIndicator* progressIndicator);
    virtual const wxString getTypeName() const;
};

#endif // FR_COLLATION_H
