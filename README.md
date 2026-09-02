[![License](https://img.shields.io/badge/license-PHP_License-blue.svg)](http://www.php.net/license/3_01.txt)
[![Documentation](https://img.shields.io/badge/manual-win32service-blue.svg)](http://php.net/manual/en/book.win32service.php)

# Win32Service

The win32service extension is a Windows-specific extension that allows PHP to communicate with the Service Control Manager to start, stop, register and unregister the services, and even allows your PHP scripts to run as a service.

| Branch | PHP Version | Support status |
|--------|-------------|----------------|
| v0.3.x | 5.x         | Unmaintained   |
| v0.4.x | 7.X         | Unmaintained   |
| v1.0.x | 8.0-8.5     | Unmaintained   |
| v1.1.x | 8.x         | Maintained     |

# Installation

## PIE Installation (recommanded)

Execute this command to install and activate Win32Service on your Windows PHP installation:

```shell
pie install win32service/win32service
```

Into you CI, you can lock the extension used to install a specific extension required by the project:

```shell
pie install --select win32service=win32service/win32service
```

## Manually

Download the latest DLL from release and copy it into your PHP ext directory.

# Build attestation

Since the version 1.1.1beta4, this repository generates a build attestation to certify the build provenance.

To verify the provenance, use the GitHub cli tool `gh attestation verify --owner win32service <release_dll_archive_path>`.

If you want to verify with [cosign](https://github.com/sigstore/cosign), use these options: `--certificate-oidc-issuer="https://token.actions.githubusercontent.com" --certificate-identity-regexp="^https://github.com/win32service/win32service/"`

# Documentation

See: [https://www.php.net/manual/en/book.win32service.php](https://www.php.net/manual/en/book.win32service.php)

# News

For project news, see the Github Discussions space: [https://github.com/win32service/win32service/discussions](https://github.com/win32service/win32service/discussions).
