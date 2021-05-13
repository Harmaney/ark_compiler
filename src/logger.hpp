int LOG_LEVEL=3;

// FATAL ERROR WARN INFO DEBUG TRACE
// 0 1 2 3 4 5

#define TRACE(a) if(5<=LOG_LEVEL) {a}
#define DEBUG(a) if(4<=LOG_LEVEL) {a}
#define INFO(a) if(3<=LOG_LEVEL) {a}
#define WARN(a) if(2<=LOG_LEVEL) {a}
#define ERROR(a) if(1<=LOG_LEVEL) {a}
#define FATAL(a) if(0<=LOG_LEVEL) {a}