/*
  Copyright (c) 2004-2024 The FlameRobin Development Team

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

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWindows headers
#ifndef WX_PRECOMP
#include "wx/wx.h"
#include "schema.h"
#endif

// needed for platform independent EOL
#include <wx/textbuf.h>

#include <string>

#include <ibpp.h>

#include "core/FRError.h"
#include "core/StringUtils.h"
#include "engine/MetadataLoader.h"
#include "core/ProgressIndicator.h"
#include "frutils.h"
#include "gui/AdvancedMessageDialog.h"
#include "metadata/14_0/schema.h"
#include "metadata/MetadataItemVisitor.h"

//#include "metadata/Charset.h"
#include "metadata/Collation.h"
#include "metadata/domain.h"
#include "metadata/exception.h"
#include "metadata/function.h"
#include "metadata/generator.h"
#include "metadata/Index.h"
#include "metadata/package.h"
#include "metadata/procedure.h"
#include "metadata/role.h"
#include "metadata/table.h"
#include "metadata/trigger.h"
#include "metadata/view.h"

Schema::Schema(MetadataItem* parent, const wxString& name)
    : MetadataItem(ntSchema, parent, name),
    characterSetNameM(wxEmptyString), characterSetSchemaNameM(wxEmptyString)
{
    ensurePropertiesLoaded();
    if (isSystem() )
    {
        collectionMetadataM.push_back(std::make_shared<SysDomains>(getDatabase()));
        collectionMetadataM.push_back(std::make_shared<SysIndices>(getDatabase()));
        collectionMetadataM.push_back(std::make_shared<SysPackages>(getDatabase()));
        collectionMetadataM.push_back(std::make_shared<SysRoles>(getDatabase()));
        collectionMetadataM.push_back(std::make_shared<SysTables>(getDatabase()));
    }
    else
    {
        //collectionMetadataM.push_back(std::make_shared<Charset>());
        collectionMetadataM.push_back(std::make_shared<Collations14>(this));
        collectionMetadataM.push_back(std::make_shared<Domains>(getDatabase()));
        collectionMetadataM.push_back(std::make_shared<Exceptions>(getDatabase()));
        collectionMetadataM.push_back(std::make_shared<FunctionSQLs>(getDatabase()));
        collectionMetadataM.push_back(std::make_shared<Generators>(getDatabase()));
        collectionMetadataM.push_back(std::make_shared<GTTables>(getDatabase()));
        collectionMetadataM.push_back(std::make_shared<UDFs>(getDatabase()));
        collectionMetadataM.push_back(std::make_shared<Indices>(getDatabase()));
        collectionMetadataM.push_back(std::make_shared<Packages>(getDatabase()));
        collectionMetadataM.push_back(std::make_shared<Procedures>(getDatabase()));
        collectionMetadataM.push_back(std::make_shared<Tables>(getDatabase()));
        collectionMetadataM.push_back(std::make_shared<DDLTriggers>(getDatabase()));
        collectionMetadataM.push_back(std::make_shared<DBTriggers>(getDatabase()));
        collectionMetadataM.push_back(std::make_shared<DMLTriggers>(getDatabase()));
        collectionMetadataM.push_back(std::make_shared<Views>(getDatabase()));
    }
}

Schema::Schema(DatabasePtr database, const wxString& name)
    : Schema(database.get(), name)
{
}

Schema::~Schema()
{
}

MetadataCollectionBasePtrs::iterator Schema::begin()
{
    return collectionMetadataM.begin();
}
MetadataCollectionBasePtrs::iterator Schema::end()
{
    return collectionMetadataM.end();
}
MetadataCollectionBasePtrs::const_iterator Schema::begin() const
{
    return collectionMetadataM.begin();
}
MetadataCollectionBasePtrs::const_iterator Schema::end() const
{
    return collectionMetadataM.end();
}

bool Schema::getChildren(std::vector<MetadataItem*>& temp)
{
    if (collectionMetadataM.empty())
        return false;

    //collectionMetadataM.getChildren(temp);
    //std::transform(collectionMetadataM.begin(), collectionMetadataM.end(),
    //    std::back_inserter(temp), std::mem_fn(&ItemType::get));
    for (const auto& m : collectionMetadataM) {
        temp.push_back(&(*m));
    }

    return !collectionMetadataM.empty() ;
}

size_t Schema::getChildrenCount() const
{
    //return methodsM.size();
    return collectionMetadataM.size();
}

void Schema::setCharacterSetName(const wxString& characterset)
{
    characterSetNameM = characterset;
}

wxString Schema::getCharacterSetName() const
{
    return characterSetNameM;
}

void Schema::setCharacterSetSchemaName(const wxString& characterset)
{
    characterSetSchemaNameM = characterset;
}

wxString Schema::getCharacterSetSchemaName() const
{
    return characterSetSchemaNameM;
}

void Schema::loadProperties()
{
    DatabasePtr db = getDatabase();

    MetadataLoader* loader = db->getMetadataLoader();
    MetadataLoaderTransaction tr(loader);
    wxMBConv* converter = db->getCharsetConverter();

    IBPP::Statement& st1 = loader->getStatement(
        "select rdb$schema_name, "
        "rdb$character_set_name, "
        "rdb$character_set_schema_name, "
        "rdb$sql_security, "
        "rdb$system_flag, "
        "rdb$description "
        "from rdb$schemas "
        "where rdb$schema_name = ? "
    );
    st1->Set(1, wx2std(getName_(), converter));
    st1->Execute();
    if (!st1->Fetch())
        throw FRError(_("User not found: ") + getName_());

    setPropertiesLoaded(false);
    std::string lstr;
    bool lbool;

    if (st1->IsNull(2))
        lstr = "";
    else
        st1->Get(2, lstr);
    setCharacterSetName(lstr);

    if (st1->IsNull(3))
        lstr = "";
    else
        st1->Get(3, lstr);
    setCharacterSetSchemaName(lstr);

    if (st1->IsNull(5))
        lbool = false;
    else
        st1->Get(5, lbool);
    setIsSystem(lbool);
    setPropertiesLoaded(true);
    notifyObservers();
}

void Schema::loadChildren()
{
    loadCollections(0);
}

void Schema::lockChildren()
{
    for (const auto& m : collectionMetadataM) {
        m->lockSubject();
    }
}

void Schema::unlockChildren()
{
    for (const auto& m : collectionMetadataM) {
        m->unlockSubject();
    }
}

void Schema::loadCollections(ProgressIndicator* progressIndicator)
{
    struct ProgressIndicatorHelper
    {
    private:
        ProgressIndicator* progressIndicatorM;
    public:
        ProgressIndicatorHelper(ProgressIndicator* progressIndicator)
            : progressIndicatorM(progressIndicator) {
        }
        void init(wxString collectionName, int stepsTotal, int currentStep)
        {
            if (progressIndicatorM)
            {
                wxString msg(wxString::Format(_("Loading %s..."),
                    collectionName.c_str()));
                progressIndicatorM->initProgress(msg, stepsTotal, currentStep, 1);
            }
        }
    };
    
    const int collectionCount = collectionMetadataM.capacity();
    std::string loadStmt;
    ProgressIndicatorHelper pih(progressIndicator);

    
    MetadataLoader* loader = getDatabase()->getMetadataLoader();
    MetadataLoaderTransaction tr(loader);
    SubjectLocker lock(this);
    for (const auto& m : collectionMetadataM) {
        
        pih.init(m->getTypeName(), collectionCount, 0);
        m->load(progressIndicator);
    }
}

void Schema::acceptVisitor(MetadataItemVisitor* visitor)
{
    visitor->visitSchema(*this);
}


// Schemas collection
Schemas::Schemas(DatabasePtr database)
    : MetadataCollection<Schema>(ntSchemas, database, _("Schemas"))
{
}

void Schemas::acceptVisitor(MetadataItemVisitor* visitor)
{
    visitor->visitSchemas(*this);
}

void Schemas::load(ProgressIndicator* progressIndicator)
{
    DatabasePtr db = getDatabase();
    wxString stmt = "select rdb$schema_name from rdb$schemas ";
    stmt << " order by rdb$schema_name ";
    setItems(db->loadIdentifiers(stmt, progressIndicator));
}

void Schemas::loadChildren()
{
    load(0);
}

const wxString Schemas::getTypeName() const
{
    return "SCHEMA_COLLECTION";
}
