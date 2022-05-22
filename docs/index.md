---
layout: default
title: Introduction
nav_order: 1
has_children: true
---

# A free, open and neutral IoT platform
{: .fs-9 }

my IoT open Tech is an IoT platform, developed on top of Things Board Community Edition (Open Source), with a user-centered design.
{: .fs-6 .fw-300 }

Users can store and chart the telemetries from their device, configure their features, and manage alarms.
{: .fs-6 .fw-300 }
---

The main aims of my IoT open Tech are:

* Lower the barrier to IoT technologies, even for non-techies.
* Protect data privacy and raise awareness of its importance. Your data is your data.
* Encourage the cooperative deployment of community IoT networks and tools.

# Features

* Easy: my IoT open Tech offers templates for diverse device types (see supported devices), and even some of them can be directly provisioned upon receiving its first telemetry.
* Flexible: If you need to use a device type that is not yet supported, you can develop the required template following the documented procedure.
* Versatile: my IoT open Tech is a web platform, but it also offers a Telegram bot, my IoT legram, that eases the access to the data and receiving alerts in portable devices.
* Free (as in free beer) but also without warranties: There is no fee for using my IoT open Tech, but neither a service level agreement of any kind. This service is supported by the membership contributions of the non-for-profit IoT open Tech association. We (the members of IoT open Tech) try to always keep the service in full working order, because we are also users of the platform. Nevertheless, there is an anti-abuse and resource-optimization mechanism based on credits; every user starts with 365 credits, and each of his devices consumes one credit per day, so the credit balance will eventually reach zero. At this point, the user has to request a balance reset to get another bunch of 365 credits; this request is free and helps to optimize the resources allocating them to the active users of the platform. 

# Most relevant capabilities

* Telemetries can be sent using HTTP requests or publishing MQTT messages. In the case of LoRaWAN, my IoT open Tech can also send downlinks through the Network Servers The Things Stack Community Edition, Everynet and ChirpStack.
* Multi-level tree structure based on assets and devices. An asset is an organizational entity that can group several other assets and/or devices. For example, in a hotel we could configure a "Floor 1" asset, that in turn would contain assets for every room in that floor, and those room assets would contain the devices of each room (like temperature sensors, door locks...)
* The user interacts with the platform mainly through two dashboards: Configuracion and Panel de control. The Configuracion dashboard is used to arrange the hierarchy of assets/devices, and to configure their features (for example, alarm's thresholds). The Panel de control dashboard offers telemetries' charts and the controls to operate the devices (for example, open an irrigation valve).
* A user can delegate a device to other user, limiting the granted telemetries and functionalities.
* Each device can have a public Panel de control dashboard, so everybody, even without a registered account in my IoT open Tech, can view it using a web explorer.
* Alarm's alerts can be received through email, IFTTT integration, Telegram, Matrix and WhatsApp (the WhatsApp integration uses the CallMeBot service, that may require fees). 
