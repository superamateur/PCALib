/*
 * Logger.cpp
 *
 *  Created on: 2017/05/20
 *      Author: work
 */

#include "Logger.h"

namespace NPCALogger {

CLogger::LogLevel CLogger::s_output_level = CLogger::LogLevel::Info;

CLogger::CLogger()
: m_output_stream(std::cout)
{
}
CLogger::CLogger(LogLevel level)
: CLogger()
{
	m_log_level = level;
	switch (m_log_level) {
	case LogLevel::Fatal:
		m_log_level_str = "[Fatal]";
		break;
	case LogLevel::Error:
		m_log_level_str = "[Error]";
		break;
	case LogLevel::Warning:
		m_log_level_str = "[Warning]";
		break;
	case LogLevel::Debug:
		m_log_level_str = "[Debug]";
		break;
	case LogLevel::Info:
		m_log_level_str = "[Info]";
		break;
	default:
		break;
	}
}

CLogger::~CLogger() {
	// TODO Auto-generated destructor stub
}

} /* namespace NPCALogger */
