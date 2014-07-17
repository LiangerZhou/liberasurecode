/* 
 * <Copyright>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice, this
 * list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.  THIS SOFTWARE IS PROVIDED BY
 * THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _ERASURECODE_BACKEND_H_
#define _ERASURECODE_BACKEND_H_

#include "list.h"
#include "erasurecode.h"
#include "erasurecode_stdinc.h"

/* ~=*=~===~=*=~==~=*=~==~=*=~=  backend infrastructure =~=*=~==~=*=~==~=*=~ */

#ifdef __cplusplus
extern "C" {
#endif

#if defined (__GNUC__) && __GNUC__ > 3
#define dl_restrict __restrict
#else
#define dl_restrict
#endif

/* ==~=*=~===~=*=~==~=*=~==~=*=~= EC backend args =~==~=*=~==~=*=~===~=*=~== */

/* Arguments passed to the backend */
#define MAX_PRIV_ARGS 4
struct ec_backend_args {
    struct ec_args uargs;           /* common args passed in by the user */
    void *pargs[MAX_PRIV_ARGS];     /* used for private backend args */
};

/* =~===~=*=~==~=*=~==~=*=~=  backend stub definitions =~=*=~==~=*=~==~=*=~= */

#define INIT            init
#define EXIT            exit
#define ENCODE          encode
#define DECODE          decode
#define FRAGSNEEDED     fragments_needed
#define RECONSTRUCT     reconstruct

#define FN_NAME(s)      str(s)
#define str(s)          #s

/* EC backend stubs - implemented per backend */
struct ec_backend_op_stubs {
    /** Backend register/init, unregister/cleanup routines **/

    /* Private backend init routine */
    void * (*INIT)(struct ec_backend_args *args, void *sohandle);

    /* Private backend cleanup routine */
    int (*EXIT)(void *);

    /* Backend stub declarations */
    int (*ENCODE)(void *desc,
            char **data, char **parity, int blocksize);
    int (*DECODE)(void *desc,
            char **data, char **parity, int *missing_idxs, int blocksize);
    int (*FRAGSNEEDED)(void *desc,
            int *missing_idxs, int *fragments_needed);
    int (*RECONSTRUCT)(void *desc,
            char **data, char **parity, int *missing_idxs, int destination_idx,
            int blocksize);
};

/* ==~=*=~==~=*=~==~=*=~= backend struct definitions =~=*=~==~=*=~==~=*==~== */

struct ec_backend_desc {
    void *backend_desc;                             /* EC backend private descriptor */
    void *backend_sohandle;                         /* EC backend shared lib handle */
};

#define MAX_LEN     64
/* EC backend common attributes */
struct ec_backend_common {
    ec_backend_id_t             id;                 /* EC backend type */
    char                        name[MAX_LEN];      /* EC backend common name */
    char                        soname[PATH_MAX];   /* EC backend shared library path */
    char                        soversion[MAX_LEN]; /* EC backend shared library version */

    struct ec_backend_op_stubs  *ops;               /* EC backend stubs */
};

/* EC backend definition */
typedef struct ec_backend {
    struct ec_backend_common    common;             /* EC backend common attributes */
    struct ec_backend_args      args;               /* EC backend instance data (private) */

    int                         idesc;              /* liberasurecode instance handle */
    struct ec_backend_desc      desc;               /* EC backend instance handle */

    SLIST_ENTRY(ec_backend)     link;
} *ec_backend_t;

/* ~=*=~==~=*=~==~=*=~==~=*= frontend <-> backend API =*=~==~=*=~==~=*=~==~= */

/* Register a backend instance with liberasurecode */
int liberasurecode_backend_instance_register(ec_backend_t instance);

/* Unregister a backend instance */
int liberasurecode_backend_instance_unregister(ec_backend_t instance);


/* Generic dlopen/dlclose routines */
int liberasurecode_backend_open(ec_backend_t instance);
int liberasurecode_backend_close(ec_backend_t instance);


/* Backend query interface */

/* Name to ID mapping for EC backend */
ec_backend_id_t liberasurecode_backend_lookup_id(const char *name);

/* Get EC backend by name */
ec_backend_t liberasurecode_backend_lookup_by_name(const char *name);

/**
 * Look up a backend instance by descriptor
 *
 * Returns pointer to a registered liberasurecode instance
 * The caller must hold active_instances_rwlock
 */
ec_backend_t liberasurecode_backend_instance_get_by_desc(int desc);

/* =~=*=~==~=*=~==~=*=~==~=*=~===~=*=~==~=*=~===~=*=~==~=*=~===~=*=~==~=*=~= */

#endif  // _ERASURECODE_BACKEND_H_

