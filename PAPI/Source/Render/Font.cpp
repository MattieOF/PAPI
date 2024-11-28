﻿#include "papipch.h"
#include "Render/Font.h"

#include <msdf-atlas-gen.h>
#include <msdfgen.h>

#include "Render/MSDFData.h"

msdfgen::FreetypeHandle *Font::s_FTHandle    = nullptr;
Ref<Font>                Font::s_DefaultFont = nullptr;

struct CharRange
{
	uint32_t Begin, End;
};

struct AtlasConfig
{
	int Width, Height;
};

constexpr uint8_t threadCount = 8;

template <typename T, typename S, int N, msdf_atlas::GeneratorFunction<S, N> GenFunc>
static Ref<Texture> GenerateAtlasTexture(const std::string &                           fontName, float size,
                                         const std::vector<msdf_atlas::GlyphGeometry> &glpyhs,
                                         const msdf_atlas::FontGeometry &              geo, const AtlasConfig &config)
{
	// Lets set up our atlas generator
	msdf_atlas::GeneratorAttributes attributes;
	attributes.scanlinePass          = true;
	attributes.config.overlapSupport = true;

	msdf_atlas::ImmediateAtlasGenerator<S, N, GenFunc, msdf_atlas::BitmapAtlasStorage<T, N>> generator(
		config.Width, config.Height);
	generator.setAttributes(attributes);
	generator.setThreadCount(threadCount);
	// MW @todo: This should be configurable, or based on the number of cores. std::thread::hardware_concurrency()?

	// Generate the atlas!
	generator.generate(glpyhs.data(), static_cast<int>(glpyhs.size()));

	// Lets get our bitmap.
	msdfgen::BitmapConstRef<T, N> bitmap = static_cast<msdfgen::BitmapConstRef<T, N>>(generator.atlasStorage());

	// And set up a GPU texture for it:
	TextureSpecification spec;
	spec.Width           = static_cast<int32_t>(config.Width);
	spec.Height          = static_cast<int32_t>(config.Height);
	spec.Format          = TextureFormat::RGB8;
	Ref<Texture> texture = CreateRef<Texture>(spec);
	texture->SetData(bitmap.pixels);

	return texture;
}

Font::Font(const std::filesystem::path &fontPath)
{
	PAPI_ASSERT(s_FTHandle && "Font system not initialized!");
	Stopwatch sw;

	// For now, we're just loading ttf files from disk.
	// In a larger project, we wouldn't have loose files or hardcoded paths, so we'd be loading it from a memory buffer
	// provided by an asset manager.
	// Even further, we'd probably cache the MSDF texture into an asset for a distribution build, at which point
	// loading the font would be a simple matter of loading the texture and the font metrics.

	std::string pathString = fontPath.string(); // Convert from path->string, as we need a c_str, but not wide!

	msdfgen::FontHandle *font = loadFont(s_FTHandle, pathString.c_str());

	if (!font)
	{
		PAPI_ERROR("Failed to load font from path: {0}", pathString);
		return;
	}

	m_Data = CreateScope<MSDFData>();
	m_Data->FontGeo = msdf_atlas::FontGeometry(&m_Data->Glyphs);

	// Now lets add our charset.
	// MW @todo: This should be configurable per font.
	static const CharRange charRanges[] = {{0x0020, 0x00FF}};
	msdf_atlas::Charset    charset;
	for (const auto [Begin, End] : charRanges)
		for (uint32_t i = Begin; i <= End; i++)
			charset.add(i);

	// Got the charset, lets load it!
	m_GlyphCount = m_Data->FontGeo.loadCharset(font, 1.0, charset);

	// Now lets pack our atlas.
	// First, set up our packer:
	// MW @todo: The following settings should probably be configurable. Also I don't know what they do!
	msdf_atlas::TightAtlasPacker packer;
	packer.setPixelRange(2.0);
	packer.setMiterLimit(1.0);
	packer.setInnerPixelPadding(0);
	packer.setOuterPixelPadding(0);
	packer.setInnerUnitPadding(0);
	packer.setOuterUnitPadding(0);
	double emSize = 40.0;
	packer.setScale(emSize);

	// And do the packing
	int remaining = packer.pack(m_Data->Glyphs.data(), static_cast<int>(m_Data->Glyphs.size()));
	PAPI_ASSERT(remaining == 0 && "Failed to pack atlas!");
	int width, height;
	packer.getDimensions(width, height);
	emSize = packer.getScale();

	// And our edge colouring, for MSDF
	constexpr uint64_t lcgMultiplier      = 6364136223846793005ull;
	constexpr uint64_t lcgIncrement       = 1442695040888963407ull;
	constexpr uint64_t colouringSeed      = 0;
	constexpr bool     expensiveColouring = true;
	
	if (expensiveColouring)
	{
		msdf_atlas::Workload([&glyphs = m_Data->Glyphs](int i, int threadNo) -> bool
		{
			// Copied from msdf-atlas-gen!
			uint64_t glyphSeed = (lcgMultiplier * (colouringSeed ^ i) + lcgIncrement) * !!colouringSeed;
			glyphs[i].edgeColoring(msdfgen::edgeColoringInkTrap, 3.0, glyphSeed);
			return true;
		}, m_Data->Glyphs.size()).finish(threadCount);	
	}
	else
	{
		uint64_t glyphSeed = colouringSeed;
		for (auto &glyph : m_Data->Glyphs)
		{
			glyphSeed *= lcgMultiplier;
			glyph.edgeColoring(msdfgen::edgeColoringInkTrap, 3.0, glyphSeed);
		}
	}

	// Now let's generate the atlas texture!
	m_Texture = GenerateAtlasTexture<uint8_t, float, 3, msdf_atlas::msdfGenerator>(
		pathString, static_cast<float>(emSize), m_Data->Glyphs, m_Data->FontGeo, {width, height});

	// Clean up!
	msdfgen::destroyFont(font);

	sw.End();
	PAPI_TRACE("Loaded font ({0}/{1} glyphs) from file \"{2}\" in {3}ms", m_GlyphCount, charset.size(), pathString,
	           sw.GetElapsedMilliseconds());
}

Font::~Font()
{
}

void Font::InitFontSystem()
{
	s_FTHandle = msdfgen::initializeFreetype();
	if (!s_FTHandle)
	{
		PAPI_ERROR("Failed to initialize FreeType!");
		return;
	}

	s_DefaultFont = CreateRef<Font>("Content/Fonts/OpenSans-Regular.ttf");
}

void Font::ShutdownFontSystem()
{
	s_DefaultFont = nullptr;
	deinitializeFreetype(s_FTHandle);
	s_FTHandle = nullptr;
}
