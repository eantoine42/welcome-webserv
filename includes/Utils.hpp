#ifndef UTILS_HPP
#define UTILS_HPP

#include <vector>
#include <iostream>

class Utils
{
    private:

        Utils();
		Utils(const Utils &copy);
		Utils &operator=(const Utils &copy);
		~Utils();

    public:

        static bool fileExists(const char* path);
        static bool fileRead(const char* path);
        static bool isDirectory(const char* path);
        static long long    getTimeOfDayMs();

};

template < class T >
inline std::ostream& operator<<(std::ostream& os, const std::vector<T>& v)
{
    for (typename std::vector<T>::const_iterator it = v.begin(); it != v.end(); ++it) {
        if (it == v.begin())
            os << *it;
        else
            os << "; " << *it;
    }
    return os;
}

#endif
