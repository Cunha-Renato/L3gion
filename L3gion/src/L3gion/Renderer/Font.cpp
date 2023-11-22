#include "lgpch.h"
#include "Font.h"

#undef INFINITE
#include <msdf-atlas-gen.h>

namespace L3gion
{
    struct MSDFData
    {
        std::vector<msdf_atlas::GlyphGeometry> glyphs;
        msdf_atlas::FontGeometry fontGeometry;
    };

    template<typename T, typename S, int N, msdf_atlas::GeneratorFunction<S, N> genFunc>
    static ref<SubTexture2D> createAndCacheAtlas(const std::string& fontName, float fontSize, const 
                                                 std::vector<msdf_atlas::GlyphGeometry>& glyphs, const msdf_atlas::FontGeometry& fontGeometry, 
                                                 uint32_t width, uint32_t height)
    {
        msdf_atlas::GeneratorAttributes attributes;
        attributes.config.overlapSupport = true;
        attributes.scanlinePass = true;

        msdf_atlas::ImmediateAtlasGenerator<S, N, genFunc, msdf_atlas::BitmapAtlasStorage<T, N>> generator(width, height);
        generator.setAttributes(attributes);
        generator.setThreadCount(8);
        generator.generate(glyphs.data(), (int)glyphs.size());

        msdfgen::BitmapConstRef<T, N> bitmap = (msdfgen::BitmapConstRef<T, N>)generator.atlasStorage();

        TextureSpecs specs;
        specs.width = bitmap.width;
        specs.height = bitmap.height;
        specs.format = ImageFormat::RGB8;
        specs.generateMips = false;

        ref<SubTexture2D> texture = SubTexture2D::create(specs);
        texture->getTexture()->setData((void*)bitmap.pixels, bitmap.width * bitmap.height * 4);
        
        return texture;
    }
    
	Font::Font(const std::filesystem::path& filepath)
        : m_Data(new MSDFData())
	{
        msdfgen::FreetypeHandle* ft = msdfgen::initializeFreetype();
        LG_CORE_ASSERT(ft, "");
     
        std::string fileString = filepath.string();
        msdfgen::FontHandle* font = loadFont(ft, fileString.c_str());
        if (!font)
        {
            LG_CORE_CRITICAL("Failed to load font at {}", fileString);
            return;
        }
        
        struct CharsetRange
        {
            uint32_t begin, end;
        };
 
        static const CharsetRange charsetRanges[] =
        {
            {0x0020, 0x00FF}
        };

        msdf_atlas::Charset charset;
        for (CharsetRange range : charsetRanges)
        {
            for (uint32_t c = range.begin; c <= range.end; c++)
                charset.add(c);
        }

        double fontScale = 1.0f;
        m_Data->fontGeometry = msdf_atlas::FontGeometry(&m_Data->glyphs);
        int glyphsLoaded = m_Data->fontGeometry.loadCharset(font, fontScale, charset);
        LG_CORE_INFO("Loaded {} glyphs out of {}", glyphsLoaded, charset.size());

        double emSize = 40.0f;
        msdf_atlas::TightAtlasPacker atlasPacker;
        //atlasPacker.setDimensionsConstraint();
        atlasPacker.setPixelRange(2.0f);
        atlasPacker.setMiterLimit(1.0f);
        atlasPacker.setPadding(0);
        atlasPacker.setScale(emSize);

        int remaining = atlasPacker.pack(m_Data->glyphs.data(), (int)m_Data->glyphs.size());
        LG_CORE_ASSERT(remaining == 0, "");

        int width, height;
        atlasPacker.getDimensions(width, height);
        emSize = atlasPacker.getScale();
        
        m_AtlasTexture = createAndCacheAtlas<uint8_t, float, 3, msdf_atlas::msdfGenerator>("Test", (float)emSize, m_Data->glyphs, m_Data->fontGeometry, width, height);


#if 0
        msdfgen::Shape shape;
        if (loadGlyph(shape, font, 'A')) 
        {
            shape.normalize();
            //                      max. angle
            edgeColoringSimple(shape, 3.0);
            //           image width, height
            msdfgen::Bitmap<float, 3> msdf(32, 32);
            //                     range, scale, translation
            msdfgen::generateMSDF(msdf, shape, 4.0, 1.0, msdfgen::Vector2(4.0, 4.0));
            msdfgen::savePng(msdf, "output.png");
        }
#endif    
        msdfgen::destroyFont(font);   
        msdfgen::deinitializeFreetype(ft);
	}

    Font::~Font()
    {
        delete m_Data;
    }
}