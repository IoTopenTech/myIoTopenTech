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

![imagen](https://user-images.githubusercontent.com/52624907/169384922-adb8ea4a-a984-444e-9c45-37123503decf.png)

The url format is:

https://my.iotopentech.io/api/v1/`access_token_copied_from_details_tab`/telemetry

## MQTT publishing
