/*
 * Logger.h
 *
 *  Created on: 2017/05/20
 *      Author: work
 */

#ifndef LOGGER_H_
#define LOGGER_H_

#include <iostream>
#include <sstream>

#define LOG_INFO(msg)    NPCALogger::CLogger::CreateLogInfoInstance()    << __FILE__ << "::" << __FUNCTION__ << "::" << __LINE__ << "::" << msg;
#define LOG_DEBUG(msg)   NPCALogger::CLogger::CreateLogDebugInstance()   << __FILE__ << "::" << __FUNCTION__ << "::" << __LINE__ << "::" << msg;
#define LOG_WARNING(msg) NPCALogger::CLogger::CreateLogWarningInstance() << __FILE__ << "::" << __FUNCTION__ << "::" << __LINE__ << "::" << msg;
#define LOG_ERROR(msg)   NPCALogger::CLogger::CreateLogErrorInstance()   << __FILE__ << "::" << __FUNCTION__ << "::" << __LINE__ << "::" << msg;
#define LOG_FATAL(msg)   NPCALogger::CLogger::CreateLogFatalInstance()   << __FILE__ << "::" << __FUNCTION__ << "::" << __LINE__ << "::" << msg;

#define LOG_ARRAY(arr, len) NPCALogger::CLogger::CreateLogInfoInstance() << __FILE__ << "::" << __FUNCTION__ << "::" << __LINE__ << "::" << NPCALogger::MakeLogArray(arr, len);

namespace NPCALogger {
class CLogger {
public:
	enum LogLevel {
		Fatal = 0, Error, Warning, Debug, Info
	};

	static CLogger& CreateLogFatalInstance() {
		static CLogger _s_fatal_log(LogLevel::Fatal);
		_s_fatal_log << "[FATAL]::";
		return _s_fatal_log;
	}

	static CLogger& CreateLogErrorInstance() {
		static CLogger _s_error_log(LogLevel::Error);
		return _s_error_log;
	}

	static CLogger& CreateLogWarningInstance() {
		static CLogger _s_warning_log(LogLevel::Warning);
		return _s_warning_log;
	}

	static CLogger& CreateLogDebugInstance() {
		static CLogger _s_debug_log(LogLevel::Debug);
		return _s_debug_log;
	}

	static CLogger& CreateLogInfoInstance() {
		static CLogger _s_info_log(LogLevel::Info);
		_s_info_log << "[INFO]::";
		return _s_info_log;
	}

	static void Configure(LogLevel level) { s_output_level = level; }

	template<typename T>
	const CLogger& operator<<(const T& msg) const {
		if(m_log_level <= s_output_level) {
			m_output_stream << msg;
		}
		return *this;
	}

	CLogger const& operator<<(std::ostream& (*os)(std::ostream&)) const
	{
		if(m_log_level <= s_output_level)
	    {
	        os(m_output_stream);
	    }

	    return *this;
	}

protected:
	CLogger();
	CLogger(LogLevel level);
	~CLogger();

private:
	static LogLevel s_output_level;
	LogLevel m_log_level;
	std::string m_log_level_str;
	std::ostream& m_output_stream;

	CLogger(const CLogger &) = delete;            // copy constructor
	CLogger& operator=(const CLogger &) = delete; // copy assignment
	CLogger(CLogger &&) = delete;                 // move constructor
	CLogger& operator=(CLogger &&) = delete;      // move assignment
};

template<typename ArrayType>
static std::string MakeLogArray(ArrayType array, const int len) {
	std::ostringstream oss;
	for(int i = 0; i < len; ++i) {
		oss << array[i];
		if(i < len - 1) oss << ", ";
		else oss << std::endl;
	}
	return oss.str();
}
} /* namespace NPCALogger */

#endif /* LOGGER_H_ */
