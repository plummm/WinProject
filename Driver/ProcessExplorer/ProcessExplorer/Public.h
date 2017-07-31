/*++

Module Name:

    public.h

Abstract:

    This module contains the common declarations shared by driver
    and user applications.

Environment:

    user and kernel

--*/

//
// Define an Interface Guid so that app can find the device and talk to it.
//

DEFINE_GUID (GUID_DEVINTERFACE_ProcessExplorer,
    0x9b48eb31,0xa975,0x42ca,0x9e,0x66,0x3c,0xbb,0x20,0x83,0x0c,0xbb);
// {9b48eb31-a975-42ca-9e66-3cbb20830cbb}
