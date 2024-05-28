#include <stdio.h>
#include <stdbool.h>

#include "raylib.h"
#include "world.h"
#include "events.h"

static float current_time = 0.0;
static float next_world_update = 0.0;
static float last_world_update = 0.0;
static const float server_update_interval = 1.0 / 20.0;

static Entity player;

typedef struct {
	Entity entity;
	float target_x, target_y;
	bool is_wandering;
	float change_state_time;
} Cow;

#define COW_COUNT 3
static Cow cows[COW_COUNT];

void init(void) {
	fprintf(stderr, "Client t=%.2fs: Initializing...\n", current_time);

	player.world_x = 0.0;
	player.world_y = 0.0;

	event_buffer_init(&player.world_events);

	world_init();
	world_add_entity(&player);

	for (int i = 0; i < COW_COUNT; ++i) {
		world_add_entity(&cows[i]);
	}
}

void move_entity(Entity *entity, float vel_x, float vel_y) {
	if (vel_x == 0.0 && vel_y == 0.0)
		return;
		
	Event move_event = {
		.entity_slot = entity->slot,
		.sequence_number = 0,

		.kind = EK_ENTITY_MOVED,
		.entity_moved = {
			.delta_x = vel_x,
			.delta_y = vel_y
		}
	};
	world_submit_event(&move_event);
}

void update_cow(Cow *cow) {

	// Change state every 2 to 5 seconds.
	if (current_time > cow->change_state_time) {
		if (GetRandomValue(0, 1)) {
			cow->is_wandering = !cow->is_wandering;
		}

		cow->target_x = GetRandomValue(-100, 100) / 100.0;
		cow->target_y = GetRandomValue(-100, 100) / 100.0;

		cow->change_state_time = current_time + GetRandomValue(2, 6);
	}

	if (cow->is_wandering)
		move_entity(cow, cow->target_x - cow->entity.world_x, cow->target_y - cow->entity.world_y);
}

void update(const float delta_time) {
	current_time += delta_time;

	fprintf(stderr, "Client t=%.2fs: Updating...\n", current_time);

	// Process events from world to client.
	Event event;
	while (event_buffer_read(&player.world_events, &event) == 0) {
		switch (event.kind) {
		
		case EK_HEARTBEAT:
			fprintf(stderr, "Player: Got heartbeat request from server.\n");

			event.kind = EK_HEARTBEAT;
			event.sequence_number++;
			event.entity_slot = player.slot;
			world_submit_event(&event);
			break;
		
		case EK_ENTITY_MOVED:
			// Move player.
			if (event.entity_slot == player.slot) {
				player.world_x += event.entity_moved.delta_x * delta_time;
				player.world_y += event.entity_moved.delta_y * delta_time;

				fprintf(stderr, "Player: I'm moving to (%.2f, %.2f).\n", player.world_x, player.world_y);
			}

			// Move cows.
			for (int i = 0; i < COW_COUNT; ++i) {
				fprintf(stderr, "Cow %d has entity slot %d.\n", i, cows[i].entity.slot);
				if (event.entity_slot != cows[i].entity.slot)
					continue;
				
				cows[i].entity.world_x += event.entity_moved.delta_x * delta_time;
				cows[i].entity.world_y += event.entity_moved.delta_y * delta_time;

				fprintf(stderr, "Cow %d: I'm moving to (%.2f, %.2f).\n", i, cows[i].entity.world_x, cows[i].entity.world_y);
			}
			break;
		
		default:
			break;
		}
	}

	// Update player.
	float move_x = (IsKeyDown(KEY_RIGHT) ? 1.0 : 0.0) - (IsKeyDown(KEY_LEFT) ? 1.0 : 0.0);
	float move_y = (IsKeyDown(KEY_DOWN) ? 1.0 : 0.0) - (IsKeyDown(KEY_UP) ? 1.0 : 0.0);

	move_entity(&player, 5.0 * move_x, 5.0 * move_y);

	// Update cows.
	for (int i = 0; i < COW_COUNT; ++i) {
		update_cow(&cows[i]);
	}

	// Update world.
	if (current_time > next_world_update) {
		world_update(current_time - last_world_update);

		next_world_update = current_time + server_update_interval;
		last_world_update = current_time;
	}
}

void draw(const int screen_width, const int screen_height) {
	ClearBackground(BLACK);
	// Draw cows :D
	for (int i = 0; i < COW_COUNT; ++i) {
		DrawCircle(0.5 * screen_width + cows[i].entity.world_x * 16.0, 0.5 * screen_height + cows[i].entity.world_y * 16.0, 15, BROWN);
	}

	// Draw player
	DrawCircle(0.5 * screen_width + player.world_x * 16.0, 0.5 * screen_height + player.world_y * 16.0, 10, RAYWHITE);
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
		draw(screen_width, screen_height);
		EndDrawing();
	}

	CloseWindow(); // Close window and OpenGL context.

	return 0;
}
