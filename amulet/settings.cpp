#include "settings.h"
#include <Arduino.h>
#include <InternalFileSystem.h>

using namespace Adafruit_LittleFS_Namespace;

bool settingsValid_ = false;

constexpr int16_t kGlobalSettingsSig = 0x6113;
constexpr int16_t kLocalSettingsSig = 0x10D2;

constexpr char kGlobalSettingsFile[] = "globalSettings";
constexpr char kLocalSettingsFile[] = "localSettings";

GlobalSettings globalSettings_{
	.signature_ = kGlobalSettingsSig,
	.version_ = 0,
	.brightness_ = {4, 8, 20},
	.txPower_ = 4,
	.runeSeenCountThreshold_ = 20,
	.ambientPowerThreshold_ = 10,
	.animationUpdateTimer_ = 2000, // in milliseconds. This determines how often we check a new top signal and change anims.

	.pad_ = {}};

constexpr anim_config_t kDefaultPattern{
	.anim_ = Anim::AnimPrism
};

LocalSettings localSettings_{
	.signature_ = kLocalSettingsSig,
	.version_ = 0,
	.configSize_ = sizeof(StartupConfig),
	.startupConfig_ = {},
	.powerPattern_ = {}};

void settings_init()
{
	localSettings_.startupConfig_.mode = AMULET_MODE_FIRSTBOOT;
	localSettings_.startupConfig_.pattern = kDefaultPattern;
	localSettings_.startupConfig_.ad = {
		.power = 100,
		.decay = 128,
		.range = -80,
	};

	InternalFS.begin();
	if (InternalFS.exists(kGlobalSettingsFile))
	{
		Serial.println("Reading global settings");
		auto file = InternalFS.open(kGlobalSettingsFile, Adafruit_LittleFS_Namespace::FILE_O_READ);
		if (file)
		{
			GlobalSettings tempSettings;
			if (file.read(&tempSettings, sizeof(GlobalSettings)) == sizeof(GlobalSettings))
			{
				if (tempSettings.signature_ == kGlobalSettingsSig && tempSettings.version_ >= globalSettings_.version_)
				{
					globalSettings_ = tempSettings;
					settingsValid_ = true;
					Serial.println("Read global settings");
				}
				else
				{
					Serial.println("Rejected global settings");
				}
			}

			file.close();
		}
	}

	if (InternalFS.exists(kLocalSettingsFile))
	{
		Serial.println("Reading local settings");
		auto file = InternalFS.open(kLocalSettingsFile, Adafruit_LittleFS_Namespace::FILE_O_READ);
		if (file)
		{
			LocalSettings tempSettings;
			if (file.read(&tempSettings, sizeof(LocalSettings)) == sizeof(LocalSettings))
			{
				if (tempSettings.signature_ == kLocalSettingsSig && tempSettings.configSize_ == sizeof(StartupConfig) && tempSettings.version_ >= localSettings_.version_)
				{
					localSettings_ = tempSettings;
					Serial.printf("Read local settings v%d\n", localSettings_.version_);
					dump_animation_to_console(localSettings_.startupConfig_.pattern);
				}
				else
				{
					Serial.println("Rejected local settings");
				}
			}
		}
		file.close();
	}

	if (!settingsValid_)
	{
		write_global_settings();
	}
}

void write_global_settings()
{
	Serial.println("Writing global settings");
	if (InternalFS.exists(kGlobalSettingsFile))
	{
		InternalFS.remove(kGlobalSettingsFile);
	}

	auto file = InternalFS.open(kGlobalSettingsFile, FILE_O_WRITE);
	if (file)
	{
		file.write((uint8_t *)&globalSettings_, sizeof(globalSettings_));
		file.close();
		settingsValid_ = true;
	}
}

void write_local_settings()
{
	if (InternalFS.exists(kLocalSettingsFile))
	{
		InternalFS.remove(kLocalSettingsFile);
	}
	localSettings_.version_++;
	Serial.printf("Writing local settings v%d\n", localSettings_.version_);
	File file(kLocalSettingsFile, FILE_O_WRITE, InternalFS);
	if (file)
	{
		size_t r = file.write((uint8_t *)&localSettings_, sizeof(localSettings_));
		Serial.printf("write %d\n", r);
		file.close();
		dump_animation_to_console(localSettings_.startupConfig_.pattern);
	}
}