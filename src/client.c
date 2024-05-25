#include <stdio.h>

#include "raylib.h"
#include "game_server.h"

static float current_time_s = 0.0;
static float next_server_update_s = 0.0;
static float last_server_update_s = 0.0;
static const float server_update_interval = 1.0 / 20.0;

static EventBuffer events_from_server;

void init(void) {
	fprintf(stderr, "t=%.2fs Client: Initializing...\n", current_time_s);

	event_buffer_init(&events_from_server);

	server_init();
	server_connect(&events_from_server);
}

void update(const float dt) {
	current_time_s += dt;

	fprintf(stderr, "t=%.2fs Client: Updating...\n", current_time_s);

	Event e;
	while (event_buffer_read(&events_from_server, &e) == 0) {
		switch (e.kind) {
		
		case EK_HEARTBEAT:
			fprintf(stderr, "Client: Got heartbeat request from server.\n");

			e.kind = EK_HEARTBEAT;
			e.sequence_number++;
			e.source_id = 1;
			server_submit_event(&e);
			break;
		
		default:
			break;
		}
	}

	if (current_time_s > next_server_update_s) {
		server_update(current_time_s - last_server_update_s);

		next_server_update_s = current_time_s + server_update_interval;
		last_server_update_s = current_time_s;
	}
}

void draw() {

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
