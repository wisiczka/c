
#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <curses.h>
#include <time.h>
#include "defines.h"
#include "verbs.h"



#include "map.h"

#include "inits.h"






#include "render.h"
#include "parse.h"










/////////////MAIN MAIN MAIN MAIN MAIN/////////////


void main(void){




	



	struct Noun nouns[MAX_NOUNS];
	struct Room rooms[MAX_ROOMS];
	struct Conversation conversations[MAX_CONVERSATIONS];



	int gamerun =1; 
	int timee =0;
	





	/// arrays store from which room ids the room can be accessed
	///places u can reach location from, fill empty space with -2
	
	/////     int [MAX_TRAVELABLE_ROOMS] = {-2,-2,-2,-2,-2,-2,-2,-2,-2,-2};

	int basement_connections[MAX_TRAVELABLE_ROOMS] = {4,2,3,1,-2,-2,-2,-2,-2,-2};
	int living_room_connections[MAX_TRAVELABLE_ROOMS] = {0,-2,-2,-2,-2,-2,-2,-2,-2,-2};
	int cave_connections[MAX_TRAVELABLE_ROOMS] = {4,3,0,-2,-2,-2,-2,-2,-2,-2};
	int narrow_pathway_connections[MAX_TRAVELABLE_ROOMS] = {2,0,-2,-2,-2,-2,-2,-2,-2,-2};
	int wide_pathway_connections[MAX_TRAVELABLE_ROOMS] = {2,0,-2,-2,-2,-2,-2,-2,-2,-2};
	int pier_market_connections[MAX_TRAVELABLE_ROOMS] = {6,7,-2,-2,-2,-2,-2,-2,-2,-2};
	int shack_connections[MAX_TRAVELABLE_ROOMS] = {5,-2,-2,-2,-2,-2,-2,-2,-2,-2};
	int registration_office_connections[MAX_TRAVELABLE_ROOMS] = {5,-2,-2,-2,-2,-2,-2,-2,-2,-2};



	int filler = 0;
	for( filler = 0; (filler < MAX_NOUNS) ; filler++ ){
		create_noun(&nouns[filler], "BUFFER NAME", 99, 992, 910, 90, "BUFFER GENDER", "BUFFER S", "BUFFER S", "BUFFER S", 9100,  95,  991,999);


	}

	for( filler = 0; (filler < MAX_ROOMS) ; filler++ ){
		create_room(&rooms[filler], "BUFFER NAME", "BUFFER DESCRIPTION", 999, basement_connections );


	}
	
	for( filler = 0; (filler < MAX_CONVERSATIONS) ; filler++ ){
		create_conversation(&conversations[filler], 999, GOD_ID, "BUFFER QUESTION", "BUFFER ANSWER", 999 ,0 );


	}






	init();
	init_map(nouns);

	




/// create_room(struct Room *room, char*  name,char* description, int position, int travelable_rooms[MAX_TRAVELABLE_ROOMS]);

	create_room(&rooms[0], "basement", "a nice 'basement', decorated with anime posters . there are wooden stairs to the living room. ", 0, basement_connections );
	create_room(&rooms[1], "living room", "the 'living room', there are walls of anime figurine displays . stairs lead down to the basement ", 1, living_room_connections );
	create_room(&rooms[2], "cave", "a dark wet cave with a large pool of water in it. there seem to be somewhat developed pathways leading to other parts of the cave. ", 2, cave_connections );
	create_room(&rooms[3], "narrow pathway", "tight fitting cavern corridors", 3, narrow_pathway_connections );
	create_room(&rooms[4], "wide pathway", "sprawling cavern mouths, seem well traveled", 4, wide_pathway_connections );
	create_room(&rooms[5], "pier market", "the bustling water market", 5, pier_market_connections );
	create_room(&rooms[6], "shack", "a falling apart wooden shack", 6, shack_connections );
	create_room(&rooms[7], "registration office", "you are given written permission to board ships here", 7, registration_office_connections );





////void create_noun(struct Noun *subject, char* name, int def_happy, int id, int helf, int pos, char* gender, char* subject_pronoun, char* object_pronoun,char* description,
///	int base_cost,  int damage,  int defense, int gold);
///single words only

	create_noun(&nouns[0], "anon", 100, 0, 10, 5, "m", "I", "me", "look in the mirror !" , 10000,  1,  1,50);
	create_noun(&nouns[1], "loli", 90, 1, 10, 2, "f", "she", "her", "kawaii oneechan", 1000,  5,  5,10);
	create_noun(&nouns[2], "shota", 90, 2, 10, 2, "m", "he", "him", "a cute boy", 1000,  5,  10,20);
	create_noun(&nouns[3], "katana", 90, 3, 0, 2, "n", "it", "it",  "fine sword", 10,  27,  1,0);
	create_noun(&nouns[4], "kimono", 90, 4, 0, 2, "n", "it", "it", "traditional silk dress", 9,  5,  1,0);
	create_noun(&nouns[5], "terry", 90, 5, 10, 1, "m", "he", "him", "holy c", 1000,  50,  1000,0);
	create_noun(&nouns[6], "seller", 90, 6, 10, 5, "f", "she", "her", "a little girl posing as a merchant. she has an eyepatch and hood covering her hair.", 1000,  17,  13,100);
	create_noun(&nouns[7], "shemagh", 90, 7, 10, 5, "n", "it", "it", "a quality piece of cloth", 3, 0,  2,0);
	take_noun( &nouns[0], &rooms[nouns[0].position], &nouns[7]);
	create_noun(&nouns[8], "dagger", 90, 8, 10, 5, "n", "it", "it", "a small yet dangerous blade", 5, 2,  2,0);
	take_noun( &nouns[6], &rooms[nouns[0].position], &nouns[8]);
	create_noun(&nouns[9], "sabre", 90, 9, 10, 5, "n", "it", "it", "a sharp and hooked western blade", 12, 5,  2,0);
	take_noun( &nouns[6], &rooms[nouns[0].position], &nouns[9]);

	
	player = &nouns[0];
	player->type = the_player;

	player->y = 2;
  player->x = 2;







	create_conversation(&conversations[0], 0,6, "trading", "looking to trade are you? better not steal anything... ", 1,1  );
	create_conversation(&conversations[1], 1,6, "weather", "pretty stormy today, wouldn't want to be the one setting out this afternoon ", 0 ,0 );
	create_conversation(&conversations[2],2,6, "leave", "bye bye !", 0,1);



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


  bsp_split(0, 0, MAP_H, MAP_W, true, nouns);
  make_entrance();


  recenter();
  calc_fov(nouns);








 //static const char *nouns[] = { " candy", " meowni", " shirt", " skirt", "", "", "", "", "", ""};


	////////////////// GAME INTROOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO



///	printf("You are in line to board a submarine. It is a refitted Ohio class SSBN that now functions primarily as a research vessel in the Arctic.\n");printf("The person in front of you has just finished being checked by the greeter, and you walk up to him. \n");printf("Following a cheery hello, the gray haired gentleman asks you for your name.\n")	printf("Ah wonderful ! Please come inside. The man says. \n");	printf("You board the ship and after a few ladders and stairs find yourself in the mess hall which has been converted into a fairly presentable dining room. \n");	printf("");
	 draw_map(nouns);
    legend(nouns);
		  parsebox();

    wnoutrefresh(board);
    wnoutrefresh(disp);

    
    wnoutrefresh(disp2);
     wmove(disp2, 1, 0);
	
	waddstr(disp2,"You are standing in the middle of the pier market. You have a ship to catch");
	 wrefresh(disp2);
	getch();
	



	while (gamerun) {




		timee++;

		draw_map(nouns);
    legend(nouns);
	///	  parsebox();

		  wrefresh(board);
    wrefresh(disp);

    
  //  wnoutrefresh(disp2);
   // doupdate();

    parse(nouns, rooms,conversations, gamerun,timee);
	//	parse_pinput(nouns, rooms,conversations, gamerun,timee);




	}}






