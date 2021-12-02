void parse(){



	int input;












	 new_input:
    input = getch();
    
    if (input == key_left)
    {
      if (move_player(0, -1) == false)
	goto new_input;
    }
    else if (input == key_right)
    {
      if (move_player(0, +1) == false)
	goto new_input;
    }
    else if (input == key_up)
    {
      if (move_player(-1, 0) == false)
	goto new_input;
    }
    else if (input == key_down)
    {
      if (move_player(+1, 0) == false)
	goto new_input;
    }
    else if (input == '.' || input == ' ')
    {
      // Wait
    }

    else if (input == 't')
    {
      parse_pinput();

    }




    else if (input == 'Q')
    {
      shutdown();
    }
    else
    {
      // Anything we don't recognize; no need to redraw the screen
      goto new_input;
    }
}




  void parse_pinput(struct Noun * nouns, struct Room * rooms, struct Conversation * conversations, int gamerun,int time){

 
    






    char comm[MAX_STR_SZ];
    char string_to_parse[MAX_STR_SZ];



    
    
    
    char *room_noun_parse;
    
    
  
    char tokenized_parse[20][20];


    
    int room_object = 0;
    int statcounter = 0;
    int tokencounter =0;

    
    room_noun_parse = 0;
    

    
  

  

    static const char *verbs[] = { "pat", "tickle", "kill", "dev", "examine", "go", "take", "drop", "wield", "unwield", "attack", "give", "move", "wear", "inventory", "buy", "sell", "talk", "BUFFER NAME"};
    static const char *prepositions[] = { "to", "from", "BUFFER NAME" };

///talk verb

   echo();
   wmove(disp2, DISP_H-1, 0);
  waddstr( disp2, ">>>                                                                                                ");
       wmove(disp2, DISP_H-1, 0);
      waddstr( disp2, ">>>");
      wgetstr(disp2, comm);
      noecho();
      parsebox();
      strcpy(string_to_parse, " ");

      strcat(string_to_parse, comm);
   
      wmove(disp2, 1, 0);
 
     

///TOKENIZATION
    char * token = strtok(comm, " ");
   // loop through the string to extract all other tokens
    for(tokencounter=0; token != NULL ; tokencounter++) {

      strcpy(tokenized_parse[tokencounter], token);
      removeStringTrailingNewline(tokenized_parse[tokencounter]);

    /// printf("token %d  = %s \n",tokencounter,tokenized_parse[tokencounter]);
      token = strtok(NULL, " ");
    }

   ///TOKENIZATION END

    int i = 0;
    
    int noun1_parse = -1;
    int first_noun = -1;
    int noun2_parse = -1;
    int second_noun = -1;
    int verb_parse = -1; /// NULL POINTER
    int verb = -1;
    int direct_object_noun = -1;
    int indirect_object_noun = -1;
    


for( i = 0; ((i < tokencounter)) ; i++){
 
  if(verb_parse){

    for( verb = 0;(strcmp(verbs[verb], "BUFFER NAME") && verb_parse); verb = verb + 1 ){
      verb_parse = strcmp(tokenized_parse[i], verbs[verb]);

    //  printf("verb parse: %d", verb_parse);



  //    printf("string_to_parse = %s, \n",string_to_parse);

     }verb--; /// for verb

    }


        
        
        
        


      if (!noun1_parse && noun2_parse){


        for( second_noun = 0; (strcmp(nouns[second_noun].name, "BUFFER NAME") && noun2_parse); second_noun = second_noun + 1 ){
      noun2_parse = strcmp(tokenized_parse[i], nouns[second_noun].name);
      
      



      }second_noun--; ///for second_noun end





      }

      if (noun1_parse){

       for( first_noun = 0; (strcmp(nouns[first_noun].name, "BUFFER NAME") && noun1_parse); first_noun = first_noun + 1 ){
      noun1_parse = strcmp(tokenized_parse[i], nouns[first_noun].name);
      
      



      }first_noun--; ///for first_noun end
    }





}


///printf(" %s SET AS INDIRECT OBJECT \n %s SET AS DIRECT OBJECT \n",nouns[first_noun].name, nouns[second_noun].name);

int prepositioned_noun_parse = -1;
int preposition = 0;
    int preposition_parse = 1;





    for( i = 0; ((i < tokencounter) && preposition_parse) ; i++){
  //    printf("token2 %d  = %s \n",i,tokenized_parse[i]);
      for (preposition=0;(strcmp(prepositions[preposition], "BUFFER NAME") && preposition_parse); preposition=preposition+1){


        preposition_parse = (strcmp(prepositions[preposition], tokenized_parse[i]));
  //      printf("preposition_parse = %d \n prepositions[preposition] = %s \n tokenized_parse[i] = %s \n",preposition_parse, prepositions[preposition],tokenized_parse[i]);
        

      }
    }i--;

int x = i;
int prepositioned_noun = 0;
if (preposition_parse == 0){
  for( x=i ; ((x < tokencounter) && prepositioned_noun_parse) ; x++ ){

          for( prepositioned_noun = 0; (strcmp(nouns[prepositioned_noun].name, "BUFFER NAME") && prepositioned_noun_parse); prepositioned_noun = prepositioned_noun + 1 ){

            


          
//               printf("noun1_parse = %d \n nouns[first_noun].name = .%s. \n tokenized_parse[%d] = .%s. \n",noun1_parse, nouns[first_noun].name,x,tokenized_parse[x]);
         prepositioned_noun_parse = strcmp(nouns[prepositioned_noun].name, tokenized_parse[x]);
         
     

     }
      
      



      }




        }prepositioned_noun--;
        
  ///printf("AAAA %s SET AS SECOND NOUN \n %s SET AS FIRST NOUN  \n  %s SET AS PREPOSITIONED\n\n",nouns[second_noun].name, nouns[first_noun].name, nouns[prepositioned_noun].name);
  
  
  

if( noun2_parse){direct_object_noun=first_noun;}
if( !noun2_parse){direct_object_noun=first_noun; indirect_object_noun=second_noun;}



  /// CLASS A VERBS: SECOND NOUN IS DIRECT OBJECT, PREPOSITIONED NOUN IS INDIRECT OBJECT
  
  
  if (!(strcmp(verbs[verb], "give")) || !(strcmp(verbs[verb], "sell"))){
    
    
    if( !noun2_parse){direct_object_noun=second_noun; indirect_object_noun=first_noun;}   
    
    
    if(!prepositioned_noun_parse ){
            indirect_object_noun = prepositioned_noun;
            if(prepositioned_noun==first_noun){direct_object_noun = second_noun;}
            if(prepositioned_noun==second_noun){direct_object_noun = first_noun;}


          }   

         }
         
         
         
       /// CLASS B VERBS: FIRST NOUN IS DIRECT OBJECT, PREPOSITIONED NOUN IS INDIRECT OBJECT
  
  
  if (!(strcmp(verbs[verb], "buy"))){
    
    

    
    
    if(!prepositioned_noun_parse ){
            indirect_object_noun = prepositioned_noun;
            if(prepositioned_noun==first_noun){direct_object_noun = second_noun;}
            if(prepositioned_noun==second_noun){direct_object_noun = first_noun;}


          }   

         }  
         
         
         
         
         
        
        
///printf("BBB %s SET AS DIRECT OBJECT NOUN \n %s SET AS INDIRECT OBJECT NOUN  \n  %s SET AS PREPOSITIONED\n\n",nouns[direct_object_noun].name, nouns[indirect_object_noun].name, nouns[prepositioned_noun].name);        


     


////printf(" %s SET AS INDIRECT OBJECT \n %s SET AS DIRECT OBJECT \n",nouns[first_noun].name, nouns[second_noun].name);




  



      for( room_object = 0; (strcmp(rooms[room_object].name, "BUFFER NAME") && !room_noun_parse); room_object = room_object + 1 ){
        
        room_noun_parse = strstr(string_to_parse, rooms[room_object].name);
  //      printf("room_object = %d",room_object);
        



      }room_object--; ///for second_noun end


//    printf("verb = %d indirect object = %d direct object = %d",verb,first_noun,second_noun);

 ///parse match events

      if (!verb_parse ){
        
        
      if (!(strcmp(verbs[verb], "inventory"))){
            inventory(&nouns[0], nouns);


          }       
        
        
        
        
        
  ///           printf("MEOW\n");
        

  if ( room_noun_parse){//printf("WAHTTHEHECK %d %d %s",room_object, verb, verbs[verb] );

        if ((!(strcmp(verbs[verb], "examine"))) && room_noun_parse){
          examine_room(&nouns[0], &rooms[room_object], nouns,rooms);
     //           printf("Tss ~\n");

        }

        if (!(strcmp(verbs[verb], "go")) || !(strcmp(verbs[verb], "move"))){
          
          

          move_noun(&nouns[0], &rooms[room_object]);
          examine_room(&nouns[0], &rooms[room_object],nouns,rooms);
          
    // BUFFER NAME      
   //               printf("Tss ~\n");

        }




      }
        


////  static const char *verbs[] = { " pat", " tickle", " kill", " dev", " examine", " go", " take", " drop", " wield", " unwield", " attack", "BUFFER NAME", ""};
        if (!noun1_parse ){

          if (!(strcmp(verbs[verb], "pat"))){
            headpat(&nouns[0], &nouns[first_noun]);


          }
          if (!(strcmp(verbs[verb], "talk"))){
            talk(&nouns[0], &nouns[first_noun],conversations, nouns);


          }

          if (!(strcmp(verbs[verb], "kill"))){
            printf("You kill the loli . Fun .\n");
            gamerun = 0;
          }

          if (!(strcmp(verbs[verb], "dev"))){
            printf("happy = %i, verbs[1] = %s, \n",nouns[1].happy, verbs[1]);
            printf("nouns[second_noun].name = %s, \n",nouns[second_noun].name);
            printf(" rooms[0].name %s rooms[0].description %s ! rooms[0].travelable_rooms[1] %d  .\n", rooms[0].name, rooms[0].description, rooms[0].travelable_rooms[1] );
            nouns[1].happy = nouns[1].happy + 10;

          }

          if (!(strcmp(verbs[verb], "examine"))&& noun2_parse){
            examine_noun(&nouns[0], &nouns[first_noun]);

          }

          

          

          if (!(strcmp(verbs[verb], "take"))){
            
            take_noun( &nouns[0], &rooms[nouns[0].position], &nouns[first_noun]);

          }

          if (!(strcmp(verbs[verb], "drop"))){

            drop_noun( &nouns[0], &rooms[nouns[0].position], &nouns[first_noun]);

          }
          if (!(strcmp(verbs[verb], "wield")) || !(strcmp(verbs[verb], "wear"))){

            wield_noun( &nouns[0],  &nouns[first_noun]);

          }
          if (!(strcmp(verbs[verb], "unwield"))){

            unwield_noun( &nouns[0],  &nouns[first_noun]);

          }

          if (!(strcmp(verbs[verb], "attack"))){

            attack(&nouns[0], &nouns[first_noun], nouns);

          }

          if (!(strcmp(verbs[verb], "give"))){

            give_noun(&nouns[0], &nouns[direct_object_noun], &nouns[indirect_object_noun]);

          }
          
          if (!(strcmp(verbs[verb], "buy"))){

            buy(&nouns[0], &nouns[direct_object_noun], &nouns[indirect_object_noun]);

          }
          
          if (!(strcmp(verbs[verb], "sell"))){

            sell(&nouns[0], &nouns[direct_object_noun], &nouns[indirect_object_noun]);

          }



      }  /// if  noun parse end

    



  } ///if verb parse end



  for( statcounter = 0;(strcmp(nouns[statcounter].name,"BUFFER NAME")); statcounter = statcounter + 1 ){

      ///stat events

    if ( (nouns[statcounter].health < 0 ) && (nouns[statcounter].alive == 1)){

      printf("%s has died !\n",nouns[statcounter].name);
      nouns[statcounter].alive = 0;

    }

     }  ///for stat events


  if (nouns[0].main_quest == 0){
    
    
  /// printf("nyaa");
    
    
    
  }



 wrefresh(disp2);

  }//// parse function declaration end





  void removeStringTrailingNewline(char *str) {
  if (str == NULL)
    return;
  int length = strlen(str);
  if ((str[length-1] == '\n') || (str[length-1] == '\n'))
    str[length-1]  = '\0';
}
