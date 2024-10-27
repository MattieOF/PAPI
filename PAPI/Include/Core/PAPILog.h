﻿#pragma once

// MW @gotcha: This file should only be included in the PCH, or else the Linux build will fail due to redefinition
//			   fmt overloads at the end of the file.

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/fmt/ostr.h" // Needed for logging some types

extern std::shared_ptr<spdlog::logger> g_PAPILogger;

void InitLog(const char *prefPath);
void AddSinkToLog(const spdlog::sink_ptr &sink);

#ifndef PAPI_NO_LOG
	#define PAPI_TRACE(format, ...)                  g_PAPILogger->trace(format "\n" __VA_OPT__(,) __VA_ARGS__)
	#define PAPI_INFO(format, ...)                   g_PAPILogger->info(format "\n" __VA_OPT__(,) __VA_ARGS__)
	#define PAPI_INFO(format, ...)                   g_PAPILogger->info(format "\n" __VA_OPT__(,) __VA_ARGS__)
	#define PAPI_WARN(format, ...)                   g_PAPILogger->warn(format "\n" __VA_OPT__(,) __VA_ARGS__)
	#define PAPI_ERROR(format, ...)                  g_PAPILogger->error(format "\n" __VA_OPT__(,) __VA_ARGS__)
	#define PAPI_CRITICAL(format, ...)               g_PAPILogger->critical(format "\n" __VA_OPT__(,) __VA_ARGS__)

	#define PAPI_TRACE_NO_NEWLINE(format, ...)       g_PAPILogger->trace(format __VA_OPT__(,) __VA_ARGS__)
	#define PAPI_INFO_NO_NEWLINE(format, ...)        g_PAPILogger->info(format __VA_OPT__(,) __VA_ARGS__)
	#define PAPI_WARN_NO_NEWLINE(format, ...)        g_PAPILogger->warn(format __VA_OPT__(,) __VA_ARGS__)
	#define PAPI_ERROR_NO_NEWLINE(format, ...)       g_PAPILogger->error(format __VA_OPT__(,) __VA_ARGS__)
	#define PAPI_CRITICAL_NO_NEWLINE(format, ...)    g_PAPILogger->critical(format __VA_OPT__(,) __VA_ARGS__)
#else
	#define PAPI_TRACE(...)      
	#define PAPI_INFO(...)       
	#define PAPI_WARN(...)       
	#define PAPI_ERROR(...)      
	#define PAPI_CRITICAL(...)

	#define PAPI_TRACE_NO_NEWLINE(format, ...)     
	#define PAPI_INFO_NO_NEWLINE(format, ...)      
	#define PAPI_WARN_NO_NEWLINE(format, ...)      
	#define PAPI_ERROR_NO_NEWLINE(format, ...)     
	#define PAPI_CRITICAL_NO_NEWLINE(format, ...)  
#endif


template <>
class fmt::formatter<glm::ivec2> {
public:
	constexpr auto parse (format_parse_context& ctx) { return ctx.begin(); }
	template <typename Context>
	constexpr auto format (glm::ivec2 const& vec, Context& ctx) const {
		return fmt::format_to(ctx.out(), "({},{})", vec.x, vec.y);
	}
};
