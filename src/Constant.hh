#ifdef ENABLE_REPORT
#define REPORT(x) std::cout << x << std::endl
#else
#define REPORT(x) 0
#endif

#define LOG(x) std::clog << x << std::endl
