#include <math.h>
#include <stdio.h>
#include <stdbool.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>

typedef struct Player {

	float hp;
	int x, y, size;
	ALLEGRO_COLOR color;

} Player;

typedef struct Pointer {

	int option;
	int x, y, size;
	ALLEGRO_COLOR color;

} Pointer;

//variaveis globais
const float FPS = 100;

const int SCREEN_W = 800;
const int SCREEN_H = 550;
const int MAX_DIST = 25;
const int PLAYER_SIZE = 25;
const int STEP_SIZE = 30;
const int END_SIZE = 30;
const int MENU_SIZE = 110;
const int MO_Y = 20; //menu options

ALLEGRO_EVENT_QUEUE *event_queue = NULL; 
ALLEGRO_DISPLAY *display = NULL; 
ALLEGRO_TIMER *timer = NULL;
ALLEGRO_FONT *font = NULL;



int init(){

	// QUE HAJA LUZ ------------------------------------------------------------------------------------------------------------
	if(!al_init()) {
		fprintf(stderr, "opa, lombrou na hora de inicializar o allegro!\n");
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

    al_init_font_addon();

    if(!al_init_ttf_addon()) {
		fprintf(stderr, "opa, lombrou na hora de inicializar o ttf!\n");
		return -1;
	}

	// INSTALANDO FONTES DE TEXTO ----------------------------------------------------------------------------------------------
	font = al_load_font("pixelfont.ttf", 32, 1);   
	if(!font) {
		fprintf(stderr, "opa, lombrou na hora de carregar a pixelfont!\n");
	}

	// INSTALANDO FONTES DE EVENTOS ----------------------------------------------------------------------------------------------
	if(!al_install_keyboard()) {
		fprintf(stderr, "opa, lombrou na hora de instalar o keyboard!\n");
		return -1;
	}

	if(!al_install_mouse()) {
		fprintf(stderr, "opa, lombrou na hora de instalar o mouse!\n");
		return -1;
	}

	al_register_event_source(event_queue, al_get_display_event_source(display));
	al_register_event_source(event_queue, al_get_timer_event_source(timer));
	al_register_event_source(event_queue, al_get_keyboard_event_source());
	al_register_event_source(event_queue, al_get_mouse_event_source());

}


// EXPLORATION ---------------------------------------------------------------------------------------------------------------------

void initExplorationPlayer(Player *p){

	p->hp = 100;
	p->size = PLAYER_SIZE;
	p->x = PLAYER_SIZE + 10;
	p->y = SCREEN_H - (PLAYER_SIZE + 10);
	p->color = al_map_rgb(255, 213, 0);

}


void drawExplorationScenario(){

	al_clear_to_color(al_map_rgb(0, 0, 0));
	al_draw_filled_rectangle(SCREEN_W - 90, 10, (SCREEN_W - 10), (SCREEN_H/4), al_map_rgb(145, 80, 0));

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

bool isHome(Player *p){
	if(p->x + (p->size * 2)>= SCREEN_W - 90 && p->y - (p->size * 2)<= 120){
		return true;
	}

	return false;
}


float dist(int x1, int y1, int x2, int y2){
	return sqrt(pow(x1 - x2, 2) + pow(y1 - y2,2));
}

bool foundGhost(Player p){

	return true;

	// if(dist(p.x, p.y, 0, 0) <= MAX_DIST){
	// 	return true;
	// }

	// return false;
}


// BATTLE ---------------------------------------------------------------------------------------------------------------------
void drawBattlePlayer(Player p){

	int x = p.x + p.size;
	int y = p.y + p.size/2;
	int z = p.y - p.size;

	al_draw_filled_circle(p.x, p.y, p.size, p.color);
	al_draw_filled_triangle(p.x, p.y, x, y, x, z, al_map_rgb(0, 0, 0));

	//damage bar
	int x1 = p.x - p.size;
	int y1 = p.y + (p.y / 3);	

	int x2 = p.x + p.size;
	int y2 = y1 + 5;

	al_draw_filled_rectangle(x1, y1, x2, y2, al_map_rgb(4, 230, 0));

}

void initBattlePlayer(Player *p){

	p->size = PLAYER_SIZE * 3;
	p->x = p->size + p->size/3;
	p->y = (SCREEN_H - MENU_SIZE)/2 + p->size;
	p->color = al_map_rgb(255, 213, 0);

}

void initPointer(Pointer *pointer){
	int constant = (SCREEN_W / 2) - 340;
	pointer->size = 5;
	pointer->option = 1;
	pointer->x = constant + (constant / 2) + (pointer->size * 3);
	pointer->y = MO_Y + 32;
	pointer->color = al_map_rgb(4, 230, 0);

	// al_draw_filled_circle(first_pointer_x, (MO_Y + 32), pointer_size, al_map_rgb(4, 230, 0));
	// al_draw_filled_circle(middle_pointer_x, (MO_Y + 32), pointer_size, al_map_rgb(4, 230, 0));
	// al_draw_filled_circle(third_pointer_x, (MO_Y + 32), pointer_size, al_map_rgb(4, 230, 0));
}

void drawBattleScenario(Player p){
	int shadow_y = MO_Y + 1;
	int middle_option_x = (SCREEN_W / 2) - 140;
	int first_option_x = middle_option_x - 200;
	int third_option_x = middle_option_x + 360;


	// DISPLAY AND MENU ----------------------------------------------------------------------------------------------
	al_clear_to_color(al_map_rgb(0, 0, 0));
	al_draw_filled_rectangle(0, 0, SCREEN_W, MENU_SIZE, al_map_rgb(230, 0, 23));
	al_draw_filled_rectangle(0, (MENU_SIZE - 1), SCREEN_W, (MENU_SIZE + 5), al_map_rgb(0, 153, 255));
	al_draw_filled_rectangle(0, (MENU_SIZE + 5), SCREEN_W, (MENU_SIZE + 7), al_map_rgb(9, 0, 255));

	// MENU OPTIONS ----------------------------------------------------------------------------------------------
	al_draw_text(font, al_map_rgb(143, 16, 7), (first_option_x + 1), shadow_y, 0, "ATTACK"); //shadow 
	al_draw_text(font, al_map_rgb(9, 0, 255), first_option_x, MO_Y, 0, "ATTACK");

	al_draw_text(font, al_map_rgb(143, 16, 7), (middle_option_x + 1), shadow_y, 0, "SPECIAL ATTACK"); //shadow 
	al_draw_text(font, al_map_rgb(9, 0, 255), middle_option_x, MO_Y, 0, "SPECIAL ATTACK");

	al_draw_text(font, al_map_rgb(143, 16, 7), (third_option_x + 1), shadow_y, 0, "RUN"); //shadow 
	al_draw_text(font, al_map_rgb(9, 0, 255), third_option_x, MO_Y, 0, "RUN");

}

void drawPointer(Pointer p){
	al_draw_filled_circle(p.x, p.y, p.size, p.color);
}

void battleKeyDown(Pointer *pointer, int key){
	// int constant = (SCREEN_W / 2) - 340;
	// int x = constant + (constant / 2) + (p->size * 3);
	int constant = (SCREEN_W / 2) - 140;

	int constant2 = constant - 200;
	int option1 = constant2 + (constant2 / 2) + (pointer->size * 3);
	int option2 = (SCREEN_W / 2) - (pointer->size * 3);
	int option3 = option2 + 260;

	// int middle_pointer_x = (SCREEN_W / 2) - (pointer_size * 3);
	// int first_pointer_x = first_option_x + (first_option_x / 2) + (pointer_size * 3);
	// int third_pointer_x =  third_option_x + (pointer_size * 5);

	switch(key){
		case ALLEGRO_KEY_LEFT:
			if(pointer->option == 1){
				pointer->x = option3;
				pointer->option = 3;
				break;
			}

			if(pointer->option == 2){
				pointer->x = option1;
				pointer->option = 1;
				break;
			}

			if(pointer->option == 3){
				pointer->x = option2;
				pointer->option = 2;
				break;
			}
		break;

		case ALLEGRO_KEY_RIGHT:
			if(pointer->option == 1){
				pointer->x = option2;
				pointer->option = 2;
				break;
			}

			if(pointer->option == 2){
				pointer->x = option3;
				pointer->option = 3;
				break;
			}

			if(pointer->option == 3){
				pointer->x = option1;
				pointer->option = 1;
				break;
			}
		break;

		case ALLEGRO_KEY_ENTER:
			if(pointer->option == 1){
				//code
				break;
			}

			if(pointer->option == 2){
				//code
				break;
			}

			if(pointer->option == 3){
				//code
				break;
			}
		break;
	}

}



int main(int argc, char const *argv[]){

	init();
	Player p; 
	Pointer pointer; 
	bool playing = true;
	bool exploration = true; //true para exploration, false para fight

	// LOOPING DO JOGO --------------------------------------------------------------------------------------------------------
	al_start_timer(timer);
	initExplorationPlayer(&p);
	initPointer(&pointer);
	
	while(playing){

		ALLEGRO_EVENT ev;
		al_wait_for_event(event_queue, &ev);

		if(ev.type == ALLEGRO_EVENT_TIMER) {
			
			if(exploration){
				drawExplorationScenario();
				drawExplorationPlayer(p);

				if(isHome(&p)){
					playing = 0;
				}

				if(foundGhost(p)){
					exploration = false;
				}

			} else {

				initBattlePlayer(&p);
				drawBattleScenario(p);
				drawPointer(pointer);
				drawBattlePlayer(p);

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
				// printf("%i", p.x);
				// printf(", ");
				// printf("%i", p.y);
				// printf("\n");
			} else {

				battleKeyDown(&pointer, ev.keyboard.keycode);
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