---
layout: default
title: The Things Network
parent: Integrations
nav_order: 1
---

# The Things Network (The Things Stack Community Edition)

Integrations in The Things Network are configured at application level, so all the devices in an application share the same integrations.

In my IoT open Tech every user has a special device called "Control". This device can queue the messages received from The Things Network to the correct final device using the Device EUI as filtering criteria, so the application in The Things Network should be integrated to send telemetries to the user's Control device, that in turn will deliver them to the device with the matching Device EUI.

## Prerequisites

* A The Things Stack Community Edition account (account can be registered at https://console.cloud.thethings.network/).
* A valid email address to register an account in my IoT open Tech service.

## General procedure

1. Register a new user account in my IoT open Tech service to get the access token of the *Control* device. This access token will be used later (step 4) to configure the integration in The Things Stack Community Edition.
2. If the device type has not auto-provisioning capability (see [supported device/asset types table](https://iotopentech.github.io/myIoTopenTech/supported-devices.html)), create a new device in my IoT open Tech and indicate its Device EUI. Otherwise, this step is optional, and the device will be automatically created at the reception of its first telemetry from The Things Stack Community Edition, and besides the user will receive an email with the codes to integrate it with my IoT legram.
3. If it hasn't been done yet, create the application and the device in The Things Stack Community Edition console.
4. Configure the my IoT open Tech integration using the access token got in step 1.
5. Verify that the telemetries are properly received in the my IoT open Tech *Panel de control* dashboard.

## Example for integrating a Dragino LHT65 device

1. Use your web browser to open the address [https://iotopentech.io/register.php](https://iotopentech.io/register.php), write your email address, and press the Send button. After a few seconds, the web service will show the access token of your *Control* device (please, remember to keep it safe), and you will also receive an email message with the link to activate your my IoT open Tech account.

    ![imagen](https://user-images.githubusercontent.com/52624907/169708987-612435e9-b969-40e5-9342-912e38004224.png)

2. Click on the *Activate your account* link in the email message from the previous step to activate your account and choose a password.

    ![imagen](https://user-images.githubusercontent.com/52624907/169709254-588868fa-c4ee-4f3e-a3dd-f5f8f8e70d79.png)

    ![imagen](https://user-images.githubusercontent.com/52624907/169709279-00dd32d7-4835-4f82-b574-5e22a958868b.png)

3. Click on *Dashboards*, and then on *Configuración*.

    ![imagen](https://user-images.githubusercontent.com/52624907/169709327-280fd753-240f-486f-a25e-6a338ca7fa98.png)
    
    ![imagen](https://user-images.githubusercontent.com/52624907/169709500-623de388-3067-4372-aa7d-ae49b12ff913.png)

4. In the center panel *ACTIVOS Y DISPOSITIVOS*, click on the *ROOT* asset to open its configuration dialog box. In the *CREAR* tab, write a name for your new device, indicate that this element is a *Device* (not an *Asset*), choose the *LHT65* device type, and click on the *CREAR* button. The new device will appear nested below the *ROOT* asset.

    ![imagen](https://user-images.githubusercontent.com/52624907/169709728-56f387f1-cda2-4188-9cac-52fb8216da13.png)

5. Click on your new device to open its configuration dialog box. In the tab *CONFIGURAR*, click on the drop down *Device EUI* section, write the Device EUI of your device, and press the *CONFIGURAR* button.

    ![imagen](https://user-images.githubusercontent.com/52624907/169709887-e9203b03-a177-45a4-b736-73c10c15782d.png)

6. If you haven't done it yet, create an application and device in The Things Stack Community Edition console. You can find more information here:

    * [Adding Applications - The Things Stack for LoRaWAN](https://www.thethingsindustries.com/docs/integrations/adding-applications/)
    * [Creating applications and adding devices to The Things Stack - YouTube](https://www.youtube.com/watch?v=PpbkBgz1CbI)
7. Select *Integrations* in the left panel of your The Things Stack Community Edition application console, then *Webhooks*, and finally press the *Add webhook* button in the upper right. Write the access token you got in step 1, and click on *Create my iot open tech webhook*.

    ![imagen](https://user-images.githubusercontent.com/52624907/169710935-bed5c57b-70e9-44bc-9c71-f03c56408164.png)

8. You can verify that the integration is working properly simulating a telemetry message. In The Things Stack Community Edition console, open the *Messaging* tab of your new device, write `CBF0090C018201089E7FFF` (this is the raw payload of an LHT65 message) in the *Payload* field of the *Simulate uplink* section, and press the button *Simulate uplink*. Now open your *Panel de control* dashboard in my IoT open Tech and click on your device nested under the *ROOT* asset; the dashboard should show the telemetry as in the last image.

    ![imagen](https://user-images.githubusercontent.com/52624907/169710410-f1ebcda8-a8b3-4fb6-a48c-210bbc56fba4.png)
    
    ![imagen](https://user-images.githubusercontent.com/52624907/169710448-56ae680f-82ae-4f92-a231-7a92c5fdddeb.png)


