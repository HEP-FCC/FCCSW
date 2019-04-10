#ifndef TRICKTRACK_LOGGER_H
#define TRICKTRACK_LOGGER_H

// keep dependencies minimal and only use spdlog optionally
#ifdef USE_SPDLOG

#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"


namespace tricktrack {

#ifdef USE_SPDLOG_STANDALONE
static std::shared_ptr<spdlog::logger> ttlog = spdlog::stdout_logger_st("ttlog");
#endif


}

#define TT_INFO(x, ...)  \
  spdlog::get("ttlog")->info(x, ##__VA_ARGS__);

#define TT_DEBUG(x, ...)  \
  spdlog::get("ttlog")->debug(x, ##__VA_ARGS__);

#define TT_WARNING(x, ...)  \
  spdlog::get("ttlog")->warn(x, ##__VA_ARGS__);

#define TT_ERROR(x, ...) \
  spdlog::get("ttlog")->error(x, ##__VA_ARGS__);

#define TT_CRITICAL(x, ...) \
  spdlog::get("ttlog")->critical(x, ##__VA_ARGS__);

#else // USE_SPDLOG

#define TT_INFO(x, ...);

#define TT_DEBUG(x, ...);

#define TT_WARNING(x, ...);

#define TT_ERROR(x, ...);

#define TT_CRITICAL(x, ...);


#endif // USE_SPDLOG


#endif // TRICKTRACK_LOGGER_H
