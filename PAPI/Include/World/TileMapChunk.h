﻿#pragma once

class VertexBuffer;
class VertexArray;
class TileMap;

struct TileData
{
	uint16_t spriteIndex;
};

class TileMapChunk
{
public:
	TileMapChunk();
	TileMapChunk(TileMap *tileMap, glm::ivec2 position, glm::ivec2 size = {32, 32});
	~TileMapChunk();
	TileMapChunk(const TileMapChunk &other)                = delete;
	TileMapChunk(TileMapChunk &&other) noexcept            = delete;
	TileMapChunk& operator=(const TileMapChunk &other)     = delete;
	TileMapChunk& operator=(TileMapChunk &&other) noexcept = delete;

	NODISCARD FORCEINLINE glm::ivec2              GetSize() const { return m_Size; }
	NODISCARD FORCEINLINE glm::ivec2              GetPosition() const { return m_Position; }
	NODISCARD FORCEINLINE TileMap*                GetTileMap() const { return m_TileMap; }
	NODISCARD FORCEINLINE const Ref<VertexArray>& GetVertexArray() const { return m_VertexArray; }

	void SetTile(int x, int y, uint32_t tile);

	void UpdateTileData();

protected:
	// Position is the top-left corner of the chunk in world space. Size is the size of the chunk in tiles.
	glm::ivec2 m_Position, m_Size;
	uint32_t * m_Tiles;
	bool       m_DataDirty = true;

	Ref<VertexArray>  m_VertexArray;
	Ref<VertexBuffer> m_TileDataBuffer;
	glm::vec2 *       m_TileTexCoords = nullptr;

	TileMap *m_TileMap;
};