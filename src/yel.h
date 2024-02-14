#pragma once

#ifdef DEBUG
#define YEL_INFO(TXT) printf("\033[34m[INFO] %s\n\033[m", TXT)
#define YEL_WARN(TXT) printf("\033[33m[WARN] %s\n\033[m", TXT)
#define YEL_CRIT(TXT) printf("\033[31m[CRIT] %s\n\033[m", TXT)
#else
#define YEL_INFO(TXT)
#define YEL_WARN(TXT)
#define YEL_CRIT(TXT)
#endif
