#include <stdio.h>

#include "raylib.h"
#include "world.h"
#include "entity.h"

static float current_time = 0.0;
static float next_world_update = 0.0;
static float last_world_update = 0.0;
static const float server_update_interval = 1.0 / 20.0;

static Entity client_entity;

void init(void) {
	fprintf(stderr, "Client t=%.2fs: Initializing...\n", current_time);

	client_entity.world_x = 0.0;
	client_entity.world_y = 0.0;

	event_buffer_init(&client_entity.world_events);

	world_init();
	world_add_entity(&client_entity);
}

void update(const float delta_time) {
	current_time += delta_time;

	fprintf(stderr, "Client t=%.2fs: Updating...\n", current_time);

	// Process events from world.
	Event event;
	while (event_buffer_read(&client_entity.world_events, &event) == 0) {
		switch (event.kind) {
		
		case EK_HEARTBEAT:
			fprintf(stderr, "Client: Got heartbeat request from server.\n");

			event.kind = EK_HEARTBEAT;
			event.sequence_number++;
			event.entity_id = client_entity.id;
			world_submit_event(&event);
			break;
		
		case EK_ENTITY_MOVED:
			client_entity.world_x += event.entity_moved.delta_x * delta_time;
			client_entity.world_y += event.entity_moved.delta_y * delta_time;

			fprintf(stderr, "Client: I'm moving to (%.2f, %.2f).\n", client_entity.world_x, client_entity.world_y);
			break;
		
		default:
			break;
		}
	}

	// Request move.
	float move_x = (IsKeyDown(KEY_RIGHT) ? 1.0 : 0.0) - (IsKeyDown(KEY_LEFT) ? 1.0 : 0.0);
	float move_y = (IsKeyDown(KEY_DOWN) ? 1.0 : 0.0) - (IsKeyDown(KEY_UP) ? 1.0 : 0.0);

	if (move_x != 0 || move_y != 0) {
		Event move_event = {
			.entity_id = client_entity.id,
			.sequence_number = 0,
			.kind = EK_ENTITY_MOVED,
			.entity_moved = {
				.delta_x = move_x * 50.0,
				.delta_y = move_y * 50.0
			}
		};
		world_submit_event(&move_event);
	}

	// Update world.
	if (current_time > next_world_update) {
		world_update(current_time - last_world_update);

		next_world_update = current_time + server_update_interval;
		last_world_update = current_time;
	}
}

void draw() {
	ClearBackground(BLACK);
	DrawCircle(client_entity.world_x, client_entity.world_y, 10, RAYWHITE);
}

int main(void) {
	// Initialization.
	const int screen_width = 640;
	const int screen_height = 480;

	const int target_framerate = 60;
	const float delta_time = 1.0f / target_framerate;

	SetConfigFlags(FLAG_MSAA_4X_HINT);
	InitWindow(screen_width, screen_height, "Digging Game Client");

	SetTargetFPS(target_framerate);

	init();

	// Main game loop.
	while (!WindowShouldClose()) {

		update(delta_time);

		BeginDrawing();
		draw();
		EndDrawing();
	}

	CloseWindow(); // Close window and OpenGL context.

	return 0;
}
