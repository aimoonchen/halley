#pragma once
#include "halley/plugin/iasset_importer.h"

namespace Halley
{
	enum class ShaderType;

	class ShaderImporter : public IAssetImporter
	{
	public:
		ImportAssetType getType() const override { return ImportAssetType::Shader; }
		void import(const ImportingAsset& asset, IAssetCollector& collector) override;

	private:
		Bytes fromHLSL(const String& name, ShaderType type, const Bytes& data, const String& dstLanguage) const;
		Bytes compileHLSL(const String& name, ShaderType type, const Bytes& data) const;
		void importOld(const ImportingAsset& asset, IAssetCollector& collector);
	};
}
