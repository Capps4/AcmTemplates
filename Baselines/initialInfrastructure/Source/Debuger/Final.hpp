#pragma once

// SNIPPET BEGIN
inline char debugerOptions[] = "color: false, space: false, precision: 6";
#if defined(COMPETITION_DEBUGER) && defined(strdup)
inline const char *$ = strdup(debugerOptions);
#else
inline const char *$ = debugerOptions;
#endif
#ifndef COMPETITION_DEBUGER
#define debug(...)
#endif
