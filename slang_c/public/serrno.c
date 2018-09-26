#include "serrno.h"
_Thread_local static int _serrno = 0;

int *serrno_location()
{
	return &_serrno;
}
