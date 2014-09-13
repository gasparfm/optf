#include "optf.hpp"
#include <string>
#include <iostream>
#include <sstream>

using namespace std;

// This class can have many options to initialize. This is just an example

class DB
{
public:
  string host;
  string username;
  string password;
  int port;

  virtual string str()
  {
    stringstream ss;
    ss << "Host: "<< host<<endl;
    ss << "Username: "<< username<<endl;
    ss << "Password: "<< password<<endl;
    ss << "Port: "<< port<<endl;

    return ss.str();
  }
};

class MySQL : public DB
{
public:
  MySQL ()
  {
    port = 3306;
  }

  string str()
  {
    stringstream ss;
    ss << " MySQL " <<endl;
    ss << DB::str();
    return ss.str();
  }
};

class PostgreSQL : public DB
{
public:
  PostgreSQL()
  {
    port = 5432;
  }

  string str()
  {
    stringstream ss;
    ss << " PostgreSQL " <<endl;
    ss << DB::str();

    return ss.str();
  }
};

// Object that can be initialized with str
class AnyObject
{
public:
  AnyObject()
  {
  }

  AnyObject(string s): st(s)
  {
  }

  string getStr()
  {
    return st;
  }
private:
  string st;
};

class DatabaseConnectorSample
{
public:
  template<typename...  Ts>
  DatabaseConnectorSample(Ts... more)
  {
    optfs o(optfs::bind(&DatabaseConnectorSample::set_option, this), more...);
  }

  void print()
  {
    cout <<dbtype->str();
    cout << "Pconnect: "<< pconnect << endl;
    cout << "object: "<<any.getStr()<<endl;
  }

private:
  DB *dbtype;
  bool pconnect;
  AnyObject any;

  // Private function
  void set_option(const string opt, const string &val)
  {
    // Lets put all settable stuff
    OPTF_TRANSFORM(dbtype, set_db_type);
    OPTF_DEFINE(pconnect, 1);
    OPTF_DEFINEX(opt, val, dbtype->username, username, 1);
    OPTF_DEFINEX(opt, val, dbtype->password, password, 1);
    OPTF_DEFINEX(opt, val, dbtype->port, port, 1);
    OPTF_DEFINEX(opt, val, dbtype->host, host, 1);
    OPTF_OBJECT(any, 1);
  }

  DB* set_db_type(const string db)
  {
    if (db == "mysql")
      return new MySQL();
    else
      return new PostgreSQL();
  }
};

int main()
{
  DatabaseConnectorSample mc("dbtype=postgresql", 
			     "username=root", 
			     "host=localhost", 
			     "pconnect=true",
			     "any=ANY OBJECT",
			     "password=mypass", 
			     "port=1234");

  mc.print();

  return 1;
}
