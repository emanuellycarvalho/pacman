#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>

typedef struct Player {

	float hp;
	int x, y, size;
	int direction;
	int score;
	ALLEGRO_COLOR color;

} Player;

typedef struct Ghost {

	float hp, radius;
	int x, y, size;
	int level, index;
	bool alive;

} Ghost;

typedef struct Pointer {

	int option;
	int x, y, size;
	ALLEGRO_COLOR color;

} Pointer;

typedef struct Attack {
	int x, y;
	int type;
	bool active;

} Attack;


//variaveis globais
const float FPS = 100;
const float PI = 3.141592;
// const float MAX_RADIUS = 50.8;

const int MIN_RADIUS = 5;
const int MAX_RADIUS = 50;
const int SCREEN_W = 800;
const int SCREEN_H = 550;
const int MIN_GHOST = 18;
const int MAX_GHOST = 22;
const int PLAYER_SIZE = 30;
const int STEP_SIZE = 30;
const int END_SIZE = 30;
const int MENU_SIZE = 110;
const int MO_Y = 20; //menu options
const int ATTACK_SIZE = 12;
const int SPECIAL_ATTACK_SIZE = 22;
const int ATTACK_DAMAGE = 20;
const int SPECIAL_ATTACK_DAMAGE = 30;
const int ATTACK_STEP = 7;
const int GHOST_SIZE = 180;

ALLEGRO_EVENT_QUEUE *event_queue = NULL; 
ALLEGRO_DISPLAY *display = NULL; 
ALLEGRO_TIMER *timer = NULL;
ALLEGRO_FONT *font = NULL;
ALLEGRO_FONT *big_font = NULL;



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

	if(!al_init_image_addon()){
		fprintf(stderr, "opa, lombrou na hora de inicializar o ttf!\n");
		return -1;
	}

	// INSTALANDO FONTES DE TEXTO ----------------------------------------------------------------------------------------------
	font = al_load_font("../assets/pixelfont.ttf", 32, 1);   
	if(!font) {
		fprintf(stderr, "opa, lombrou na hora de carregar a pixelfont do menu!\n");
	}

	big_font = al_load_font("../assets/pixelfont.ttf", 80, 1);   
	if(!big_font) {
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

int randomInteger(int min, int max){
	return (int)min + rand()%(max - min +1);
}

float randomFloat(float min, float max){
	return min + (float)rand() / RAND_MAX * (max - min + 1);
}


float dist(int x1, int y1, int x2, int y2){
	return sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2));
}

bool validateSpots(float radius, int x, int y){
	if(x  + radius > (SCREEN_W - 90) && y + radius < (SCREEN_W / 4)){
		return false;
	}

	if(x  + radius < 6 && y + radius < 6){
		return false;
	}

	const c = PLAYER_SIZE * 3.5;
	if(x  + radius < c && y + radius > SCREEN_H - c){
		return false;
	}

	return true;
}

void initExplorationGhost(Ghost *g){
	float radius = randomFloat(MIN_RADIUS, MAX_RADIUS);
	int x = randomInteger(radius, (SCREEN_W - radius));
	int y = randomInteger(radius, (SCREEN_H - radius));

	while(!validateSpots(radius, x, y)){ 
		radius = randomFloat(MIN_RADIUS, MAX_RADIUS);
		x = randomInteger(radius, SCREEN_W - radius);
		y = randomInteger(radius, SCREEN_H - radius);
	}

	g->x = x;
	g->y = y;
	g->radius = radius;
	g->alive = true;
	
}

bool areGhostsColliding(Ghost g, Ghost ghosts[], int index){
	int i = 0;
	for (i = 0; i < index; ++i){
		if(dist(g.x, g.y, ghosts[i].x, ghosts[i].y) < g.radius + ghosts[i].radius){
			return true;
		}
	}

	return false;

}

void drawTestGhosts(Ghost ghosts[], int amt){
	int i;
	for (i = 0; i < amt; ++i){
		if(ghosts[i].alive){
			al_draw_filled_circle(ghosts[i].x, ghosts[i].y, ghosts[i].radius/3.1415962, al_map_rgb(9, 0, 255));
		}
	}
}

// EXPLORATION ---------------------------------------------------------------------------------------------------------------------
void victoryScreen(int score){
	int x =  SCREEN_W / 2 - 200;
	int y = SCREEN_H / 2 - 50; 
	al_clear_to_color(al_map_rgb(0, 0, 0));
	al_draw_text(big_font, al_map_rgb(230, 0, 23), x + 1, y + 1, 0, "YOU DID IT!");  
	al_draw_text(big_font, al_map_rgb(9, 0, 255), x, y, 0, "YOU DID IT!"); //shadow

	char scores[10];
	itoa(score, scores, 10);
	char scoreText[17] = "Your score: ";

	x -= 100;
	y += 100;

	al_draw_text(big_font, al_map_rgb(9, 0, 255), x, y, 0, strcat(scoreText, scores)); //shadow
}

void initExplorationPlayer(Player *p){

	p->hp = 100;
	p->size = PLAYER_SIZE;
	p->x = PLAYER_SIZE;
	p->y = SCREEN_H - (PLAYER_SIZE * 2.5);
	p->color = al_map_rgb(255, 213, 0);
	p->direction = 1;

}

void drawExplorationScenario(int score){
	char scores[10];
	itoa(score, scores, 10);
	char scoreText[17] = "Score: ";

	al_clear_to_color(al_map_rgb(0, 0, 0));
	al_draw_filled_rectangle(SCREEN_W - 90, 10, (SCREEN_W - 10), (SCREEN_H/4), al_map_rgb(145, 80, 0));
	al_draw_text(font, al_map_rgb(10, 255, 153), 5, 5, 0, strcat(scoreText, scores));  


}

void drawExplorationPlayer(Player p){
	ALLEGRO_BITMAP *player;

	switch(p.direction){
		case 1:
			player = al_load_bitmap("../assets/img/pacman-right.png"); 
		break;
		case 2:
			player = al_load_bitmap("../assets/img/pacman-left.png"); 
		break;
		case 3:
			player = al_load_bitmap("../assets/img/pacman-down.png"); 
		break;
		case 4:
			player = al_load_bitmap("../assets/img/pacman-up.png"); 
		break;

	}

	al_draw_bitmap(player, p.x, p.y, 0);

	// int x = p.x + p.size;
	// int y = p.y + p.size/2;
	// int z = p.y - p.size;

	// al_draw_filled_circle(p.x, p.y, p.size, p.color);
	// al_draw_filled_triangle(p.x, p.y, x, y, x, z, al_map_rgb(0, 0, 0));
	// al_draw_filled_triangle(p.x, p.y, p.tx1, p.ty1, p.tx2, p.tx1, al_map_rgb(0,0,0));

}


void explorationKeyDown(Player *p, int key){

	switch(key){

		case ALLEGRO_KEY_UP:
			if(p->y + PLAYER_SIZE - STEP_SIZE < p->size){
				p->y = 0;
				break;
			}

			p->direction = 4;
			p->y = p->y - STEP_SIZE;
		break;

		case ALLEGRO_KEY_DOWN:
			if(p->y + PLAYER_SIZE * 2.5 > SCREEN_H - p->size){
				p->y = SCREEN_H - PLAYER_SIZE * 2.5;
				break;
			}

			p->direction = 3;
			p->y = p->y + STEP_SIZE;
		break;

		case ALLEGRO_KEY_LEFT:
			if(p->x + PLAYER_SIZE - STEP_SIZE < p->size){
				p->x = 0;
				break;
			}

			p->direction = 2;
			p->x = p->x - STEP_SIZE;
		break;

		case ALLEGRO_KEY_RIGHT:
			if(p->x + PLAYER_SIZE * 2.5 > SCREEN_W - PLAYER_SIZE){
				p->x = SCREEN_W - PLAYER_SIZE * 2.5;
				break;
			}
			
			p->direction = 1;
			p->x = p->x + STEP_SIZE;
		break;
	}
}

bool isHome(Player *p){
	if(p->x + (p->size * 2)>= SCREEN_W - 90 && p->y <= 120){
		return true;
	}

	return false;
}

int foundGhost(Player p, Ghost ghosts[], int amt){
	int i;
	int x = p.x + (p.size/2);
	int y = p.y + (p.size/2);
	for (i = 0; i < amt; ++i){
		if(dist(p.x, p.y, ghosts[i].x, ghosts[i].y) < p.size + ghosts[i].radius && ghosts[i].alive){
			return i;
		}
	}

	return -1;
}


// BATTLE ---------------------------------------------------------------------------------------------------------------------
void gameOverScreen(){
	int x =  SCREEN_W / 2 - 200;
	int y = SCREEN_H / 2 - 50; 
	al_clear_to_color(al_map_rgb(0, 0, 0));
	al_draw_text(big_font, al_map_rgb(230, 0, 23), x + 1, y + 1, 0, "GAME OVER");  
	al_draw_text(big_font, al_map_rgb(9, 0, 255), x, y, 0, "GAME OVER"); //shadow

}

void scoreRecordScreen(int score){
	int x =  SCREEN_W / 2 - 200;
	int y = SCREEN_H / 2 - 50; 
	al_clear_to_color(al_map_rgb(0, 0, 0));
	al_draw_text(big_font, al_map_rgb(230, 0, 23), x + 1, y + 1, 0, "NEW RECORD");  
	al_draw_text(big_font, al_map_rgb(9, 0, 255), x, y, 0, "NEW RECORD"); //shadow

	char scores[10];
	itoa(score, scores, 10);
	x += 50;
	y += 100;

	al_draw_text(big_font, al_map_rgb(9, 0, 255), x, y, 0, scores); //shadow
}

void drawPlayerDamageBar(Player p){
	int x1 = p.x - p.size;
	int y1 = p.y + (p.y / 3);	

	int complete = p.x + p.size; 
	int x2 = (p.hp/100) * complete; 
	int y2 = y1 + 5;

	al_draw_filled_rectangle(x1, y1, x2, y2, al_map_rgb(4, 230, 0));
}

void drawBattlePlayer(Player p){

	int x = p.x + p.size;
	int y = p.y + p.size/2;
	int z = p.y - p.size;

	al_draw_filled_circle(p.x, p.y, p.size, p.color);
	al_draw_filled_triangle(p.x, p.y, x, y, x, z, al_map_rgb(0, 0, 0));

	drawPlayerDamageBar(p);

}

void initBattlePlayer(Player *p){

	p->hp = 100;
	p->size = PLAYER_SIZE * 3;
	p->x = p->size + p->size/3;
	p->y = (SCREEN_H - MENU_SIZE)/2 + p->size;
	p->color = al_map_rgb(255, 213, 0);

}

void initBattleGhost(Player *p, Ghost *g){
	g->x = p->x + p->size + 370;
	g->y = p->y - (p->size);
	g->level = randomInteger(1, 4);
	g->hp = 100 - (2 * g->level / 100); // pra ficar mais fácil de combater tbm
}

void initPointer(Pointer *pointer){
	int constant = (SCREEN_W / 2) - 340;
	pointer->size = 5;
	pointer->option = 1;
	pointer->x = constant + (constant / 2) + (pointer->size * 3);
	pointer->y = MO_Y + 32;
	pointer->color = al_map_rgb(4, 230, 0);

}


void drawBattleScenario(Player p, Pointer pointer){
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

	// MENU POINTER ----------------------------------------------------------------------------------------------
	al_draw_filled_circle(pointer.x, pointer.y, pointer.size, pointer.color);

}

void drawGhostDamageBar(Ghost g, Player p){
	int x1 = 90 + g.x - p.size;
	int y1 = p.y + (p.y / 3);	

	int complete = 90 + p.size;
	int x2 = g.x + (g.hp/100) * complete;
	int y2 = y1 + 5;

	al_draw_filled_rectangle(x1, y1, x2, y2, al_map_rgb(4, 230, 0));
}

void drawGhost(Player p, Ghost g){
	char ghostLevelText[15] =  "Ghost level ";
	char ghostLevelNumber[1];
	itoa(g.level, ghostLevelNumber, 10);

	al_draw_text(font, al_map_rgb(9, 0, 255), SCREEN_W - 100, SCREEN_H - 50, 0, strcat(ghostLevelText, ghostLevelNumber));

	ALLEGRO_BITMAP *ghost = al_load_bitmap("../assets/img/2.bmp"); //entre 2 e 7
	al_draw_bitmap(ghost, g.x, g.y, 0);

	drawGhostDamageBar(g, p);

}

int battleKeyDown(Pointer *pointer, int key){
	int constant = (SCREEN_W / 2) - 140;

	int constant2 = constant - 200;
	int option1 = constant2 + (constant2 / 2) + (pointer->size * 3);
	int option2 = (SCREEN_W / 2) - (pointer->size * 3);
	int option3 = option2 + 260;

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
				return 1;
			}

			if(pointer->option == 2){
				return 2;
			}

			if(pointer->option == 3){
				return 0;
			}
		break;
	}

	return 10;

}

void initPlayerAttack(Attack *a, Player p){
	a->x = p.x + p.size;
	a->y = p.y;

	a->active = false;
}

void drawPlayerAttack(Attack a){
	if(a.type == 1){
		al_draw_filled_circle(a.x, a.y, ATTACK_SIZE, al_map_rgb(10, 255, 153));
	}

	if(a.type == 2){
		al_draw_filled_circle(a.x, a.y, SPECIAL_ATTACK_SIZE, al_map_rgb(88, 10, 255));
	}
}

void drawGhostAttack(Attack a, Ghost g){
	int size;
	ALLEGRO_COLOR color;

	if(a.type == 1){
		size = ATTACK_SIZE + ATTACK_SIZE * (g.level / 10);
		color = al_map_rgb(0, 153, 255);
	}

	if(a.type == 2){
		size = SPECIAL_ATTACK_SIZE + ATTACK_SIZE * (g.level / 10);
		color = al_map_rgb(230, 0, 203);
	}

	al_draw_filled_circle(a.x, a.y, size, color);
}

void calculateGhostDamage(Ghost *g, Attack a){
	if(a.type == 1){
		g->hp -= ATTACK_DAMAGE;
	}

	if(a.type == 2){
		g->hp -= SPECIAL_ATTACK_DAMAGE;
	}
}

void initGhostAttack(Attack *a, Ghost g){
	a->x = g.x;
	a->y = g.y + GHOST_SIZE / 2;

	a->type = randomInteger(0, 2);
	a->active = false;

}

void calculatePlayerDamage(Player *p, Attack a, Ghost g){
	float damageVariable = 1;

	if(g.level == 2){
		damageVariable = 1,2;
	}

	if(g.level == 3){
		damageVariable = 1,35;
	}

	if(g.level == 4){
		damageVariable = 1,55;
	}

	if(a.type == 1){
		p->hp -= ATTACK_DAMAGE * damageVariable;
	}

	if(a.type == 2){
		p->hp -= SPECIAL_ATTACK_DAMAGE * damageVariable;
	}
}

int main(int argc, char const *argv[]){

	init();
	Ghost eg, bg;
	Player ep, bp; 
	Pointer pointer; 
	Attack ghostAttack;
	Attack playerAttack;

	Ghost ghosts[MAX_GHOST];
	int amt = randomInteger(MIN_GHOST, MAX_GHOST);

	int i;
	srand(time(NULL));
	for (i = 0; i < amt; ++i){
		initExplorationGhost(&ghosts[i]);
		if(areGhostsColliding(ghosts[i], ghosts, i)){
			i--;
		} else{
			ghosts[i].index = i;
		}
	}

	bool playing = true;
	bool exploration = true; //true para exploration, false para battle

	int kc;
	int playerScore = 0;
	int scoreRecord = 0;

	al_start_timer(timer);
	initExplorationPlayer(&ep);
	initBattlePlayer(&bp);
	initBattleGhost(&bp, &bg);
	initPointer(&pointer);
	initPlayerAttack(&playerAttack, bp);
	initGhostAttack(&ghostAttack, bg);

	// LOOPING DO JOGO --------------------------------------------------------------------------------------------------------

	while(playing){

		ALLEGRO_EVENT ev;
		al_wait_for_event(event_queue, &ev);

		if(ev.type == ALLEGRO_EVENT_TIMER) {
			
			if(exploration){
				drawExplorationScenario(playerScore);
				drawTestGhosts(ghosts, amt);
				drawExplorationPlayer(ep);

				if(isHome(&ep)){
					victoryScreen(playerScore);
					al_flip_display();
					al_rest(3.5);
					playing = false;
				}


				int time = (int)(al_get_timer_count(timer)/FPS);
				if(time > 8){ //nao acha fantasma com menos de 8s de jogo
					int index = foundGhost(ep, ghosts, amt);
					if(index != -1){
						exploration = false;
						eg = ghosts[index];
					}
				}

			} else {

				drawBattleScenario(bp, pointer);
				drawGhost(bp, bg);
				drawBattlePlayer(bp);

				if(playerAttack.active){
					ghostAttack.active = false;
					drawPlayerAttack(playerAttack);

					if(playerAttack.x < bg.x){
						playerAttack.x += ATTACK_STEP;
					} else {
						calculateGhostDamage(&bg, playerAttack);
						initPlayerAttack(&playerAttack, bp);
						if(bg.hp <= 0){
							playerScore += bg.level * 100;
							ghosts[eg.index].alive = false;
							// if(playerScore > scoreRecord){
							// 	scoreRecordScreen(playerScore);
							// 	al_flip_display();
							// 	al_rest(3.5);
							// }
							initBattleGhost(&bp, &bg);
							initBattlePlayer(&bp);
							exploration = true;
						} else {
							ghostAttack.active = true;
						}
					}
				}

				if(ghostAttack.active){
					playerAttack.active = false;
					drawGhostAttack(ghostAttack, bg);

					if(ghostAttack.x > bp.x){
						ghostAttack.x -= ATTACK_STEP;
					} else {
						calculatePlayerDamage(&bp, ghostAttack, bg);
						initGhostAttack(&ghostAttack, bg);
						if(bp.hp <= 0){
							initBattleGhost(&bp, &bg);
							initBattlePlayer(&bp);
							gameOverScreen();
							al_flip_display();
							al_rest(3.5);
							playing = false;
						}
					}

				}
			}

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

				explorationKeyDown(&ep, ev.keyboard.keycode);
			} else {

				if(!playerAttack.active && !ghostAttack.active){
					kc = battleKeyDown(&pointer, ev.keyboard.keycode);
					
					if(kc == 0){ //fugir
						exploration = true;
					}

					if(kc == 1 || kc == 2){
						playerAttack.active = true;
						playerAttack.type = kc;
					}	
				}

			}
		}
	}

	al_destroy_timer(timer);
	al_destroy_display(display);
	al_destroy_event_queue(event_queue);

	return 0;
}