---
layout: default
title: The Things Network
parent: Integrations
nav_order: 2
---

# The Things Network (The Things Stack Community Edition)

Integrations in The Things Network are configured at application level, so all the devices in an application share the same integrations.

In my IoT open Tech every user has an special device called "Control". This device can queue the messages received from The Things Network to the correct final device using the Device EUI as filtering criteria, so the application in The Things Network should be integrated to send telemetries to the user's Control device, that in turn will deliver them to the device with the matching Device EUI.

##Prerequisites

* A The Things Stack Communnity Edition account (account can be registered at https://console.cloud.thethings.network/).
* A valid email address to register an account in my IoT open Tech service.

###General procedure

1. Register a new user account in my IoT open Tech service to get the access token of the Control device. This token will be used later to configure the integration in The Things Stack Community Edition.
2. If the device type has not auto-provisioning functionality (see supported devices table), create a new device in my IoT open Tech and indicate its Device EUI. Otherwise, this step is optional, and the device will be automatically created at the reception of its first telemetry from The Things Stack Community Edition, and besides the user will receive an email with the codes to integrate it with my IoT legram.
3. 
