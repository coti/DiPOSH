/*
 * Copyright (c) 2014-2019 LIPN - Universite Paris 13
 *                    All rights reserved.
 *
 * This file is part of POSH.
 * 
 * POSH is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * POSH is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with POSH.  If not, see <http://www.gnu.org/licenses/>.
 */

/* Performance information infrastructure */

#ifndef _POSH_PERF_H_
#define _POSH_PERF_H_

//#include <ostream>
#include <iostream>
#include <map>

#ifdef PERFPROFILING
#include <TAU.h>
#endif // PERFPROFILING

#include "shmem_utils.h" // for getArguments

/* NUMA hit and miss information */

class stats_all_t {
 public:
    long long numa_hit;
    std::string numa_hit_name = "numa_hit";
    long long numa_miss;
    std::string numa_miss_name = "numa_miss";
    long long numa_foreign;
    std::string numa_foreign_name = "numa_foreign";
    long long local_node;
    std::string local_node_name = "local_node" ;
    long long other_node;
    std::string other_node_name = "other_node";
    long long interleave_hit;
    std::string interleave_hit_name = "interleave_hit_name";

    std::map<std::string, long long> statsdico;
    
    friend std::ostream& operator<<( std::ostream&, stats_all_t& );
    friend stats_all_t operator-( stats_all_t , const stats_all_t& );

    /* constructor: all the values are set to -1 */
    stats_all_t(){
        this->numa_hit = -1;
        this->numa_miss = -1;
        this->numa_foreign = -1;
        this->local_node = -1;
        this->other_node = -1;
        this->interleave_hit = -1;
    }
};

/* Time spent in each function */

#ifdef __i386
extern __inline__ uint64_t rdtsc(void) {
  uint64_t x;
  __asm__ volatile ("rdtsc" : "=A" (x));
  return x;
}
#elif defined __amd64
extern __inline__ uint64_t rdtsc(void) {
  uint64_t a, d;
  __asm__ volatile ("rdtsc" : "=a" (a), "=d" (d));
  return (d<<32) | a;
}
#endif

class profile_functions_t{
 public:
    std::map<std::string, long long> functionsdico;

    void reinit( void ) {
        this->functionsdico.clear();
    }

    void output( void ) {
        std::cout << *this << std::endl;
    }
    
    friend std::ostream& operator<<( std::ostream&, profile_functions_t& );
};

/* WARNING: NOT THREAD SAFE */

extern profile_functions_t function_prof_info;

/* statistics for the whole machine */

int posh_get_machine_numa_stats( stats_all_t& );

/* profiling information */

/* TODO add a test to activate / desactivate */

#ifdef PERFPROFILING
#define POSH_PROF_INIT                                      \
    int __myargc;                                           \
    char **__myargv;                                        \
    getArguments( &__myargc, &__myargv );                   \
    TAU_PROFILE_INIT( __myargc, __myargv );
#else
#define POSH_PROF_INIT ;;
#endif // def PERFPROFILING

#ifdef PERFPROFILING
#define POSH_PROF_FUNCTION_PRELUDE                                      \
    uint64_t __posh_perf_time_enter = rdtsc();                          \
    TAU_PROFILE_TIMER( timer, __func__, "", TAU_USER );                 \
    TAU_REGISTER_CONTEXT_EVENT( hit_event, "NUMA hits");                \
    TAU_REGISTER_CONTEXT_EVENT( miss_event, "NUMA miss");               \
    TAU_REGISTER_CONTEXT_EVENT( foreign_event, "NUMA foreign");         \
    TAU_REGISTER_CONTEXT_EVENT( local_node_event, "NUMA local node");   \
    TAU_REGISTER_CONTEXT_EVENT( other_node_event, "NUMA other node");   \
    TAU_REGISTER_CONTEXT_EVENT( interleave_hit_event, "NUMA interleave hit event"); \
    TAU_PROFILE_START( timer );    
#else
#define POSH_PROF_FUNCTION_PRELUDE ;;
#endif // def PERFPROFILING

#ifdef PERFPROFILING
#define POSH_PROF_FUNCTION_CONCLUSION  do{                              \
        uint64_t __posh_perf_time_spent = rdtsc() -  __posh_perf_time_enter; \
        if( function_prof_info.functionsdico.count( __func__ ) > 0 ) {  \
            function_prof_info.functionsdico[ __func__ ] +=  __posh_perf_time_spent; \
        } else {                                                        \
            function_prof_info.functionsdico[ __func__ ] =  __posh_perf_time_spent; \
        }                                                               \
        /* Tau stuff */                                                 \
        TAU_PROFILE_STOP( timer );                                      \
        /* Get NUMA information */                                      \
        stats_all_t stats;                                              \
        posh_get_machine_numa_stats( stats );                           \
        TAU_CONTEXT_EVENT( hit_event, stats.statsdico["numa_hit"] );    \
        TAU_CONTEXT_EVENT( miss_event, stats.statsdico["numa_miss"] );  \
        TAU_CONTEXT_EVENT( foreign_event, stats.statsdico["numa_foreign"] ); \
        TAU_CONTEXT_EVENT( local_node_event, stats.statsdico["local_node"] ); \
        TAU_CONTEXT_EVENT( other_node_event, stats.statsdico["other_node"] ); \
        TAU_CONTEXT_EVENT( interleave_hit_event, stats.statsdico["interleave_hit"] ); \
} while( 0 ) ;
#else
#define POSH_PROF_FUNCTION_CONCLUSION ;;
#endif // def PERFPROFILING


#endif // _POSH_PERF_H_
