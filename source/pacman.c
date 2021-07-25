#include <stdio.h>
#include <stdbool.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>

typedef struct Player {

	ALLEGRO_COLOR color;
	int x, y, size;
	float hp;

} Player;

//variaveis globais
const float FPS = 100;

const int SCREEN_W = 800;
const int SCREEN_H = 550;
const int END_SIZE = 30;
const int PLAYER_SIZE = 25;
	  int STEP_SIZE = PLAYER_SIZE - 5;

ALLEGRO_DISPLAY *display = NULL; 
ALLEGRO_EVENT_QUEUE *event_queue = NULL; 
ALLEGRO_TIMER *timer = NULL;



int init(){

	// QUE HAJA LUZ ------------------------------------------------------------------------------------------------------------
	if(!al_init()) {
		fprintf(stderr, "failed to initialize allegro!\n");
		return -1;
	}

	display = al_create_display(SCREEN_W, SCREEN_H);
	if(!display) {
		fprintf(stderr, "opa, lombrou na hora de inicializar o display!\n");
		return -1;
	}

	event_queue = al_create_event_queue();
	if(!event_queue) {
		fprintf(stderr, "opa, lombrou na hora de inicializar a event_queue!\n");
		al_destroy_display(display);
		return -1;
	}

	timer = al_create_timer(1.0 / FPS);
    if(!timer) {
		fprintf(stderr, "opa, lombrou na hora de inicializar o timer!\n");
		return -1;
	}

	if(!al_init_primitives_addon()){
		fprintf(stderr, "opa, lombrou na hora de inicializar as primitivas!\n");
        return -1;
    }	

	// INSTALANDO FONTES DE EVENTOS ----------------------------------------------------------------------------------------------
	if(!al_install_keyboard()) {
		fprintf(stderr, "failed to install keyboard!\n");
		return -1;
	}

	if(!al_install_mouse()) {
		fprintf(stderr, "failed to initialize mouse!\n");
		return -1;
	}

	al_register_event_source(event_queue, al_get_display_event_source(display));
	al_register_event_source(event_queue, al_get_timer_event_source(timer));
	al_register_event_source(event_queue, al_get_keyboard_event_source());
	al_register_event_source(event_queue, al_get_mouse_event_source());

}

void init_cenario(){

	al_clear_to_color(al_map_rgb(0, 0, 0));
	al_draw_filled_ellipse(SCREEN_W, 0, -30, 30, al_map_rgb(0, 255, 0))

}

void init_player(Player *p){

	p->hp = 100;
	p->size = PLAYER_SIZE;
	p->x = PLAYER_SIZE + 10;
	p->y = SCREEN_H - (PLAYER_SIZE + 10);
	p->color = al_map_rgb(255, 213, 0);

}

void drawExplorationPlayer(Player p){

	int x = p.x + p.size;
	int y = p.y + p.size/2;
	int z = p.y - p.size;

	al_draw_filled_circle(p.x, p.y, p.size, p.color);
	al_draw_filled_triangle(p.x, p.y, x, y, x, z, al_map_rgb(0, 0, 0));

}

void explorationKeyDown(Player *p, int key){

	switch(key){

		case ALLEGRO_KEY_UP:
			if(p->y - STEP_SIZE < p->size){
				p->y = p->size;
				break;
			}

			p->y = p->y - STEP_SIZE;
		break;

		case ALLEGRO_KEY_DOWN:
			if(p->y + STEP_SIZE > SCREEN_H - p->size){
				p->y = SCREEN_H - p->size;
				break;
			}

			p->y = p->y + STEP_SIZE;
		break;

		case ALLEGRO_KEY_LEFT:
			if(p->x - STEP_SIZE < p->size){
				p->x = p->size;
				break;
			}

			p->x = p->x - STEP_SIZE;
		break;

		case ALLEGRO_KEY_RIGHT:
			if(p->x + STEP_SIZE > SCREEN_W - p->size){
				p->x = SCREEN_W - p->size;
				break;
			}
			
			p->x = p->x + STEP_SIZE;
		break;
	}
}

int main(int argc, char const *argv[]){

	init();
	Player p; 
	bool playing = true;
	bool exploration = true; //true para exploration, false para fight

	// LOOPING DO JOGO --------------------------------------------------------------------------------------------------------
	al_start_timer(timer);
	init_player(&p);
	
	while(playing){

		ALLEGRO_EVENT ev;
		al_wait_for_event(event_queue, &ev);

		if(ev.type == ALLEGRO_EVENT_TIMER) {
			init_cenario();
			
			if(exploration){
				drawExplorationPlayer(p);
			} else {

			}

			// printf("att\n");
			al_flip_display();
		}
		
		if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
			playing = false;
		}

		if(ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
			printf("\nmouse clicado em: %d, %d", ev.mouse.x, ev.mouse.y);
		}

		if(ev.type == ALLEGRO_EVENT_KEY_DOWN) {

			if(exploration){

				explorationKeyDown(&p, ev.keyboard.keycode);
				printf("%i", p.x);
				printf(", ");
				printf("%i", p.y);
				printf("\n");
			}


			//imprime qual tecla foi
			// printf("\ncodigo tecla: %d", ev.keyboard.keycode);
		}
	}

	al_destroy_timer(timer);
	al_destroy_display(display);
	al_destroy_event_queue(event_queue);

	return 0;
}