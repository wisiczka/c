


/*
  BSP generation. Given the rectangle (T,L)-(T+H,L+W) it will
  recursively try to split this into reasonable chunks, alternating
  the direction with VERT. Once the map has been split into pieces
  small enough, it will build something interesting in that location.
*/
void bsp_split(int t, int l, int h, int w, bool vert)
{
  int new_w;
  int new_h;

  // If the rectangle is small enough, build something
  if (h <= BSP_MAX_H && w <= BSP_MAX_W)
  {
    fill_area(t, l , h, w);
    return;
  }

  /*
    Normally we will alternate between horizontal and vertical split,
    but we allow a shortcut to be taken if the area is already small
    enough and it would make more sense to split it the other way (a
    very tall but thin rectangle or a very low but wide).

    The rectangle will be split somewhere along the middle with a
    small variation (horizontal split allows more randomness than
    vertical). We will leave 1 tile in between so paths will be 3
    tiles wide most of the time.

    We will also provide a hint which way we want the next split to
    go; this may, of course, be ignored.
  */

  if (!vert || (h <= BSP_MAX_H && w > BSP_MAX_W))
  {
    new_w = (w / 2) - 3 + rand() % 7;
    bsp_split(t, l, h, new_w, true);
    bsp_split(t, l + new_w + 1, h, w - (new_w + 1), true);
  }
  else if (vert || (h > BSP_MAX_H && w <= BSP_MAX_W))
  {
    new_h = (h / 2) - 2 + rand() % 5;
    bsp_split(t, l, new_h, w, false);
    bsp_split(t + new_h + 1, l, h - (new_h + 1), w, false);
  }

  return;
}



/*
  Tries to build an animal enclosure in the box described by T, L, H,
  W (with a small chance something "special" will be build instead).
 */
void fill_area(int t, int l, int h, int w)
{
  int y;
  int x;

  int box_t;
  int box_l;
  int box_b;
  int box_r;

  int fence_n = true;
  int fence_e = true;
  int fence_s = true;
  int fence_w = true;

  int wall_n = 0;
  int wall_e = 0;
  int wall_s = 0;
  int wall_w = 0;

  // Leave one tile of path in each direction, so the player can move
  // around everywhere. Two enclosures padded with the one tile left
  // in bsp_split() averages three tiles for paths.
  box_t = t + 1;
  box_b = t + h - 1;
  box_l = l + 1;
  box_r = l + w - 1;

  // This is so we can do <= inclusive
  box_b--;
  box_r--;

  // Check for edges. If the enclosure lies along any of the edges we
  // will grow the enclosure one tile in that direction (we already
  // have an edge wall). We won't place a fence in that direction.

  if (t == 0)
  {
    wall_n = 1;
    fence_n = false;
  }

  if (l == 0)
  {
    wall_w = 1;
    fence_w = false;
  }

  if (t + h == MAP_H)
  {
    wall_s = 1;
    fence_s = false;
  }

  if (l + w == MAP_W)
  {
    wall_e = 1;
    fence_e = false;
  }

  // If an enclosure lies along the edge they will share walls and not
  // leave any path padding in between. Along the north/south edge we
  // will remove horizontal padding, along the east/west we will
  // remove vertical. We accomplish this by "swelling" the enclosure
  // in the perpendicular direction.

  if (t == 0 || t + h == MAP_H)
  {
    wall_w = 2;
    wall_e = 2;

    fence_w = false;
    fence_e = false;
  }

  if (l == 0 || l + w == MAP_W)
  {
    wall_n = 2;
    wall_s = 2;

    fence_n = false;
    fence_s = false;
  }

  // 1 will make the enclosure fill out its designated area, 2 will
  // make it overlap with its neighbour so they can share a wall.

  box_t -= wall_n;
  box_l -= wall_w;
  box_b += wall_s;
  box_r += wall_e;
  
  // Make sure it doesn't get out of bounds

  box_t = MAX(box_t, 0);
  box_l = MAX(box_l, 0);
  box_b = MIN(box_b, MAP_H - 1);
  box_r = MIN(box_r, MAP_W - 1);

  // Corner cases. If this enclosure lies in a corner we will fill it
  // with wall UNLESS this is the corner designated as entrance. The
  // entrance will be left as path; we will do something special there
  // later, in make_entrance().

  if ((box_t == 0         && box_l == 0) ||
      (box_b == MAP_H - 1 && box_l == 0) ||
      (box_t == 0         && box_r == MAP_W - 1) ||
      (box_b == MAP_H - 1 && box_r == MAP_W - 1))
  {
    if ((entrance == ENT_NW && box_t == 0         && box_l == 0) ||
	(entrance == ENT_SW && box_b == MAP_H - 1 && box_l == 0) ||
	(entrance == ENT_NE && box_t == 0         && box_r == MAP_W - 1) ||
	(entrance == ENT_SE && box_b == MAP_H - 1 && box_r == MAP_W - 1))
    {
      return;
    }

    tilefill(box_t, box_l, box_b, box_r, tile_wall);

    return;
  }
  else if (h <= 7 || w <= 8 || rand() % 6 == 0)
  {
    // If it's not a corner and the area is deemed too small for
    // animals (or randomly chosen) we will build something special
    // instead. Note that we return after this call.

    fill_special(t, l, h, w);

    return;
  }

  // By default we are allowed to have fences in all directions. If we
  // are allowed to have them _both_ n/s or e_w there is a random
  // chance we will disable either of them. The animals need to have
  // some privacy.

  if (fence_n && fence_s)
  {
    switch (rand() % 3)
    {
    case 0: fence_n = false; break;
    case 1: fence_s = false; break;
    case 2: break;
    }
  }

  if (fence_e && fence_w)
  {
    switch (rand() % 3)
    {
    case 0: fence_e = false; break;
    case 1: fence_w = false; break;
    case 2: break;
    }
  }

  // Make wall or fence along the box edges
  for (x = box_l + 1; x <= box_r - 1; x++)
  {
    stile(box_t, x, (fence_n ? tile_fence : tile_wall));
    stile(box_b, x, (fence_s ? tile_fence : tile_wall));
  }
  
  for (y = box_t + 1; y <= box_b - 1; y++)
  {
    stile(y, box_l, (fence_w ? tile_fence : tile_wall));
    stile(y, box_r, (fence_e ? tile_fence : tile_wall));
  }

  // The corners will always be walls
  stile(box_t, box_l, tile_wall);
  stile(box_t, box_r, tile_wall);
  stile(box_b, box_l, tile_wall);
  stile(box_b, box_r, tile_wall);
  
  // Let someone move in. Note this box is one smaller than the
  // enclosure itself.
  populate(box_t + 1, box_l + 1, box_b - 1, box_r - 1);

  return;
}


/*
  Builds something "special" (i.e. anything but an animal enclosure)
  in the box described by T, L, H, W.
*/
void fill_special(int t, int l, int h, int w)
{
  int y;
  int x;
  int special;

  special = rand() % 10;

  if (special <= 4)
  {
    // Make it a house
    house(t, l, h, w);
  }
  else if (special == 6)
  {
    // Make it a lawn
    tilefill(t + 1, l + 1, t + h - 2, l + w - 2, tile_grass);

    // Maybe add some trees?
    if (rand() % 3 == 0)
    {
      int trees;
      trees = rand() % 50;

      while (trees--)
      {
	y = t + 1 + rand() % MAX(1, h - 2);
	x = l + 1 + rand() % MAX(1, w - 2);
	stile(y, x, tile_tree);
      }
    }
  }
  else if (special == 7)
  {
    // A fountain
    tilefill(t + 1, l + 1, t + h - 2, l + w - 2, tile_gravel);
    tilefill(t + 2, l + 2, t + h - 3, l + w - 3, tile_water);
  }
  else
  {
    // Leave it as "path"
  }

  return;
}



/*
  Builds a house at (T,L)-(T+H,L+W). The type of the house is random.
*/
void house(int t, int l, int h, int w)
{
  int y;
  int x;
  int doorwall;
  int door_y;
  int door_x;
  
  // Make the walls, then make a room within them
  tilefill(t + 1, l + 1, t + h - 2, l + w - 2, tile_wall);
  tilefill(t + 2, l + 2, t + h - 3, l + w - 3, tile_gravel);
  
  // Pick a wall place the door in
  doorwall = rand() % 4;
  
  // Generate random coordinates for the door along that wall
  switch (doorwall)
  {
  case DW_W:
    door_y = t + 2 + rand() % MAX(1, h - 4);
    door_x = l + 1;
    break;
    
  case DW_E:
    door_y = t + 2 + rand() % MAX(1, h - 4);
    door_x = l + w - 2;
    break;
    
  case DW_N:
    door_y = t + 1;
    door_x = l + 2 + rand() % MAX(1, w - 4);
    break;
    
  case DW_S:
    door_y = t + h - 2;
    door_x = l + 2 + rand() % MAX(1, w - 4);
    break;
  }

  stile(door_y, door_x, tile_door_closed);

  // Is there room for anything inside the house?
  if (h <= 7 && w <= 7)
    return; // There ain't
  
  // Make a bathroom. This will be a line of stalls along the wall
  // opposing the door. We will only do this if the house is of an odd
  // size; this is to keep the last stall aligned with the wall and
  // avoid making double walls at the end.

  // We will also try to make a sink along the same wall as the door,
  // but only if there's room for it.

  if (h % 2 && w % 2 && rand() % 2 == 0)
  {
    int y_s = 0;
    int x_s = 0;
    int i;

    // y_s and x_s determine the direction of the line of stalls

    if (doorwall == DW_S)
    {
      y = t + 2;
      x = l + 2;
      x_s = +1;

      if (gtile(door_y - 1, door_x - 2) == tile_gravel)
	stile(door_y - 1, door_x - 2, tile_sink);
      else if (gtile(door_y - 1, door_x + 2) == tile_gravel)
	stile(door_y - 1, door_x + 2, tile_sink);
    }
    else if (doorwall == DW_E)
    {
      y = t + 2;
      x = l + 2;
      y_s = +1;

      if (gtile(door_y - 2, door_x - 1) == tile_gravel)
	stile(door_y - 2, door_x - 1, tile_sink);
      else if (gtile(door_y + 2, door_x - 1) == tile_gravel)
	stile(door_y + 2, door_x - 1, tile_sink);
    }
    else if (doorwall == DW_W)
    {
      y = t + h - 3;
      x = l + w - 3;
      y_s = -1;

      if (gtile(door_y - 2, door_x + 1) == tile_gravel)
	stile(door_y - 2, door_x + 1, tile_sink);
      else if (gtile(door_y + 2, door_x + 1) == tile_gravel)
	stile(door_y + 2, door_x + 1, tile_sink);
    }
    else if (doorwall == DW_N)
    {
      y = t + h - 3;
      x = l + w - 3;
      x_s = -1;

      if (gtile(door_y + 1, door_x - 2) == tile_gravel)
	stile(door_y + 1, door_x - 2, tile_sink);
      else if (gtile(door_y + 1, door_x + 2) == tile_gravel)
	stile(door_y + 1, door_x + 2, tile_sink);
    }

    // Ok, time to build the stalls.

    for (i = 0; i < 10; i++)
    {
      if (gtile(y, x) != tile_gravel)
      {
	// We've hit a wall, most probably.
	break;
      }

      // The toilet will be along the wall, the door will be offset at
      // a right angle with the inverted coordinate/speed pairs

      stile(y, x, tile_toilet);
      stile(y + x_s, x + y_s,  tile_door_closed);
      x += x_s;
      y += y_s;

      stile(y, x, tile_wall);
      stile(y + x_s, x + y_s, tile_wall);
      x += x_s;
      y += y_s;
    }
  }
  else if (rand() % 2 == 0)
  {
    // Make two rooms in the house. If the door is on a vertical wall,
    // make another vertical wall in the middle of the house and put
    // another door in it. Likewise for horizontal.

    if (doorwall == DW_W || doorwall == DW_E)
    {
      for (y = t + 1; y < t + h - 2; y++)
	stile(y, l + (w / 2), tile_wall);
      
      stile(t + 2 + rand() % MAX(1, h - 4), l + (w / 2), tile_door_closed);
    }
    else
    {
      for (x = l + 1; x < l + w - 2; x++)
	stile(t + (h / 2), x, tile_wall);
      
      stile(t + (h / 2), l + 2 + rand() % MAX(1, w - 4), tile_door_closed);
    }
  }

  return;
}



/*
  Returns the tile at (Y,X).
*/
int gtile(int y, int x)
{
  if (y < 0 || y >= MAP_H || x < 0 || y >= MAP_W)
    return tile_wall;
  
  return map[y][x];
}



/*
  Sets the tile at (Y,X) to TILE. Checks for out of bounds, so there's
  no risk writing outside the map.
*/
void stile(int y, int x, int tile)
{
  if (y < 0 || y >= MAP_H || x < 0 || y >= MAP_W)
    return;
  
  map[y][x] = tile;

  return;
}



/*
  Fills a rectangle from (T,L) to (B,R) (inclusive) with TILE.
*/
void tilefill(int t, int l, int b, int r, int tile)
{
  int y;
  int x;

  for (y = t; y <= b; y++)
    for (x = l; x <= r; x++)
      stile(y, x, tile);

  return;
}



/*
  Opens up a hole in the edge wall so the player can leave the
  zoo. Also makes the entrance area a bit narrower.
*/
void make_entrance()
{
  int y;
  int x;

  // Pick a starting for the player
  if (entrance == ENT_NW)
  {
    player->y = 2;
    player->x = 2;
  }
  else if (entrance == ENT_SW)
  {
    player->y = MAP_H - 3;
    player->x = 2;
  }
  else if (entrance == ENT_NE)
  {
    player->y = 2;
    player->x = MAP_W - 3;
  }
  else if (entrance == ENT_SE)
  {
    player->y = MAP_H - 3;
    player->x = MAP_W - 3;
  }

  if (rand() % 2)
  {
    // Make a horizontal exit
    stile(player->y - 1, player->x - 2, tile_path);
    stile(player->y - 0, player->x - 2, tile_path);
    stile(player->y + 1, player->x - 2, tile_path);

    stile(player->y - 1, player->x + 2, tile_path);
    stile(player->y - 0, player->x + 2, tile_path);
    stile(player->y + 1, player->x + 2, tile_path);

    for (x = player->x - 2; x <= player->x + 2; x++)
    {
      for (y = player->y - 2; y >= 0; y--)
      {
	if (gtile(y, x) != tile_path)
	  break;
	
	stile(y, x, tile_wall);
      }	

      for (y = player->y + 2; y < BOARD_H - 1; y++)
      {
	if (gtile(y, x) != tile_path)
	  break;
	
	stile(y, x, tile_wall);
      }	
    }
  }
  else
  {
    // Make a vertical exit
    stile(player->y - 2, player->x - 1, tile_path);
    stile(player->y - 2, player->x - 0, tile_path);
    stile(player->y - 2, player->x + 1, tile_path);
    
    stile(player->y + 2, player->x - 1, tile_path);
    stile(player->y + 2, player->x - 0, tile_path);
    stile(player->y + 2, player->x + 1, tile_path);

    for (y = player->y - 2; y <= player->y + 2; y++)
    {
      for (x = player->x - 2; x >= 0; x--)
      {
	if (gtile(y, x) != tile_path)
	  break;
	
	stile(y, x, tile_wall);
      }	

      for (x = player->x + 2; x < BOARD_W - 1; x++)
      {
	if (gtile(y, x) != tile_path)
	  break;
	
	stile(y, x, tile_wall);
      }	
    }
  }


  return;
}



/*
  Adds an animal with suitable terrain to the rectangle (T,L)-(B,R).
*/
void populate(int t, int l, int b, int r)
{
  int type;
  int terrain;
  int base_tile;
  int y;
  int x;
  int mobs;
  int rocks;
  int trees;
  int water;
  int invert;

  // Pick an animal that isn't yet represented in the zoo
  type = new_animal();

  if (type == nobody)
    return; // No more animals available

  // Check what terrain this animal prefers
  terrain = animal_terrain(type);

  // Each terrain has a base tile and a number of decorations
  rocks = 0;
  trees = 0;
  water = 15;

  // How many "inverted" tiles of the "other" kind (grass<->sand)
  invert = 0;

  switch (terrain)
  {
  case terr_pond:
    base_tile = tile_grass;
    water += 50;
    rocks = 5 + rand() % 5;
    trees = 5 + rand() % 10;
    invert = 5;
    break;

  case terr_arctic:
    base_tile = tile_gravel;
    water += 60;
    rocks = 10 + rand() % 10;
    break;

  case terr_grass:
    base_tile = tile_grass;
    water += 30;
    rocks = rand() % 6;
    trees = rand() % 8;
    invert = 10;
    break;

  case terr_forest:
    base_tile = tile_grass;
    water += 30;
    rocks = 2 + rand() % 3;
    trees = 25 + rand() % 10;
    invert = 5;
    break;

  case terr_savannah:
    base_tile = tile_sand;
    water += 25;
    rocks = 1 + rand() % 6;
    trees = 1 + rand() % 6;
    invert = 10;
    break;
  }

  tilefill(t, l, b, r, base_tile);

  while (rocks--)
  {
    y = t + rand() % MAX(1, (b - t));
    x = l + rand() % MAX(1, (r - l));
    stile(y, x, tile_rock);
  }

  while (trees--)
  {
    y = t + rand() % MAX(1, (b - t));
    x = l + rand() % MAX(1, (r - l));
    stile(y, x, tile_tree);
  }

  while (invert--)
  {
    y = t + rand() % MAX(1, (b - t));
    x = l + rand() % MAX(1, (r - l));
    stile(y, x, (base_tile == tile_grass ? tile_sand : tile_grass));
  }

  // Add some water with a drunken walk.
  // Don't go outside the enclosure.

  y = t + rand() % MAX(1, (b - t));
  x = l + rand() % MAX(1, (r - l));

  while (water--)
  {
    int new_y;
    int new_x;

    if (rand() % 2)
    {
      new_y = y - 1 + rand() % 3;
      y = MAX(t, MIN(b, new_y));
    }
    else
    {
      new_x = x - 1 + rand() % 3;
      x = MAX(l, MIN(r, new_x));
    }

    stile(y, x, tile_water);
  }

  // Finally add some animals at random locations, but not on top of
  // obstacles. Try to make at least two of each kind.

  mobs = 2 + rand() % 6;

  while (mobs && next_mob < MAX_MOBS)
  {
    nouns[next_mob].y = t + rand() % MAX(1, (b - t));
    nouns[next_mob].x = l + rand() % MAX(1, (r - l));

    if ((blocking(nouns[next_mob].y, nouns[next_mob].x) & CANT_MOVE) == 0)
    {
      nouns[next_mob].type = type;
      next_mob++;
    }

    mobs--;
  }

  return;
}





/*
  Returns the kind of terrain an animal prefers.
*/
int animal_terrain(int type)
{
  switch (type)
  {
  case anml_anteater:
  
  default:
    return terr_grass;

  case anml_boar:
    return terr_arctic;


  
  case anml_alligator:
    return terr_pond;

 
  }
}




/*
  Selects a random animal that is not yet represented in the zoo.
*/
int new_animal()
{
  int start;
  int i;
  int ret;

  // Pick a random starting point in the animal list. If that slot is
  // "nobody" the animal is already in the zoo; move on to the
  // next. If we go around the list and return to the starting point,
  // all animals have already been selected.

  i = start = rand() % anml_last;

  do
  {
    if (animals[i] != nobody)
    {
      ret = animals[i];
      animals[i] = nobody;
      return ret;
    }

    i = (i + 1) % anml_last;
  }
  while (i != start);

  return nobody; // Give up
}



/*
  Returns how lazy an animal is. A large number makes it less likely
  for it to move randomly.
*/
int lazy(int type)
{
  switch (type)
  {
  default:
    return 50;



  case anml_boar:
  case anml_alligator:
 
    return 50;

  case anml_anteater:
    return 100;



  }
}

