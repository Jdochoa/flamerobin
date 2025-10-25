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

#ifndef FR_METADATACONTAINER_H
#define FR_METADATACONTAINER_H


#include <algorithm>
#include <iterator>
#include <vector>
#include <functional>

#include "frutils.h"
#include "engine/MetadataLoader.h"
#include "core/ProgressIndicator.h"


#include "metadata/MetadataClasses.h"
#include "metadata/metadataitem.h"
#include "metadata/collection.h"

class MetadataContainer 
    :public std::enable_shared_from_this<MetadataContainer>
{
public:
    using CollectionPtr = std::shared_ptr<MetadataCollectionBase>;
    using ContainerType = std::vector<CollectionPtr>;

private:
    ContainerType collectionsM;
public:
    MetadataContainer();

    void loadCollections(ProgressIndicator* pi, DatabasePtr db);
    void dropObject(MetadataItem* object);
    void addObject(NodeType type, const wxString& name);
    void addCollection(const std::shared_ptr<MetadataCollectionBase>& collection);

    ContainerType getCollections() const { return collectionsM; }
    void getCollections(std::vector<MetadataItem*>& temp, bool system);
    MetadataItemPtr findByName(const wxString& name) const;
    MetadataItemPtr findByTypeAndName(NodeType nt, const wxString& name);
    MetadataItemPtr findByTypeAndId(NodeType nt, int id);
    std::vector<wxString> getAllNames() const;
    void getIdentifiers(std::vector<Identifier>& temp);
    DomainPtr getDomain(const wxString& name);
    RelationPtr findRelation(const Identifier& name);



    MetadataCollectionBasePtrs::iterator begin();
    MetadataCollectionBasePtrs::iterator end();
    MetadataCollectionBasePtrs::const_iterator begin() const;
    MetadataCollectionBasePtrs::const_iterator end() const;
    size_t capacity() const;
    bool empty() const;
    size_t size() const;

    void lockSubject();
    void unlockSubject();
    void forEachCollection(const std::function<void(const MetadataCollectionBasePtr&)>& func) const;

    template <class P, class T>
    P getCollectionPtr(NodeType type) {
        for (const auto& m : collectionsM) {
            if (m->getType() == type) {
                return std::make_shared<T>(dynamic_cast<T&>(*m));
            }
        }
        return nullptr;
    }



};

#endif // FR_METADATACONTAINER_H


