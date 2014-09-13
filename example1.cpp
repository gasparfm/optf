#include "optf.hpp"
#include <string>
#include <iostream>

using namespace std;

// This class can have many options to initialize
class MyClass
{
public:
  MyClass(string arg1, string arg2)
  {
    optfs o(optfs::bind(&MyClass::set_option, this), arg1, arg2);
    // optfs o(bind(&MyClass::set_option, this, placeholders::_1, placeholders::_2), arg1, arg2);
  }

  void print()
  {
    cout << "BOOLEAN = "<< boolean <<endl;
    cout << "NUMBER = "<< number << endl;
  }

private:
  bool boolean;
  double number;

  // Private function
  void set_option(const string opt, const string &val)
  {
    // Lets put all settable stuff
    OPTF_DEFINE(boolean);
    OPTF_DEFINE(number, 1);
  }
};

int main()
{
  MyClass mc("boolean=yes", "number=123.45");

  mc.print();

  return 1;
}
