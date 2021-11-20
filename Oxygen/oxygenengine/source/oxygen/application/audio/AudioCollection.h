/*
*	Part of the Oxygen Engine / Sonic 3 A.I.R. software distribution.
*	Copyright (C) 2017-2021 by Eukaryot
*
*	Published under the GNU GPLv3 open source software license, see license.txt
*	or https://www.gnu.org/licenses/gpl-3.0.en.html
*/

#pragma once

#include <rmxbase.h>

class AudioSourceBase;


class AudioCollection
{
public:
	enum class Package
	{
		ORIGINAL,
		REMASTERED,
		MODDED
	};

	struct AudioDefinition;

	struct SourceRegistration
	{
		enum class Type
		{
			FILE,					// Reading from an audio file
			EMULATION_BUFFERED,		// Using emulation -- playback once, then use buffered data (used for music and sounds that can't dynamically change)
			EMULATION_DIRECT,		// Using emulation -- direct playback, no caching (used e.g. for music that can be dynamically sped up)
			EMULATION_CONTINUOUS	// Using emulation -- direct playback, no caching, repeated starts are processed by sound driver (used for continuous sound effects)
		};

		AudioDefinition* mAudioDefinition = nullptr;
		Package mPackage = Package::ORIGINAL;
		Type mType = Type::FILE;
		std::wstring mSourceFile;
		uint8 mEmulationSfxId = 0;
		uint32 mSourceAddress = 0;
		uint32 mContentOffset = 0;
		bool mIsLooping = false;
		uint32 mLoopStart = 0;
		float mVolume = 1.0f;

		AudioSourceBase* mAudioSource = nullptr;	// Managed by AudioPlayer
	};

	struct AudioDefinition
	{
		enum class Type
		{
			MUSIC   = 0,
			JINGLE  = 1,
			SOUND   = 2
		};

		uint64 mKeyId = 0;
		std::string mKeyString;
		Type mType = Type::SOUND;
		uint8 mChannel = 0xff;

		SourceRegistration* mActiveSource = nullptr;
		std::vector<SourceRegistration> mSources;
	};

public:
	AudioCollection();
	~AudioCollection();

	inline const std::map<uint64, AudioDefinition>& getAudioDefinitions() const  { return mAudioDefinitions; }

	void clear();
	void clearPackage(Package package);
	bool loadFromJson(const std::wstring& basepath, const std::wstring& filename, Package package);

	void determineActiveSourceRegistrations(bool preferOriginalSoundtrack);

	const AudioDefinition* getAudioDefinition(uint64 keyId) const;
	SourceRegistration* getSourceRegistration(uint64 keyId) const;
	SourceRegistration* getSourceRegistration(uint64 keyId, Package preferredPackage) const;

private:
	std::map<uint64, AudioDefinition> mAudioDefinitions;
};
