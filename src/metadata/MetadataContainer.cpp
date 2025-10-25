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
#endif


#include <chrono>
#include <thread>

#include "MetadataContainer.h"
#include "metadata/domain.h"
#include "metadata/table.h"
#include "metadata/view.h"
#include "metadata/14_0/schema.h"

MetadataContainer::MetadataContainer() 
{ 
    collectionsM.clear(); 
}

void MetadataContainer::addCollection(const std::shared_ptr<MetadataCollectionBase>& collection) 
{
    collectionsM.push_back(collection);
}

void MetadataContainer::getCollections(std::vector<MetadataItem*>& temp, bool system)
{
    for (const auto& col : collectionsM) {
        if (col){
            if (/*system && */col->showCollection())
                temp.push_back(&(*col));
        }
    }
}
MetadataItemPtr MetadataContainer::findByName(const wxString& name) const 
{
    for (const auto& col : collectionsM) {
        if (col) {
            return col->findByName_(name);
        }
    }
    return nullptr;
}

MetadataItemPtr MetadataContainer::findByTypeAndName(NodeType nt, const wxString& name) 
{
    for (const auto& col : collectionsM) {
        if (col->getType() == nt) {
            return col->findByName_(name);
        }
    }
    return nullptr;
}

MetadataItemPtr MetadataContainer::findByTypeAndId(NodeType nt, int id)
{
    for (const auto& col : collectionsM) {
        if (col->getType() == nt) {
            return col->findByMetadataId_(id);
        }
    }
    return nullptr;
}

std::vector<wxString> MetadataContainer::getAllNames() const 
{
    std::vector<wxString> list;
    for (const auto& col : collectionsM) {
        col->forEachItem([&list](const MetadataItemPtr& item) {
            if (item)
                list.push_back(item->getName_());
            }
        );
    }
    return list;
}

void MetadataContainer::getIdentifiers(std::vector<Identifier>& temp)
{
    forEachCollection([&temp](const MetadataCollectionBasePtr& item) {
        if (item) {
            item->forEachItem([&temp](const MetadataItemPtr& item) {
                if (item) {
                    temp.push_back(item->getIdentifier());
                }
                });
        }
        }
    );
}

DomainPtr MetadataContainer::getDomain(const wxString& name)
{
    if (MetadataItem::hasSystemPrefix(name))
        return getCollectionPtr<SysDomainsPtr, SysDomains>(ntSysDomains)->getDomain(name);
    else
        return getCollectionPtr<DomainsPtr, Domains>(ntDomains)->getDomain(name);    
}

RelationPtr MetadataContainer::findRelation(const Identifier& name)
{
    wxString s(name.get());
    TablePtr t;
    t = getCollectionPtr<TablesPtr, Tables>(ntTables)->findByName(s);
    if (t)
        return std::static_pointer_cast<Relation>(t);
    else {
        t = getCollectionPtr<GTTablesPtr, GTTables>(ntGTTs)->findByName(s);
        if (t)
            return std::static_pointer_cast<Relation>(t);
        else
        {
            ViewPtr v = getCollectionPtr<ViewsPtr, Views>(ntViews)->findByName(s);
            if (v)
                return std::static_pointer_cast<Relation>(v);
            else
            {
                t = getCollectionPtr<SysTablesPtr, SysTables>(ntSysTables)->findByName(s);
                if (t)
                    return std::static_pointer_cast<Relation>(t);
                else
                    return nullptr;
            }
        }

    }
    return nullptr;
}

MetadataCollectionBasePtrs::iterator MetadataContainer::begin()
{
    return collectionsM.begin();
}

MetadataCollectionBasePtrs::iterator MetadataContainer::end() 
{
    return collectionsM.end();
}

MetadataCollectionBasePtrs::const_iterator MetadataContainer::begin() const 
{
    return collectionsM.begin();
}

MetadataCollectionBasePtrs::const_iterator MetadataContainer::end() const 
{
    return collectionsM.end();
}

bool MetadataContainer::empty() const 
{
    return  collectionsM.empty();
}

size_t MetadataContainer::size() const 
{
    return collectionsM.size();
}

size_t MetadataContainer::capacity() const 
{
    return collectionsM.capacity();
}

void MetadataContainer::lockSubject() {
    for (const auto& col : collectionsM) {
        col->lockSubject();
    }
}

void MetadataContainer::unlockSubject() {
    for (const auto& col : collectionsM) {
        col->unlockSubject();
    }
}

void MetadataContainer::forEachCollection(
        const std::function<void(const MetadataCollectionBasePtr&)>& func) const
{
    for (const auto& col : collectionsM) {
        func(std::static_pointer_cast<MetadataCollectionBase>(col));
    }
}

void MetadataContainer::loadCollections(ProgressIndicator* pi, DatabasePtr db)
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
    const int collectionCount = capacity();
    ProgressIndicatorHelper pih(pi);


    MetadataLoader* loader = db->getMetadataLoader();
    MetadataLoaderTransaction tr(loader);
    //SubjectLocker lock(db);
    //if (pi) review performance later 
    {
        for (auto col : collectionsM) {
            pih.init(col->getSchemaName_() + col->getName_(), collectionCount, 0);
            if (col->getType() == ntSchemas) {
                col->ensureChildrenLoaded();
                col->forEachItem([&pi](const MetadataItemPtr& item) {
                    if (item) {
                        dynamic_cast<Schema*>(item.get())->loadCollections(pi);
                    }
                    }
                );
            }
            else {
                col->load(pi);
            }
            //std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
    }
}
void MetadataContainer::dropObject(MetadataItem* object)
{
    NodeType type = object->getType();
    type = static_cast<NodeType>(static_cast<int>(type) + 1);
    for (auto col : collectionsM) {
        if (col) {
            if (col->getType() == type) {
                col->remove(object);
            }
        }
    }
}
void MetadataContainer::addObject(NodeType type, const wxString& name)
{
    type = static_cast<NodeType>(static_cast<int>(type) + 1);
    for (auto col : collectionsM) {
        if (col) {
            if (col->getType() == type) {
                col->insert_(name);
            }
        }
    }
}