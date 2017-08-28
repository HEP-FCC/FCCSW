

#ifndef SIMPAPAS_PAPASLOGGER_H
#define SIMPAPAS_PAPASLOGGER_H

#include "papas/utility/Logger.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/MsgStream.h"

class GaudiFilterPolicy : public papaslog::Logging::OutputFilterPolicy {
public:
  GaudiFilterPolicy(IMessageSvc* owner ) : m_messenger(owner), m_currentLevel(m_messenger.currentLevel()) {
     }
  
  bool doPrint(const papaslog::Logging::Level& lvl) const {
    return true;
    /*MSG::Level l = MSG::VERBOSE;
    MSG::Level cl = m_currentLevel;*/
    /*"switch (cl) {
      case MSG::VERBOSE:
      
        std::cout << "verb" << l ;
        break;
      case MSG::DEBUG:
        std::cout << "debug";
        break;
      case MSG::INFO:
        std::cout << "info";
        break;
      case MSG::WARNING:
        std::cout << "warn";
        break;
      case MSG::ERROR:
        std::cout << "error";
        break;
      case MSG::FATAL:
        std::cout << "fatal" << l ;
        break;
      default:
        std::cout << "  NOT SET  " << l ;
        cl = MSG::DEBUG;
    */
    /*
    
    switch (lvl) {
      case papaslog::Logging::VERBOSE:
        l = MSG::VERBOSE;
        break;
      case papaslog::Logging::DEBUG:
        l = MSG::DEBUG;
        break;
      case papaslog::Logging::INFO:
        l = MSG::INFO;
        break;
      case papaslog::Logging::WARNING:
        l = MSG::WARNING;
        break;
      case papaslog::Logging::ERROR:
        l = MSG::ERROR;
        break;
      case papaslog::Logging::FATAL:
        l = MSG::FATAL;
                break;
    }
    return true;
    // << cl<< "level=" << l <<" p="<< lvl << "PAPASALICE"<< endmsg;
   if (cl <= l )
     std::cout << "true";
    //std::cout << " XXXXXXXdoprint" << cl << " " << l<< endmsg;;
    return cl <= l;
    //return true;*/
  }
  
private:
  MsgStream m_messenger;
  MSG::Level m_currentLevel;
};

class GaudiPrintPolicy : public papaslog::Logging::OutputPrintPolicy {
public:
  GaudiPrintPolicy(IMessageSvc* owner) : m_messenger(owner) {}
  
  void flush(const papaslog::Logging::Level& lvl, const std::ostringstream& input) {
    MSG::Level l = MSG::VERBOSE;
    switch (lvl) {
      case papaslog::Logging::VERBOSE:
        l = MSG::VERBOSE;
        break;
      case papaslog::Logging::DEBUG:
        l = MSG::DEBUG;
        break;
      case papaslog::Logging::INFO:
        l = MSG::INFO;
        break;
      case papaslog::Logging::WARNING:
        l = MSG::WARNING;
        break;
      case papaslog::Logging::ERROR:
        l = MSG::ERROR;
        break;
      case papaslog::Logging::FATAL:
        l = MSG::FATAL;
        break;
    }
    m_messenger << l << input.str() << endmsg;
  }
  
private:
  MsgStream m_messenger;
};

#endif  // SIMPAPS_PAPASLOGGER_H
