#include <iostream>
#include <string>
#include <ctime>
#include <vector>
#include <grpc++/grpc++.h>

#define MAX_DATA 256

enum IStatus
{
    SUCCESS,
    FAILURE_ALREADY_EXISTS,
    FAILURE_NOT_EXISTS,
    FAILURE_INVALID_USERNAME,
    FAILURE_INVALID,
    FAILURE_UNKNOWN
};
