#include <ntifs.h>

#include "driver/callbacks.h"

#include "shared/ioctl.h"

NTSTATUS DriverCreateClose(PDEVICE_OBJECT DeviceObject, PIRP Irp) {
    UNREFERENCED_PARAMETER(DeviceObject);

    Irp->IoStatus.Status = STATUS_SUCCESS;
    Irp->IoStatus.Information = 0;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    return STATUS_SUCCESS;
}

NTSTATUS DriverControl(PDEVICE_OBJECT DeviceObject, PIRP Irp) {
    UNREFERENCED_PARAMETER(DeviceObject);

    PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(Irp);
    NTSTATUS status = STATUS_SUCCESS;
    ULONG written = 0;

    switch (stack->Parameters.DeviceIoControl.IoControlCode) {
        case IOCTL_HEARTBEAT: {
            if (stack->Parameters.DeviceIoControl.OutputBufferLength < sizeof(ULONG)) {
                status = STATUS_BUFFER_TOO_SMALL;
                break;
            }

            if (!g_Cs2Pid) {
                *(PULONG)Irp->AssociatedIrp.SystemBuffer = HEARTBEAT_STATUS_CS2_NOT_FOUND;
                written = sizeof(ULONG);
                break;
            } else {
                *(PULONG)Irp->AssociatedIrp.SystemBuffer = HEARTBEAT_MAGIC;
                written = sizeof(ULONG);                
            }

            break;
        }
        
        default: {
            status = STATUS_INVALID_DEVICE_REQUEST;
        }
    }

    Irp->IoStatus.Status = status;
    Irp->IoStatus.Information = written;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    return status;
}

VOID DriverUnload(IN PDRIVER_OBJECT DriverObject) {
    UNICODE_STRING symlink = RTL_CONSTANT_STRING(SATURN_SYMLINK_NAME);
    IoDeleteSymbolicLink(&symlink);
    IoDeleteDevice(DriverObject->DeviceObject);

    NTSTATUS threadStatus;
    threadStatus = PsRemoveCreateThreadNotifyRoutine(createThreadRoutine);
    if (!NT_SUCCESS(threadStatus)) {
        DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL,
            "saturn: failed to unset ThreadNotifyRoutine, err: %lu\n", threadStatus);
    }

    NTSTATUS processStatus;
    processStatus = PsSetCreateProcessNotifyRoutineEx(createProcessRoutine, TRUE);
    if (!NT_SUCCESS(processStatus)) {
        DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL,
            "saturn: failed to unset ProcessNotifyRoutineEx, err: %lu\n", processStatus);
    }

    NTSTATUS imageStatus;
    imageStatus = PsRemoveLoadImageNotifyRoutine(loadImageRoutine);
    if (!NT_SUCCESS(imageStatus)) {
        DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL,
            "saturn: failed to unset ImageNotifyRoutineEx, err: %ld\n", processStatus);
    }

    DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "saturn: unloaded\n");
}

NTSTATUS DriverEntry(
    IN PDRIVER_OBJECT  DriverObject,
    IN PUNICODE_STRING RegistryPath
) {
    UNREFERENCED_PARAMETER(RegistryPath);
    
    DriverObject->DriverUnload = DriverUnload;
    DriverObject->MajorFunction[IRP_MJ_CREATE] = DriverCreateClose;
    DriverObject->MajorFunction[IRP_MJ_CLOSE] = DriverCreateClose;
    DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DriverControl;

    UNICODE_STRING devName = RTL_CONSTANT_STRING(SATURN_DEVICE_NAME);
    UNICODE_STRING symlink = RTL_CONSTANT_STRING(SATURN_SYMLINK_NAME);
    PDEVICE_OBJECT device;

    NTSTATUS deviceStatus = IoCreateDevice(DriverObject, 0, &devName,
        0x8000, FILE_DEVICE_SECURE_OPEN, FALSE, &device);
    if (!NT_SUCCESS(deviceStatus)) {
        DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL,
            "saturn: failed to create device, err: %ld\n", deviceStatus);
        return deviceStatus;
    }

    deviceStatus = IoCreateSymbolicLink(&symlink, &devName);
    if (!NT_SUCCESS(deviceStatus)) {
        DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL,
            "saturn: failed to create symlink, err: %ld\n", deviceStatus);
        IoDeleteDevice(device);
        return deviceStatus;
    }

    NTSTATUS threadStatus;
    threadStatus = PsSetCreateThreadNotifyRoutine(createThreadRoutine);
    if (!NT_SUCCESS(threadStatus)) {
        DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL,
            "saturn: failed to set ThreadNotifyRoutine, err: %ld\n", threadStatus);
    }

    NTSTATUS processStatus;
    processStatus = PsSetCreateProcessNotifyRoutineEx(createProcessRoutine, FALSE);
    if (!NT_SUCCESS(threadStatus)) {
        DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL,
            "saturn: failed to set ProcessNotifyRoutineEx, err: %ld\n", processStatus);
    }

    NTSTATUS imageStatus;
    imageStatus = PsSetLoadImageNotifyRoutineEx(loadImageRoutine, 0);
    if (!NT_SUCCESS(imageStatus)) {
        DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL,
            "saturn: failed to set LoadImageNotifyRoutineEx, err: %ld\n", processStatus);
    }
    
    DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "saturn: loaded\n");

    return STATUS_SUCCESS;
}