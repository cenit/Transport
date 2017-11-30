#ifndef WIN32

#define MIN(a,b)    (((a) < (b)) ? (a) : (b))
#define MAX(a,b)    (((a) > (b)) ? (a) : (b))

#else

template <class T>
T MAX(T x, T y)
{
   return (x > y) ? x : y;
}

template <class T>
T MIN(T x, T y)
{
   return (x < y) ? x : y;
}

#endif
