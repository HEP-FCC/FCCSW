#include "KeepDropSwitch.h"

#include <sstream>
#include <iostream>
#include <stdexcept>

int wildcmp(const char *wild, const char *string) {
  // Written by Jack Handy - <A href="mailto:jakkhandy@hotmail.com">jakkhandy@hotmail.com</A>
  const char *cp = NULL, *mp = NULL;
  while ((*string) && (*wild != '*')) {
    if ((*wild != *string) && (*wild != '?')) {
      return 0;
    }
    wild++;
    string++;
  }
  while (*string) {
    if (*wild == '*') {
      if (!*++wild) {
        return 1;
      }
      mp = wild;
      cp = string+1;
    } else if ((*wild == *string) || (*wild == '?')) {
      wild++;
      string++;
    } else {
      wild = mp;
      string = cp++;
    }
  }
  while (*wild == '*') {
    wild++;
  }
  return !*wild;
}


std::vector<std::string> split(const std::string &s,
			       char delim) {
  std::vector<std::string> elems; 
  std::stringstream ss(s);
  std::string item;
  while (std::getline(ss, item, delim)) {
    if( item != "" )
      elems.push_back(item);
  }
  return elems;
}

bool KeepDropSwitch::isOn(const std::string& astring) const {
  typedef std::map<std::string, bool>::const_iterator MIter; 
  MIter im = m_cache.find(astring);
  if(im!=m_cache.end())
    return im->second;
  else {
    bool val = getFlag(astring);
    m_cache.insert( std::pair<std::string, bool>(astring, val) );
    return val;
  }
}

bool KeepDropSwitch::getFlag(const std::string& astring) const {
  bool flag = true;
  for(const auto& cmdline : m_commandlines) {
    std::vector<std::string> words = split(cmdline, ' ');
    if(words.size()!=2) {
      std::ostringstream msg;
      msg << "malformed command string : " << cmdline;
      throw std::invalid_argument(msg.str());
    }
    std::string cmd = words[0];
    std::string pattern = words[1];
    Cmd theCmd = UNKNOWN;
    if(cmd == "keep")
      theCmd = KEEP;
    else if (cmd == "drop")
      theCmd = DROP;
    else {
      std::ostringstream msg;
      msg << "malformed command in line: " << std::endl;
      msg << cmdline << std::endl;
      msg << "should be keep or drop, lower case" << std::endl;
      throw std::invalid_argument(msg.str());
    }
    bool match = wildcmp(pattern.c_str(), astring.c_str());
    if(not match) continue;
    else if (theCmd == KEEP) 
      flag = true; 
    else
      flag = false;
  }
  return flag;
}

KeepDropSwitch::Cmd KeepDropSwitch::extractCommand(const std::string cmdline) const {
  std::vector<std::string> words = split(cmdline, ' ');
  for(auto& word : words)
    std::cout<<"'"<<word<<"' ";
  std::cout<<std::endl;
  return UNKNOWN;
}
