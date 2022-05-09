/*************************************************************************
** File: hs_test_utils.c
**
** NASA Docket No. GSC-18,476-1, and identified as "Core Flight System
** (cFS) Health and Safety (HS) Application version 2.3.2”
**
** Copyright © 2020 United States Government as represented by the
** Administrator of the National Aeronautics and Space Administration.
** All Rights Reserved.
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
** http://www.apache.org/licenses/LICENSE-2.0
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
**
** Purpose:
**   This file contains unit test utilities for the HS application.
**
*************************************************************************/

/*
 * Includes
 */

#include "hs_tbldefs.h"
#include "hs_app.h"
#include "hs_verify.h"
#include "hs_tbl.h"
#include "hs_msgdefs.h"
#include "hs_version.h"
#include "hs_events.h"
#include "hs_utils.h"
#include "hs_cmds.h"
#include "hs_custom.h"
#include "hs_monitors.h"
#include "hs_msg.h"
#include "hs_test_utils.h"

/* UT includes */
#include "uttest.h"
#include "utassert.h"
#include "utstubs.h"

/*
 * Function Definitions
 */

void HS_Test_Setup(void)
{
    /* initialize test environment to default state for every test */
    UT_ResetState(0);

    CFE_PSP_MemSet(&HS_AppData, 0, sizeof(HS_AppData_t));

} /* end HS_Test_Setup */

void HS_Test_TearDown(void)
{
    /* cleanup test environment */
} /* end HS_Test_TearDown */

void CFE_PSP_WatchdogEnable(void)
{
    UT_DEFAULT_IMPL(CFE_PSP_WatchdogEnable);
}

void CFE_PSP_WatchdogService(void)
{
    UT_DEFAULT_IMPL(CFE_PSP_WatchdogService);
}

void CFE_PSP_WatchdogSet(uint32 WatchdogValue)
{
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_PSP_WatchdogSet), WatchdogValue);
    UT_DEFAULT_IMPL(CFE_PSP_WatchdogSet);
}

int32 UT_Utils_stub_reporter_hook(void *UserObj, int32 StubRetcode, uint32 CallCount, const UT_StubContext_t *Context)
{
    uint8 i            = 0;    /* i is index */
    uint8 size_used    = 0;    /* determines size of argument to be saved */
    void *val_location = NULL; /* pointer to arg value to be saved */
    void *obj_ptr;             /* tracker indicates where to push data into UserObj */

    /* Determine where in the UserObj we should be located dependent upon CallCount */
    if (CallCount == 0)
    {
        obj_ptr = UserObj;
    }
    else
    {
        uint8 context_size = 0;

        for (i = 0; i < Context->ArgCount; ++i)
        {
            /* A UT_STUBCONTEXT_ARG_TYPE_DIRECT type indicates the arg itself is the ptr argument, add a (void*) size */
            if (Context->Meta[i].Type == UT_STUBCONTEXT_ARG_TYPE_DIRECT)
            {
                context_size += sizeof(void *);
            }
            else /* UT_STUBCONTEXT_ARG_TYPE_INDIRECT indicates the arg is pointing to the value to be saved, add its
                    size */
            {
                context_size += Context->Meta[i].Size;
            }
        }

        /* obj_ptr moves a full context_size for every call (initial value is 0) -- user object for calls > 1 must be an
         * array of contexts */
        obj_ptr = UserObj + (context_size * CallCount);
    }

    for (i = 0; i < Context->ArgCount; ++i)
    {
        /* UT_STUBCONTEXT_ARG_TYPE_DIRECT indicates the arg is the ptr that is to be saved */
        if (Context->Meta[i].Type == UT_STUBCONTEXT_ARG_TYPE_DIRECT)
        {
            val_location = &Context->ArgPtr[i];
            size_used    = sizeof(void *);
        }
        else /* UT_STUBCONTEXT_ARG_TYPE_INDIRECT indicates the arg is pointing to the value to be saved */
        {
            val_location = Context->ArgPtr[i];
            size_used    = Context->Meta[i].Size;
        }
        /* put the argument value into the user object */
        memcpy(obj_ptr, val_location, size_used);
        /* move to end of this size item in the user object */
        obj_ptr += size_used;
    }

    return StubRetcode;
}

/************************/
/*  End of File Comment */
/************************/
