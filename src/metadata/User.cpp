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

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWindows headers
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include <ibpp.h>

#include "core/FRError.h"
#include "core/StringUtils.h"
#include "engine/MetadataLoader.h"
#include "metadata/MetadataItemVisitor.h"
#include "metadata/server.h"
#include "metadata/User.h"

UserAttributePtrs::iterator User12_0::begin()
{
    return userAttibutesM->begin();
}

UserAttributePtrs::iterator User12_0::end()
{
    return userAttibutesM->end();
}

UserAttributePtrs::const_iterator User12_0::begin() const
{
    return userAttibutesM->begin();
}

UserAttributePtrs::const_iterator User12_0::end() const
{
    return userAttibutesM->end();
}

UserAttribute::UserAttribute(MetadataItem* user, wxString name)
    : MetadataItem(ntUserAttribute, user, name)
{}

UserAttribute::UserAttribute(DatabasePtr database, const wxString& name)
    : MetadataItem(ntUserAttribute, database.get(), name)
{}

wxString UserAttribute::getValue() const
{
    return valueM;
}

wxString UserAttribute::getPlugin() const
{
    return pluginM;
}

void UserAttribute::setValue(const wxString& value)
{
    valueM = value;
}

void UserAttribute::setPlugin(const wxString& plugin)
{
    pluginM = plugin;
}

void UserAttribute::loadProperties()
{
    DatabasePtr db = getDatabase();

    MetadataLoader* loader = db->getMetadataLoader();
    MetadataLoaderTransaction tr(loader);
    wxMBConv* converter = db->getCharsetConverter();

    IBPP::Statement& st1 = loader->getStatement(
        "select sec$user_name, "
        "sec$first_name, "
        "sec$middle_name, "
        "sec$last_name, "
        "sec$active, "
        "sec$admin, "
        "sec$description, "
        "sec$plugin "
        "from sec$users "
        "where sec$user_name = ? "
    );
    st1->Set(1, wx2std(getName_(), converter));
    st1->Execute();
    if (!st1->Fetch())
        throw FRError(_("User not found: ") + getName_());

    setPropertiesLoaded(false);
    std::string lstr;


    if (st1->IsNull(2))
        lstr = "";
    else
        st1->Get(2, lstr);
    //setFirstName(lstr);

    if (st1->IsNull(3))
        lstr = "";
    else
        st1->Get(3, lstr);
    //setMiddleName(lstr);

    if (st1->IsNull(4))
        lstr = "";
    else
        st1->Get(4, lstr);
    //setLastName(lstr);

    if (st1->IsNull(4))
        lstr = "";
    else
        st1->Get(4, lstr);
    //setLastName(lstr);

    setPropertiesLoaded(true);
    notifyObservers();

}



void User::loadProperties()
{
    serverM = getDatabase()->getServer();
}

User::User(ServerPtr server)
    : MetadataItem(ntUser, server.get()), serverM(server)
{
}

User::User(ServerPtr server, const IBPP::User& src)
    : MetadataItem(ntUser, server.get()), serverM(server),
        useridM(src.userid), groupidM(src.groupid)
{
    usernameM = src.username;
    passwordM = src.password;
    firstnameM = src.firstname;
    middlenameM = src.middlename;
    lastnameM = src.lastname;
}

User::User(DatabasePtr database, const wxString& name)
    : MetadataItem(ntUser, database.get(), name, -1)
{
}

ServerPtr User::getServer() const
{
    return ServerPtr(serverM);
}

wxString User::getUsername() const
{
    return usernameM;
}

wxString User::getPassword() const
{
    return passwordM;
}

wxString User::getFirstName() const
{
    return firstnameM;
}

wxString User::getMiddleName() const
{
    return middlenameM;
}

wxString User::getLastName() const
{
    return lastnameM;
}

uint32_t User::getUserId() const
{
    return useridM;
}

uint32_t User::getGroupId() const
{
    return groupidM;
}

IBPP::User User::getUserIBPP() 
{
    ensurePropertiesLoaded();

    IBPP::User usr;
    
    usr.username = wx2std(usernameM);
    usr.password = wx2std(passwordM);
    usr.firstname = wx2std(firstnameM);
    usr.lastname = wx2std(lastnameM);
    usr.middlename = wx2std(middlenameM);
    usr.userid = useridM;
    usr.groupid = groupidM;

    return usr;
}

void User::setServer(ServerPtr srv)
{
    serverM = srv;
}

void User::setUsername(const wxString& value)
{
    if (usernameM != value)
    {
        usernameM = value;
        notifyObservers();
    }
}

void User::setPassword(const wxString& value)
{
    if (passwordM != value)
    {
        passwordM = value;
        notifyObservers();
    }
}

void User::setFirstName(const wxString& value)
{
    if (firstnameM != value)
    {
        firstnameM = value;
        notifyObservers();
    }
}

void User::setMiddleName(const wxString& value)
{
    if (middlenameM != value)
    {
        middlenameM = value;
        notifyObservers();
    }
}

void User::setLastName(const wxString& value)
{
    if (lastnameM != value)
    {
        lastnameM = value;
        notifyObservers();
    }
}

void User::setUserId(uint32_t value)
{
    if (useridM != value)
    {
        useridM = value;
        notifyObservers();
    }
}

void User::setGroupId(uint32_t value)
{
    if (groupidM != value)
    {
        groupidM = value;
        notifyObservers();
    }
}

void User::setUserIBPP(const IBPP::User& usr) 
{
    setUsername(usr.username);
    setUsername(usr.username);
    setPassword(usr.password);
    setFirstName(usr.firstname);
    setMiddleName(usr.middlename);
    setLastName(usr.lastname);
    setUserId(usr.userid);
    setGroupId(usr.groupid);

    notifyObservers();
}

void User::assignTo(IBPP::User& dest) const
{
    //dest = getUserIBPP();
    dest.username = wx2std(usernameM);
    dest.password = wx2std(passwordM);
    dest.firstname = wx2std(firstnameM);
    dest.lastname = wx2std(lastnameM);
    dest.middlename = wx2std(middlenameM);
    dest.userid = useridM;
    dest.groupid = groupidM;

}

void User::acceptVisitor(MetadataItemVisitor* visitor)
{
    visitor->visitUser(*this);
}

const wxString User::getTypeName() const
{
    return "USER";
}

wxString User::getSource()
{
    ensurePropertiesLoaded();
    wxString sql = "FIRSTNAME '" + getFirstName() + "' \n" +
        "MIDDLENAME '" + getMiddleName() + "' \n"
        "LASTNAME '" + getLastName() + "' \n"
        ;
    return sql;
}

bool User::isSystem() const
{
    return usernameM == "SYSDBA";
}



void User11_0::loadProperties()
{
    setPropertiesLoaded(false);

    DatabasePtr db = getDatabase();
    setServer(db->getServer());

    IBPP::Service svc;
    if (db->getServer()->getService(svc, NULL, true)) {
        IBPP::User usr;
        usr.username = getName_();
        svc->GetUser(usr);
        setUserIBPP(usr);
    }

    setPropertiesLoaded(true);
    notifyObservers();
}

User11_0::User11_0(ServerPtr server)
    : User(server)
{}

User11_0::User11_0(ServerPtr server, const IBPP::User& src)
    :User(server, src)
{}

User11_0::User11_0(DatabasePtr database, const wxString& name)
    :User(database, name)
{}


void Users::loadChildren()
{
    load(0);
}

Users::Users(DatabasePtr database)
    : MetadataCollection<User>(ntUsers, database, _("Users"))
{
}

void Users::acceptVisitor(MetadataItemVisitor* visitor)
{
    visitor->visitUsers(*this);
}

const wxString Users::getTypeName() const
{
    return "USERS_COLLECTION";
}

Users11_0::Users11_0(DatabasePtr database)
    :Users(database)
{}

void Users11_0::load(ProgressIndicator*)
{
    DatabasePtr db = getDatabase();
    IBPP::Service svc;
    if (db->getServer()->getService(svc, NULL, true)) {   // true = SYSDBA

        std::vector<IBPP::User> usr;
        svc->GetUsers(usr);
        for (std::vector<IBPP::User>::iterator it = usr.begin();
            it != usr.end(); ++it)
        {
            insert(it->username);
        }
        notifyObservers();
        setChildrenLoaded(true);

    }

}

Users12_0::Users12_0(DatabasePtr database)
    :Users11_0(database)
{}

void Users12_0::load(ProgressIndicator* progressIndicator)
{
    DatabasePtr db = getDatabase();
    wxString stmt = "select sec$user_name from sec$users order by 1 ";
    setItems(db->loadIdentifiers(stmt, progressIndicator));
}

void Users12_0::loadChildren()
{
    load(0);
}


void User12_0::loadProperties()
{
    DatabasePtr db = getDatabase();

    MetadataLoader* loader = db->getMetadataLoader();
    MetadataLoaderTransaction tr(loader);
    wxMBConv* converter = db->getCharsetConverter();

    IBPP::Statement& st1 = loader->getStatement(
        "select sec$user_name, "
        "sec$first_name, "
        "sec$middle_name, "
        "sec$last_name, "
        "sec$active, "
        "sec$admin, "
        "sec$description, "
        "sec$plugin "
        "from sec$users "
        "where sec$user_name = ? "
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
    setFirstName(lstr);

    if (st1->IsNull(3))
        lstr = "";
    else
        st1->Get(3, lstr);
    setMiddleName(lstr);

    if (st1->IsNull(4))
        lstr = "";
    else
        st1->Get(4, lstr);
    setLastName(lstr);

    if (st1->IsNull(4))
        lstr = "";
    else
        st1->Get(4, lstr);
    setLastName(lstr);

    if (st1->IsNull(5))
        lbool = false;
    else
        st1->Get(5, lbool);
    setActive(lbool);

    if (st1->IsNull(6))
        lbool = false;
    else
        st1->Get(6, lbool);
    setAdmin(lbool);

    if (st1->IsNull(8))
        lstr = "";
    else
        st1->Get(8, lstr);
    setPlugin(lstr);

    setPropertiesLoaded(true);
    notifyObservers();
}

void User12_0::loadChildren()
{
    ensurePropertiesLoaded();

    setChildrenLoaded(false);

    userAttibutesM->setUserName(getName_());
    userAttibutesM->setPlugin(getPlugin());

    userAttibutesM->load(0);

    setChildrenLoaded(true);
}

void User12_0::lockChildren()
{
    userAttibutesM->lockSubject();
    //std::for_each(userAttibutesM.begin(), userAttibutesM.end(),
    //    std::mem_fn(&UserAttribute::lockSubject));

}

void User12_0::unlockChildren()
{
    userAttibutesM->unlockSubject();
    //std::for_each(userAttibutesM.begin(), userAttibutesM.end(),
    //    std::mem_fn(&UserAttribute::unlockSubject));

}

User12_0::User12_0(DatabasePtr database, const wxString& name)
    :User11_0(database, name)
{
    userAttibutesM.reset(new UserAttributes(getDatabase()));
}

wxString User12_0::getPlugin() const
{
    return pluginM;
}

bool User12_0::getActive() const
{
    return activeM;
}

bool User12_0::getAdmin() const
{
    return adminM;
}

wxString User12_0::getSource()
{
    wxString sql = User::getSource();
    sql = sql
        << (getActive() ? "ACTIVE" : "INACTIVE") << " \n"
        << (getAdmin() ? "GRANT ADMIN ROLE \n" : "")
        << (!getPlugin().IsEmpty() ? "USING PLUGIN " + getPlugin() : "")
        ;

    return sql;
}

wxString User12_0::getAlterSqlStatement()
{
    return  "ALTER USER "
        + getQuotedName() + " \n"
        + getSource() + " \n"
        + getAttributes() + " \n"
        + ";\n";
}

wxString User12_0::getCreateSqlStatement()
{
    return  "CREATE USER "
        + getQuotedName() + " \n"
        + getSource() + " \n"
        + getAttributes() + " \n"
        + ";\n";
}

wxString User12_0::getAttributes()
{
    wxString sql;

    ensureChildrenLoaded();
    wxString attr = "";
    for (UserAttributePtrs::iterator it = begin(); it != end(); ++it)
    {
        if (!attr.IsEmpty())
            attr << ", ";
        attr << (*it).get()->getName_() << " = '" << (*it).get()->getValue();
    }

    if (!attr.IsEmpty()) {
        sql << "TAGS ( " << attr << ")" << " \n";
    }

    return sql;
}

bool User12_0::getChildren(std::vector<MetadataItem*>& temp)
{
    if (userAttibutesM->empty())
        return false;
    userAttibutesM->getChildren(temp);
    //temp.push_back(userAttibutesM->get());
    //std::transform(userAttibutesM.begin(), userAttibutesM.end(),
    //    std::back_inserter(temp), std::mem_fn(&UserAttributePtr::get));

    return !userAttibutesM->empty();

}

size_t User12_0::getChildrenCount() const
{
    return userAttibutesM->getChildrenCount();
}

void User12_0::setPlugin(const wxString& plugin)
{
    pluginM = plugin;
}

void User12_0::setActive(const bool& value)
{
    activeM = value;
}

void User12_0::setAdmin(const bool& value)
{
    adminM = value;
}


UserAttributes::UserAttributes(DatabasePtr database)
    : MetadataCollection<UserAttribute>(ntUserAttributes, database, _("UserAttributes"))
{}

void UserAttributes::load(ProgressIndicator* progressIndicator)
{
    DatabasePtr db = getDatabase();

    MetadataLoader* loader = db->getMetadataLoader();
    MetadataLoaderTransaction tr(loader);
    wxMBConv* converter = db->getCharsetConverter();

    IBPP::Statement& st1 = loader->getStatement(
        "Select  SEC$KEY, SEC$VALUE "
        "From SEC$USER_ATTRIBUTES "
        "Where  SEC$USER_NAME = ? "
        "And SEC$PLUGIN = ? "
    );
    st1->Set(1, wx2std(getUserName(), converter));
    st1->Set(2, wx2std(getPlugin(), converter));

    st1->Execute();
    UserAttributePtrs attributes;
    while (st1->Fetch())
    {
        std::string s;
        wxString key;
        if (!st1->IsNull(1)) {
            st1->Get(1, s);
            key = std2wxIdentifier(s, converter);
        }
        wxString value;
        if (!st1->IsNull(2)) {
            st1->Get(2, s);
            value = std2wxIdentifier(s, converter);
        }


        UserAttributePtr attr;// = findParameter(param_name);
        attr.reset(new UserAttribute(this, key));
        initializeLockCount(attr, getLockCount());
        attributes.push_back(attr);
        attr->setValue(value);
    }
    setItems(attributes);
    //setItems(db->loadIdentifiers(stmt, progressIndicator));
}

void UserAttributes::setUserName(const wxString& userName)
{
    userNameM = userName;
}

void UserAttributes::setPlugin(const wxString& plugin)
{
    pluginM = plugin;
}

wxString UserAttributes::getUserName() const
{
    return userNameM;
}

wxString UserAttributes::getPlugin() const
{
    return pluginM;
}

void UserAttribute::acceptVisitor(MetadataItemVisitor* visitor)
{
    visitor->visitUserAttribute(*this);
}