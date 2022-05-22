---
layout: default
title: Integrations
has_children: true
---

# Integrations

my IoT open Tech can receive telemetries through HTTP requests and MQTT publishing.
{: .fs-6 .fw-300 }

## HTTP Request

Every device has an access token in its details tab.

![imagen](https://user-images.githubusercontent.com/52624907/169706220-d9e40e8c-794e-4ca1-817a-930c0429926a.png)

The url format for sending telemetries is:

https://my.iotopentech.io/api/v1/`access_token_copied_from_details_tab`/telemetry

The request's type mush be `POST` and should use the following header:

`Content-Type: application/json`

## MQTT publishing

To send a telemetry using MQTT, a JSON message must be published in topic `v1/devices/me/telemetry` of the my.iotopentech.io server using the access token of the device as the user name (the password should be empty).
