#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winioctl.h>

#include "shared/ioctl.h"

DWORD startDriver(const wchar_t *sysPath) {
    SC_HANDLE scm = OpenSCManagerW(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    if (!scm) {
        return GetLastError();
    }

    SC_HANDLE svc = OpenServiceW(scm, L"SaturnAC", SERVICE_ALL_ACCESS);
    if (!svc) {
        svc = CreateServiceW(scm, L"SaturnAC", L"SaturnAC",
            SERVICE_ALL_ACCESS, SERVICE_KERNEL_DRIVER,
            SERVICE_DEMAND_START, SERVICE_ERROR_NORMAL,
            sysPath, NULL, NULL, NULL, NULL, NULL
        );
    }

    DWORD err = 0;
    if (!svc || !StartServiceW(svc, 0, NULL)) {
        err = GetLastError();
    }

    if (err == ERROR_SERVICE_ALREADY_RUNNING) {
        err = 0;
    }

    if (svc) {
        CloseServiceHandle(svc);
    }

    CloseServiceHandle(scm);
    return err;
}

void stopDriver(void) {
    SC_HANDLE scm = OpenSCManagerW(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    SC_HANDLE svc = OpenServiceW(scm, L"SaturnAC", SERVICE_ALL_ACCESS);
    SERVICE_STATUS status;
    ControlService(svc, SERVICE_CONTROL_STOP, &status);
    DeleteService(svc);
    CloseServiceHandle(svc);
    CloseServiceHandle(scm);
}

typedef enum  {
    HEARTBEAT_SUCCESS, HEARTBEAT_FAILED, HEARTBEAT_CS2_NOT_FOUND, HEARTBEAT_PENDING
} HEARTBEAT_STATUS;

HEARTBEAT_STATUS pingDriver(void) {
    HANDLE dev = CreateFileW(SATURN_USER_PATH, GENERIC_READ | GENERIC_WRITE,
        0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (dev == INVALID_HANDLE_VALUE) {
        return HEARTBEAT_FAILED;
    }

    ULONG reply = 0;
    DWORD got = 0;
    BOOL ok = DeviceIoControl(dev, IOCTL_HEARTBEAT, NULL, 0,
        &reply, sizeof reply, &got, NULL);

    CloseHandle(dev);

    if (reply == HEARTBEAT_STATUS_CS2_NOT_FOUND) {
        return HEARTBEAT_CS2_NOT_FOUND;
    } else if (reply == HEARTBEAT_MAGIC && ok && got == sizeof reply) {
        return HEARTBEAT_SUCCESS;
    } else {
        return HEARTBEAT_FAILED;
    }
}
