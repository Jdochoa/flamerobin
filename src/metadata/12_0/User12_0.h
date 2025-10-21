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


#pragma once
#ifndef FR_USER_12_0_H
#define FR_USER_12_0_H

#include <ibpp.h>

#include "metadata/MetadataClasses.h"
#include "metadata/metadataitem.h"
#include "metadata/User.h"


class UserAttribute : public MetadataItem
    ,public std::enable_shared_from_this<UserAttribute>
{
private:
    wxString valueM;
    wxString pluginM;
 protected:
    //virtual void loadProperties();
    virtual void loadProperties();
public:
    UserAttribute(MetadataItem* user, wxString name);
    UserAttribute(DatabasePtr database, const wxString& name);

    wxString getValue() const;
    wxString getPlugin() const;

    void setValue(const wxString& value);
    void setPlugin(const wxString& plugin);

    virtual const wxString getTypeName() const { return "USERATTIBUTES"; };
    virtual void acceptVisitor(MetadataItemVisitor* visitor);
;
};


class User12_0 : public User
{
private:
    bool activeM;
    bool adminM;
    wxString pluginM;

    UserAttributesPtr userAttibutesM;
    //UserAttributePtrs userAttibutesM;
    //void loadAttributes(ProgressIndicator* progressIndicator);
protected:
    virtual void loadProperties();
    
    virtual void loadChildren();
    virtual void lockChildren();
    virtual void unlockChildren();

    virtual wxString getAttributes();


public:
    User12_0(DatabasePtr database, const wxString& name);

 
    wxString getPlugin() const;
    bool getActive() const;
    bool getAdmin() const;

    void setPlugin(const wxString& value);
    void setActive(const bool& value);
    void setAdmin(const bool& value);


    virtual bool getChildren(std::vector<MetadataItem*>& temp);
    virtual size_t getChildrenCount() const;

    UserAttributePtrs::iterator begin();
    UserAttributePtrs::iterator end();
    UserAttributePtrs::const_iterator begin() const;
    UserAttributePtrs::const_iterator end() const;

    // MetadataItem interface
    virtual const wxString getTypeName() const { return "USER"; };
    virtual void acceptVisitor(MetadataItemVisitor* visitor) { visitor->visitUser(*this); };
    virtual wxString getSource();
    virtual wxString getAlterSqlStatement();
    virtual wxString getCreateSqlStatement();


};

class UserAttributes : public MetadataCollection<UserAttribute>
{
private:
    wxString userNameM;
    wxString pluginM;
public:
    UserAttributes(DatabasePtr database);
    virtual void load(ProgressIndicator* progressIndicator);

    virtual ItemType newItem(const wxString& name) {
        ItemType item(new UserAttribute(getDatabase(), name));
        return item;
    }
    
    void setUserName(const wxString& userName);
    void setPlugin(const wxString& plugin);
    wxString getUserName() const;
    wxString getPlugin() const;
};


class Users12_0 : public Users
{
protected:
    virtual void loadChildren();
public:
    Users12_0(DatabasePtr database);
    virtual void load(ProgressIndicator* progressIndicator);

    ItemType newItem(const wxString& name) override{
        ItemType item(new User12_0(getDatabase(), name));
        return item;
    }
};

#endif // FR_USER_12_0_H
