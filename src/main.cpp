/*
 * main.cpp
 *
 *  Created on: Sep 1, 2016
 *      Author: robin
 */


#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Adafruit_NeoPixel.h>
#include <ESP8266WiFiScan.h>

#define NUMPIXELS 24
#define PIN D5

#define SSID "LaurenAndRobin"
#define SCAN_LOOPS 8

ESP8266WiFiScanClass scan;

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

int c = 0;
int cc = 0;

bool active;
int networks;
bool scanning;
float v;

int imax(int l, int r) {
	return (l < r) ? r : l;
}

void setup() {
	pixels.begin();
	active = true;
	networks = -1;
	cc = 0;
	v = 1;
	scan.scanNetworks(true);
	scanning = true;
}

void check_scan() {
	if(networks < 0) {
		networks = scan.scanComplete();
		if(networks >= 0) {
			scanning = false;
			WiFi.softAP(SSID);
			WiFi.setOutputPower(0.25);
		}
	}
	if(networks < 0 || cc < active * SCAN_LOOPS)
		return;
	active = false;

	String ssid;
	uint8_t encryptionType;
	int32_t RSSI;
	uint8_t *BSSID;
	int32_t channel;
	bool isHidden;
	for(int i = 0; scan.getNetworkInfo(i, ssid, encryptionType, RSSI, BSSID, channel, isHidden); i++) {
		if(ssid == SSID) {
			active = true;
		}
	}
	scan.scanDelete();
	networks = -1;
	cc = 0;
}

void loop() {
	check_scan();
	for(int n = 0; n < NUMPIXELS; n++) {
		int o = (c + (n * 256) / NUMPIXELS) % 256;
		int r = 0, g = 0, b = 0;
/*
		if(active) {
			if(o < 256)
				r = imax(0, (o % 256) - 32);
			else if(o < 512)
				g = imax(0, (o % 256) - 32);
			else
				b = imax(0, (o % 256) - 32);
		}
*/
		r = 255 * (active ? (v + 0.25) / 1.25 : v);
		b = 255 * (active ? v : (v + 0.25) / 1.25);
		g = 255 * v;

		pixels.setPixelColor(n, r , g, b);
	}
	c--;
	if(c < 0) {
		c = 256 - 1;
		cc++;
		if(!scanning && cc >= SCAN_LOOPS) {
			scan.scanNetworks(true);
			networks = -1;
			scanning = true;
		}
	}
	if(active)
		v = (v + 0.0001) / 1.0001;
	else
		v = v / 1.003;
	pixels.show();
	delay(1);
}
