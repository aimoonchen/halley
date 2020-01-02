#pragma once

#include "halley/text/halleystring.h"
#include <memory>
#include <map>

namespace Halley
{
	class Deserializer;
	class Serializer;
	class ResourceDataStatic;

	class Metadata
	{
	public:
		Metadata();
		Metadata(const Metadata& other) = default;
		~Metadata();

		bool hasKey(String key) const;

		bool getBool(String key) const;
		int getInt(String key) const;
		float getFloat(String key) const;
		String getString(String key) const;

		bool getBool(String key, bool defaultValue) const;
		int getInt(String key, int defaultValue) const;
		float getFloat(String key, float defaultValue) const;
		String getString(String key, String defaultValue) const;
		const std::map<String, String>& getEntries() const;

		bool set(String key, bool value);
		bool set(String key, int value);
		bool set(String key, float value);
		bool set(String key, const char* value);
		bool set(String key, const std::string& value);
		bool set(String key, String value);
		bool erase(const String& key);

		static std::unique_ptr<Metadata> fromBinary(ResourceDataStatic& data);
		
		void serialize(Serializer& s) const;
		void deserialize(Deserializer& s);

		bool operator==(const Metadata& rhs) const;
		bool operator!=(const Metadata& rhs) const;

		String toString() const;
		String toYAMLString() const;

	private:
		std::map<String, String> entries;
	};
}
