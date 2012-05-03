/*++

Copyright (c) 1997  Microsoft Corporation

Module Name:

    volmgr.h

Abstract:

    This file defines the services supplied by the volume managers.

Author:

    norbertk

Revision History:

--*/

#ifndef _VOLMGR_
#define _VOLMGR_

#define VOLMGRCONTROLTYPE   ((ULONG) 'v')

#define IOCTL_INTERNAL_VOLMGR_PARTITION_ARRIVED             CTL_CODE(VOLMGRCONTROLTYPE, 0, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_INTERNAL_VOLMGR_PARTITION_REMOVED             CTL_CODE(VOLMGRCONTROLTYPE, 1, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_INTERNAL_VOLMGR_WHOLE_DISK_REMOVED            CTL_CODE(VOLMGRCONTROLTYPE, 2, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_INTERNAL_VOLMGR_REFERENCE_DEPENDANT_VOLUMES   CTL_CODE(VOLMGRCONTROLTYPE, 3, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_INTERNAL_VOLMGR_LEGACY_PARTITION_ARRIVED      CTL_CODE(VOLMGRCONTROLTYPE, 4, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_INTERNAL_VOLMGR_LEGACY_PARTITION_REMOVED      CTL_CODE(VOLMGRCONTROLTYPE, 5, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_INTERNAL_VOLMGR_QUERY_CHANGE_PARTITION        CTL_CODE(VOLMGRCONTROLTYPE, 6, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_INTERNAL_VOLMGR_CANCEL_CHANGE_PARTITION       CTL_CODE(VOLMGRCONTROLTYPE, 7, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_INTERNAL_VOLMGR_PARTITION_CHANGED             CTL_CODE(VOLMGRCONTROLTYPE, 8, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_INTERNAL_VOLMGR_PMWMICOUNTERLIB_CONTEXT       CTL_CODE(VOLMGRCONTROLTYPE, 9, METHOD_BUFFERED, FILE_ANY_ACCESS)

//
// Input structure for IOCTL_INTERNAL_VOLMGR_PARTITION_ARRIVED,
// IOCTL_INTERNAL_VOLMGR_PARTITION_REMOVED,
// IOCTL_INTERNAL_VOLMGR_REFERENCE_DEPENDANT_VOLUMES,
// IOCTL_INTERNAL_VOLMGR_QUERY_CHANGE_PARTITION,
// IOCTL_INTERNAL_VOLMGR_CANCEL_CHANGE_PARTITION, and
// IOCTL_INTERNAL_VOLMGR_PARTITION_CHANGED.
//

typedef struct _VOLMGR_PARTITION_INFORMATION {
    PDEVICE_OBJECT  PartitionDeviceObject;
    PDEVICE_OBJECT  WholeDiskPdo;
} VOLMGR_PARTITION_INFORMATION, *PVOLMGR_PARTITION_INFORMATION;

//
// Input structure for IOCTL_INTERNAL_VOLMGR_WHOLE_DISK_REMOVED.
//

typedef struct _VOLMGR_WHOLE_DISK_INFORMATION {
    PDEVICE_OBJECT  WholeDiskPdo;
} VOLMGR_WHOLE_DISK_INFORMATION, *PVOLMGR_WHOLE_DISK_INFORMATION;

//
// Output structure for IOCTL_INTERNAL_VOLMGR_REFERENCE_DEPENDANT_VOLUMES.
//

typedef struct _VOLMGR_DEPENDANT_VOLUMES_INFORMATION {
    PDEVICE_RELATIONS   DependantVolumeReferences;
} VOLMGR_DEPENDANT_VOLUMES_INFORMATION, *PVOLMGR_DEPENDANT_VOLUMES_INFORMATION;

//
// Input structure for IOCTL_INTERNAL_VOLMGR_LEGACY_PARTITION_ARRIVED and
// IOCTL_INTERNAL_VOLMGR_LEGACY_PARTITION_REMOVED.
//

typedef struct _VOLMGR_LEGACY_PARTITION_INFORMATION {
    PDEVICE_OBJECT  LegacyWholeDiskDeviceObject;
    ULONG           DiskNumber;
    ULONG           PartitionNumber;
} VOLMGR_LEGACY_PARTITION_INFORMATION, *PVOLMGR_LEGACY_PARTITION_INFORMATION;

//
// Input structure for IOCTL_INTERNAL_VOLMGR_PMWMICOUNTERLIB_CONTEXT
//

typedef
NTSTATUS
(*PPMWMICOUNTERENABLE) (
    IN OUT PVOID* CounterContext
    );

typedef
BOOLEAN
(*PPMWMICOUNTERDISABLE) (
    IN PVOID* CounterContext,
    IN BOOLEAN ForceDisable,
    IN BOOLEAN DeallocateOnZero
    );

typedef
VOID
(*PPMWMICOUNTERIOSTART) (
    IN PVOID CounterContext,
    OUT PLARGE_INTEGER TimeStamp
    );

typedef
VOID
(*PPMWMICOUNTERIOCOMPLETE) (
    IN PVOID CounterContext,
    IN PIRP Irp,
    IN PLARGE_INTEGER TimeStamp
    );

typedef
VOID
(*PPMWMICOUNTERQUERY) (
    IN PVOID CounterContext,
    IN OUT PDISK_PERFORMANCE CounterBuffer,
    IN PWCHAR StorageManagerName,
    IN ULONG StorageDeviceNumber
    );

typedef struct _PMWMICOUNTERLIB_CONTEXT {
    PPMWMICOUNTERENABLE     PmWmiCounterEnable;
    PPMWMICOUNTERDISABLE    PmWmiCounterDisable;
    PPMWMICOUNTERIOSTART    PmWmiCounterIoStart;
    PPMWMICOUNTERIOCOMPLETE PmWmiCounterIoComplete;
    PPMWMICOUNTERQUERY      PmWmiCounterQuery;
} PMWMICOUNTERLIB_CONTEXT, *PPMWMICOUNTERLIB_CONTEXT;

//
// Volume managers should report this GUID in IoRegisterDeviceInterface.
//

DEFINE_GUID(VOLMGR_VOLUME_MANAGER_GUID, 0x53f5630e, 0xb6bf, 0x11d0, 0x94, 0xf2, 0x00, 0xa0, 0xc9, 0x1e, 0xfb, 0x8b);

#endif
