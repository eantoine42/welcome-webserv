#include "Utils.hpp"
#include <sys/stat.h> // stat
#include <unistd.h> // access
#include <sys/time.h> // gettimeofday

/*****************
* CANNONICAL FORM
*****************/

Utils::Utils()
{}

Utils::Utils(Utils const & copy)
{
    (void) copy;
}

Utils & Utils::operator=(Utils const & rhs)
{
    (void) rhs;
    return (*this);
}

Utils::~Utils()
{}
/******************************************************************************/

bool    Utils::fileExists(const char* path) {
    return access(path, F_OK) != -1;
}
bool    Utils::fileRead(const char* path) {
    return access(path, R_OK) != -1;
}

bool    Utils::isDirectory(const char* path)
{
    struct stat statbuf;
    if (stat(path, &statbuf) != 0) {
        return false;
    }
    return S_ISDIR(statbuf.st_mode);
}

long long	Utils::getTimeOfDayMs(){
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return static_cast<long long>(tv.tv_sec) *1000LL + static_cast<long long> (tv.tv_usec)/1000LL;
}
