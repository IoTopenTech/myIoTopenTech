---
layout: default
title: The Things Network
parent: Integrations
nav_order: 1
---

# The Things Network (The Things Stack Community Edition)

Integrations in The Things Network are configured at application level, so all the devices in an application share the same integrations.

In my IoT open Tech every user has an special device called "Control". This device can queue the messages received from The Things Network to the correct final device using the Device EUI as filtering criteria, so the application in The Things Network should be integrated to send telemetries to the user's Control device, that in turn will deliver them to the device with the matching Device EUI.

## Prerequisites

* A The Things Stack Communnity Edition account (account can be registered at https://console.cloud.thethings.network/).
* A valid email address to register an account in my IoT open Tech service.

## General procedure

1. Register a new user account in my IoT open Tech service to get the access token of the *Control* device. This access token will be used later (step 4) to configure the integration in The Things Stack Community Edition.
2. If the device type has not auto-provisioning capability (see [supported device/asset types table](https://iotopentech.github.io/myIoTopenTech/supported-devices.html)), create a new device in my IoT open Tech and indicate its Device EUI. Otherwise, this step is optional, and the device will be automatically created at the reception of its first telemetry from The Things Stack Community Edition, and besides the user will receive an email with the codes to integrate it with my IoT legram.
3. If it hasn't been done yet, create the application and the device in The Things Stack Community Edition console.
4. Configure the my IoT open Tech integration using the access token got in step 1.
5. Verify that the telemetries are properly received in the my IoT open Tech *Panel de control* dashboard.

## Example for integrating a Dragino LHT65 device

1. Use your web browser to open the address https://iotopentech.io/register.php, write your email address, and press the Send button. After a few seconds, the web service will show the access token of your *Control* device (please, remember to keep it safe), and you will also receive an email message with the link to activate your my IoT open Tech account.

![imagen](https://user-images.githubusercontent.com/52624907/169708987-612435e9-b969-40e5-9342-912e38004224.png)

2. Click on the *Activate your account* link in the email message from the previous step to activate your account and choose a password.

![imagen](https://user-images.githubusercontent.com/52624907/169709254-588868fa-c4ee-4f3e-a3dd-f5f8f8e70d79.png)

![imagen](https://user-images.githubusercontent.com/52624907/169709279-00dd32d7-4835-4f82-b574-5e22a958868b.png)

3. 
