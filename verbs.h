


void create_conversation(struct Conversation *conversation, int id, int owner_id, char* question,char* answer, int show_inventory, int leave_phrase)

{

	
	



	strcpy( conversation->question, question);
	strcpy( conversation->answer, answer);
	conversation->id=id;
	conversation->show_inventory=show_inventory;
	conversation->owner_id=owner_id;
	conversation->leave_phrase = leave_phrase;
	

}




void create_noun(struct Noun *subject, char* name, int def_happy, int id, int helf, int pos, char* gender, char* subject_pronoun, char* object_pronoun,char* description,
	int base_cost,  int damage,  int defense, int gold)
{




	subject->gold = gold;

	strcpy( subject->name, name);
	subject->happy = def_happy;
	subject->noun_id = id;
	subject->owner_id = GOD_ID;
	subject->wielded = 0;
	subject->health = helf;
	subject->main_quest = 0;
	if (helf>0){subject->alive = 1;subject->animate = 1;}
	if (helf==0){subject->alive = 0;subject->animate = 0;}
	if (helf<0){subject->alive = 0;subject->animate = 1;}



	subject->position = pos;
	strcpy( subject->gender , gender);
	strcpy( subject->subject_pronoun , subject_pronoun);
	strcpy( subject->object_pronoun , object_pronoun);
	strcpy( subject->description, description);
//reflexive pronouns
	char stringy[25];
	strcpy( stringy, subject->object_pronoun);
	strcat(stringy, "self");
 //  wprintw(disp2,"%s",stringy);

	strcpy( subject->reflexive_pronoun, stringy);
//reflexive pronouns end

	subject->base_cost = base_cost;
	subject->damage = damage;
	subject->defense = defense;


}






void create_room(struct Room *room, char*  name,char* description, int position, int travelable_rooms[])
{

	
	int jeep;



	for( jeep  = 0; jeep<MAX_TRAVELABLE_ROOMS; jeep = jeep + 1){
		room->travelable_rooms[jeep]=GOD_ID;
	}
	for( jeep  = 0; jeep<MAX_TRAVELABLE_ROOMS; jeep = jeep + 1){
		room->travelable_rooms[jeep]=travelable_rooms[jeep];
	}





	strcpy( room->name, name);
	strcpy( room->description, description);
	room->position = position;

}




/////////////////////////////VERB FUNCTIONS
////////////////////////////








void headpat(struct Noun *subject , struct Noun *direct_object );
void headpat(struct Noun *subject , struct Noun *direct_object )
{


	if((subject->position == direct_object->position) && (direct_object->animate == 0)){
		wprintw(disp2,"%s headpats the %s ! that was interesting .\n", subject->name, direct_object->name);
		subject->happy = (subject->happy) + 10;
	}

	if((subject->position == direct_object->position) && (direct_object->alive == 1) && (direct_object->animate == 1)){
		wprintw(disp2,"%s headpats the %s ! %s blushes .\n", subject->name, direct_object->name, direct_object->subject_pronoun );
		direct_object->happy = (direct_object->happy) + 5;
	}

	if((subject->position == direct_object->position) && (direct_object->alive == 0) && (direct_object->animate == 1)){
		wprintw(disp2,"%s headpats the %s's dead body ! that was not very pleasant .\n", subject->name, direct_object->name);
		subject->happy = (subject->happy) - 10;
	}
	

}



void examine_noun(struct Noun *subject , struct Noun *direct_object );
void examine_noun(struct Noun *subject , struct Noun *direct_object )
{
	if((subject->position == direct_object->position) || (subject->noun_id == direct_object->owner_id)){
		wprintw(disp2," %s \n", direct_object->description );

}
}

void examine_room(struct Noun *subject , struct Room *direct_object, struct Noun * nouns,struct Room * rooms  );
void examine_room(struct Noun *subject , struct Room *direct_object,struct Noun * nouns,struct Room * rooms  )
{
	
	int i=0;
	if(subject->position == direct_object->position){
		wprintw(disp2," %s \n",direct_object->description );
		wprintw(disp2,"connections: ");
		for(i =0; (direct_object->travelable_rooms[i] != -2) && (i<MAX_TRAVELABLE_ROOMS); i++ ){
		
			wprintw(disp2," '%s' ",rooms[direct_object->travelable_rooms[i]].name);
		
		
		
		}
		wprintw(disp2,"\n");
		wprintw(disp2,"contains: ");
		for( i  = 1; (strcmp(nouns[i].name,"BUFFER NAME")); i = i + 1){

			if((nouns[i].position == direct_object->position)){
//            wprintw(disp2,"nouns[jeep]->damage %d", nouns[jeep].damage);
				wprintw(disp2," '%s' ",nouns[i].name);


			}


		}
		wprintw(disp2,"\n");
	}//else{wprintw(disp2,"%s is not in the %s !\n", subject->name, direct_object->name);} removed cause of move merge


}



void move_noun(struct Noun *subject , struct Room *direct_object  );
void move_noun(struct Noun *subject , struct Room *direct_object )
{
	int i=0;
	for (i=0; (direct_object->travelable_rooms[i] != -2) && (i<MAX_TRAVELABLE_ROOMS); i++){
	//	wprintw(disp2,"%d",i);
	
	if (subject->position == direct_object->travelable_rooms[i]){

	wprintw(disp2,"%s moves to the %s  .\n", subject->name, direct_object->name );
	subject->position = direct_object->position;
	
	

	}}
	
	
	wprintw(disp2,"\n");
}


void inventory(struct Noun *subject,struct Noun * nouns);
void inventory(struct Noun *subject,struct Noun * nouns){
	int jeep;
	
	wprintw(disp2,"\n %s has: ", subject->name);
	for( jeep  = 0; (strcmp(nouns[jeep].name,"BUFFER NAME")); jeep = jeep + 1){

			if((nouns[jeep].owner_id == subject->noun_id)){
//            wprintw(disp2,"nouns[jeep]->damage %d", nouns[jeep].damage);
				wprintw(disp2," '%s' ",nouns[jeep].name);


			}


		}
		wprintw(disp2,"\n");
	
	
	
	
}



void take_noun(struct Noun *subject, struct Room *room, struct Noun *direct_object);
void take_noun(struct Noun *subject, struct Room *room, struct Noun *direct_object)
{

	if(direct_object->position == subject->position){
		wprintw(disp2,"%s takes the %s .\n", subject->name, direct_object->name);
		direct_object->owner_id = subject->noun_id;
		direct_object->position = -1;
		

	}



}




void drop_noun(struct Noun *subject, struct Room *room, struct Noun *direct_object);
void drop_noun(struct Noun *subject, struct Room *room, struct Noun *direct_object)
{

	if((direct_object->owner_id == subject->noun_id)){
		wprintw(disp2,"%s drops the %s .\n", subject->name, direct_object->name);
		direct_object->owner_id = GOD_ID;
		direct_object->position = subject->position;
		direct_object->wielded = 0;
		

	}else{wprintw(disp2,"%s does not have a %s to drop!\n",subject->name, direct_object->name);}



}


void wield_noun(struct Noun *subject, struct Noun *direct_object);
void wield_noun(struct Noun *subject, struct Noun *direct_object)
{
if((direct_object->wielded == 0) && (direct_object->owner_id == subject->noun_id)){
		wprintw(disp2,"%s wields the %s .\n", subject->name, direct_object->name);
		direct_object->wielded = 1;
		
	
	}else{wprintw(disp2,"%s does not have a %s in their inventory to wield !\n",subject->name, direct_object->name);}



}

void unwield_noun(struct Noun *subject, struct Noun *direct_object);
void unwield_noun(struct Noun *subject, struct Noun *direct_object)
{

if((direct_object->wielded == 1) && (direct_object->owner_id == subject->owner_id)){
		wprintw(disp2,"%s unwields the %s .\n", subject->name, direct_object->name);
		direct_object->wielded = 0;

	}else{wprintw(disp2,"%s is not currently wielding a %s !\n",subject->name, direct_object->name);}



}


void give_noun(struct Noun *subject, struct Noun *direct_object, struct Noun *indirect_object);
void give_noun(struct Noun *subject, struct Noun *direct_object, struct Noun *indirect_object)
{

	if((subject->position == indirect_object->position) && (direct_object->owner_id == subject->noun_id)){
		wprintw(disp2,"%s gives the %s to %s .\n", subject->name, direct_object->name, indirect_object->name);
	///	wprintw(disp2,"direct_object->name: %s, indirect_object->name: %s, direct_object->noun_id: %d,indirect_object->noun_id: %d,\n",  direct_object->name, indirect_object->name,direct_object->noun_id,indirect_object->noun_id);
		direct_object->owner_id = indirect_object->noun_id;
		direct_object->wielded = 0;
		
		

	}



}




void buy(struct Noun *subject , struct Noun *direct_object, struct Noun *indirect_object);
void buy(struct Noun *subject , struct Noun *direct_object, struct Noun *indirect_object){
	
	if((subject->position == indirect_object->position) && (subject->gold >= direct_object->base_cost) && (direct_object->owner_id == indirect_object->noun_id)){
		
		
		wprintw(disp2,"%s buys the %s from %s for %d gold.\n", subject->name, direct_object->name, indirect_object->name, direct_object->base_cost );
		give_noun(indirect_object,direct_object,subject);
		subject->gold = (subject->gold ) - (direct_object->base_cost);
		indirect_object->gold = (indirect_object->gold ) + (direct_object->base_cost);
		wprintw(disp2,"%s now has %d gold . \n", subject->name, subject->gold);
		wprintw(disp2,"%s now has %d gold . \n", indirect_object->name, indirect_object->gold);
		
		

	}///else{wprintw(disp2,"You cannot buy that here \n");}
		
		
		
	
	
	
	
}


void sell(struct Noun *subject , struct Noun *direct_object , struct Noun *indirect_object );
void sell(struct Noun *subject , struct Noun *direct_object , struct Noun *indirect_object ){
	
	if((subject->position == indirect_object->position) && (indirect_object->gold >= direct_object->base_cost) && (direct_object->owner_id == subject->noun_id)){
		
		
		wprintw(disp2,"%s sells the %s to %s for %d gold.\n", subject->name, direct_object->name, indirect_object->name, direct_object->base_cost );
		give_noun(subject,direct_object,indirect_object);
		indirect_object->gold = (indirect_object->gold ) - (direct_object->base_cost);
		subject->gold = (subject->gold) + (direct_object->base_cost);
		wprintw(disp2,"%s now has %d gold . \n", subject->name, subject->gold);
		wprintw(disp2,"%s now has %d gold . \n", indirect_object->name, indirect_object->gold);
		
		

	}///else{wprintw(disp2,"You cannot buy that here \n");}
		
		
		
	
	
	
	
}



///add defense
void attack(struct Noun *subject, struct Noun *direct_object,struct Noun * nouns );
void attack(struct Noun *subject, struct Noun *direct_object,struct Noun * nouns )
{

	int damage = 1;
	int jeep;

	if(subject->position == direct_object->position){
		for( jeep  = 0; (strcmp(nouns[jeep].name,"BUFFER NAME")); jeep = jeep + 1){

			if((nouns[jeep].wielded == 1) &&(nouns[jeep].owner_id == subject->noun_id)){
//            wprintw(disp2,"nouns[jeep]->damage %d", nouns[jeep].damage);
				damage = damage + nouns[jeep].damage;


			}


		}
		wprintw(disp2,"%s does %d damage to %s .\n", subject->name, damage, direct_object->name);
		direct_object->health = direct_object->health - damage;
		wprintw(disp2," The %s now has %d health .\n", direct_object->name,  direct_object->health);


	}else{wprintw(disp2,"%s is not currently in the same room as %s !\n",subject->name, direct_object->name);}



}












void talk(struct Noun *subject , struct Noun *direct_object ,struct Conversation * conversations, struct Noun * nouns);
void talk(struct Noun *subject , struct Noun *direct_object ,struct Conversation * conversations, struct Noun * nouns){
	
	
	
	int jeep;
	
	int leave = 0;

	if(subject->position == direct_object->position){
		
		wprintw(disp2,"%s greets %s\n",subject->name,direct_object->name);
		wprintw(disp2,"What would you like to talk about?\n");
		
		
		
		wprintw(disp2,"TOPICS: ");
		
		
		for( jeep  = 0; (strcmp(conversations[jeep].question,"BUFFER QUESTION")); jeep = jeep + 1){

			if((conversations[jeep].owner_id == direct_object->noun_id) ){
            wprintw(disp2," %s ,",  conversations[jeep].question);
				


			}


		}
		
		while(!leave){
			
			char talk_string[MAX_STR_SZ];
	char string_to_parse[MAX_STR_SZ];
	int talk_object = 0;
	char *talk_noun_parse =  0;
		
		   echo();
   wmove(disp2, DISP_H-1, 0);
  waddstr( disp2, ">>>                                                                                                ");
       wmove(disp2, DISP_H-1, 0);
      waddstr( disp2, ">>>");
      wgetstr(disp2, talk_string);
      noecho();
      parsebox();
		strcpy(string_to_parse, " ");

		strcat(string_to_parse, talk_string);
		
		
			for( talk_object = 0; (strcmp(conversations[talk_object].question, "BUFFER NAME") && !talk_noun_parse); talk_object = talk_object + 1 ){
				
				talk_noun_parse = strstr(string_to_parse, conversations[talk_object].question);
	//			wprintw(disp2,"room_object = %d",room_object);
				



			}talk_object--; ///for second_noun end
			
			
			if(talk_noun_parse){
				
				wprintw(disp2,"\n %s: %s\n",direct_object->name,conversations[talk_object].answer);
				if(conversations[talk_object].show_inventory){inventory(&nouns[direct_object->noun_id],nouns);}
				if(conversations[talk_object].leave_phrase){leave = 1;}
				
				
				
			}
	

	}
	
	}
	
	
}











/*
  Tries to move the player SPEED_Y north/south and SPEED_X left/right.
  Returns false if the way is blocked.
*/
bool move_player(int speed_y, int speed_x,struct Noun * nouns)
{
  int new_y;
  int new_x;

  new_y = player->y + speed_y;
  new_x = player->x + speed_x;

  if (new_y < 0 || new_y >= MAP_H || new_x < 0 || new_x >= MAP_W)
  {
    // Leaving the map ends the game
    shutdown();
  }

  // If there is a door in the way, open it
  if (gtile(new_y, new_x) == tile_door_closed)
  {
    stile(new_y, new_x, tile_door_open);
    calc_fov(nouns);
    return true;
  }

  // Is something in the way?

  if (blocking(new_y, new_x, nouns) & (CANT_MOVE | SOMEONE_THERE))
    return false;

  // The way is clear; go there

  player->y = new_y;
  player->x = new_x;

  recenter();
  calc_fov(nouns);

  return true;
}


































