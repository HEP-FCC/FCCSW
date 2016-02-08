#ifndef __KEEPDROPSWITCH__
#define __KEEPDROPSWITCH__

#include <string>
#include <vector>
#include <map>

std::vector<std::string> split(const std::string &s,
			       char delim);

int wildcmp(const char *wild, const char *string);

class KeepDropSwitch {
 public:
  enum Cmd {
    KEEP,
    DROP,
    UNKNOWN
  };
  typedef std::vector<std::string> CommandLines; 
  KeepDropSwitch() {}
  KeepDropSwitch(const CommandLines& cmds) {m_commandlines=cmds;}
  bool isOn(const std::string& astring) const;
  
 private:
  bool getFlag(const std::string& astring) const;
  Cmd extractCommand(const std::string cmdLine) const;
  CommandLines m_commandlines;
  mutable std::map<std::string, bool> m_cache;
};

#endif

