Example
=======

In service managment, you must separate 3 parts:
- The service administration actions: register, unregistrer. The Administrator's privilegies is needed.
- The current service managment actions: start, stop, pause, continue. The Administrator's privilegies is needed but you can set ACL in the service for delegate this action to another non administrator account.
- The service execution. This code is executed with the service account defined in the service configuration and execute your code.

# sample.php

This example, is short for show the minimum needed code for the service run.

# service.php

This is an all-in-one oriented object exemple. The 3 parts of action are in one class and all code is into.
