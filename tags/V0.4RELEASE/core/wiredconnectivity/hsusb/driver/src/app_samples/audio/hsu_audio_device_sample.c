/* Jungo Confidential, Copyright (c) 2008 Jungo Ltd.  http://www.jungo.com */

/*==============================================================================
Digital Audio over High Speed USB - device loopback sample application

GENERAL DESCRIPTION
Sample loopback application, which reads data from an input audio device and
writes it to an output audio device, using the Audio Function Driver.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2009 by QUALCOMM, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: 

when      who     what, where, why
--------  ---     ------------------------------------------------------
06/01/08  merez     First revision of file

==============================================================================*/

/*==============================================================================

INCLUDE FILES FOR MODULE

==============================================================================*/
#include <jusb_common.h>
#include <uw_device_audio.h>
#include "hsu_audio_device_sample.h"

/* If FEATURE_HS_USB_DIGITAL_AUDIO_LOOPBACK_H2D is defined the incoming frames are 
   expected from host.
   If the feature is not defined, the incoming frames are generated by the device */
#ifdef FEATURE_HS_USB_DIGITAL_AUDIO_LOOPBACK_H2D
#include <lists.h>

/*==============================================================================

LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

==============================================================================*/
/*==============================================================================
Constants and Macros
==============================================================================*/
#define SIZE_OF_BUFFER(app) app->buf_len + (juint32_t)sizeof(buf_desc_t)

/*==============================================================================
Typedefs
==============================================================================*/

typedef struct buf_desc {
    jdma_handle dma;
    juint32_t len;
    STAILQ_ENTRY(buf_desc) next;
} buf_desc_t, *buf_desc_h;

#endif

#define BUFFERING_TIME_MS  20

typedef struct {
    jbool_t started;
    juint32_t xfered;
} stream_info_t;

typedef struct {
    uwd_audio_drv_h fd;

    stream_info_t read;
    stream_info_t write;

    juint32_t buf_len;
    juint32_t samp_freq;
    juint8_t  samp_len;

#ifdef FEATURE_HS_USB_DIGITAL_AUDIO_LOOPBACK_H2D
    STAILQ_HEAD(done_list, buf_desc) done;

    juint32_t buffered;
    uwd_audio_stream_h wstream;
#endif
} app_ctx_t, *app_ctx_h;

/*          1                        2
 *        MIC IT  ---------------  USB OT 
 *                             (IN streaming)
 *     
 *          3                        4
 *        USB IT  --------------  SPEAKER OT
 *    (OUT streaming)
 */

#define ELEMENTS_IN_ARRAY(arr) (sizeof(arr) / sizeof(arr[0]))

#define MIC_TRM_ID 1
#define USB_OT_ID 2
#define USB_IT_ID 3
#define SPEAKER_TRM_ID 4

#define MONO 1
#define STEREO 2
#define CHANNELS 2

/*==============================================================================
Forward Declarations
==============================================================================*/

static void hsu_audio_device_sample_stream_select_cb(uwd_audio_app_h app_ctx, 
                                                     uwd_audio_stream_h stream, uwd_audio_stream_alt_h stream_alt, 
                                                     juint32_t samp_freq);

static void hsu_audio_device_sample_uninit(uwd_audio_app_h app_ctx);

#ifdef FEATURE_HS_USB_DIGITAL_AUDIO_LOOPBACK_H2D
static void hsu_audio_device_sample_done_free_all(app_ctx_h app);
#endif

/*==============================================================================
Global definitions
==============================================================================*/

/* Microphone Input Terminal */
static uwd_audio_in_trm_t mic_trm = {
    UWD_AUDIO_TT_MICROPHONE,                            /* trm_type       */
    USB_OT_ID,                                          /* assoc_terminal */
    CHANNELS,                                           /* nr_channels    */
    UWD_AUDIO_CC_LEFT_FRONT | UWD_AUDIO_CC_RIGHT_FRONT  /* channel_config */
};

/* USB Output Terminal */
static uwd_audio_out_trm_t usb_ot = {
    UWD_AUDIO_TT_USB_STREAMING,             /* trm_type       */
    MIC_TRM_ID,                             /* assoc_terminal */
    MIC_TRM_ID                              /* source_id      */
};

/* USB Input Terminal */
static uwd_audio_in_trm_t usb_it = {
    UWD_AUDIO_TT_USB_STREAMING,                         /* trm_type       */
    SPEAKER_TRM_ID,                                     /* assoc_terminal */
    CHANNELS,                                           /* nr_channels    */
    UWD_AUDIO_CC_LEFT_FRONT | UWD_AUDIO_CC_RIGHT_FRONT  /* channel_config */
};

/* Speaker Output Terminal */
static uwd_audio_out_trm_t speaker_trm = {
    UWD_AUDIO_TT_SPEAKER,                   /* trm_type       */
    USB_IT_ID,                              /* assoc_terminal */
    USB_IT_ID                               /* source_id      */
};

static uwd_audio_entity_t entities[] = {
    {
        MIC_TRM_ID,                         /* id             */
        UWD_AUDIO_ET_INPUT_TERMINAL,        /* type           */
        &mic_trm,                           /* *data          */
        "Microphone",                       /* *string        */
        NULL,                               /* req_cb         */
        NULL                                /* *app_priv      */
    },
    {
        USB_OT_ID,                          /* id             */
        UWD_AUDIO_ET_OUTPUT_TERMINAL,       /* type           */
        &usb_ot,                            /* *data          */
        "USB IN Stream",                    /* *string        */
        NULL,                               /* req_cb         */
        NULL                                /* *app_priv      */
    },
    {
        USB_IT_ID,                          /* id             */
        UWD_AUDIO_ET_INPUT_TERMINAL,        /* type           */
        &usb_it,                            /* *data          */
        "USB OUT Stream",                   /* *string        */
        NULL,                               /* req_cb         */
        NULL                                /* *app_priv      */
    },
    {
        SPEAKER_TRM_ID,                     /* id             */
        UWD_AUDIO_ET_OUTPUT_TERMINAL,       /* type           */
        &speaker_trm,                       /* *data          */
        "Speaker",                          /* *string        */
        NULL,                               /* req_cb         */
        NULL                                /* *app_priv      */
    }
};

/* Sampling Frequencies */
static juint32_t freq1[] = {16000};
static uwd_audio_freq_discrete_t samp_freq_16k  = {
    ELEMENTS_IN_ARRAY(freq1), 
    freq1
};

/* Data Formats */
static uwd_audio_format_I_t format_16k_16bit_mono = {
    MONO,                               /* nr_channels */
    16,                                 /* bit_resolution */
    &samp_freq_16k,                     /* *samp_freqs */
    NULL                                /* *freq_continuous */
};

/* Microphone Stream Alternate Settings */
static uwd_audio_stream_alt_t mic_stream_alts[] = {
    {
        UWD_AUDIO_IVAL_1_MS,                /* ep_ival */
        UWD_AUDIO_FD_CALC_MAX_PACKET,       /* ep_max_packet */
        USB_OT_ID,                          /* trm_link */
        1,                                  /* delay */
        UWD_AUDIO_DF_I_PCM,                 /* format_type */
        &format_16k_16bit_mono              /* format_data */
    }
};

/* Speaker Stream Alternate Settings */
static uwd_audio_stream_alt_t speaker_stream_alts[] = {
    {
        UWD_AUDIO_IVAL_1_MS,                /* ep_ival */
        UWD_AUDIO_FD_CALC_MAX_PACKET,       /* ep_max_packet */
        USB_IT_ID,                          /* trm_link */
        1,                                  /* delay */
        UWD_AUDIO_DF_I_PCM,                 /* format_type */
        &format_16k_16bit_mono              /* format_data */
    }
};

static uwd_audio_stream_t streams[] = {
    {
        ELEMENTS_IN_ARRAY(mic_stream_alts),     /* alts_num */
        mic_stream_alts,                        /* *alts */
        hsu_audio_device_sample_stream_select_cb, /* select_cb */
        NULL,                                   /* req_cb */
        NULL,                                   /* ep_req_cb */
        NULL                                    /* *app_priv */
    },
    {
        ELEMENTS_IN_ARRAY(speaker_stream_alts), /* alts_num */
        speaker_stream_alts,                    /* *alts */
        hsu_audio_device_sample_stream_select_cb, /* select_cb */
        NULL,                                   /* req_cb */
        NULL,                                   /* ep_req_cb */
        NULL                                    /* *app_priv */
    }
};


/* Audio Application Information 
 * (Should be available and unchanged until uninit callback called) */
static uwd_audio_config_t app_config = {
    entities,                    /* *entities */
    ELEMENTS_IN_ARRAY(entities), /* entities_num */
    streams,                     /* *streams */
    ELEMENTS_IN_ARRAY(streams),  /* streams_num */
    UWD_AUDIO_IVAL_16_MS,        /* intr_ep_ival */
    hsu_audio_device_sample_uninit  /* uninit */
};

/*==============================================================================

EXTERNAL FUNCTION DEFINITIONS FOR MODULE

==============================================================================*/

/*===========================================================================

FUNCTION uwd_audio_app_init

DESCRIPTION
This function is called by the stack and allocates the loopback application

DEPENDENCIES
None

RETURN VALUE


SIDE EFFECTS
None
===========================================================================*/
jresult_t uwd_audio_app_init(uwd_audio_drv_h fd_ctx, uwd_audio_app_h *app_ctx,
    uwd_audio_config_t **config)
{
    app_ctx_h app;
    
    DBG_I(DSLAVE_AUDIO_APP, ("AUDIO_APP: uwd_audio_app_init\n"));
        
    app = (app_ctx_h)jmalloc(sizeof(app_ctx_t), M_ZERO);
    if(!app)
    {
        DBG_E(DSLAVE_AUDIO_APP, ("AUDIO_APP: No memory\n"));
        return JENOMEM;
    }
    
    app->fd = fd_ctx;

    *config = &app_config;
    *app_ctx = app;

#ifdef FEATURE_HS_USB_DIGITAL_AUDIO_LOOPBACK_H2D
    /* Microphone stream */
    app->wstream = &streams[0];
    STAILQ_INIT(&app->done);
#endif
    return 0;
}

/*===========================================================================

FUNCTION hsu_audio_device_sample_uninit

DESCRIPTION
This function is called by the stack and releases all the allocated memory.

DEPENDENCIES
None

RETURN VALUE


SIDE EFFECTS
None
===========================================================================*/
static void hsu_audio_device_sample_uninit(uwd_audio_app_h app_ctx)
{
  DBG_I(DSLAVE_AUDIO_APP, ("AUDIO_APP: hsu_audio_device_sample_uninit\n"));
#ifdef FEATURE_HS_USB_DIGITAL_AUDIO_LOOPBACK_H2D
  hsu_audio_device_sample_done_free_all((app_ctx_h)app_ctx);
#endif
  jfree(app_ctx);
}

/*===========================================================================

FUNCTION hsu_audio_device_sample_stream_init

DESCRIPTION
The function initializes the stream 

DEPENDENCIES
None

RETURN VALUE
0 for failure, 1 for success

SIDE EFFECTS
None
===========================================================================*/
static jbool_t hsu_audio_device_sample_stream_init(app_ctx_h app, 
    uwd_audio_stream_alt_h stream_alt, juint32_t samp_freq)
{
    uwd_audio_format_I_t *fmt = (uwd_audio_format_I_t *)stream_alt->format;
    juint32_t samp_len = UWD_AUDIO_SUBFRAME_SIZE(fmt) * fmt->nr_channels;

    app->buf_len = samp_freq * samp_len * BUFFERING_TIME_MS / 1000;

    DBG_I(DSLAVE_AUDIO_APP, ("AUDIO_APP:  hsu_audio_device_sample_stream_init, freq %d Hz %d "
        "bits %s (buf_len=%d)\n", samp_freq, fmt->bit_resolution, 
        fmt->nr_channels == MONO ? "MONO":"STEREO", app->buf_len));

    if (!app->read.started && !app->write.started)
    {
        app->samp_freq = samp_freq;
        app->samp_len = samp_len;
        return 1;
    }

    if (app->samp_freq != samp_freq || app->samp_len != samp_len)
    {
        DBG_E(DSLAVE_AUDIO_APP, ("AUDIO_APP: Write and read settings are "
            "different\n"));
        return 0;
    }

    return 1;
}

#ifndef FEATURE_HS_USB_DIGITAL_AUDIO_LOOPBACK_H2D
/*===========================================================================

FUNCTION hsu_audio_device_sample_fill_check_buf

DESCRIPTION
This function fills the incoming frames data, in case we don't recieve them 
from the host

DEPENDENCIES
None

RETURN VALUE
0 for failure, 1 for success

SIDE EFFECTS
None
===========================================================================*/
static jbool_t hsu_audio_device_sample_fill_check_buf(stream_info_t *stream_info, jbool_t fill, 
    jdma_handle dma, juint32_t len)
{
    juint32_t i;

    for (i = 0; i < len; i++, stream_info->xfered++)
    {
        juint32_t count = stream_info->xfered;
        juint8_t x = count >> ((count & 3) * 8);

        if (fill)
        {
            *(juint8_t *)jdma_virt_addr(dma, i) = x;
        }
        else if (*(juint8_t *)jdma_virt_addr(dma, i) != x)
        {
            DBG_E(DSLAVE_AUDIO_APP, ("AUDIO_APP: ERROR DATA, xfered=%d, "
                "expected=%d received=%d\n", stream_info->xfered, x,
                *(juint8_t *)jdma_virt_addr(dma, i)));
            return 0;
        }
    }
    return 1;
}

static void hsu_audio_device_sample_start_io(app_ctx_h app, uwd_audio_stream_h stream, 
    stream_info_t *stream_info, jdma_handle dma);

/*===========================================================================

FUNCTION hsu_audio_device_sample_complete_cb

DESCRIPTION
A callback function to be called when the read / write operation is completed

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None
===========================================================================*/
static void hsu_audio_device_sample_complete_cb(uwd_audio_app_h app_ctx, uwd_audio_stream_h stream,
    void *arg, jresult_t stat, juint32_t len)
{
    app_ctx_h app = (app_ctx_h)app_ctx;
    jdma_handle dma = (jdma_handle)arg;
    stream_info_t *stream_info = stream->read ? &app->read : &app->write;

    if (!stream_info->started)
        goto Error;

    if (stat)
    {
        DBG_E(DSLAVE_AUDIO_APP, ("AUDIO_APP: read_cb error, stat=%d\n", stat));
        goto Error;
    }

    if (stream->read && ! hsu_audio_device_sample_fill_check_buf(stream_info, 0, dma, len))
        goto Error;

    DBG_V(DSLAVE_AUDIO_APP, ("AUDIO_APP: hsu_audio_device_sample_complete_cb, %s len=%d(%d)\n", 
        stream->read ? "READ" : "WRITE", len, stream_info->xfered));

    hsu_audio_device_sample_start_io(app, stream, stream_info, dma);
    return;
    
Error:
    jdma_free(dma);
}

/*===========================================================================

FUNCTION hsu_audio_device_sample_start_io

DESCRIPTION
This function starts the loopback streaming

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None
===========================================================================*/
static void hsu_audio_device_sample_start_io(app_ctx_h app, uwd_audio_stream_h stream, 
    stream_info_t *stream_info, jdma_handle dma)
{
    jresult_t rc;
    juint_t num = dma ? 1 : 2;

    for (rc = 0; num && !rc; num--, dma = NULL)
    {
        if (!dma)
        {
            rc = jdma_alloc(app->buf_len, 0, NULL, NULL, 0, &dma);
            if (rc)
                goto Exit;
        }

        if (stream->read)
        {
            rc = uwd_audio_stream_read(app->fd, stream,
                jdma_virt_addr(dma, 0), app->buf_len, hsu_audio_device_sample_complete_cb, dma);
        }
        else
        {
             hsu_audio_device_sample_fill_check_buf(stream_info, 1, dma, app->buf_len);

            rc = uwd_audio_stream_write(app->fd, stream,
                jdma_virt_addr(dma, 0), app->buf_len, hsu_audio_device_sample_complete_cb, dma);
        }

        if (rc)
            jdma_free(dma);
    }

Exit:
    stream_info->started = !rc;

    if (rc)
        DBG_E(DSLAVE_AUDIO_APP, ("AUDIO_APP: start_io failed, rc=%d\n", rc));
}

/*===========================================================================

FUNCTION hsu_audio_device_sample_stream_select_cb

DESCRIPTION
This function is called by the stack when the stream alternate setting is changed 
and start / stops the stream according to the alternate setting.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None
===========================================================================*/
static void hsu_audio_device_sample_stream_select_cb(uwd_audio_app_h app_ctx, 
    uwd_audio_stream_h stream, uwd_audio_stream_alt_h stream_alt, 
    juint32_t samp_freq)
{
    app_ctx_h app = (app_ctx_h)app_ctx;
    stream_info_t *stream_info = stream->read ? &app->read : &app->write;

    DBG_I(DSLAVE_AUDIO_APP, ("AUDIO_APP: %s stream %s\n", 
        stream->read ? "READ" : "WRITE",
        stream_alt ? "ENABLED" : "DISABLED"));

    if (!stream_alt)
    {
        stream_info->started = 0;

        return;
    }

    if (! hsu_audio_device_sample_stream_init(app, stream_alt, samp_freq))
        return;

    stream_info->xfered = 0;
    hsu_audio_device_sample_start_io(app, stream, stream_info, NULL);
}

#else
/* Host to Device */

/*===========================================================================

FUNCTION hsu_audio_device_sample_alloc_buf

DESCRIPTION
This function allocates a buffer for the incoming data

DEPENDENCIES
None

RETURN VALUE
A pointer to the allocated buffer

SIDE EFFECTS
None
===========================================================================*/
static buf_desc_h hsu_audio_device_sample_alloc_buf(app_ctx_h app)
{
    jresult_t rc;
    buf_desc_h buf = (buf_desc_h)jmalloc(sizeof(buf_desc_t), M_ZERO);

    if (!buf)
        return NULL;

    rc = jdma_alloc(app->buf_len, 0, NULL, NULL, 0, &buf->dma);
    if (rc)
    {
        jfree(buf);
        return NULL;
    }
    
    return buf;
}

/*===========================================================================

FUNCTION hsu_audio_device_sample_free_buf

DESCRIPTION
This function frees an allocated data buffer.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None
===========================================================================*/
static void hsu_audio_device_sample_free_buf(buf_desc_h buf)
{
    jdma_free(buf->dma);
    jfree(buf);
}

/*===========================================================================

FUNCTION hsu_audio_device_sample_done_remove_head

DESCRIPTION
This function remove a buffer from the buffering list (in order to write it 
to the write stream)

DEPENDENCIES
None

RETURN VALUE
A pointer to the data buffer removed from the list

SIDE EFFECTS
None
===========================================================================*/
static buf_desc_h hsu_audio_device_sample_done_remove_head(app_ctx_h app)
{
    buf_desc_h buf = STAILQ_FIRST(&app->done);

    if (buf)
    {
        STAILQ_REMOVE_HEAD(&app->done, next);
    }

    return buf;
}

/*===========================================================================

FUNCTION hsu_audio_device_sample_done_add_tail

DESCRIPTION
This function adds a buffer to the buffering list (where it waits to be written 
to the write stream)

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None
===========================================================================*/
static void hsu_audio_device_sample_done_add_tail(app_ctx_h app, buf_desc_h buf)
{
    if (app->buffered + SIZE_OF_BUFFER(app) > HSU_AUDIO_LOOPBACK_STACK_SIZE)
    {
        /* the queue is full, new buffer is deleted. */
        hsu_audio_device_sample_free_buf(buf);
        return;
    }

    app->buffered += SIZE_OF_BUFFER(app);
    STAILQ_INSERT_TAIL(&app->done, buf, next);
}


/*===========================================================================

FUNCTION hsu_audio_device_sample_done_free_all

DESCRIPTION
This function frees all the buffers taht are waiting to be written to the write stream
in the buffering list.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None
===========================================================================*/
static void hsu_audio_device_sample_done_free_all(app_ctx_h app)
{
    buf_desc_h buf;

    DBG_V(DSLAVE_AUDIO_APP, ("AUDIO_APP: done_free_all (%d)\n", app->buffered));

    while ((buf = STAILQ_FIRST(&app->done)) != NULL)
    {
        STAILQ_REMOVE_HEAD(&app->done, next);
        hsu_audio_device_sample_free_buf(buf);
    }
    app->buffered = 0;
}

static void hsu_audio_device_sample_read_start(app_ctx_h app, uwd_audio_stream_h stream, juint_t num);
static void hsu_audio_device_sample_write_reschedule(app_ctx_h app);

/*===========================================================================

FUNCTION hsu_audio_device_sample_write_cb

DESCRIPTION
This function is called when the write operation is completed.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None
===========================================================================*/
static void hsu_audio_device_sample_write_cb(uwd_audio_app_h app_ctx, uwd_audio_stream_h stream,
    void *arg, jresult_t stat, juint32_t len)
{
    app_ctx_h app = (app_ctx_h)app_ctx;
    buf_desc_h buf = (buf_desc_h)arg;

    if (stat)
        DBG_E(DSLAVE_AUDIO_APP, ("AUDIO_APP: write_cb error, stat=%d\n", stat));
    else
        DBG_X(DSLAVE_AUDIO_APP, ("AUDIO_APP: write_cb complete, len=%d\n",len));

    hsu_audio_device_sample_free_buf(buf);
    app->buffered -= SIZE_OF_BUFFER(app);
    if (app->write.started)
        hsu_audio_device_sample_write_reschedule(app);
}

/*===========================================================================

FUNCTION hsu_audio_device_sample_write_reschedule

DESCRIPTION
This function writes a buffer to the write stream.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None
===========================================================================*/
static void hsu_audio_device_sample_write_reschedule(app_ctx_h app)
{
    buf_desc_h buf;
    jresult_t rc = 0;

    while ((buf = hsu_audio_device_sample_done_remove_head(app)) != NULL)
    {
        rc = uwd_audio_stream_write(app->fd, app->wstream, 
            jdma_virt_addr(buf->dma, 0), buf->len, hsu_audio_device_sample_write_cb, buf);
        if (rc)
        {
            DBG_E(DSLAVE_AUDIO_APP, ("AUDIO_APP: Write error, rc=%d\n", rc));
            hsu_audio_device_sample_free_buf(buf);
            app->buffered -= SIZE_OF_BUFFER(app);
            /* Retry to write the next buffer from the done queue */
        }
    }
}

/*===========================================================================

FUNCTION hsu_audio_device_sample_read_cb

DESCRIPTION
This function is called when the read operation is completed.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None
===========================================================================*/
static void hsu_audio_device_sample_read_cb(uwd_audio_app_h app_ctx, uwd_audio_stream_h stream,
    void *arg, jresult_t stat, juint32_t len)
{
    app_ctx_h app = (app_ctx_h)app_ctx;
    buf_desc_h buf = (buf_desc_h)arg;

    if (!app->read.started)
        stat = JENXIO;

    if (stat)
        goto Error;

    DBG_V(DSLAVE_AUDIO_APP, ("AUDIO_APP: read_cb, len=%d\n", len));

    buf->len = len;
    
    /* Keep the incoming frame only if we can write it */
    if (app->write.started)
    {
        hsu_audio_device_sample_done_add_tail(app, buf);
        hsu_audio_device_sample_write_reschedule(app);
    }
    else
    {
        hsu_audio_device_sample_free_buf(buf);
    }

    hsu_audio_device_sample_read_start(app, stream, 1);
    
    return;
Error:
    DBG_E(DSLAVE_AUDIO_APP, ("AUDIO_APP: read_cb error, stat=%d\n", stat));
    hsu_audio_device_sample_free_buf(buf);
}

/*===========================================================================

FUNCTION hsu_audio_device_sample_read_start

DESCRIPTION
This function reads a buffer from the read stream.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None
===========================================================================*/
static void hsu_audio_device_sample_read_start(app_ctx_h app, uwd_audio_stream_h stream, juint_t num)
{
    jresult_t rc = 0;
    juint_t i;

    for (i = 0; i < num; i++)
    {
        buf_desc_h buf = hsu_audio_device_sample_alloc_buf(app);
        if (!buf)
        {
            rc = JENOMEM;
            goto Exit;
        }

        rc = uwd_audio_stream_read(app->fd, stream, jdma_virt_addr(buf->dma, 0),
            app->buf_len, hsu_audio_device_sample_read_cb, buf);
        if (rc)
        {
            hsu_audio_device_sample_free_buf(buf);
            goto Exit;
        }
    }

Exit:
    if (rc)
        DBG_E(DSLAVE_AUDIO_APP, ("AUDIO_APP: read_start failed, rc=%d\n", rc));
}

/*===========================================================================

FUNCTION hsu_audio_device_sample_stream_select_cb

DESCRIPTION
This function is called by the stack when the stream alternate setting is changed 
and start / stops the stream according to the alternate setting.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None
===========================================================================*/
static void hsu_audio_device_sample_stream_select_cb(uwd_audio_app_h app_ctx, 
    uwd_audio_stream_h stream, uwd_audio_stream_alt_h stream_alt, 
    juint32_t samp_freq)
{
    app_ctx_h app = (app_ctx_h)app_ctx;

    DBG_I(DSLAVE_AUDIO_APP, ("AUDIO_APP: %s stream %s\n", 
        stream->read ? "READ" : "WRITE", stream_alt ? "ENABLED" : "DISABLED"));

    if (!stream_alt)
    {
        if (stream->read)
        {
            app->read.started = 0;
        }
        else
        {
            app->write.started = 0;
            hsu_audio_device_sample_done_free_all(app);
        }

        return;
    }

    if (! hsu_audio_device_sample_stream_init(app, stream_alt, samp_freq))
        return;

    if (stream->read)
    {
        app->read.started = 1;
        hsu_audio_device_sample_read_start(app, stream, 2);
    }
    else
    {
        app->write.started = 1;
        hsu_audio_device_sample_write_reschedule(app);
    }
}
#endif
