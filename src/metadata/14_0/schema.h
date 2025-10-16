/*
  Copyright (c) 2004-2025 The FlameRobin Development Team

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
#ifndef FR_SCHEMA_H
#define FR_SCHEMA_H

#include <vector>

#include <ibpp.h>

#include "metadata/MetadataClasses.h"
#include "metadata/metadataitem.h"

class ProgressIndicator;

class Schema : public MetadataItem,
    public std::enable_shared_from_this<Schema>
{
private:
    collectionMetadata collectionMetadataM;
    wxString characterSetNameM; 
    wxString characterSetSchemaNameM;


    void loadCollections(ProgressIndicator* progressIndicator);
protected:
    virtual void loadProperties();

    virtual void loadChildren();
    virtual void lockChildren();
    virtual void unlockChildren();
public:
    Schema(MetadataItem* parent, const wxString& name);
    Schema(DatabasePtr database, const wxString& name);
    ~Schema();

    MetadataCollectionBasePtrs::iterator begin();
    MetadataCollectionBasePtrs::iterator end();
    MetadataCollectionBasePtrs::const_iterator begin() const;
    MetadataCollectionBasePtrs::const_iterator end() const;

    virtual bool getChildren(std::vector<MetadataItem*>& temp);
    virtual size_t getChildrenCount() const;
    virtual void setCharacterSetName(const wxString& characterset);
    virtual wxString getCharacterSetName() const;
    virtual void setCharacterSetSchemaName(const wxString& characterset);
    virtual wxString getCharacterSetSchemaName() const;

    /*void getCollections(std::vector<MetadataItem*>& temp, bool system);
    MetadataCollectionPtrs getCollections();
    
    virtual wxString getDropSqlStatement() const;
    virtual wxString getAlterSqlStatement();
    virtual wxString getCreateSqlStatement();
    virtual DatabasePtr getDatabase() const;
    virtual const wxString getTypeName() const;*/
    virtual void acceptVisitor(MetadataItemVisitor* visitor);
};


class Schemas : public MetadataCollection<Schema>
{
protected:
    virtual void loadChildren();
public:
    Schemas(DatabasePtr database);

    virtual void acceptVisitor(MetadataItemVisitor* visitor);
    void load(ProgressIndicator* progressIndicator);
    virtual const wxString getTypeName() const;
};
#endif // FR_SCHEMA_H
