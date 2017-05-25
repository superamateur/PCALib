/*
 * Logger.h
 *
 *  Created on: 2017/05/20
 *      Author: work
 */

#ifndef LOGGER_H_
#define LOGGER_H_

#include <iostream>

namespace NPCALogger {
#define DLOG(msg) { std::cout << __FILE__ << "::" << __FUNCTION__ << "::" << __LINE__ << "::" << msg << std::endl; }
#define DLOG_ARRAY(arr, len) { \
	std::cout << __FUNCTION__ << "::" << __LINE__ << "::"; \
	for(int i = 0; i < len; ++i) std::cout << arr[i] << " "; \
	std::cout << std::endl; \
	}

enum LogLevel {
	Info, Debug, Warning, Error, Fatal
};
class CLogger {
public:
	CLogger();
	virtual ~CLogger();

};

} /* namespace NPCALogger */

#endif /* LOGGER_H_ */
