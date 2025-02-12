/**************************************************************************/
/*                                                                        */
/*       Copyright (c) Microsoft Corporation. All rights reserved.        */
/*                                                                        */
/*       This software is licensed under the Microsoft Software License   */
/*       Terms for Microsoft Azure RTOS. Full text of the license can be  */
/*       found in the LICENSE file at https://aka.ms/AzureRTOS_EULA       */
/*       and in the root directory of this software.                      */
/*                                                                        */
/**************************************************************************/


/**************************************************************************/
/**************************************************************************/
/**                                                                       */ 
/** USBX Component                                                        */ 
/**                                                                       */
/**   Device Storage Class                                                */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/

#define UX_SOURCE_CODE


/* Include necessary system files.  */

#include "ux_api.h"
#include "ux_device_class_storage.h"
#include "ux_device_stack.h"


#if UX_SLAVE_REQUEST_DATA_MAX_LENGTH < UX_SLAVE_CLASS_STORAGE_READ_CAPACITY_RESPONSE_LENGTH
#error UX_SLAVE_REQUEST_DATA_MAX_LENGTH is too small, please check
#endif

/**************************************************************************/ 
/*                                                                        */ 
/*  FUNCTION                                               RELEASE        */ 
/*                                                                        */ 
/*    _ux_device_class_storage_read_capacity              PORTABLE C      */ 
/*                                                           6.1.10       */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Chaoqiong Xiao, Microsoft Corporation                               */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */ 
/*    This function performs a READ_CAPACITY command.                     */ 
/*                                                                        */ 
/*  INPUT                                                                 */ 
/*                                                                        */ 
/*    storage                               Pointer to storage class      */ 
/*    endpoint_in                           Pointer to IN endpoint        */
/*    endpoint_out                          Pointer to OUT endpoint       */
/*    cbwcb                                 Pointer to CBWCB              */ 
/*                                                                        */ 
/*  OUTPUT                                                                */ 
/*                                                                        */ 
/*    Completion Status                                                   */ 
/*                                                                        */ 
/*  CALLS                                                                 */ 
/*                                                                        */ 
/*    _ux_device_class_storage_csw_send     Send CSW                      */ 
/*    _ux_device_stack_transfer_request     Transfer request              */ 
/*    _ux_device_stack_endpoint_stall       Stall endpoint                */
/*    _ux_utility_long_put_big_endian       Put 32-bit big endian         */ 
/*    _ux_utility_memory_copy               Copy memory                   */ 
/*    _ux_utility_memory_set                Set memory                    */ 
/*                                                                        */ 
/*  CALLED BY                                                             */ 
/*                                                                        */ 
/*    Device Storage Class                                                */ 
/*                                                                        */ 
/*  RELEASE HISTORY                                                       */ 
/*                                                                        */ 
/*    DATE              NAME                      DESCRIPTION             */ 
/*                                                                        */ 
/*  05-19-2020     Chaoqiong Xiao           Initial Version 6.0           */
/*  09-30-2020     Chaoqiong Xiao           Modified comment(s),          */
/*                                            optimized command logic,    */
/*                                            verified memset and memcpy  */
/*                                            cases,                      */
/*                                            resulting in version 6.1    */
/*  01-31-2022     Chaoqiong Xiao           Modified comment(s),          */
/*                                            added standalone support,   */
/*                                            resulting in version 6.1.10 */
/*                                                                        */
/**************************************************************************/
UINT  _ux_device_class_storage_read_capacity(UX_SLAVE_CLASS_STORAGE *storage, ULONG lun,
                                            UX_SLAVE_ENDPOINT *endpoint_in,
                                            UX_SLAVE_ENDPOINT *endpoint_out, UCHAR * cbwcb)
{

UINT                    status;
ULONG                   media_status;
UX_SLAVE_TRANSFER       *transfer_request;
UCHAR                   *read_capacity_buffer;

    UX_PARAMETER_NOT_USED(cbwcb);
    UX_PARAMETER_NOT_USED(endpoint_out);

    /* If trace is enabled, insert this event into the trace buffer.  */
    UX_TRACE_IN_LINE_INSERT(UX_TRACE_DEVICE_CLASS_STORAGE_READ_CAPACITY, storage, lun, 0, 0, UX_TRACE_DEVICE_CLASS_EVENTS, 0, 0)

    /* Obtain the status of the device.  */
    status =  storage -> ux_slave_class_storage_lun[lun].ux_slave_class_storage_media_status(storage, lun, 
                                storage -> ux_slave_class_storage_lun[lun].ux_slave_class_storage_media_id, &media_status);

    /* Update the request sense.  */
    storage -> ux_slave_class_storage_lun[lun].ux_slave_class_storage_request_sense_status = media_status;

    /* Check the status for error.  */
    if (status != UX_SUCCESS)
    {

#if !defined(UX_DEVICE_STANDALONE)

        /* We need to STALL the IN endpoint.  The endpoint will be reset by the host.  */
        _ux_device_stack_endpoint_stall(endpoint_in);
#endif

        /* Now we set the CSW with Error.  */
        storage -> ux_slave_class_storage_csw_status = UX_SLAVE_CLASS_STORAGE_CSW_FAILED;
        status = UX_SUCCESS;
    }
    else
    {
    
        /* Obtain the pointer to the transfer request.  */
        transfer_request =  &endpoint_in -> ux_slave_endpoint_transfer_request;

        /* Obtain read capacity response buffer.  */
        read_capacity_buffer = transfer_request -> ux_slave_transfer_request_data_pointer;
    
        /* Ensure it is cleaned.  */
        _ux_utility_memory_set(read_capacity_buffer, 0, UX_SLAVE_CLASS_STORAGE_READ_CAPACITY_RESPONSE_LENGTH); /* Use case of memcpy is verified. */
    
        /* Insert the last LBA address in the response.  */
        _ux_utility_long_put_big_endian(&read_capacity_buffer[UX_SLAVE_CLASS_STORAGE_READ_CAPACITY_RESPONSE_LAST_LBA],
                                        storage -> ux_slave_class_storage_lun[lun].ux_slave_class_storage_media_last_lba);
    
        /* Insert the block length in the response.  */
        _ux_utility_long_put_big_endian(&read_capacity_buffer[UX_SLAVE_CLASS_STORAGE_READ_CAPACITY_RESPONSE_BLOCK_SIZE],
                                        storage -> ux_slave_class_storage_lun[lun].ux_slave_class_storage_media_block_length);
    
#if defined(UX_DEVICE_STANDALONE)

        /* Next: Transfer (DATA).  */
        storage -> ux_device_class_storage_state = UX_DEVICE_CLASS_STORAGE_STATE_TRANS_START;
        storage -> ux_device_class_storage_cmd_state = UX_DEVICE_CLASS_STORAGE_CMD_READ;

        storage -> ux_device_class_storage_transfer = transfer_request;
        storage -> ux_device_class_storage_device_length =
                        UX_SLAVE_CLASS_STORAGE_READ_CAPACITY_RESPONSE_LENGTH;
        storage -> ux_device_class_storage_data_length =
                        UX_SLAVE_CLASS_STORAGE_READ_CAPACITY_RESPONSE_LENGTH;
        storage -> ux_device_class_storage_data_count = 0;
        UX_SLAVE_TRANSFER_STATE_RESET(storage -> ux_device_class_storage_transfer);

#else

        /* Send a data payload with the read_capacity response buffer.  */
        _ux_device_stack_transfer_request(transfer_request, 
                                      UX_SLAVE_CLASS_STORAGE_READ_CAPACITY_RESPONSE_LENGTH,
                                      UX_SLAVE_CLASS_STORAGE_READ_CAPACITY_RESPONSE_LENGTH);
#endif

        /* Now we set the CSW with success.  */
        storage -> ux_slave_class_storage_csw_status = UX_SLAVE_CLASS_STORAGE_CSW_PASSED;
        status = UX_SUCCESS;
    }
        
    /* Return completion status.  */
    return(status);
}

