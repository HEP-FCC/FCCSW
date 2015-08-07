// $Id: StringParse.h,v 1.3 2006-04-04 14:58:24 robbep Exp $

#ifndef GENERATORS_STRINGPARSE_H
#define GENERATORS_STRINGPARSE_H

// Include files
#include <string>
#include <vector>

/** @class StringParse StringParse.h "Generators/StringParse.h"
 * 
 *   Utility class to parse a string.
 *   It breaks it into components which are seperated by spaces  
 *   in the original.
 * 
 *  @author Ian Hinchliffe
 *  @date   200-04-01
 */

class StringParse {
 public:
  StringParse( const std::string & input); ///< Constructor with string
  
  StringParse(); ///< Default constructor
  
  ~StringParse(); ///< Destructor
  
  /// Returns the num-th component of the input string as a string.
  std::string piece( const int & num ) ;
  
  /// Returns the num-th component of the input string as an integer.
  int intpiece( const int & num ) ; 
  
  /// Returns the num-th component of the input string as a double
  double numpiece(const int & num); 

 private:
  /// Input string
  std::string m_lstring;

  /// Vector of substrings in input string
  std::vector<std::string> m_lsubstring;

  /// Number of words in input string.
  int m_nword;

  /// String to return if past end.
  std::string m_past_end;
};
#endif  //GENERATORS_STRINGPARSE_H
