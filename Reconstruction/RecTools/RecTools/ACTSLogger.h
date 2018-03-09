

#ifndef RECTRACKER_ACTSLOGGER_H
#define RECTRACKER_ACTSLOGGER_H

#include "ACTS/Utilities/Logger.hpp"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/MsgStream.h"

class GaudiFilterPolicy : public Acts::Logging::OutputFilterPolicy {
public:
  GaudiFilterPolicy(IMessageSvc* owner) : m_messenger(owner), m_currentLevel(m_messenger.currentLevel()) {}

  bool doPrint(const Acts::Logging::Level& lvl) const {
    MSG::Level l = MSG::VERBOSE;
    switch (lvl) {
    case Acts::Logging::VERBOSE:
      l = MSG::VERBOSE;
      break;
    case Acts::Logging::DEBUG:
      l = MSG::DEBUG;
      break;
    case Acts::Logging::INFO:
      l = MSG::INFO;
      break;
    case Acts::Logging::WARNING:
      l = MSG::WARNING;
      break;
    case Acts::Logging::ERROR:
      l = MSG::ERROR;
      break;
    case Acts::Logging::FATAL:
      l = MSG::FATAL;
      break;
    }
    MSG::Level cl = m_currentLevel;
    return l < cl;
  }

private:
  MsgStream m_messenger;
  MSG::Level m_currentLevel;
};

class GaudiPrintPolicy : public Acts::Logging::OutputPrintPolicy {
public:
  GaudiPrintPolicy(IMessageSvc* owner) : m_messenger(owner) {}

  void flush(const Acts::Logging::Level& lvl, const std::ostringstream& input) {
    MSG::Level l = MSG::VERBOSE;
    switch (lvl) {
    case Acts::Logging::VERBOSE:
      l = MSG::VERBOSE;
      break;
    case Acts::Logging::DEBUG:
      l = MSG::DEBUG;
      break;
    case Acts::Logging::INFO:
      l = MSG::INFO;
      break;
    case Acts::Logging::WARNING:
      l = MSG::WARNING;
      break;
    case Acts::Logging::ERROR:
      l = MSG::ERROR;
      break;
    case Acts::Logging::FATAL:
      l = MSG::FATAL;
      break;
    }

    m_messenger << l << input.str() << endmsg;
  }

private:
  MsgStream m_messenger;
};

#endif  // RECTRACKER_ACTSLOGGER_H
