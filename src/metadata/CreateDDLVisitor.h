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


#ifndef FR_CREATEDDLVISITOR_H
#define FR_CREATEDDLVISITOR_H

#include "sql/SqlTokenizer.h"
#include "metadata/MetadataItemVisitor.h"

class ProgressIndicator;

class CreateDDLVisitor: public MetadataItemVisitor
{
private:
    wxString sqlM;      // main

    wxString preSqlM;   // used for scripts to create entire database
    wxString postSqlM;  // sometimes it's the same as sqlM, sometimes not
    wxString grantSqlM; // grant statements at the very end (for easy diff)

    ProgressIndicator* progressIndicatorM;

protected:
    wxString getCommentOn(MetadataItem& metadataitem);

public:
    CreateDDLVisitor(ProgressIndicator* progressIndicator = 0);
    virtual ~CreateDDLVisitor();
    wxString getSql() const;
    wxString getPrefixSql() const;
    wxString getSuffixSql() const;

    virtual void visitCollation(Collation& collation);
    virtual void visitColumn(Column& column);
    virtual void visitDatabase(Database& database);
    virtual void visitDomain(Domain& domain);
    virtual void visitDBTrigger(DBTrigger& trigger);
    virtual void visitDDLTrigger(DDLTrigger& trigger);
    virtual void visitDMLTrigger(DMLTrigger& trigger);
    virtual void visitException(Exception& exception);
    virtual void visitForeignKey(ForeignKey& fk);
    virtual void visitFunctionSQL(FunctionSQL& function);
    virtual void visitGenerator(Generator& generator);
    virtual void visitIndex(Index& index);
    virtual void visitPrimaryKeyConstraint(PrimaryKeyConstraint& pk);
    virtual void visitPackage(Package& package);
    virtual void visitProcedure(Procedure& procedure);
    virtual void visitRole(Role& role);
    virtual void visitTable(Table& table);
    virtual void visitGTTable(GTTable& table);
    virtual void visitUDF(UDF& function);
    virtual void visitUniqueConstraint(UniqueConstraint& unq);
    virtual void visitUser(User& user);
    virtual void visitView(View& view);

};

#endif
