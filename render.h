

/*
  Draws the map and updates the screen.
*/
void draw_map()
{
  int y;
  int x;
  int i;
  int tile;

  werase(board);

  for (y = 0; y < BOARD_H; y++)
  {
    for (x = 0; x < BOARD_W; x++)
    {
      // Only draw tiles that we have visited (FOV_SEEN bit, but we
      // can just check for anything)

      if (fov[y + view_y][x + view_x] == 0)
  continue;

      wmove(board, y, x);

      // No need for gtile() here, we know what we are doing
      tile = map[y + view_y][x + view_x];

      switch (tile)
      {
      case tile_grass:
  waddch(board, '.' | COLOR_PAIR(pair_green));
  break;

      case tile_path:
  waddch(board, '.' | COLOR_PAIR(pair_red));
  break;

      case tile_gravel:
  waddch(board, '.' | COLOR_PAIR(pair_white));
  break;

      case tile_sand:
  waddch(board, '.' | COLOR_PAIR(pair_brown));
  break;

      case tile_door_closed:
  waddch(board, '+' | COLOR_PAIR(pair_brown) | A_REVERSE);
  break;

      case tile_door_open:
  waddch(board, '/' | COLOR_PAIR(pair_brown));
  break;

      case tile_toilet:
  waddch(board, 'o' | COLOR_PAIR(pair_white));
  break;

      case tile_sink:
  waddch(board, 'D' | COLOR_PAIR(pair_cyan));
  break;

      case tile_wall:
  waddch(board, ACS_CKBOARD);
  break;

      case tile_fence:
  waddch(board, ACS_PLUS);
  break;

      case tile_water:
  waddch(board, ACS_CKBOARD | COLOR_PAIR(pair_cyan));
  break;

      case tile_tree:
  waddch(board, '&' | COLOR_PAIR(pair_green));
  break;

      case tile_rock:
  waddch(board, '*');
  break;

      case tile_error:
      default:
  waddch(board, '?' | COLOR_PAIR(pair_red) | A_REVERSE);
  break;
      }
    }
  }

  // Draws mobs, but only those currently within view.

  for (i = 0; i < MAX_MOBS; i++)
  {
    if (nouns[i].type == nobody)
      break; // No use proceeding

    y = nouns[i].y;
    x = nouns[i].x;

    if (y < view_y || y >= view_y + BOARD_H ||
  x < view_x || x >= view_x + BOARD_W ||
  (fov[y][x] & FOV_IN_VIEW) == 0)
    {
      continue;
    }

    wmove(board, y - view_y, x - view_x);

    waddch(board, anml_glyph[nouns[i].type]);
  }

  return;
}


/*
  Display names of the animals currently on screen.
*/
void parsebox()
{
  int type;
  int i;
  int printed = 0;
  int y;
  int x;
  bool seen;

 werase(disp2);

  wmove(disp2, 0, 0);
  waddstr(disp2, "~~world~~");


 



  return;
}




void legend()
{
  int type;
  int i;
  int printed = 0;
  int y;
  int x;
  bool seen;

  werase(disp);

  wmove(disp, 0, 0);
  waddstr(disp, "info");

 

  wmove(disp, (BOARD_H+DISP_H) - 1, 0);
  waddstr(disp, "qt/3.14");

  return;
}
























/*
  Recenters the viewport around the player, if needed.
*/
void recenter()
{
  int p_y;
  int p_x;

  p_y = player->y;
  p_x = player->x;

  if (p_y - view_y < 5 || p_y - view_y >= BOARD_H - 4)
  {
    view_y = MIN(MAP_H - BOARD_H, MAX(0, p_y - (BOARD_H / 2)));
  }

  if (p_x - view_x < 10 || p_x - view_x >= BOARD_W - 10)
  {
    view_x = MIN(MAP_W - BOARD_W, MAX(0, p_x - (BOARD_W / 2)));
  }

  return;
}






/*
  Recalculates field of view.
*/
void calc_fov()
{
  float fy;
  float fx;
  float speed_y;
  float speed_x;
  int y;
  int x;
  int radius;
  int d;
  int deg;

  // Wipe the "in view" bit of the FOV map. By ANDing the FOV_SEEN
  // bit, we get to keep the areas we have previously explored.

  for (y = 0; y < MAP_H; y++)
    for (x = 0; x < MAP_W; x++)
      fov[y][x] &= FOV_SEEN;

  radius = FOV_RADIUS;

  fov[player->y][player->x] = true;
  
  // Project a ray in each direction and mark touched tiles as
  // "in view". Stop when we leave the map run into something
  // that obstructs the view.

  for (deg = 0; deg < 360; deg++)
  {
    // Add 0.5 since the player is actually in _the middle_ of the
    // tile; otherwise the result will have strangely offset edges.

    fy = (float)player->y + 0.5f;
    fx = (float)player->x + 0.5f;

    speed_y = sin_tbl[deg];
    speed_x = sin_tbl[(deg + 90) % 360];

    for (d = 0; d < radius; d++)
    {
      fy += speed_y;
      fx += speed_x;

      // Truncate coordinates so we can use them as array indices
      y = (int)fy;
      x = (int)fx;

      // Outside map?
      if (y < 0 || y >= MAP_H || x < 0 || x >= MAP_W)
  break;

      fov[y][x] |= FOV_IN_VIEW | FOV_SEEN;

      if (blocking(y, x) & CANT_SEE)
  break; // Can't see any further in this direction
    }
  }

  return;
}





/*
  Returns a bitmask of the blocking properties of the tile at (Y,X).
*/
int blocking(int y, int x)
{
  int i;

  if (y < 0 || y >= MAP_H || x < 0 || x >= MAP_W)
    return (CANT_MOVE | CANT_SEE | OUTSIDE_MAP);

  // Some tiles we can see past, but not move over. Some tiles we
  // can't see or move through. In theory there could be "dark" tiles
  // that we can move through but not see where we are going.

  switch (map[y][x])
  {
  case tile_wall:
  case tile_rock:
  case tile_tree:
  case tile_door_closed:
    return (CANT_MOVE | CANT_SEE);
    
  case tile_fence:
  case tile_water:
  case tile_toilet:
  case tile_sink:
    return (CANT_MOVE);

  default:
    break;
  }

  // Check if there is a mob occupying that space

  for (i = 0; i < MAX_MOBS; i++)
  {
    if (nouns[i].type == nobody)
      break;

    if (nouns[i].y == y && nouns[i].x == x)
      return SOMEONE_THERE;
  }

  return 0;
}



