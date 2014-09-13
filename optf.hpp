/* @(#)optf.hpp
 */

/**
*************************************************************
* @file optf.hpp
* @brief c++11 class to specify variable values as strings, in 
*  any order with the possibility of optional values
*
*
* @author Gaspar Fernández <blakeyed@totaki.com>
* @version 0.1
* @date 30 ago 2014
*
*
*
*
*************************************************************/

#ifndef _OPTF_HPP
#define _OPTF_HPP 1

#include <string>
#include <functional>
#include <algorithm>		// std::transform
#include <type_traits>		// std::remove_const

#define OPTF_ELSE
#define OPTF_ELSE0
#define OPTF_ELSE1 else

// the last param could be empty if it's the first condition, or just put 1 to 
// write else before the if
#define OPTF_DEFINE(variable, ...) OPTF_ELSE ## __VA_ARGS__ if (opt==#variable) optf<std::remove_const<decltype(opt)>::type >::_optf_define(variable, val)
#define OPTF_DEFINEX(opt, val, destvar, variable, ...) OPTF_ELSE ## __VA_ARGS__ if (opt==#variable) optf<std::remove_const<decltype(opt)>::type >::_optf_define(destvar, val)
#define OPTF_OBJECT(variable, ...) OPTF_ELSE ## __VA_ARGS__ if (opt==#variable) variable = decltype(variable)(val)
#define OPTF_OBJECTX(opt, val, destvar, variable, ...) OPTF_ELSE ## __VA_ARGS__ if (opt==#variable) destvar = decltype(destvar)(val)
#define OPTF_TRANSFORM(variable, transform, ...) OPTF_ELSE ## __VA_ARGS__ if (opt==#variable) variable = transform(val)
#define OPTF_TRANSFORMX(opt, val, destvar, variable, transform, ...) OPTF_ELSE ## __VA_ARGS__ if (opt==#variable) destvar = transform(val)

template <typename string_type = std::string>
class optf
{
public:
  /**
   * Option callback definition. A function / method of this type will be called
   * everytime a new option comes.
   *
   *   void optionCallback(const string optionName, const string& optionValue)
   */
  using optf_callback = std::function<void (const string_type, const string_type&)>;

  /**
   * @brief Constructor.
   *
   * @param cb Callback when any option comes
   */
  optf(optf_callback cb):callback(cb)
  {
  }

  /**
   * @brief Constructor.
   *
   * @param cb Callback when any option comes
   * @param options... string options.
   */
  template<typename... Ts>
  optf(optf_callback cb, Ts... options):optf(cb)
  {
    (*this)(options...);
  }

  /**
   * @brief Destructor.
   */
  ~optf()
  {
  }

  /**
   * @brief This operator will be called when there are
   *        no more options in the variadic template operator 
   *        below.
   */
  void operator()()
  {
  }

  /**
   * @brief Split strings with the first = sign (make in configurable in the future?)
   *        Then calls our callback function to set the options definitely
   * @param option First option
   * @param more... More options
   */
  template<typename...  Ts>
  void operator()(const string_type& option, Ts... more)
  {
    typename string_type::size_type space = option.find_first_of('=');
    if (space != string_type::npos)
      callback(option.substr(0, space), option.substr(space+1));

    (*this)(more...);
  }

  // some utils.
  /**
   * @brief binds callback with our object. Useful to use class members to set
   *        options. This is just to make it a bit simpler to initialize.
   * @param func Callback function
   * @param obj Object which func belongs to (e.g. this, when calling from inside a class.
   *
   * @return callback
   */
  template <typename F, typename o>
  static optf_callback bind(F func, o obj)
  {
    return std::bind(func, obj, std::placeholders::_1, std::placeholders::_2);
  }

  /**
   * @brief Transforms from string to boolean.
   *        yes, y, true are considered true, anything else is false
   *
   * @param str
   *
   * @return bool
   */
  static bool stob(const string_type &str)
  {
    try
      {
	string_type temp;
	std::transform(str.begin(), str.end(), std::back_inserter(temp), ::toupper);

	return (temp == "YES" || temp == "Y" || temp == "TRUE" || std::stoi(temp)!=0 );
      }
    catch (...)
      {
	return false;
      }
  }

  // define option utils
  /**
   * @brief Sets a variable value from a string type. It'll be done
   *        calling the appropiate transformation function.
   *
   * @param var Variable to set
   * @param val Val to put
   */
  template <typename T>
  static void _optf_define (T& var, const string_type &val)
  {
    var = val;
  }

  // this macro will make it easier to create transformation methods from a string to a variable with given type.
#define optf_define_with_transform(type, transform)   static void _optf_define (type& var, const string_type &val) \
  {						\
    var = transform(val);			\
  }

  /**
   * @brief string to integer (stoi wrapper)
   *
   * @param var Variable
   * @param transform Text value
   */
  optf_define_with_transform(int, std::stoi)

  /**
   * @brief string to long (stol wrapper)
   *
   * @param var Variable
   * @param transform Text value
   */
  optf_define_with_transform(long, std::stol)

  /**
   * @brief string to long long (stoll wrapper)
   *
   * @param var Variable
   * @param transform Text value
   */
  optf_define_with_transform(long long, std::stoll)

  /**
   * @brief string to unsigned long (stoul wrapper)
   *
   * @param var Variable
   * @param transform Text value
   */
  optf_define_with_transform(unsigned long, std::stoul)

  /**
   * @brief string to unsigned long long (stoull wrapper)
   *
   * @param var Variable
   * @param transform Text value
   */
  optf_define_with_transform(unsigned long long, std::stoull)

  /**
   * @brief string to float (stof wrapper)
   *
   * @param var Variable
   * @param transform Text value
   */
  optf_define_with_transform(float, std::stof);

  /**
   * @brief string to double (stod wrapper)
   *
   * @param var Variable
   * @param transform Text value
   */
  optf_define_with_transform(double, std::stod);

  /**
   * @brief string to long double (stold wrapper)
   *
   * @param var Variable
   * @param transform Text value
   */
  optf_define_with_transform(long double, std::stold);

  /**
   * @brief string to bool (optf::stob wrapper)
   *
   * @param var Variable
   * @param transform Text value
   */
  optf_define_with_transform(bool, optf<string_type>::stob);
#undef optf_define_with_transform

private:
  /* callbac func */
  optf_callback callback;
};

/* optf using std::string */
typedef optf<> optfs;
#endif /* _OPTF_HPP */
