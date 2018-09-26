#ifndef _SERRNO_H_
#define _SERRNO_H_

/// Errno-like error handling
int *serrno_location();
#define serrno (*serrno_location())

enum SERRNO_LIST {
	SWRONG_SYNT = 1,
	SUNKNWN_SYNT,
	SWRONG_DATA,
};

#endif // _SERRNO_H_
