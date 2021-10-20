/*
 *   utility.h
 *
 *   Define return values, cmd line arguments format, and other macros for main.cpp
 *
 *   Created: 9/16/21
 *   Last edited: 9/20/21
 */

#ifndef UTILITY_H
#define UTILITY_H

#define ARGS_NEEDED 2  // need at least two args

enum RET_VALUES
{
    SUCCESS,
    INVALID_ARG,
    NO_FILE,
    NO_PLAY_NAME,
    NO_VALID_ROLE
};

enum ARGS_FORMAT
{
    PROGRAM_NAME,
    FILE_NAME
};

#endif  // #define UTILITY_H