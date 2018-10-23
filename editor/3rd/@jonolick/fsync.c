#if (defined _WIN32 || defined __WIN32__) && ! defined __CYGWIN__
#include <windows.h> // For FlushFileBuffers, GetLastError, etc...
#include <io.h> // for _get_ofshandle
#else
#include <unistd.h>
#endif

int jo_fsync(int fd) {
#if (defined _WIN32 || defined __WIN32__) && ! defined __CYGWIN__
	HANDLE h = (HANDLE)_get_osfhandle(fd);
	if (h == INVALID_HANDLE_VALUE) {
		errno = EBADF;
		return -1;
	}

	if(!FlushFileBuffers(h)) {
		DWORD err = GetLastError();
		switch (err) {
			case ERROR_ACCESS_DENIED: return 0;
			case ERROR_INVALID_HANDLE: errno = EINVAL; break;
			default: errno = EIO; break;
		}
		return -1;
	}
	return 0;
#else
	return fsync(fd);
#endif
}