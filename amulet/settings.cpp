#include "settings.h"
#include <Arduino.h>
#include <InternalFileSystem.h>

bool settingsValid_ = false;

constexpr int16_t kGlobalSettingsSig = 0x610B;
constexpr int16_t kLocalSettingsSig = 0x10CA;

constexpr char kGlobalSettingsFile[] = "globalSettings";
constexpr char kLocalSettingsFile[] = "localSettings";

GlobalSettings globalSettings_ {
	.signature_ = kGlobalSettingsSig,
	.version_ = 0,
	.brightness_ = {4,8,20},
	.txPower_=4,
	.pad_ = {}
};

LocalSettings localSettings_ {
	.signature_ = kLocalSettingsSig,
	.version_ = 0,
	.pad_ = {}
};

void settings_init()
{
	InternalFS.begin();
	if (InternalFS.exists(kGlobalSettingsFile)) {
		Serial.println("Reading global settings");
		auto file = InternalFS.open(kGlobalSettingsFile,  Adafruit_LittleFS_Namespace::FILE_O_READ);
		if (file) {
			GlobalSettings tempSettings;
			if (file.read(&tempSettings, sizeof(GlobalSettings)) == sizeof(GlobalSettings))
			{
				if (tempSettings.signature_ == kGlobalSettingsSig 
					&& tempSettings.version_ >= globalSettings_.version_) {
					globalSettings_ = tempSettings;
					settingsValid_ = true;
					Serial.println("Read global settings");
					} else {
						Serial.println("Rejected global settings");
					}
			}

			file.close();
		}

	}

	if (InternalFS.exists(kLocalSettingsFile)) {
		Serial.println("Reading local settings");
		auto file = InternalFS.open(kLocalSettingsFile,  Adafruit_LittleFS_Namespace::FILE_O_READ);
		if (file) {
			LocalSettings tempSettings;
			if (file.read(&tempSettings, sizeof(LocalSettings)) == sizeof(LocalSettings))
			{
				if (tempSettings.signature_ == kLocalSettingsSig 
					&& tempSettings.version_ >= localSettings_.version_) {
					localSettings_ = tempSettings;
					Serial.println("Read local settings");
					} else {
						Serial.println("Rejected local settings");
					}
			}

			file.close();
		}

	}

	if (!settingsValid_) {
		write_global_settings();
	}
}

void write_global_settings() {
	Serial.println("Writing global settings");
	auto file = InternalFS.open(kGlobalSettingsFile,  Adafruit_LittleFS_Namespace::FILE_O_WRITE);
	if (file) {
		file.write((uint8_t*) &globalSettings_, sizeof(globalSettings_));
		file.close();
		settingsValid_ = true;
	}
}

void write_local_settings() {
	Serial.println("Writing local settings");
	auto file = InternalFS.open(kLocalSettingsFile,  Adafruit_LittleFS_Namespace::FILE_O_WRITE);
	if (file) {
		file.write((uint8_t*) &localSettings_, sizeof(localSettings_));
		file.close();
	}
}