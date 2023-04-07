/**
 * \ingroup processes
 * @file fork.h
 * @brief Definition of fork-related functions.
 */

#ifndef FORK_H
#define FORK_H

#include <stdint.h>

/**
 * \ingroup processes
 * Function for creating a new process.
 * @param fn A function to execute in a new thread.
 * @param arg An argument that need to be passed to this function.
 * @return
 * - On success: 0
 * - On failure: 1
 * @details Allocates a new @ref task_struct and makes it available for the scheduler.
 * @see task_struct, get_free_page()
 */
int copy_process(uint64_t fn, uint64_t arg);

#endif