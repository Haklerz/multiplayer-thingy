#ifndef CHUNKS_H_
#define CHUNKS_H_

#define CHUNK_SIZE 3 // Side length of a chunk. [Tiles]
#define TILE_SIZE 16 // Side length of a tile. [Pixels]

// Represents a single chunk.
typedef struct Chunk *Chunk;

// Attempts to load a chunk.
void chunks_load(int chunk_x, int chunk_y);




// Initialization:
//   initialize_world(width, height): Initializes the world with given dimensions.
//   generate_chunk(chunk_x, chunk_y): Generates a new chunk at the specified position.

// Chunk Management:
//   get_chunk(chunk_x, chunk_y): Retrieves the chunk at the specified position.
//   update_chunk(chunk_x, chunk_y): Updates the content of the chunk at the specified position.
//   unload_chunk(chunk_x, chunk_y): Unloads the chunk at the specified position from memory.

// Tile Management:
//   get_tile(chunk_x, chunk_y, tile_x, tile_y): Retrieves the tile at the specified position within a chunk.
//   set_tile(chunk_x, chunk_y, tile_x, tile_y, tile_type): Sets the tile at the specified position within a chunk to the specified type.
//   update_tile(chunk_x, chunk_y, tile_x, tile_y, new_tile_type): Updates the type of the tile at the specified position within a chunk.
//   get_tile_neighbors(chunk_x, chunk_y, tile_x, tile_y): Retrieves the neighboring tiles of a given tile within a chunk.

// Rendering:
//   render_chunk(chunk_x, chunk_y): Renders the chunk at the specified position.
//   render_visible_chunks(player_x, player_y, viewport_width, viewport_height): Renders the visible chunks around the player within the viewport.

// Utility:
//   get_chunk_size(): Returns the size of each chunk (e.g., width and height).
//   get_world_size(): Returns the size of the entire world (e.g., total width and height).
//   save_world(): Saves the current state of the world to a file.
//   load_world(file_path): Loads a previously saved world from a file.

#endif // CHUNKS_H_
