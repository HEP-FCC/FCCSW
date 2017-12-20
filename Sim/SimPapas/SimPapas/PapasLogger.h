#ifndef SIMPAPAS_PAPASLOGGER_H
#define SIMPAPAS_PAPASLOGGER_H

#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ServiceHandle.h"
#include "papas/utility/Logger.h"

/*
 Based on ATS logging system.
 This code allows Papas log messages to be processed in the Gaudi logging.
 This should be regarded as a preliminary implementation.
*/

class GaudiFilterPolicy : public papaslog::Logging::OutputFilterPolicy {
public:
  GaudiFilterPolicy(IMessageSvc* owner) : m_messenger(owner), m_currentLevel(m_messenger.currentLevel()) {}
  bool doPrint(const papaslog::Logging::Level& lvl) const { return true; }

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
