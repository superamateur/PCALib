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
	for(int _IiSaIiEERKNS = 0; _IiSaIiEERKNS < (int)len; ++_IiSaIiEERKNS) std::cout << arr[_IiSaIiEERKNS] << " "; \
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
