<?php


/*
 * php_win32service.dll provided functions
 * Author: Credomane
 * Date: 5/25/16
 * Version: 2.2
 *
 * Requirements: php_win32service.dll V1.6-dev
 */


/*
 * This file is NOT to be used in production!
 * This file is for IDE's so they don't complain about unknown functions.
 * Also allows IDE's to know how to use them. Made for Eclipse + PDT
 * Your mileage may vary. 
 */





/**
 * Registers the script with the SCM, so that it can act as the service with the given name.
 * @param string $ServiceName Name of the service to represent.
 * @Return boolean|integer FALSE on success or Error code on failure.
 */
function win32_start_service_ctrl_dispatcher( $ServiceName ) {}

/**
 * Tells SCM the current status of the service.
 * @param string $ServiceStatus Status of the service.
 * @param number $Checkpoint [optional] only valid for pending start, stop, pause, or continue operations.
    Prevents SCM from thinking service is hung when pending operations take too long.
 * @Return boolean|integer TRUE on success. FALSE or Error code on failure.
 */
function win32_set_service_status( $ServiceStatus , $Checkpoint ) {}

/**
 * Adds specified service to the SCM database.
 * @param array $Details Associative array. See WindowsServiceControl -> Install for more details.
 * @param string $Machine [optional] Remote machine to install the service on. Probably only works in a Windows Domain.
 * @Return integer an WIN32_NO_ERROR or an error code.
 */
function win32_create_service( $Details , $Machine ) {}

/**
 * Removes specified service from the SCM database.
 * @param string $ServiceName Name of the service to delete. I advise you to stop the service first or SCM might have a heart attack.
 * @param string $Machine  [optional] Remote machine to install the service on. Probably only works in a Windows Domain.
 * @Return integer an WIN32_NO_ERROR or an error code.
 */
function win32_delete_service( $ServiceName , $Machine ) {}

/**
 * @Return integer the last control message that was sent to this service process.
 */
function win32_get_last_control_message() {}

/**
 * Queries SCM for the status of the specified service.
 * @param string $ServiceName Name of the service to query.
 * @param string $Machine [optional] Remote machine to install the service on. Probably only works in a Windows Domain.
 * @Return array an associative array containing status of the specified service.
 */
function win32_query_service_status( $ServiceName , $Machine ) {}

/**
 * Starts the specified service.
 * @param string $ServiceName Name of the service to query.
 * @param string $Machine [optional] Remote machine to start the service on. Probably only works in a Windows Domain.
 * @Return boolean|integer WIN32_NO_ERROR on success. FALSE or Error code on failure.
 */
function win32_start_service( $ServiceName , $Machine ) {}

/**
 * Stops the specified service.
 * @param string $ServiceName Name of the service to query.
 * @param string $Machine [optional] Remote machine to stop the service on. Probably only works in a Windows Domain.
 * @Return boolean|integer WIN32_NO_ERROR on success. FALSE or Error code on failure.
 */
function win32_stop_service( $ServiceName , $Machine ) {}

/**
 * Pauses the specified service.
 * @param string $ServiceName Name of the service to query.
 * @param string $Machine [optional] Remote machine to pause the service on. Probably only works in a Windows Domain.
 * @Return boolean|integer WIN32_NO_ERROR on success. FALSE or Error code on failure.
 */
function win32_pause_service( $ServiceName , $Machine ) {}

/**
 * Unpauses the specified service.
 * @param string $ServiceName Name of the service to query.
 * @param string $Machine [optional] Remote machine to unpause the service on. Probably only works in a Windows Domain.
 * @Return boolean|integer WIN32_NO_ERROR on success. FALSE or Error code on failure.
 */
function win32_continue_service( $ServiceName , $Machine ) {}

