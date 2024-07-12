//
// Created by jbnahan on 14/07/24.
//

#include "win32service_right.h"

#include <aclapi.h>

long change_service_right(char *service, char *username, long right, long type, char *machine, char *returnMessage) {

    long returnValue = 0;
    EXPLICIT_ACCESS      ea;
    SECURITY_DESCRIPTOR  sd;
    PSECURITY_DESCRIPTOR psd            = NULL;
    PACL                 pacl           = NULL;
    PACL                 pNewAcl        = NULL;
    BOOL                 bDaclPresent   = FALSE;
    BOOL                 bDaclDefaulted = FALSE;
    DWORD                dwError        = 0;
    DWORD                dwSize         = 0;
    DWORD                dwBytesNeeded  = 0;

    SC_HANDLE schSCManager;
    SC_HANDLE schService;
    sprintf(returnMessage, "OK" );

    // Get a handle to the SCM database.

    schSCManager = OpenSCManager(
            machine,                    // local computer
            NULL,                    // ServicesActive database
            SC_MANAGER_ALL_ACCESS);  // full access rights

    if (!schSCManager) {
        sprintf(returnMessage, "OpenSCManager failed" );
        return GetLastError();
    }


    // Get a handle to the service

    schService = OpenService(
            schSCManager,              // SCManager database
            service,                 // name of service
            READ_CONTROL | WRITE_DAC); // access

    if (schService == NULL)
    {
        sprintf(returnMessage, "OpenService failed" );
        CloseServiceHandle(schSCManager);
        return GetLastError();
    }

    // Get the current security descriptor.

    if (!QueryServiceObjectSecurity(schService,
                                    DACL_SECURITY_INFORMATION,
                                    &psd,           // using NULL does not work on all versions
                                    0,
                                    &dwBytesNeeded))
    {
        if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
        {
            dwSize = dwBytesNeeded;
            psd = (PSECURITY_DESCRIPTOR)HeapAlloc(GetProcessHeap(),
                                                  HEAP_ZERO_MEMORY, dwSize);
            if (psd == NULL)
            {
                // Note: HeapAlloc does not support GetLastError.
                sprintf(returnMessage, "HeapAlloc failed" );
                returnValue = 16000;
                goto dacl_cleanup;
            }

            if (!QueryServiceObjectSecurity(schService,
                                            DACL_SECURITY_INFORMATION, psd, dwSize, &dwBytesNeeded))
            {
                sprintf(returnMessage, "QueryServiceObjectSecurity failed" );
                returnValue = GetLastError();
                goto dacl_cleanup;
            }
        }
        else
        {
            sprintf(returnMessage, "QueryServiceObjectSecurity failed" );
            returnValue = GetLastError();
            goto dacl_cleanup;
        }
    }

    // Get the DACL.

    if (!GetSecurityDescriptorDacl(psd, &bDaclPresent, &pacl,
                                   &bDaclDefaulted))
    {
        sprintf(returnMessage, "GetSecurityDescriptorDacl failed" );
        returnValue = GetLastError();
        goto dacl_cleanup;
    }

    // Build the ACE.
    BuildExplicitAccessWithName(&ea, username,
                                right,
                                type, NO_INHERITANCE);

    dwError = SetEntriesInAcl(1, &ea, pacl, &pNewAcl);
    if (dwError != ERROR_SUCCESS)
    {
        sprintf(returnMessage, "SetEntriesInAcl failed");
        returnValue = dwError;

        goto dacl_cleanup;
    }

    // Initialize a new security descriptor.

    if (!InitializeSecurityDescriptor(&sd,
                                      SECURITY_DESCRIPTOR_REVISION))
    {
        sprintf(returnMessage, "InitializeSecurityDescriptor failed" );
        returnValue = GetLastError();
        goto dacl_cleanup;
    }

    // Set the new DACL in the security descriptor.

    if (!SetSecurityDescriptorDacl(&sd, TRUE, pNewAcl, FALSE))
    {
        sprintf(returnMessage, "SetSecurityDescriptorDacl failed" );
        returnValue = GetLastError();
        goto dacl_cleanup;
    }

    // Set the new DACL for the service object.

    if (!SetServiceObjectSecurity(schService,
                                  DACL_SECURITY_INFORMATION, &sd))
    {
        sprintf(returnMessage, "SetServiceObjectSecurity failed" );
        returnValue = GetLastError();
        goto dacl_cleanup;
    }
    else sprintf(returnMessage, "Service DACL updated successfully");

    dacl_cleanup:
    CloseServiceHandle(schSCManager);
    CloseServiceHandle(schService);

    if(NULL != pNewAcl)
        LocalFree((HLOCAL)pNewAcl);
    if(NULL != psd)
        HeapFree(GetProcessHeap(), 0, (LPVOID)psd);
    return returnValue;
}