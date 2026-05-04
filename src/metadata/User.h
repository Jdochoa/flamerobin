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


#ifndef FR_USER_H
#define FR_USER_H

#include <ibpp.h>

#include "metadata/MetadataClasses.h"
#include "metadata/metadataitem.h"
#include "metadata/metadataitemvisitor.h"

class UserAttribute : public MetadataItem
    , public std::enable_shared_from_this<UserAttribute>
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



class User: public MetadataItem
    ,public std::enable_shared_from_this<User>
{
private:
    ServerWeakPtr serverM;
    wxString usernameM;
    wxString passwordM;
    wxString firstnameM;
    wxString middlenameM;
    wxString lastnameM;
    uint32_t useridM;
    uint32_t groupidM;
protected:
    virtual void loadProperties();
public:
    User(ServerPtr server);
    User(ServerPtr server, const IBPP::User& src);
    User(DatabasePtr database, const wxString& name);


    ServerPtr getServer() const;
    virtual bool isSystem() const;

    wxString getUsername() const;
    wxString getPassword() const;
    wxString getFirstName() const;
    wxString getMiddleName() const;
    wxString getLastName() const;
    uint32_t getUserId() const;
    uint32_t getGroupId() const;
    IBPP::User getUserIBPP();

    void setServer(ServerPtr srv);
    void setUsername(const wxString& value);
    void setPassword(const wxString& value);
    void setFirstName(const wxString& value);
    void setMiddleName(const wxString& value);
    void setLastName(const wxString& value);
    void setUserId(uint32_t value);
    void setGroupId(uint32_t value);
    void setUserIBPP(const IBPP::User& usr);

    void assignTo(IBPP::User& dest) const;

    virtual void acceptVisitor(MetadataItemVisitor* visitor);
    virtual const wxString getTypeName() const;
    virtual wxString getSource();
    
};

class User11_0 : public User
{
protected:
    virtual void loadProperties();
public:
    User11_0(ServerPtr server);
    User11_0(ServerPtr server, const IBPP::User& src);
    User11_0(DatabasePtr database, const wxString& name);
};

class User12_0 : public User11_0
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



class Users : public MetadataCollection<User>
{
protected:
    virtual void loadChildren();
public:
    Users(DatabasePtr database);

    virtual void load(ProgressIndicator* ) {};

    virtual void acceptVisitor(MetadataItemVisitor* visitor);
    virtual const wxString getTypeName() const;

};

class Users11_0 : public Users
{
public:
    Users11_0(DatabasePtr database);
    virtual void load(ProgressIndicator* progressIndicator);

    virtual ItemType newItem(const wxString& name) {
        ItemType item(new User11_0(getDatabase(), name));
        return item;
    }

};

class Users12_0 : public Users11_0
{
protected:
    virtual void loadChildren();
public:
    Users12_0(DatabasePtr database);
    virtual void load(ProgressIndicator* progressIndicator);

    ItemType newItem(const wxString& name) override {
        ItemType item(new User12_0(getDatabase(), name));
        return item;
    }
};

#endif // FR_USER_H
